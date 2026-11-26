// The Portal app's game picker and "Manage games" screen
// (runtime/ios/game_picker_ios.m; rules in game_picker.c).
#pragma once

// At launch, before anything game-specific (ios/main_ios.c): the game to start.
// P2_GAME_CHOICE=<name> picks it (simulator checks, devicectl); otherwise the
// remembered choice, if "Remember my choice" is on and that game is installed;
// otherwise the picker (UIKit, run until a tap). The choice is remembered for
// "Switch game" and the last-played date.
const char *rt_ios_choose_game(void);
// "Switch game" (in-game settings menu): forget the remembered choice, so the
// picker shows at the next launch.
void rt_ios_forget_game_choice(void);
// Several games in this app (the Portal app with a hosted game).
int rt_ios_has_game_choice(void);
// Round 26: a playable installed game uses this touch layout (the launcher's
// Controls offers its "Edit button layout" row).
#include "game_ios.h"
int rt_ios_layout_installed(rt_ios_layout layout);
