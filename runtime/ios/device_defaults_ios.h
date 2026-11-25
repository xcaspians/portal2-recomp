// Defaults per device on iOS (device_defaults_ios.m; rules in device_defaults.c).
#pragma once
#include <stddef.h>
#include "device_defaults.h"

// This device's recommended defaults (NULL: no table in the app).
const rt_dd_choice *rt_ios_device_choice(void);
// At launch, before the settings are read (ios/main_ios.c): first launch only,
// writes the defaults the player never set; summary for the log.
int rt_ios_device_defaults_first_launch(char *summary, size_t size);
// "Use recommended" (in-game menu): the three settings again, now.
void rt_ios_device_defaults_use_recommended(void);
// The recommended stored value of P2ResolutionScale, P2Upscale or P2MaxFPS
// (Reset to defaults goes back to it), NULL for another key.
const char *rt_ios_device_recommended(const char *key);
// Low-memory guard (lowmem in device_defaults.txt; P2_NO_LOWMEM=1 turns it off):
// on, and its launch arguments ("+mat_picmip 1") and mtld3d keys ("" when off).
int rt_ios_device_low_memory(void);
const char *rt_ios_device_low_memory_args(void);
const char *rt_ios_device_low_memory_metal(void);
