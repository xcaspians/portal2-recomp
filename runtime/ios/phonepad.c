// A phone as a controller: the plain-C rules (see phonepad.h).
#include "phonepad.h"
#include "touch_layout.h"
#include <CommonCrypto/CommonDigest.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void rt_phonepad_code(uint32_t random, char out[5]) { snprintf(out, 5, "%04u", random % 10000u); }

int rt_phonepad_code_ok(const char *query, const char *code) {
    if (!query || !code || !code[0]) return 0;
    for (const char *p = query; p && *p; p = strchr(p, '&') ? strchr(p, '&') + 1 : NULL) {
        if (strncmp(p, "code=", 5)) continue;
        size_t n = strlen(code);
        return !strncmp(p + 5, code, n) && (p[5 + n] == 0 || p[5 + n] == '&');
    }
    return 0;
}

int rt_phonepad_local_address(const char *a) {
    if (!a || !*a) return 0;
    unsigned b0, b1, b2, b3;
    if (sscanf(a, "%u.%u.%u.%u", &b0, &b1, &b2, &b3) == 4 && b0 < 256 && b1 < 256)
        return b0 == 10 || b0 == 127 || (b0 == 192 && b1 == 168) || (b0 == 172 && b1 >= 16 && b1 < 32) ||
               (b0 == 169 && b1 == 254);
    if (!strncasecmp(a, "::ffff:", 7)) return rt_phonepad_local_address(a + 7);
    if (!strcmp(a, "::1")) return 1;
    unsigned head = 0;
    if (sscanf(a, "%x:", &head) == 1)
        return (head & 0xffc0) == 0xfe80 || (head & 0xfe00) == 0xfc00;
    return 0;
}

static void copy_until(char *out, size_t cap, const char *from, const char *stop) {
    size_t n = (size_t)(stop - from);
    if (n >= cap) n = cap - 1;
    memcpy(out, from, n);
    out[n] = 0;
}
int rt_phonepad_parse_request(const char *text, size_t len, rt_phonepad_request *out) {
    memset(out, 0, sizeof(*out));
    char head[2048];
    if (len >= sizeof(head)) len = sizeof(head) - 1;
    memcpy(head, text, len);
    head[len] = 0;
    char *end = strstr(head, "\r\n\r\n");
    if (!end) return 0;
    out->complete = 1;
    *end = 0;
    char *sp1 = strchr(head, ' '), *sp2 = sp1 ? strchr(sp1 + 1, ' ') : NULL;
    if (!sp1 || !sp2 || sp1 - head >= (long)sizeof(out->method)) return 0;
    copy_until(out->method, sizeof(out->method), head, sp1);
    char *target = sp1 + 1, *q = memchr(target, '?', (size_t)(sp2 - target));
    copy_until(out->path, sizeof(out->path), target, q ? q : sp2);
    if (q) copy_until(out->query, sizeof(out->query), q + 1, sp2);
    for (char *line = strstr(head, "\r\n"); line; line = strstr(line + 2, "\r\n")) {
        char *l = line + 2;
        if (!strncasecmp(l, "Sec-WebSocket-Key:", 18)) {
            l += 18;
            while (*l == ' ') ++l;
            char *e = strstr(l, "\r\n");
            copy_until(out->ws_key, sizeof(out->ws_key), l, e ? e : l + strlen(l));
        }
    }
    return out->path[0] == '/';
}

const char *rt_phonepad_file(const char *path, const char **type) {
    static const char *const files[][3] = {
        {"/", "index.html", "text/html; charset=utf-8"},
        {"/index.html", "index.html", "text/html; charset=utf-8"},
        {"/manifest.webmanifest", "manifest.webmanifest", "application/manifest+json"},
        {"/icon.png", "icon.png", "image/png"},
    };
    for (size_t i = 0; path && i < sizeof(files) / sizeof(*files); ++i)
        if (!strcmp(path, files[i][0])) { if (type) *type = files[i][2]; return files[i][1]; }
    return NULL;
}

