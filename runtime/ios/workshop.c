// Community Maps: Workshop answers, requests and files (see workshop.h).
#include "workshop.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

// ---- JSON ----
typedef struct { int tok, object, expect_key; } frame;

static int add(rt_json_tok *toks, int *count, int max, int type, int start, int end) {
    if (*count >= max) return -1;
    toks[*count] = (rt_json_tok){type, start, end, 0};
    return (*count)++;
}
// A value (or a key) starts inside the innermost container: count it there.
static int attach(rt_json_tok *toks, frame *stack, int depth, int is_string) {
    if (!depth) return 0;
    frame *f = &stack[depth - 1];
    if (!f->object) { toks[f->tok].size++; return 0; }
    if (f->expect_key) {
        if (!is_string) return -1;   // object members start with a string key
        toks[f->tok].size++;
        f->expect_key = 0;
        return 1;                    // this string is a key
    }
    return 0;
}

int rt_json_parse(const char *js, size_t len, rt_json_tok *toks, int max) {
    frame stack[64];
    int depth = 0, count = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = js[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ':') continue;
        if (c == ',') {
            if (depth && stack[depth - 1].object) stack[depth - 1].expect_key = 1;
            continue;
        }
        if (c == '{' || c == '[') {
            if (depth >= 64 || attach(toks, stack, depth, 0) < 0) return -1;
            int t = add(toks, &count, max, c == '{' ? RT_JSON_OBJECT : RT_JSON_ARRAY, (int)i, -1);
            if (t < 0) return -1;
            stack[depth++] = (frame){t, c == '{', 1};
            continue;
        }
        if (c == '}' || c == ']') {
            if (!depth || stack[depth - 1].object != (c == '}')) return -1;
            toks[stack[--depth].tok].end = (int)i + 1;
            continue;
        }
        if (c == '"') {
            size_t j = i + 1;
            while (j < len && js[j] != '"') j += js[j] == '\\' ? 2 : 1;
            if (j >= len) return -1;
            if (attach(toks, stack, depth, 1) < 0) return -1;
            if (add(toks, &count, max, RT_JSON_STRING, (int)i + 1, (int)j) < 0) return -1;
            i = j;
            continue;
        }
        if (!strchr("-0123456789tfn", c)) return -1;
        size_t j = i;
        while (j < len && !strchr(",]} \t\r\n", js[j])) ++j;
        if (attach(toks, stack, depth, 0) < 0) return -1;
        if (add(toks, &count, max, RT_JSON_PRIMITIVE, (int)i, (int)j) < 0) return -1;
        i = j - 1;
    }
    return depth ? -1 : count;
}

int rt_json_next(const rt_json_tok *t, int count, int i) {
    if (i < 0 || i >= count) return count;
    int j = i + 1;
    if (t[i].type == RT_JSON_OBJECT)
        for (int k = 0; k < t[i].size && j < count; ++k) j = rt_json_next(t, count, rt_json_next(t, count, j));
    else if (t[i].type == RT_JSON_ARRAY)
        for (int k = 0; k < t[i].size && j < count; ++k) j = rt_json_next(t, count, j);
    return j;
}

int rt_json_get(const char *js, const rt_json_tok *t, int count, int obj, const char *key) {
    if (obj < 0 || obj >= count || t[obj].type != RT_JSON_OBJECT) return -1;
    size_t n = strlen(key);
    int j = obj + 1;
    for (int k = 0; k < t[obj].size && j + 1 < count; ++k) {
        if (t[j].type == RT_JSON_STRING && (size_t)(t[j].end - t[j].start) == n && !memcmp(js + t[j].start, key, n))
            return j + 1;
        j = rt_json_next(t, count, j + 1);
    }
    return -1;
}

