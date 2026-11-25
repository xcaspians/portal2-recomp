// Defaults per device (runtime/ios/device_defaults.c), read from the real
// ios/device_defaults.txt: the table lookup, the fallback for an unknown model,
// and never writing over a setting the player changed.
#include "checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../runtime/ios/device_defaults.c"
#include "../runtime/ios/settings_menu.h"

static rt_dd_table *table(void) {
    static rt_dd_table t;
    static int loaded;
    if (loaded) return &t;
    FILE *f = fopen("ios/device_defaults.txt", "r");
    assert(f);
    static char text[16384];
    size_t n = fread(text, 1, sizeof(text) - 1, f);
    fclose(f);
    text[n] = 0;
    assert(n < sizeof(text) - 1);
    assert(rt_dd_parse(text, &t));
    loaded = 1;
    return &t;
}

// Fake preferences.
typedef struct { char key[8][32], value[8][32]; int n; } Prefs;
static int pget(void *ctx, const char *key, char *out, size_t size) {
    Prefs *p = ctx;
    for (int i = 0; i < p->n; ++i) if (!strcmp(p->key[i], key)) { snprintf(out, size, "%s", p->value[i]); return 1; }
    return 0;
}
static void pset(void *ctx, const char *key, const char *value) {
    Prefs *p = ctx;
    for (int i = 0; i < p->n; ++i) if (!strcmp(p->key[i], key)) { snprintf(p->value[i], 32, "%s", value); return; }
    assert(p->n < 8);
    snprintf(p->key[p->n], 32, "%s", key); snprintf(p->value[p->n], 32, "%s", value); ++p->n;
}
static const char *pv(Prefs *p, const char *key) {
    static char out[32];
    return pget(p, key, out, sizeof(out)) ? out : NULL;
}

RT_CHECK(ios_device_defaults_table_lookup) {
    rt_dd_choice c;
    // iPhone 13, 4 GB, 60 Hz
    assert(rt_dd_choose(table(), "iPhone14,5", 3700, 60, &c) && c.known);
    assert(!strcmp(c.name, "iPhone 13") && !strcmp(c.tier, "a15"));
    assert(!strcmp(c.scale, "1.5") && !strcmp(c.upscale, "0.75") && !strcmp(c.fps, "30") && c.low_memory);
    // iPhone 13 Pro, 6 GB, ProMotion: 40 fps; iPhone 14 (same tier, 60 Hz): 30
    assert(rt_dd_choose(table(), "iPhone14,2", 5700, 120, &c) && !strcmp(c.fps, "40") && !c.low_memory);
    assert(rt_dd_choose(table(), "iPhone14,7", 5700, 60, &c) && !strcmp(c.fps, "30"));
    // A16, A17 Pro (upstream's iPhone 15 Pro Max: today's defaults, unchanged)
    assert(rt_dd_choose(table(), "iPhone15,4", 5700, 60, &c) && !strcmp(c.scale, "2") && !strcmp(c.upscale, "0.75"));
    assert(rt_dd_choose(table(), "iPhone16,2", 7600, 120, &c));
    assert(!strcmp(c.scale, "2") && !strcmp(c.upscale, "1") && !strcmp(c.fps, "60"));
    // Caspian's devices; a family entry; the exact entry wins over it
    assert(rt_dd_choose(table(), "iPhone19,2", 11000, 120, &c) && !strcmp(c.name, "iPhone 18 Pro") && !strcmp(c.tier, "strong"));
    assert(rt_dd_choose(table(), "iPad19,1", 11000, 120, &c) && !strcmp(c.name, "iPad mini (A20 Pro)"));
    assert(rt_dd_choose(table(), "iPhone19,1", 11000, 120, &c) && c.known && !strcmp(c.name, "iPhone 18 family"));
    // Never Auto by default (round 1 rule: not measured yet)
    for (int i = 0; i < table()->tier_count; ++i) assert(strcmp(table()->tiers[i].fps, "auto"));
    // Every value is one the settings page offers
    for (int i = 0; i < table()->tier_count; ++i) {
        const rt_dd_tier *t = &table()->tiers[i];
        const rt_menu_item *fps = rt_menu_item_named("P2MaxFPS");
        int ok = 0;
        for (int k = 0; fps->values[k]; ++k) ok |= !strcmp(fps->values[k], t->fps);
        assert(ok);
    }
}

RT_CHECK(ios_device_defaults_unknown_model_gets_the_safe_middle) {
    rt_dd_choice c;
    assert(rt_dd_choose(table(), "iPhone99,9", 0, 120, &c) && !c.known);
    assert(!strcmp(c.tier, "a16") && !strcmp(c.name, "iPhone99,9") && !c.low_memory);   // ram unknown: no guard
    assert(rt_dd_choose(table(), "", 3500, 60, &c) && !strcmp(c.model, "unknown") && c.low_memory);
    assert(rt_dd_choose(table(), "arm64", 16000, 60, &c) && !strcmp(c.tier, "a16"));   // a Mac's hw.machine
    // A table whose model points at a missing tier, without an unknown line
    rt_dd_table t;
    assert(rt_dd_parse("tier lo scale=1 upscale=0.75 fps=30\ntier mid scale=2 upscale=0.75 fps=60\n"
                       "tier hi scale=2 upscale=1 fps=60\nmodel X1 nope X\n", &t));
    assert(rt_dd_choose(&t, "X1", 0, 60, &c) && !strcmp(c.tier, "mid"));
    // Values outside the settings' choices are refused: such a tier is dropped
    assert(rt_dd_parse("tier bad scale=3 upscale=0.75 fps=30\ntier ok scale=1 upscale=1 fps=60\n", &t) && t.tier_count == 1);
    assert(!rt_dd_parse("# nothing\n", &t));
}

RT_CHECK(ios_device_defaults_never_override_a_changed_setting) {
    rt_dd_choice c;
    assert(rt_dd_choose(table(), "iPhone14,5", 3700, 60, &c));
    // First launch, the player already chose a frame rate (in the Settings app)
    Prefs p = {0};
    pset(&p, "P2MaxFPS", "60");
    assert(rt_dd_first_launch(&c, pget, pset, &p) == 2);
    assert(!strcmp(pv(&p, "P2ResolutionScale"), "1.5") && !strcmp(pv(&p, "P2Upscale"), "0.75"));
    assert(!strcmp(pv(&p, "P2MaxFPS"), "60"));                   // kept
    assert(!strcmp(pv(&p, RT_DD_MARKER), "iPhone14,5"));
    // Later launches change nothing, even what the player set back to anything
    pset(&p, "P2ResolutionScale", "native");
    assert(rt_dd_first_launch(&c, pget, pset, &p) == 0 && !strcmp(pv(&p, "P2ResolutionScale"), "native"));
    // "Use recommended": all three, because the player asked
    rt_dd_use_recommended(&c, pset, &p);
    assert(!strcmp(pv(&p, "P2ResolutionScale"), "1.5") && !strcmp(pv(&p, "P2MaxFPS"), "30"));
    assert(rt_dd_value(&c, "P2Upscale") == c.upscale && rt_dd_value(&c, "P2Gyro") == NULL);
}
