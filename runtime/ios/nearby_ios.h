// Round 25: Play nearby (nearby_ios.m; rules in nearby.h). C header: nothing from UIKit.
#pragma once

// Words for the game's command line once Host or Join was picked in the
// launcher ("+sv_lan 1 +map mp_coop_lobby_3", "+connect 192.168.1.21:27015"), or NULL.
const char *rt_ios_nearby_launch_args(void);
// Main thread, every frame (the settings menu's tick): the session's messages.
void rt_ios_nearby_tick(void);
// Browse for hosts on or off (the Join list).
void rt_ios_nearby_browse(int on);
// The name this device advertises under while hosting ("" before).
const char *rt_ios_nearby_own_name(void);
#ifdef __OBJC__
@class UIView;
// The Play nearby panel: Host, Join and the list. `close`: its X; `started`:
// Host or a host was tapped (the launcher then starts Portal 2; the gear menu closes).
UIView *rt_ios_nearby_panel(void (^close)(void), void (^started)(void));
#endif
