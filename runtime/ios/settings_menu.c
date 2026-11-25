// The in-game settings menu's items (see settings_menu.h).
#include "settings_menu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const portal_values[] = {"auto", "1", "0", NULL};
static const char *const portal_labels[] = {"Auto", "Always", "Never", NULL};
static const char *const fps_values[] = {"30", "40", "60", "120", "auto", NULL};
static const char *const fps_labels[] = {"30", "40", "60", "120", "Auto", NULL};
static const char *const look_values[] = {"0.5", "0.75", "1", "1.5", "2", "3", NULL};
static const char *const look_labels[] = {"×0.5", "×0.75", "Normal", "×1.5", "×2", "×3", NULL};
static const char *const gyro_values[] = {"0.5", "0.75", "1", "1.5", "2", NULL};
static const char *const gyro_labels[] = {"×0.5", "×0.75", "Normal", "×1.5", "×2", NULL};

const rt_menu_item rt_menu_items[] = {
    {"P2TouchOpacity", "On-screen buttons", "Button opacity", RT_MENU_SLIDER, "1", 0, 1, .05, 100, "%.0f %%", NULL, NULL, 0},
    {"P2TouchSize", NULL, "Button size", RT_MENU_SLIDER, "1", .75, 1.5, .05, 100, "%.0f %%", NULL, NULL, 0},
    {"P2PortalButtons", NULL, "Show portal buttons", RT_MENU_CHOICE, "auto", 0, 0, 0, 0, NULL, portal_values, portal_labels, 0},
    // Same switch as the Settings app's (P2BackTap): off brings the portal buttons back.
    {"P2BackTap", "Back taps", "Back taps", RT_MENU_TOGGLE, "1", 0, 0, 0, 0, NULL, NULL, NULL, 0},
    {"P2BackTapSensitivity", NULL, "Tap sensitivity", RT_MENU_SLIDER, "1", .5, 3, .05, 1, "%.2f", NULL, NULL, 0},
    {"P2BackTapInvert", NULL, "Swap left and right", RT_MENU_TOGGLE, "0", 0, 0, 0, 0, NULL, NULL, NULL, 0},
    {"P2Haptics", NULL, "Vibrate when firing", RT_MENU_TOGGLE, "1", 0, 0, 0, 0, NULL, NULL, NULL, 0},
    {"P2TouchLook", "Camera", "Camera speed", RT_MENU_CHOICE, "1", 0, 0, 0, 0, NULL, look_values, look_labels, 0},
    {"P2Gyro", NULL, "Gyro aiming", RT_MENU_TOGGLE, "0", 0, 0, 0, 0, NULL, NULL, NULL, 0},
    {"P2GyroSpeed", NULL, "Gyro speed", RT_MENU_CHOICE, "1", 0, 0, 0, 0, NULL, gyro_values, gyro_labels, 0},
    // Applies at once, Metal too (round 11: metal_ios.m paces under mtld3d's launch cap).
    {"P2MaxFPS", "Display", "Frames per second", RT_MENU_CHOICE, "60", 0, 0, 0, 0, NULL, fps_values, fps_labels, 0},
    // Corner readout: frames per second, the display's refresh rate, the cap asked for.
    {"P2ShowFPS", NULL, "FPS counter", RT_MENU_TOGGLE, "0", 0, 0, 0, 0, NULL, NULL, NULL, 0},
    {"P2ICloudSaves", "iCloud", "iCloud saves", RT_MENU_TOGGLE, "1", 0, 0, 0, 0, NULL, NULL, NULL, 0},
};
const int rt_menu_item_count = (int)(sizeof(rt_menu_items) / sizeof(*rt_menu_items));

const rt_menu_item *rt_menu_item_named(const char *key) {
    for (int i = 0; key && i < rt_menu_item_count; ++i)
        if (!strcmp(rt_menu_items[i].key, key)) return &rt_menu_items[i];
    return NULL;
}

static double snap(const rt_menu_item *it, double v) {
    if (!(v >= it->min)) v = it->min;   // NaN too
    if (v > it->max) v = it->max;
    double steps = round((v - it->min) / it->step);
    v = it->min + steps * it->step;
    return v > it->max ? it->max : v;
}
static int parse(const char *value, double *out) {
    char *end = NULL;
    if (!value || !*value) return 0;
    *out = strtod(value, &end);
    return end && !*end && isfinite(*out);
}
double rt_menu_slider_value(const rt_menu_item *it, const char *value) {
    double v;
    if (!parse(value, &v) && !parse(it->def, &v)) v = it->min;
    return snap(it, v);
}
static int choice_index(const rt_menu_item *it, const char *value) {
    int def = 0;
    for (int i = 0; it->values[i]; ++i) {
        if (value && !strcmp(it->values[i], value)) return i;
        if (!strcmp(it->values[i], it->def)) def = i;
    }
    return def;
}
static int switch_on(const rt_menu_item *it, const char *value) {
    if (!value || !*value) value = it->def;
    return strcmp(value, "0") && strcmp(value, "false");
}

const char *rt_menu_step(const rt_menu_item *it, const char *value, int dir, char *out, size_t size) {
    switch (it->kind) {
    case RT_MENU_SLIDER: {
        double v = snap(it, rt_menu_slider_value(it, value) + dir * it->step);
        snprintf(out, size, "%g", round(v * 1000) / 1000);   // 0.35, not 0.35000000000000003
        break;
    }
    case RT_MENU_TOGGLE:
        snprintf(out, size, "%d", switch_on(it, value) != (dir != 0));
        break;
    case RT_MENU_CHOICE: {
        int i = choice_index(it, value) + dir, n = 0;
        while (it->values[n]) ++n;
        if (i < 0) i = 0;
        if (i >= n) i = n - 1;
        snprintf(out, size, "%s", it->values[i]);
        break;
    }
    }
    return out;
}

void rt_menu_display(const rt_menu_item *it, const char *value, char *out, size_t size) {
    switch (it->kind) {
    case RT_MENU_SLIDER: snprintf(out, size, it->format, rt_menu_slider_value(it, value) * it->display_factor); break;
    case RT_MENU_TOGGLE: snprintf(out, size, "%s", switch_on(it, value) ? "On" : "Off"); break;
    case RT_MENU_CHOICE: snprintf(out, size, "%s", it->labels[choice_index(it, value)]); break;
    }
}

int rt_menu_pad_dropped(rt_pad_event kind, int options_button, int value, int paused, int captured) {
    if (paused && options_button && (kind == RT_PAD_BUTTON_DOWN || kind == RT_PAD_BUTTON_UP)) return 1;
    if (!captured) return 0;
    switch (kind) {
    case RT_PAD_AXIS: return value > 6000 || value < -6000;   // under the game's dead zone: a return to centre
    case RT_PAD_BUTTON_DOWN: case RT_PAD_OTHER_MOTION: return 1;
    default: return 0;
    }
}

int rt_menu_item_for_game(const rt_menu_item *it, int back_taps) {
    if (back_taps) return 1;
    return strncmp(it->key, "P2BackTap", 9) && strcmp(it->key, "P2PortalButtons") && strcmp(it->key, "P2Haptics");
}
const char *rt_menu_item_default(const rt_menu_item *it, const char *game) {
    if (!strcmp(it->key, "P2Gyro") && game && !strcmp(game, "hl2")) return "1";
    return it->def;
}