static int hex4(const char *p, unsigned *v) {
    *v = 0;
    for (int i = 0; i < 4; ++i) {
        int c = p[i], d = isdigit(c) ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'a' + 10 : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
        if (d < 0) return 0;
        *v = *v * 16 + (unsigned)d;
    }
    return 1;
}
// Length of the valid UTF-8 sequence at p (at most avail bytes), 0 if invalid.
static int utf8_length(const unsigned char *p, int avail) {
    unsigned c = p[0];
    int len = c < 0x80 ? 1 : (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 0;
    if (!len || len > avail) return 0;
    for (int k = 1; k < len; ++k) if ((p[k] & 0xC0) != 0x80) return 0;
    unsigned cp = len == 1 ? c : len == 2 ? (c & 31u) << 6 | (p[1] & 63u)
                : len == 3 ? (c & 15u) << 12 | (p[1] & 63u) << 6 | (p[2] & 63u)
                : (c & 7u) << 18 | (p[1] & 63u) << 12 | (p[2] & 63u) << 6 | (p[3] & 63u);
    if ((len == 2 && cp < 0x80) || (len == 3 && cp < 0x800) || (len == 4 && cp < 0x10000) || cp > 0x10FFFF ||
        (cp >= 0xD800 && cp < 0xE000)) return 0;
    return len;
}
// Appends a code point; 0 when it doesn't fit (nothing half-written).
static int put_utf8(char *out, size_t size, size_t *n, unsigned cp) {
    char b[4]; size_t k;
    if (cp < 0x80) { b[0] = (char)cp; k = 1; }
    else if (cp < 0x800) { b[0] = (char)(0xC0 | cp >> 6); b[1] = (char)(0x80 | (cp & 63)); k = 2; }
    else if (cp < 0x10000) { b[0] = (char)(0xE0 | cp >> 12); b[1] = (char)(0x80 | (cp >> 6 & 63)); b[2] = (char)(0x80 | (cp & 63)); k = 3; }
    else { b[0] = (char)(0xF0 | cp >> 18); b[1] = (char)(0x80 | (cp >> 12 & 63)); b[2] = (char)(0x80 | (cp >> 6 & 63));
           b[3] = (char)(0x80 | (cp & 63)); k = 4; }
    if (*n + k >= size) return 0;
    memcpy(out + *n, b, k); *n += k;
    return 1;
}

int rt_json_text(const char *js, const rt_json_tok *t, char *out, size_t size) {
    if (!size) return 0;
    size_t n = 0;
    out[0] = 0;
    if (!t || (t->type != RT_JSON_STRING && t->type != RT_JSON_PRIMITIVE)) return 0;
    for (int i = t->start; i < t->end; ++i) {
        unsigned cp = (unsigned char)js[i];
        if (t->type == RT_JSON_STRING && cp == '\\' && i + 1 < t->end) {
            char e = js[++i];
            switch (e) {
            case 'n': cp = '\n'; break; case 't': cp = '\t'; break; case 'r': cp = '\r'; break;
            case 'b': cp = '\b'; break; case 'f': cp = '\f'; break;
            case 'u':
                if (i + 4 >= t->end || !hex4(js + i + 1, &cp)) { out[n] = 0; return 0; }
                i += 4;
                unsigned low;
                if (cp >= 0xD800 && cp < 0xDC00 && i + 6 < t->end && js[i + 1] == '\\' && js[i + 2] == 'u' &&
                    hex4(js + i + 3, &low) && low >= 0xDC00 && low < 0xE000) {
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                    i += 6;
                } else if (cp >= 0xD800 && cp < 0xE000) cp = 0xFFFD;   // lone surrogate
                break;
            default: cp = (unsigned char)e; break;
            }
            if (!put_utf8(out, size, &n, cp)) { out[n] = 0; return 0; }
            continue;
        }
        // Raw bytes: a valid UTF-8 sequence is copied whole or not at all;
        // anything else (stray byte, overlong form, surrogate) becomes U+FFFD.
        int len = utf8_length((const unsigned char *)js + i, t->end - i);
        if (!len) {
            if (!put_utf8(out, size, &n, 0xFFFD)) { out[n] = 0; return 0; }
            continue;
        }
        if (n + (size_t)len >= size) { out[n] = 0; return 0; }
        memcpy(out + n, js + i, (size_t)len); n += (size_t)len; i += len - 1;
    }
    out[n] = 0;
    return 1;
}

long long rt_json_number(const char *js, const rt_json_tok *t) {
    char text[32];
    if (!t || t->end - t->start >= (int)sizeof(text) || (t->type != RT_JSON_STRING && t->type != RT_JSON_PRIMITIVE)) return 0;
    memcpy(text, js + t->start, (size_t)(t->end - t->start)); text[t->end - t->start] = 0;
    return strtoll(text, NULL, 10);
}

// ---- answers ----
typedef struct { const char *js; rt_json_tok *t; int n; } doc;

static int open_doc(doc *d, const char *json, size_t len) {
    int max = (int)(len / 2) + 16;
    d->js = json;
    d->t = malloc((size_t)max * sizeof(rt_json_tok));
    d->n = d->t ? rt_json_parse(json, len, d->t, max) : -1;
    return d->n > 0 && d->t[0].type == RT_JSON_OBJECT;
}
static int get(const doc *d, int obj, const char *key) { return rt_json_get(d->js, d->t, d->n, obj, key); }
static void text(const doc *d, int obj, const char *key, char *out, size_t size) {
    int v = get(d, obj, key);
    if (v < 0 || !rt_json_text(d->js, &d->t[v], out, size)) { if (v < 0) out[0] = 0; }
}
static long long number(const doc *d, int obj, const char *key, long long def) {
    int v = get(d, obj, key);
    return v < 0 ? def : rt_json_number(d->js, &d->t[v]);
}
static int ends_with(const char *s, const char *tail) {
    size_t a = strlen(s), b = strlen(tail);
    return a >= b && !strcasecmp(s + a - b, tail);
}

int rt_ws_parse_items(const char *json, size_t len, rt_ws_item *items, int max, char *cursor, size_t cursor_size,
                      long long *total) {
    doc d;
    int found = -1;
    if (cursor && cursor_size) cursor[0] = 0;
    if (total) *total = 0;
    if (!open_doc(&d, json, len)) goto done;
    int response = get(&d, 0, "response");
    if (response < 0) goto done;
    if (cursor) text(&d, response, "next_cursor", cursor, cursor_size);
    if (total) *total = number(&d, response, "total", 0);
    int list = get(&d, response, "publishedfiledetails");
    found = 0;
    if (list < 0 || d.t[list].type != RT_JSON_ARRAY) goto done;
    int j = list + 1;
    for (int k = 0; k < d.t[list].size && found < max; ++k, j = rt_json_next(d.t, d.n, j)) {
        if (d.t[j].type != RT_JSON_OBJECT) continue;
        rt_ws_item *it = &items[found++];
        memset(it, 0, sizeof(*it));
        it->id = (unsigned long long)number(&d, j, "publishedfileid", 0);
        long long app = number(&d, j, "consumer_app_id", number(&d, j, "consumer_appid", 620));
        it->found = number(&d, j, "result", 1) == 1 && app == 620 && it->id;
        text(&d, j, "title", it->title, sizeof(it->title));
        text(&d, j, "creator", it->creator, sizeof(it->creator));
        text(&d, j, "file_url", it->file_url, sizeof(it->file_url));
        text(&d, j, "filename", it->filename, sizeof(it->filename));
        text(&d, j, "preview_url", it->preview_url, sizeof(it->preview_url));
        char raw[sizeof(it->description)];
        text(&d, j, get(&d, j, "short_description") >= 0 ? "short_description" : "description", raw, sizeof(raw));
        rt_ws_plain_text(raw, it->description, sizeof(it->description));
        it->file_size = number(&d, j, "file_size", 0);
        it->time_created = number(&d, j, "time_created", 0);
        it->time_updated = number(&d, j, "time_updated", it->time_created);
        it->subscriptions = number(&d, j, "lifetime_subscriptions", number(&d, j, "subscriptions", 0));
        it->favorited = number(&d, j, "favorited", 0);
        it->is_collection = number(&d, j, "file_type", 0) == 2 || strstr(it->filename, "collection/") != NULL;
        it->is_map = !it->is_collection && ends_with(it->filename, ".bsp");
        int tags = get(&d, j, "tags");
        if (tags >= 0 && d.t[tags].type == RT_JSON_ARRAY) {
            int m = tags + 1;
            for (int q = 0; q < d.t[tags].size; ++q, m = rt_json_next(d.t, d.n, m)) {
                char tag[64];
                text(&d, m, "tag", tag, sizeof(tag));
                if (!strcasecmp(tag, "Cooperative")) it->coop = 1;
                if (!strcasecmp(tag, "Singleplayer")) it->singleplayer = 1;
            }
        }
    }
done:
    free(d.t);
    return found;
}

int rt_ws_parse_collection(const char *json, size_t len, unsigned long long *ids, int max) {
    doc d;
    int found = -1;
    if (!open_doc(&d, json, len)) goto done;
    int response = get(&d, 0, "response"), list = response < 0 ? -1 : get(&d, response, "collectiondetails");
    if (list < 0 || d.t[list].type != RT_JSON_ARRAY || !d.t[list].size) goto done;
    int children = get(&d, list + 1, "children");
    found = 0;
    if (children < 0 || d.t[children].type != RT_JSON_ARRAY) goto done;
    int j = children + 1;
    for (int k = 0; k < d.t[children].size && found < max; ++k, j = rt_json_next(d.t, d.n, j)) {
        unsigned long long id = (unsigned long long)number(&d, j, "publishedfileid", 0);
        if (id) ids[found++] = id;
    }
done:
    free(d.t);
    return found;
}

int rt_ws_parse_names(const char *json, size_t len, const char *const *steamids, int n, char (*names)[64]) {
    doc d;
    int found = -1;
    for (int i = 0; i < n; ++i) names[i][0] = 0;
    if (!open_doc(&d, json, len)) goto done;
    int response = get(&d, 0, "response"), players = response < 0 ? -1 : get(&d, response, "players");
    found = 0;
    if (players < 0 || d.t[players].type != RT_JSON_ARRAY) goto done;
    int j = players + 1;
    for (int k = 0; k < d.t[players].size; ++k, j = rt_json_next(d.t, d.n, j)) {
        char id[24], name[64];
        text(&d, j, "steamid", id, sizeof(id));
        text(&d, j, "personaname", name, sizeof(name));
        for (int i = 0; i < n; ++i)
            if (!strcmp(steamids[i], id) && !names[i][0]) { snprintf(names[i], 64, "%s", name); ++found; }
    }
done:
    free(d.t);
    return found;
}

void rt_ws_plain_text(const char *in, char *out, size_t size) {
    static const char *const tags[] = {"b", "i", "u", "s", "h1", "h2", "h3", "url", "list", "olist", "quote", "code",
                                       "spoiler", "noparse", "strike", "table", "tr", "td", "th", "hr", "img", "*", NULL};
    size_t n = 0;
    if (!size) return;
    for (const char *p = in; *p && n + 1 < size;) {
        if (*p == '[') {
            const char *close = strchr(p, ']');
            char name[16] = {0};
            const char *q = p + 1;
            if (*q == '/') ++q;
            size_t k = 0;
            while (q < close && *q != '=' && k + 1 < sizeof(name)) name[k++] = (char)tolower((unsigned char)*q++);
            int known = 0;
            for (int i = 0; close && tags[i]; ++i) if (!strcmp(name, tags[i])) known = 1;
            if (known) {
                if (!strcmp(name, "*") && n + 4 < size) { if (n && out[n - 1] != '\n') out[n++] = '\n'; memcpy(out + n, "\xE2\x80\xA2 ", 4); n += 4; }
                else if (!strcmp(name, "hr") && n + 1 < size) out[n++] = '\n';
                else if (!strcmp(name, "img") && p[1] != '/') {   // the image's URL is no text
                    const char *end = strstr(close, "[/img]");
                    p = end ? end + 6 : close + 1;
                    continue;
                }
                p = close + 1;
                continue;
            }
        }
        if (*p != '\r') out[n++] = *p;
        ++p;
    }
    while (n && (out[n - 1] == '\n' || out[n - 1] == ' ')) --n;
    out[n] = 0;
}

// ---- requests ----
static int encode(char *out, size_t size, size_t *n, const char *s) {
    for (; s && *s; ++s) {
        unsigned char c = (unsigned char)*s;
        if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
            if (*n + 1 >= size) return 0;
            out[(*n)++] = (char)c;
        } else {
            if (*n + 3 >= size) return 0;
            *n += (size_t)snprintf(out + *n, size - *n, "%%%02X", c);
        }
    }
    out[*n] = 0;
    return 1;
}
static int append(char *out, size_t size, size_t *n, const char *s) {
    size_t k = strlen(s);
    if (*n + k >= size) return 0;
    memcpy(out + *n, s, k + 1); *n += k;
    return 1;
}

