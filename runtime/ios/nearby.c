// Round 25: Play nearby's rules (nearby.h).
#include "nearby.h"
#include <stdio.h>
#include <string.h>

// ---- TXT records ----
int rt_nearby_txt_get(const unsigned char *txt, size_t len, const char *key, char *out, size_t size) {
    size_t klen = strlen(key), i = 0;
    while (txt && i < len) {
        size_t n = txt[i++];
        if (n > len - i) return 0;   // truncated
        const char *entry = (const char *)txt + i;
        if (n > klen && entry[klen] == '=' && !strncmp(entry, key, klen)) {
            size_t v = n - klen - 1;
            if (out && size) {
                if (v >= size) v = size - 1;
                memcpy(out, entry + klen + 1, v);
                out[v] = 0;
            }
            return 1;
        }
        i += n;
    }
    return 0;
}
static size_t txt_put(unsigned char *out, size_t size, size_t at, const char *key, const char *value) {
    size_t n = strlen(key) + 1 + strlen(value);
    if (n > 255 || at + 1 + n > size) return at;
    out[at] = (unsigned char)n;
    snprintf((char *)out + at + 1, size - at - 1, "%s=%s", key, value);
    return at + 1 + n;
}
size_t rt_nearby_txt_build(unsigned char *out, size_t size, const char *game) {
    size_t at = txt_put(out, size, 0, "game", game);
    return txt_put(out, size, at, "proto", RT_NEARBY_PROTO);
}

// ---- The list ----
int rt_nearby_add(rt_nearby_list *l, const char *name, const unsigned char *txt, size_t txt_len, const char *game) {
    if (!name || !name[0] || !strcmp(name, l->own)) return 0;
    rt_nearby_host h;
    memset(&h, 0, sizeof(h));
    snprintf(h.name, sizeof(h.name), "%s", name);
    char proto[8] = "";
    // Before its TXT is resolved a host is listed but not yet joinable.
    if (txt && rt_nearby_txt_get(txt, txt_len, "game", h.game, sizeof(h.game)) &&
        rt_nearby_txt_get(txt, txt_len, "proto", proto, sizeof(proto)))
        h.usable = !strcmp(h.game, game) && !strcmp(proto, RT_NEARBY_PROTO);
    int at = 0;
    while (at < l->count && strcmp(l->hosts[at].name, h.name) < 0) ++at;
    if (at < l->count && !strcmp(l->hosts[at].name, h.name)) {   // known: TXT update
        if (!txt) return 0;
        int changed = memcmp(&l->hosts[at], &h, sizeof(h)) != 0;
        l->hosts[at] = h;
        return changed;
    }
    if (l->count >= RT_NEARBY_MAX) return 0;
    memmove(&l->hosts[at + 1], &l->hosts[at], (size_t)(l->count - at) * sizeof(h));
    l->hosts[at] = h;
    ++l->count;
    return 1;
}
int rt_nearby_remove(rt_nearby_list *l, const char *name) {
    for (int i = 0; i < l->count; ++i)
        if (!strcmp(l->hosts[i].name, name)) {
            memmove(&l->hosts[i], &l->hosts[i + 1], (size_t)(l->count - i - 1) * sizeof(l->hosts[0]));
            --l->count;
            return 1;
        }
    return 0;
}

// ---- A2S_INFO ----
size_t rt_nearby_info_query(unsigned char *out, size_t size) {
    static const char q[] = "\xff\xff\xff\xffTSource Engine Query";   // + its terminating 0
    if (size < sizeof(q)) return 0;
    memcpy(out, q, sizeof(q));
    return sizeof(q);
}
// A zero-terminated string at *at, copied (truncated) into out; 0 if it runs off the end.
static int take_string(const unsigned char *r, size_t len, size_t *at, char *out, size_t size) {
    size_t start = *at;
    while (*at < len && r[*at]) ++*at;
    if (*at >= len) return 0;
    if (out && size) {
        size_t n = *at - start < size - 1 ? *at - start : size - 1;
        memcpy(out, r + start, n);
        out[n] = 0;
    }
    ++*at;
    return 1;
}
int rt_nearby_parse_info(const unsigned char *r, size_t len, int *players, int *max_players, char *map, size_t map_size) {
    // FF FF FF FF 'I' protocol, name, map, folder, game, app id (2), players, max, ...
    if (len < 6 || memcmp(r, "\xff\xff\xff\xff", 4) || r[4] != 'I') return 0;
    size_t at = 6;
    if (!take_string(r, len, &at, NULL, 0) || !take_string(r, len, &at, map, map_size) ||
        !take_string(r, len, &at, NULL, 0) || !take_string(r, len, &at, NULL, 0))
        return 0;
    if (at + 4 > len) return 0;
    at += 2;   // app id
    *players = r[at];
    *max_players = r[at + 1];
    return *players <= *max_players;
}

