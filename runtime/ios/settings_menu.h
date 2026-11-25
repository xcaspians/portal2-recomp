// The in-game settings menu (settings_menu_ios.m): what it shows and how a
// controller steps each value. Pure C, checked on the Mac by
// tests/checks_ios_settings.c against ios/Settings.bundle, so both pages
// offer the same values with the same defaults. Values are the strings stored
// in the app's preferences (NSUserDefaults), the single source of truth.
#pragma once
#include <stddef.h>

typedef enum { RT_MENU_SLIDER, RT_MENU_TOGGLE, RT_MENU_CHOICE } rt_menu_kind;
enum {
    RT_MENU_RELAUNCH_ON_METAL = 1,   // Metal reads it once at start: "Applies on next launch"
};

typedef struct {
    const char *key;        // preference key (rt_ios_settings, Settings.bundle)
    const char *section;    // heading shown above the first item of a section
    const char *title;
    rt_menu_kind kind;
    const char *def;        // default, as stored ("1", "0.5", "auto")
    double min, max, step;  // slider
    double display_factor;  // slider: shown value = value × factor, printed with format
    const char *format;
    const char *const *values, *const *labels;   // choice: NULL-terminated, same order
    unsigned flags;
} rt_menu_item;

extern const rt_menu_item rt_menu_items[];
extern const int rt_menu_item_count;

const rt_menu_item *rt_menu_item_named(const char *key);
// A slider's number for this stored value (the default if unreadable),
// clamped and snapped to its step.
double rt_menu_slider_value(const rt_menu_item *it, const char *value);
// The value one step left (dir -1) or right (+1) of this one, as stored; dir 0
// normalizes it. Sliders clamp, choices stop at both ends, switches flip on
// any dir. Unreadable or unknown values start from the default.
const char *rt_menu_step(const rt_menu_item *it, const char *value, int dir, char *out, size_t size);
// Text shown next to the title: "75 %", "×1.25", "Auto", "On".
void rt_menu_display(const rt_menu_item *it, const char *value, char *out, size_t size);

// Controller events kept from the game around the menu (sdl_ios.m). kind:
// what the event is; value: an axis's position (-32768..32767). In the game's
// pause menu the options button belongs to our menu; while the menu holds the
// controller, presses and deflections stay away from the game, but releases
// and a stick's return to centre go through, so nothing stays held.
typedef enum { RT_PAD_BUTTON_DOWN, RT_PAD_BUTTON_UP, RT_PAD_AXIS, RT_PAD_OTHER_MOTION, RT_PAD_NOT_PAD } rt_pad_event;
int rt_menu_pad_dropped(rt_pad_event kind, int options_button, int value, int paused, int captured);