int rt_ws_query_url(char *out, size_t size, const char *key, rt_ws_sort sort, const char *cursor, const char *search,
                    int coop) {
    // EPublishedFileQueryType: 9 most subscribed, 1 newest, 3 trending, 12 text search.
    static const int types[] = {9, 1, 3, 12};
    char head[96];
    size_t n = 0;
    snprintf(head, sizeof(head), "&appid=620&query_type=%d&numperpage=20&filetype=0&cursor=", types[sort]);
    int ok = append(out, size, &n, "https://api.steampowered.com/IPublishedFileService/QueryFiles/v1/?key=") &&
             encode(out, size, &n, key) && append(out, size, &n, head) &&
             encode(out, size, &n, cursor && *cursor ? cursor : "*") &&
             append(out, size, &n, "&requiredtags%5B0%5D=") &&
             append(out, size, &n, coop ? "Cooperative" : "Singleplayer") &&
             append(out, size, &n, "&return_previews=1&return_short_description=1&return_tags=1");
    if (ok && sort == RT_WS_TRENDING) ok = append(out, size, &n, "&days=7");
    if (ok && sort == RT_WS_SEARCH) ok = append(out, size, &n, "&search_text=") && encode(out, size, &n, search ? search : "");
    return ok;
}

int rt_ws_details_body(char *out, size_t size, const unsigned long long *ids, int n) {
    size_t used = (size_t)snprintf(out, size, "itemcount=%d", n);
    for (int i = 0; i < n && used < size; ++i)
        used += (size_t)snprintf(out + used, size - used, "&publishedfileids[%d]=%llu", i, ids[i]);
    return used < size;
}
int rt_ws_collection_body(char *out, size_t size, unsigned long long id) {
    return (size_t)snprintf(out, size, "collectioncount=1&publishedfileids[0]=%llu", id) < size;
}

