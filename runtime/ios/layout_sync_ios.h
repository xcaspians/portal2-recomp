// Round 26: custom button layouts through iCloud, per device kind (layout_sync_ios.m).
#pragma once
#include "game_ios.h"

// A sync now, off the main thread: at launch (game or launcher), on coming back, after a save.
void rt_ios_layouts_sync(const char *why);
// The editor saved this game layout: its time is stamped, then a sync.
void rt_ios_layouts_saved(rt_ios_layout layout);
// One plain sentence for the editor: shared through iCloud, or saved on this device only and why.
const char *rt_ios_layouts_sync_status(void);
