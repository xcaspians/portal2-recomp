// Round 26: custom button layouts through iCloud, per device kind (layout_sync.h).
#include "layout_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *base(int ipad) { return ipad ? "P2TouchLayout.pad" : "P2TouchLayout.phone"; }

const char *rt_layout_sync_file(int ipad) { return ipad ? "Layouts/iPad.txt" : "Layouts/iPhone.txt"; }

int rt_layout_sync_key_of_kind(const char *key, int ipad) {
    const char *b = base(ipad);
    size_t n = strlen(b);
    if (!key || strncmp(key, b, n)) return 0;
    if (!key[n]) return 1;
    if (key[n] != '.' || !key[n + 1]) return 0;
    for (const char *p = key + n + 1; *p; ++p)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9'))) return 0;
    return 1;
}

const rt_layout_entry *rt_layout_set_get(const rt_layout_set *s, const char *key) {
    for (int i = 0; i < s->count; ++i)
        if (!strcmp(s->e[i].key, key)) return &s->e[i];
    return NULL;
}

int rt_layout_set_put(rt_layout_set *s, int ipad, const char *key, double time, const char *value) {
    if (!rt_layout_sync_key_of_kind(key, ipad) || strlen(key) >= sizeof(s->e[0].key) || !value ||
        strlen(value) >= RT_LAYOUT_SYNC_VALUE || strchr(value, '\n') || strchr(value, '\t') || strncmp(value, "v1", 2))
        return 0;
    rt_layout_entry *e = (rt_layout_entry *)rt_layout_set_get(s, key);
    if (!e) {
        if (s->count >= RT_LAYOUT_SYNC_MAX) return 0;
        e = &s->e[s->count++];
        snprintf(e->key, sizeof(e->key), "%s", key);
    }
    e->time = time;
    snprintf(e->value, sizeof(e->value), "%s", value);
    return 1;
}

int rt_layout_set_format(const rt_layout_set *s, int ipad, char *out, size_t size) {
    size_t used = (size_t)snprintf(out, size, "sauce-layouts v1 %s\n", ipad ? "ipad" : "iphone");
    for (int i = 0; i < s->count && used < size; ++i)
        if (rt_layout_sync_key_of_kind(s->e[i].key, ipad))
            used += (size_t)snprintf(out + used, size - used, "%s\t%.3f\t%s\n", s->e[i].key, s->e[i].time, s->e[i].value);
    return used < size;
}

int rt_layout_set_parse(const char *text, int ipad, rt_layout_set *out) {
    memset(out, 0, sizeof(*out));
    char head[40];
    snprintf(head, sizeof(head), "sauce-layouts v1 %s\n", ipad ? "ipad" : "iphone");
    if (!text || strncmp(text, head, strlen(head))) return 0;   // another kind's file, or not one
    for (const char *line = text + strlen(head); *line;) {
        const char *end = strchr(line, '\n');
        size_t len = end ? (size_t)(end - line) : strlen(line);
        char buf[64 + 32 + RT_LAYOUT_SYNC_VALUE];
        if (len < sizeof(buf)) {
            memcpy(buf, line, len);
            buf[len] = 0;
            char *t1 = strchr(buf, '\t'), *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
            if (t1 && t2) {
                *t1 = *t2 = 0;
                char *num_end = NULL;
                double time = strtod(t1 + 1, &num_end);
                if (num_end && !*num_end && time >= 0) rt_layout_set_put(out, ipad, buf, time, t2 + 1);
            }
        }
        line += len + (end ? 1 : 0);
    }
    return 1;
}

int rt_layout_set_merge(rt_layout_set *into, const rt_layout_set *from, int ipad) {
    int changed = 0;
    for (int i = 0; i < from->count; ++i) {
        const rt_layout_entry *f = &from->e[i];
        const rt_layout_entry *have = rt_layout_set_get(into, f->key);
        if (have && have->time >= f->time) continue;
        if (have && !strcmp(have->value, f->value)) {   // same layout, later stamp: just the time
            ((rt_layout_entry *)have)->time = f->time;
            continue;
        }
        changed += rt_layout_set_put(into, ipad, f->key, f->time, f->value);
    }
    return changed;
}