int rt_ws_parse_link(const char *text, unsigned long long *id) {
    // "id=" as its own parameter (after ? or &), not the end of "appid=".
    const char *p = NULL;
    for (const char *q = text ? strstr(text, "id=") : NULL; q; q = strstr(q + 1, "id="))
        if (q > text && (q[-1] == '?' || q[-1] == '&')) { p = q; break; }
    if (p) p += 3;
    else if (text && strstr(text, "id=")) return 0;
    else {
        while (text && isspace((unsigned char)*text)) ++text;
        p = text;
        for (const char *q = p; q && *q; ++q)
            if (!isdigit((unsigned char)*q) && !isspace((unsigned char)*q)) return 0;   // only the number alone
    }
    if (!p || !isdigit((unsigned char)*p)) return 0;
    int digits = 0;
    unsigned long long v = 0;
    for (; isdigit((unsigned char)*p); ++p, ++digits) {
        if (digits >= 20) return 0;
        v = v * 10 + (unsigned)(*p - '0');
    }
    if (!v) return 0;
    *id = v;
    return 1;
}

// ---- files ----
int rt_ws_item_dir(unsigned long long id, char *out, size_t size) {
    return (size_t)snprintf(out, size, "portal2/maps/workshop/%llu", id) < size;
}

int rt_ws_map_path(const rt_ws_item *it, char *out, size_t size) {
    const char *base = strrchr(it->filename, '/');
    base = base ? base + 1 : it->filename;
    char name[128];
    size_t n = 0;
    for (const char *p = base; *p && n + 1 < sizeof(name); ++p) {
        unsigned char c = (unsigned char)*p;
        name[n++] = isalnum(c) || c == '_' || c == '-' || (c == '.' && p[1] != '.') ? (char)c : '_';
    }
    name[n] = 0;
    if (ends_with(name, ".bsp")) name[n - 4] = 0;
    if (!name[0] || name[0] == '.' || name[strlen(name) - 1] == '.') snprintf(name, sizeof(name), "%llu", it->id);
    return (size_t)snprintf(out, size, "portal2/maps/workshop/%llu/%s.bsp", it->id, name) < size;
}

