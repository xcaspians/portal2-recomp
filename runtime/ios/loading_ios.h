// Sauce's setting-up screen (round 31): shown while a game starts, never pure black.
#pragma once

// Over everything from the moment the game is chosen until the engine has
// presented its first frames (fps_overlay_presented), then it fades out by
// itself. Its spinner is a Core Animation rotation, so it keeps turning while
// the engine's start blocks the main thread. Nothing when P2_LOADING_SCREEN=0.
void rt_ios_loading_show(const char *game_name, const char *game_title);
// Changes its line ("Setting up Left 4 Dead 2…"); on screen at once.
void rt_ios_loading_say(const char *line);
