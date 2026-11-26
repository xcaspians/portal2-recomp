// In-game settings menu (settings_menu_ios.m): a gear in the game's pause
// menu opens a panel drawn over the game, with the touch, back-tap, camera and
// frame rate settings applied live (live_settings.h). Touch and controller.
// C header: nothing from UIKit.
#pragma once

// Called by the touch overlay (touch_controls.m), main thread: its view goes
// inside the overlay (kept in front of SDL's views), then one tick per frame.
void *rt_ios_settings_menu_create(void);   // UIView, retained by the overlay
void rt_ios_settings_menu_tick(void *menu);
// The panel is open, or a controller button pressed to close it is still
// held: the game gets no controller press or stick motion (sdl_ios.m).
int rt_ios_settings_menu_captures_pad(void);
// The panel is open: the overlay draws its buttons behind it (live preview).
int rt_ios_settings_menu_open(void);

// From game_state.c: game UI visible with a level loaded (the pause menu),
// and commands for the engine (Portal 2; 0 when unavailable).
int rt_ios_game_paused(void);
// The main menu or the pause menu is up (game_state.c): the gear shows.
int rt_ios_game_menu_shown(void);
int rt_ios_game_in_level(void);   // game_state.c: a level is loaded
const char *rt_ios_level_map(void);   // game_state.c: the loaded map ("" outside a level)
// The game's own Community Test Chambers dialog opened since the last call (game_state.c).
int rt_ios_game_take_community_request(void);
int rt_ios_game_commands_available(void);
int rt_ios_game_command(const char *line);

#ifdef __OBJC__
@class UIView;
// Round 25: the same panel from Sauce's launcher (no game running): the settings
// every game shares. `close` runs once it is closed for good.
UIView *rt_ios_settings_menu_launcher(void (^close)(void));
#endif