int rt_ws_meta_write(const char *path, const rt_ws_item *it) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    char title[sizeof(it->title)];
    snprintf(title, sizeof(title), "%s", it->title);
    for (char *p = title; *p; ++p) if (*p == '\n' || *p == '\r') *p = ' ';
    int ok = fprintf(f, "updated=%lld\nsize=%lld\ntitle=%s\n", it->time_updated, it->file_size, title) > 0;
    return fclose(f) == 0 && ok;
}

int rt_ws_meta_read(const char *path, long long *updated, long long *size, char *title, size_t title_size) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[256];
    *updated = *size = 0;
    if (title_size) title[0] = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (!strncmp(line, "updated=", 8)) *updated = strtoll(line + 8, NULL, 10);
        else if (!strncmp(line, "size=", 5)) *size = strtoll(line + 5, NULL, 10);
        else if (!strncmp(line, "title=", 6) && title_size) snprintf(title, title_size, "%s", line + 6);
    }
    fclose(f);
    return *updated > 0;
}

int rt_ws_update_available(long long saved_updated, long long saved_size, long long workshop_updated,
                           long long workshop_size) {
    if (saved_updated <= 0 || workshop_updated <= 0) return 0;   // unknown: no badge rather than a wrong one
    if (workshop_updated > saved_updated) return 1;
    return workshop_updated == saved_updated && saved_size > 0 && workshop_size > 0 && workshop_size != saved_size;
}

