// "Edit button layout" (runtime/ios/touch_editor_ios.m; rules in touch_layout.c).
#pragma once

// A full-screen UIView (retained, __bridge_transfer it); close() on Save or Cancel.
// Round 26: `layout` is the game layout edited (the running game's in a game,
// the one picked in the launcher's Controls).
#include "game_ios.h"
void *rt_ios_touch_editor_create(void (*close)(void *), void *ctx, rt_ios_layout layout);
