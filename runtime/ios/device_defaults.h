// Defaults per device (runtime/ios/device_defaults.c): resolution scale,
// MetalFX and frame cap picked from the device model and its memory, at first
// launch only and never over a setting the player changed. The table and
// thresholds are in ios/device_defaults.txt (copied into the app). Pure C,
// checked on the Mac by tests/checks_device_defaults.c.
#pragma once
#include <stddef.h>

enum { RT_DD_TIERS = 16, RT_DD_MODELS = 96 };
typedef struct { char name[16], scale[8], upscale[8], fps[8]; } rt_dd_tier;
typedef struct { char id[24], tier[16], name[48]; } rt_dd_model;
typedef struct {
    rt_dd_tier tiers[RT_DD_TIERS];
    rt_dd_model models[RT_DD_MODELS];
    int tier_count, model_count;
    char unknown[16];
    unsigned lowmem_mb;
    char lowmem_args[256], lowmem_metal[256];
} rt_dd_table;

// What this device gets.
typedef struct {
    char model[24], name[48], tier[16];
    char scale[8], upscale[8], fps[8];   // stored values (P2ResolutionScale, P2Upscale, P2MaxFPS)
    int known;                           // the model is in the table (else the "unknown" tier)
    int low_memory;
} rt_dd_choice;

// Parses the file's text; 0 if it has no usable tier.
int rt_dd_parse(const char *text, rt_dd_table *t);
// The device's defaults: exact model, else the longest "prefix*" entry, else
// the unknown tier (the safe middle). screen_max: the screen's refresh rate
// (40 fps on a 60 Hz screen becomes 30). ram_mb: 0 if unknown.
int rt_dd_choose(const rt_dd_table *t, const char *model, unsigned ram_mb, int screen_max, rt_dd_choice *out);

// Preferences: read one (1 if the player or an earlier launch stored it), write one.
typedef int (*rt_dd_get)(void *ctx, const char *key, char *out, size_t size);
typedef void (*rt_dd_set)(void *ctx, const char *key, const char *value);
// Keys this sets, in order: P2ResolutionScale, P2Upscale, P2MaxFPS; the
// marker P2DefaultsFor (the model they were picked for).
extern const char *const rt_dd_keys[3];
#define RT_DD_MARKER "P2DefaultsFor"
// First launch (no marker yet): writes each of the three the player never
// stored, then the marker. Later launches change nothing. Returns how many
// were written.
int rt_dd_first_launch(const rt_dd_choice *c, rt_dd_get get, rt_dd_set set, void *ctx);
// "Use recommended": writes all three (the player asked), and the marker.
void rt_dd_use_recommended(const rt_dd_choice *c, rt_dd_set set, void *ctx);
// The recommended stored value of one of the three keys, or NULL for another key.
const char *rt_dd_value(const rt_dd_choice *c, const char *key);