static int numeric(const char *s) {
    if (!*s) return 0;
    for (; *s; ++s) if (!isdigit((unsigned char)*s)) return 0;
    return 1;
}

int rt_ws_cleanup(const char *workshop_dir) {
    DIR *top = opendir(workshop_dir);
    if (!top) return 0;
    int removed = 0;
    struct dirent *e;
    while ((e = readdir(top))) {
        if (!numeric(e->d_name)) continue;   // only item folders (named by their Workshop id)
        char dir[1024];
        snprintf(dir, sizeof(dir), "%s/%s", workshop_dir, e->d_name);
        DIR *item = opendir(dir);
        if (!item) continue;
        int maps = 0, others = 0;
        struct dirent *f;
        while ((f = readdir(item))) {
            if (f->d_name[0] == '.' && (!f->d_name[1] || (f->d_name[1] == '.' && !f->d_name[2]))) continue;
            char path[1280];
            snprintf(path, sizeof(path), "%s/%s", dir, f->d_name);
            if (ends_with(f->d_name, ".part")) { removed += unlink(path) == 0; continue; }
            if (ends_with(f->d_name, ".bsp")) ++maps;
            else if (strcmp(f->d_name, "p2workshop.txt")) ++others;
        }
        closedir(item);
        // No map left and nothing but our own note: the folder of a canceled download.
        if (!maps && !others) {
            char meta[1280];
            snprintf(meta, sizeof(meta), "%s/p2workshop.txt", dir);
            removed += unlink(meta) == 0;
            rmdir(dir);
        }
    }
    closedir(top);
    return removed;
}

int rt_ws_delete(const char *item_dir) {
    DIR *item = opendir(item_dir);
    if (!item) return 0;
    struct dirent *f;
    while ((f = readdir(item))) {
        if (f->d_name[0] == '.' && (!f->d_name[1] || (f->d_name[1] == '.' && !f->d_name[2]))) continue;
        char path[1280];
        snprintf(path, sizeof(path), "%s/%s", item_dir, f->d_name);
        unlink(path);
    }
    closedir(item);
    return rmdir(item_dir) == 0;
}
