// Defaults per device (see device_defaults.h).
#include "device_defaults.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *const rt_dd_keys[3] = {"P2ResolutionScale", "P2Upscale", "P2MaxFPS"};

static const char *skip_space(const char *p) { while (*p == ' ' || *p == '\t') ++p; return p; }
// Next word of the line into out (at most size - 1 characters); returns where it stopped.
static const char *word(const char *p, char *out, size_t size) {
    p = skip_space(p);
    size_t n = 0;
    while (*p && *p != ' ' && *p != '\t' && *p != '\n') { if (n + 1 < size) out[n++] = *p; ++p; }
    out[n] = 0;
    return p;
}
static int one_of(const char *v, const char *const *allowed) {
    for (; *allowed; ++allowed) if (!strcmp(v, *allowed)) return 1;
    return 0;
}
static const char *const scales[] = {"native", "2", "1.5", "1", NULL};
static const char *const upscales[] = {"1", "0.75", "0.5", NULL};
static const char *const rates[] = {"30", "40", "60", "120", "auto", NULL};

static const rt_dd_tier *tier_named(const rt_dd_table *t, const char *name) {
    for (int i = 0; i < t->tier_count; ++i) if (!strcmp(t->tiers[i].name, name)) return &t->tiers[i];
    return NULL;
}

int rt_dd_parse(const char *text, rt_dd_table *t) {
    memset(t, 0, sizeof(*t));
    for (const char *line = text; line && *line; line = strchr(line, '\n') ? strchr(line, '\n') + 1 : NULL) {
        char kind[16], a[256], b[256];
        const char *p = word(line, kind, sizeof(kind));
        if (!kind[0] || kind[0] == '#') continue;
        const char *end = strchr(p, '\n');
        size_t rest_len = end ? (size_t)(end - p) : strlen(p);
        char rest[256];
        snprintf(rest, sizeof(rest), "%.*s", (int)rest_len, p);
        const char *r = skip_space(rest);
        if (!strcmp(kind, "tier") && t->tier_count < RT_DD_TIERS) {
            rt_dd_tier tier = {0};
            const char *q = word(r, tier.name, sizeof(tier.name));
            while (*(q = skip_space(q))) {
                q = word(q, a, sizeof(a));
                char *eq = strchr(a, '=');
                if (!eq) continue;
                *eq = 0;
                const char *v = eq + 1;
                if (!strcmp(a, "scale") && one_of(v, scales)) snprintf(tier.scale, sizeof(tier.scale), "%s", v);
                if (!strcmp(a, "upscale") && one_of(v, upscales)) snprintf(tier.upscale, sizeof(tier.upscale), "%s", v);
                if (!strcmp(a, "fps") && one_of(v, rates)) snprintf(tier.fps, sizeof(tier.fps), "%s", v);
            }
            if (tier.name[0] && tier.scale[0] && tier.upscale[0] && tier.fps[0]) t->tiers[t->tier_count++] = tier;
        } else if (!strcmp(kind, "model") && t->model_count < RT_DD_MODELS) {
            rt_dd_model m = {0};
            const char *q = word(r, m.id, sizeof(m.id));
            q = word(q, m.tier, sizeof(m.tier));
            snprintf(m.name, sizeof(m.name), "%s", skip_space(q));
            size_t n = strlen(m.name);
            while (n && (m.name[n - 1] == ' ' || m.name[n - 1] == '\r')) m.name[--n] = 0;
            if (m.id[0] && m.tier[0]) t->models[t->model_count++] = m;
        } else if (!strcmp(kind, "unknown")) {
            word(r, t->unknown, sizeof(t->unknown));
        } else if (!strcmp(kind, "lowmem")) {
            word(r, b, sizeof(b));
            t->lowmem_mb = (unsigned)strtoul(b, NULL, 10);
        } else if (!strcmp(kind, "lowmem-args")) {
            snprintf(t->lowmem_args, sizeof(t->lowmem_args), "%s", r);
        } else if (!strcmp(kind, "lowmem-metal")) {
            snprintf(t->lowmem_metal, sizeof(t->lowmem_metal), "%s", r);
        }
    }
    // Models pointing at a missing tier fall back to the unknown tier at lookup.
    return t->tier_count > 0;
}

int rt_dd_choose(const rt_dd_table *t, const char *model, unsigned ram_mb, int screen_max, rt_dd_choice *out) {
    memset(out, 0, sizeof(*out));
    snprintf(out->model, sizeof(out->model), "%s", model && *model ? model : "unknown");
    const rt_dd_model *found = NULL;
    size_t best = 0;
    for (int i = 0; i < t->model_count; ++i) {
        const rt_dd_model *m = &t->models[i];
        size_t n = strlen(m->id);
        if (!strcmp(m->id, out->model) && tier_named(t, m->tier)) { found = m; break; }
        if (n > 1 && m->id[n - 1] == '*' && !strncmp(m->id, out->model, n - 1) && n > best && tier_named(t, m->tier)) {
            found = m; best = n;
        }
    }
    const rt_dd_tier *tier = found ? tier_named(t, found->tier) : NULL;
    if (!tier) tier = tier_named(t, t->unknown);
    if (!tier && t->tier_count) tier = &t->tiers[t->tier_count / 2];   // no unknown line: the middle one
    if (!tier) return 0;
    out->known = found != NULL;
    snprintf(out->name, sizeof(out->name), "%s", found && found->name[0] ? found->name : out->model);
    snprintf(out->tier, sizeof(out->tier), "%s", tier->name);
    snprintf(out->scale, sizeof(out->scale), "%s", tier->scale);
    snprintf(out->upscale, sizeof(out->upscale), "%s", tier->upscale);
    snprintf(out->fps, sizeof(out->fps), "%s", tier->fps);
    if (!strcmp(out->fps, "40") && screen_max < 120) snprintf(out->fps, sizeof(out->fps), "30");
    out->low_memory = ram_mb && t->lowmem_mb && ram_mb <= t->lowmem_mb;
    return 1;
}

const char *rt_dd_value(const rt_dd_choice *c, const char *key) {
    if (!key) return NULL;
    if (!strcmp(key, rt_dd_keys[0])) return c->scale;
    if (!strcmp(key, rt_dd_keys[1])) return c->upscale;
    if (!strcmp(key, rt_dd_keys[2])) return c->fps;
    return NULL;
}

int rt_dd_first_launch(const rt_dd_choice *c, rt_dd_get get, rt_dd_set set, void *ctx) {
    char value[64];
    if (get(ctx, RT_DD_MARKER, value, sizeof(value))) return 0;   // picked before: the player's now
    int written = 0;
    for (int i = 0; i < 3; ++i) {
        if (get(ctx, rt_dd_keys[i], value, sizeof(value))) continue;   // the player chose it
        set(ctx, rt_dd_keys[i], rt_dd_value(c, rt_dd_keys[i]));
        ++written;
    }
    set(ctx, RT_DD_MARKER, c->model);
    return written;
}

void rt_dd_use_recommended(const rt_dd_choice *c, rt_dd_set set, void *ctx) {
    for (int i = 0; i < 3; ++i) set(ctx, rt_dd_keys[i], rt_dd_value(c, rt_dd_keys[i]));
    set(ctx, RT_DD_MARKER, c->model);
}
