// iCloud save sync for Portal 2 (icloud_saves_ios.m): the game's saves and
// their screenshots, between the player's devices, through the app's iCloud
// Drive container. Rules in save_sync_rules.c. Setting "iCloud saves"
// (P2_ICLOUD_SAVES, on by default) in the in-game menu and the Settings app.
// C header: nothing from UIKit.
#pragma once
#include <stddef.h>

// Main thread, once UIKit is up (touch_controls.m): first sync, then after
// each save, on leaving the app and on coming back.
void rt_ios_icloud_start(void);
// P2_ICLOUD_SAVES changed (live_settings_ios.m): starts or stops.
void rt_ios_icloud_reload(void);
// One line for the menu: "Last synced today at 14:05", "Syncing…", "Off",
// or why it can't ("iCloud isn't set up for this app yet").
void rt_ios_icloud_status(char *out, size_t size);
