// Settings changed while the game runs (live_settings_ios.m): the in-game menu
// (settings_menu_ios.m) and the Settings app both write the app's preferences
// (NSUserDefaults), the single source of truth. A change sets the setting's
// P2_* variable (settings.c) at once and tells the parts that read it: back
// taps, gyro, frame rate cap and the touch overlay ("P2SettingsChanged").
// C header: nothing from UIKit.
#pragma once
#include <stddef.h>

// Stores a value (as the Settings app would: a switch as a boolean, a slider as
// a number, a choice as a string) and applies it (main thread).
void rt_ios_setting_set(const char *key, const char *value);
// Removes the stored value (the Settings app shows its default again) and
// applies the default.
void rt_ios_setting_reset(const char *key);
// Stored value, else the menu's default ("" if neither).
const char *rt_ios_setting_value(const char *key, char *out, size_t size);
// Applies changes made in the Settings app while the game was in the background.
void rt_ios_settings_watch(void);
