// Community Maps browser (workshop_ios.m), opened from the in-game settings
// menu (settings_menu_ios.m). C header: nothing from UIKit.
#pragma once

// A UIView (retained by the caller) covering the settings panel. on_close runs
// when the player backs out of it (all = 0: back to the settings) or starts a
// map (all = 1: the whole menu closes so the map loads).
void *rt_ios_community_create(void (*on_close)(void *ctx, int all), void *ctx);
// Controller, once per frame while it is shown: d-pad or stick steps
// (dx, dy: -1, 0, +1, already repeated), A and B pressed this frame.
void rt_ios_community_pad(void *view, int dx, int dy, int a, int b);
// Opens the details of one item at once (simulator checks:
// P2_COMMUNITY_TEST=<id> downloads it and starts it).
void rt_ios_community_open_item(void *view, unsigned long long id, int download_and_play);
// Where downloaded maps go: <user folder>/portal2/maps/workshop; leftovers
// of canceled downloads are removed (rt_ws_cleanup).
void rt_ios_community_cleanup(void);
