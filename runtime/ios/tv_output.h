// A connected TV (round 20): an HDMI cable through USB-C, or AirPlay to an
// Apple TV. The game draws to the TV at the TV's own shape and resolution; the
// iPad becomes the pad (touch controls only). Settings (gear menu > TV, shared
// by every game): P2_TV_OUTPUT (on by default), P2_TV_RESOLUTION (auto, 1080,
// 1440, 2160) and P2_TV_MAX_FPS (the frame cap while on the TV). The UIKit side
// is tv_output_ios.m; this part is plain C, checked in tests/checks_tv_output.c.
#ifndef RT_TV_OUTPUT_H
#define RT_TV_OUTPUT_H

// Is the TV picture wanted (P2_TV_OUTPUT: anything but "0", "off", "false")?
int rt_tv_enabled(const char *value);

// The game's picture size on a TV whose screen is tv_w x tv_h pixels: the TV's
// own shape, at most `resolution` lines ("1080", "1440", "2160"; "auto" or
// unreadable: the TV's own lines, capped at 2160), never larger than the TV,
// even sizes. 0 when the TV's size is unknown.
int rt_tv_render_size(int tv_w, int tv_h, const char *resolution, int *w, int *h);

// The same for a screen whose pixel grid may not have the shape it shows (an
// anamorphic AirPlay or HDMI mode, round 23): mode_w x mode_h pixels (lines from
// mode_h) showing an area of shape `aspect` (width / height of the screen's
// bounds), so circles stay circles. aspect <= 0: the mode's own shape.
int rt_tv_render_size_shown(int mode_w, int mode_h, double aspect, const char *resolution, int *w, int *h);

// The frame cap on the TV: P2_TV_MAX_FPS (a number, or "auto"/empty for the TV's
// own refresh rate), never above the TV's rate when it is known (> 0).
int rt_tv_frame_cap(const char *value, int tv_hz);

// The engine command that switches the game's picture to w x h, in a window
// (the iOS app has no real fullscreen mode switch): "mat_setvideomode W H 1".
int rt_tv_mode_command(int w, int h, char *out, unsigned long size);

// The touch controls' mode (P2_TOUCH_CONTROLS, touch_layout.h) while the
// picture is on a TV: P2_TV_TOUCH "always" keeps them on the iPad even with a
// controller or keyboard connected ("1"); "auto" or unset leaves `mode` as is
// (hidden while a controller or keyboard is connected, until a touch).
const char *rt_tv_touch_mode(const char *mode, int on_tv, const char *tv_touch);

// P2_TV_SIMULATE="1920x1080" (tests on an iPad without a TV): the iPad's own
// screen stands in for a TV of that size. 1 and the size when set and readable.
int rt_tv_simulated(const char *value, int *w, int *h);

// The UIKit side (tv_output_ios.m), main thread: follow screens and settings,
// a picture size the engine couldn't take yet (each frame), the TV's refresh
// rate while the picture is on it (0 otherwise; power_ios.m caps by it).
void rt_ios_tv_watch(void);
void rt_ios_tv_refresh(void);
void rt_ios_tv_poll(void);
int rt_ios_tv_hz(void);

#endif