void rt_phonepad_ws_accept(const char *key, char out[32]) {
    char joined[128];
    snprintf(joined, sizeof(joined), "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", key);
    unsigned char sha[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1(joined, (CC_LONG)strlen(joined), sha);
    static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t o = 0;
    for (int i = 0; i < 20; i += 3) {
        unsigned v = (unsigned)sha[i] << 16 | (i + 1 < 20 ? (unsigned)sha[i + 1] << 8 : 0) | (i + 2 < 20 ? sha[i + 2] : 0);
        out[o++] = b64[v >> 18 & 63]; out[o++] = b64[v >> 12 & 63];
        out[o++] = i + 1 < 20 ? b64[v >> 6 & 63] : '=';
        out[o++] = i + 2 < 20 ? b64[v & 63] : '=';
    }
    out[o] = 0;
}

int rt_phonepad_ws_decode(const uint8_t *buf, size_t len, int *opcode, char *payload, size_t cap) {
    if (len < 2) return 0;
    int fin = buf[0] & 0x80, masked = buf[1] & 0x80;
    size_t n = buf[1] & 0x7f, at = 2;
    if (n == 126) {
        if (len < 4) return 0;
        n = (size_t)buf[2] << 8 | buf[3]; at = 4;
    } else if (n == 127) return -1;   // a controller never sends that much
    if (!fin || n > 4096 || n + 1 > cap) return -1;
    size_t need = at + (masked ? 4 : 0) + n;
    if (len < need) return 0;
    const uint8_t *mask = masked ? buf + at : NULL;
    at += masked ? 4 : 0;
    for (size_t i = 0; i < n; ++i) payload[i] = (char)(buf[at + i] ^ (mask ? mask[i & 3] : 0));
    payload[n] = 0;
    *opcode = buf[0] & 0x0f;
    return (int)need;
}

size_t rt_phonepad_ws_text(const char *text, uint8_t *out, size_t cap) {
    size_t n = strlen(text), head = n < 126 ? 2 : 4;
    if (n > 65535 || head + n > cap) return 0;
    out[0] = 0x81;
    if (n < 126) out[1] = (uint8_t)n;
    else { out[1] = 126; out[2] = (uint8_t)(n >> 8); out[3] = (uint8_t)n; }
    memcpy(out + head, text, n);
    return head + n;
}

int rt_phonepad_parse(const char *text, rt_phone_message *m) { return rt_phonepad_parse_for(text, RT_IOS_LAYOUT_PORTAL2, m); }
int rt_phonepad_parse_for(const char *text, rt_ios_layout layout, rt_phone_message *m) {
    memset(m, 0, sizeof(*m));
    m->button = m->trigger = m->action = -1;
    if (text && !strcmp(text, "x")) { m->kind = RT_PHONE_RELEASE; return 1; }
    if (!text || !text[0] || text[1] != ' ') return 0;
    if (text[0] == 'm' || text[0] == 'l') {
        double x, y;
        if (sscanf(text + 2, "%lf %lf", &x, &y) != 2 || !isfinite(x) || !isfinite(y)) return 0;
        m->kind = text[0] == 'm' ? RT_PHONE_MOVE : RT_PHONE_LOOK;
        m->x = fmax(-1, fmin(1, x)); m->y = fmax(-1, fmin(1, y));
        return 1;
    }
    if (text[0] == 'b') {
        char name[16]; int down;
        if (sscanf(text + 2, "%15s %d", name, &down) != 2) return 0;
        if (!strcmp(name, "blue")) snprintf(name, sizeof(name), "fire");          // round 23's page
        else if (!strcmp(name, "orange")) snprintf(name, sizeof(name), "alt_fire");
        for (int a = 0; a < RT_TOUCH_ACTION_COUNT; ++a)
            if (!strcmp(name, rt_touch_action_name((rt_touch_action)a))) {
                if (!rt_phonepad_route(layout, a, &m->button, &m->trigger, &m->tap, &m->command)) return 0;
                m->kind = RT_PHONE_BUTTON; m->action = a; m->down = down != 0;
                return 1;
            }
        return 0;
    }
    if (text[0] == 'p') {
        m->kind = RT_PHONE_PING;
        snprintf(m->ping, sizeof(m->ping), "%s", text + 2);
        return m->ping[0] != 0;
    }
    return 0;
}

int16_t rt_phonepad_axis(double value, int is_y) {
    double v = fmax(-1, fmin(1, value)) * (is_y ? -1 : 1);
    return (int16_t)lround(v * 32767);
}

int rt_pad_trigger_axis(int down) { return down ? RT_PAD_TRIGGER_DOWN : RT_PAD_TRIGGER_UP; }
int rt_pad_trigger_seen(int v) { return (int)(((long)v + 32768) * 32767 / 65535); }

void rt_phonepad_pad_rest(rt_phone_pad *p) {
    memset(p, 0, sizeof(*p));
    p->axes[4] = p->axes[5] = RT_PAD_TRIGGER_UP;   // SDL_CONTROLLER_AXIS_TRIGGERLEFT, TRIGGERRIGHT
}
void rt_phonepad_pad_apply(rt_phone_pad *p, const rt_phone_message *m) {
    switch (m->kind) {
    case RT_PHONE_MOVE: p->axes[0] = rt_phonepad_axis(m->x, 0); p->axes[1] = rt_phonepad_axis(m->y, 1); break;
    case RT_PHONE_LOOK: p->axes[2] = rt_phonepad_axis(m->x, 0); p->axes[3] = rt_phonepad_axis(m->y, 1); break;
    case RT_PHONE_BUTTON:
        if (m->button >= 0 && m->button < 32) {
            if (m->down) p->buttons |= 1u << m->button; else p->buttons &= ~(1u << m->button);
        }
        if (m->trigger == 4 || m->trigger == 5) p->axes[m->trigger] = (int16_t)rt_pad_trigger_axis(m->down);
        break;
    case RT_PHONE_RELEASE: rt_phonepad_pad_rest(p); break;
    default: break;
    }
}
int rt_phonepad_pad_held(const rt_phone_pad *p) {
    rt_phone_pad rest;
    rt_phonepad_pad_rest(&rest);
    return memcmp(p, &rest, sizeof(rest)) != 0;
}

// ---- Round 26: the running game's buttons on the phone ----
int rt_phonepad_route(rt_ios_layout layout, int action, int *button, int *trigger, int *tap, const char **command) {
    *button = *trigger = -1; *tap = 0; *command = NULL;
    rt_touch_action a = (rt_touch_action)action;
    if (a == RT_TOUCH_PAUSE) { *button = 6; return 1; }   // Start: gameui_activate, pause
    if (rt_touch_buttons_on_pad(layout)) {                 // Left 4 Dead 2 (touch_layout.c)
        rt_touch_pad_route r = rt_touch_pad_route_for(layout, a);
        *button = r.button; *trigger = r.trigger; *tap = r.tap; *command = r.command;
        return r.button >= 0 || r.trigger >= 0 || r.command;
    }
    if (layout == RT_IOS_LAYOUT_HL2) {   // Half-Life 2's controller binds: A +jump, B toggle_duck, X +use
        switch (a) {
        case RT_TOUCH_JUMP: *button = 0; return 1;
        case RT_TOUCH_CROUCH: *button = 1; *tap = 1; return 1;
        case RT_TOUCH_USE: *button = 2; return 1;
        case RT_TOUCH_FIRE: *trigger = 5; return 1;
        case RT_TOUCH_ALT_FIRE: *trigger = 4; return 1;
        default: return 0;
        }
    }
    *button = rt_touch_pad_button(a);    // Portal 2 and Mel: touch as player 2's mapping
    *trigger = rt_touch_pad_trigger(a);
    return *button >= 0 || *trigger >= 0;
}

int rt_phonepad_layout_message(rt_ios_layout layout, int coop, char *out, size_t size) {
    size_t used = (size_t)snprintf(out, size, "g %s %d ", rt_touch_layout_id(layout), coop ? 1 : 0);
    int n, sent = 0, b, t, tap;
    const char *cmd;
    const rt_touch_button *buttons = rt_touch_layout(layout, &n);
    for (int i = 0; i < n && used < size; ++i) {
        const rt_touch_button *k = &buttons[i];
        if ((k->flags & RT_TOUCH_DATASET) || ((k->flags & RT_TOUCH_COOP) && !coop)) continue;
        if (!rt_phonepad_route(layout, k->action, &b, &t, &tap, &cmd)) continue;
        // Portal 2's portal buttons: once each (the touch layout mirrors them on both sides).
        int dup = 0;
        for (int j = 0; j < i; ++j) dup |= buttons[j].action == k->action;
        if (dup) continue;
        used += (size_t)snprintf(out + used, size - used, "%s%s,%s,%d,%.3f,%.3f,%.1f,%.1f,%.1f,%u", sent ? ";" : "",
                                 rt_touch_action_name(k->action), k->symbol ? k->symbol : "", (int)k->color, k->ax, k->ay,
                                 k->ox, k->oy, k->size, k->flags);
        ++sent;
    }
    return used < size && sent > 0;
}

void rt_phonepad_move_axes(double x, double y, int16_t out[2]) {
    // touch_controls.m: speed from kMinWalk (40 of 175 units/s) to full, past the game's 0.15 threshold.
    const double min_walk = 40.0 / 175, threshold = .15, dead = .08;
    double d = fmin(1, hypot(x, y));
    double v[2] = {x, y};
    for (int i = 0; i < 2; ++i) {
        double c = d > dead ? v[i] / hypot(x, y) * (min_walk + (1 - min_walk) * (d - dead) / (1 - dead)) : 0;
        double s = fmin(1, fabs(c));
        double axis = s < 1e-3 ? 0 : copysign(threshold + (1 - threshold) * s, c);
        out[i] = rt_phonepad_axis(axis, i == 1);
    }
}