// ---- The session ----
void rt_nearby_start_host(rt_nearby_session *s, double now) {
    memset(s, 0, sizeof(*s));
    s->phase = RT_NB_HOST_STARTING;
    s->since = now;
    s->players = -1;
}
void rt_nearby_start_join(rt_nearby_session *s, double now, const char *peer) {
    memset(s, 0, sizeof(*s));
    s->phase = RT_NB_JOIN_CONNECTING;
    s->since = now;
    s->players = -1;
    snprintf(s->peer, sizeof(s->peer), "%s", peer ? peer : "");
}
void rt_nearby_stop(rt_nearby_session *s) { s->phase = RT_NB_IDLE; }
void rt_nearby_heard(rt_nearby_session *s, double now, int players) {
    s->heard_at = now;
    s->players = players;
    if (players >= 2) s->few_since = 0;
    else if (!s->few_since) s->few_since = now;
}
static rt_nearby_event end(rt_nearby_session *s, rt_nearby_event e) {
    s->phase = RT_NB_IDLE;
    return e;
}
static double later(double a, double b) { return a > b ? a : b; }
rt_nearby_event rt_nearby_step(rt_nearby_session *s, double now, int in_level) {
    if (s->phase == RT_NB_IDLE) return RT_NB_NONE;
    // Coming into or out of a level (a co-op map change leaves one for a while).
    if (in_level && !s->level_since) { s->level_since = now; s->out_since = 0; }
    if (!in_level && s->level_since) { s->level_since = 0; s->out_since = now; }
    switch (s->phase) {
    case RT_NB_HOST_STARTING:
        if (in_level) { s->phase = RT_NB_HOST_WAITING; s->since = now; return RT_NB_ADVERTISE; }
        if (now - s->since > RT_NB_HOST_START_LIMIT) return end(s, RT_NB_HOST_FAILED);
        return RT_NB_NONE;
    case RT_NB_HOST_WAITING:
    case RT_NB_HOST_PLAYING:
        if (s->out_since && now - s->out_since > RT_NB_OUT_LIMIT) return end(s, RT_NB_ENDED);   // the host left
        if (s->phase == RT_NB_HOST_WAITING) {
            if (s->players >= 2) { s->phase = RT_NB_HOST_PLAYING; s->since = now; return RT_NB_FRIEND_JOINED; }
            return RT_NB_NONE;
        }
        // Counted only once the level has been up a while (the friend reconnects after a map change).
        if (in_level && s->few_since && now - later(s->few_since, s->level_since + RT_NB_GRACE) > RT_NB_FRIEND_GONE)
            return end(s, RT_NB_FRIEND_LEFT);
        return RT_NB_NONE;
    case RT_NB_JOIN_CONNECTING:
        if (in_level) {
            s->phase = RT_NB_JOIN_PLAYING;
            s->since = now;
            s->heard_at = later(s->heard_at, now);
            return RT_NB_JOINED;
        }
        if (now - s->since > RT_NB_JOIN_LIMIT) return end(s, RT_NB_JOIN_FAILED);
        return RT_NB_NONE;
    case RT_NB_JOIN_PLAYING:
        if (in_level && now - later(s->heard_at, s->level_since) > RT_NB_SILENT) return end(s, RT_NB_HOST_LOST);
        if (!in_level && now - later(s->heard_at, s->out_since) > RT_NB_SILENT_OUT) return end(s, RT_NB_HOST_LOST);
        if (s->out_since && now - s->out_since > RT_NB_OUT_LIMIT) return end(s, RT_NB_ENDED);   // left, host still up
        return RT_NB_NONE;
    case RT_NB_IDLE: break;
    }
    return RT_NB_NONE;
}

// ---- Texts ----
const char *rt_nearby_denied_message(const char *app) {
    static char text[400];
    snprintf(text, sizeof(text),
             "%s isn't allowed to use the local network, so it can't find or reach other devices.\n\n"
             "To turn it back on: open Settings, then Privacy & Security > Local Network, and switch on %s. "
             "Then try Play nearby again.", app, app);
    return text;
}
const char *rt_nearby_message(rt_nearby_event e, const char *peer, char *out, size_t size) {
    const char *who = peer && peer[0] ? peer : "the other device";
    switch (e) {
    case RT_NB_FRIEND_LEFT:
        snprintf(out, size, "Your friend left or lost the connection. The co-op game has ended, so you're back at the menu.");
        return out;
    case RT_NB_HOST_FAILED:
        snprintf(out, size, "The co-op lobby didn't start, so no one could join. You're back at the menu: try Host again.");
        return out;
    case RT_NB_JOIN_FAILED:
        snprintf(out, size, "Couldn't connect to %s. Check that both devices are on the same Wi-Fi and that it is "
                 "still hosting, then try Join again.", who);
        return out;
    case RT_NB_HOST_LOST:
        snprintf(out, size, "Lost the connection to %s: it stopped hosting or left the Wi-Fi. You're back at the menu.", who);
        return out;
    default: return NULL;
    }
}
