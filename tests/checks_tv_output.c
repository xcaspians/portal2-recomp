// A connected TV's picture (runtime/ios/tv_output.c, round 20): size, cap,
// engine command and the iPad-only stand-in used for tests.
#include "checks.h"
#include <math.h>
#include <string.h>
#include "../runtime/ios/tv_output.c"

RT_CHECK(tv_output_sizes_follow_the_tv) {
    int w = 0, h = 0;
    // A 4K TV over a cable: Auto renders at 4K, the setting can lower it.
    assert(rt_tv_render_size(3840, 2160, "auto", &w, &h) && w == 3840 && h == 2160);
    assert(rt_tv_render_size(3840, 2160, "1080", &w, &h) && w == 1920 && h == 1080);
    assert(rt_tv_render_size(3840, 2160, "1440", &w, &h) && w == 2560 && h == 1440);
    // AirPlay at 1080p: 4K asked, the TV's own lines given (never upscaled).
    assert(rt_tv_render_size(1920, 1080, "2160", &w, &h) && w == 1920 && h == 1080);
    // A wider-than-16:9 screen keeps its shape; sizes stay even.
    assert(rt_tv_render_size(2560, 1080, "auto", &w, &h) && w == 2560 && h == 1080);
    assert(rt_tv_render_size(1366, 768, "auto", &w, &h) && h == 768 && w == 1366);
    assert(rt_tv_render_size(1280, 721, "auto", &w, &h) && !(w & 1) && !(h & 1));
    // An 8K screen: Auto stops at 2160 lines.
    assert(rt_tv_render_size(7680, 4320, NULL, &w, &h) && w == 3840 && h == 2160);
    // Unknown size, or a nonsense setting (treated as Auto).
    assert(!rt_tv_render_size(0, 1080, "auto", &w, &h));
    assert(rt_tv_render_size(1920, 1080, "720", &w, &h) && h == 1080);
}

RT_CHECK(tv_output_switch_cap_and_command) {
    assert(rt_tv_enabled(NULL) && rt_tv_enabled("1") && !rt_tv_enabled("0") && !rt_tv_enabled("off"));
    assert(rt_tv_frame_cap(NULL, 60) == 60 && rt_tv_frame_cap("auto", 120) == 120);
    assert(rt_tv_frame_cap("30", 60) == 30 && rt_tv_frame_cap("120", 60) == 60);   // never above the TV
    assert(rt_tv_frame_cap("40", 0) == 40 && rt_tv_frame_cap(NULL, 0) == 60);     // TV rate unknown
    char line[64];
    assert(rt_tv_mode_command(1920, 1080, line, sizeof(line)) && !strcmp(line, "mat_setvideomode 1920 1080 1"));
    assert(!rt_tv_mode_command(0, 1080, line, sizeof(line)) && !rt_tv_mode_command(1920, 1080, line, 8));
    int w = 0, h = 0;
    assert(rt_tv_simulated("1920x1080", &w, &h) && w == 1920 && h == 1080);
    assert(!rt_tv_simulated("1920", &w, &h) && !rt_tv_simulated("1920x1080x2", &w, &h) && !rt_tv_simulated("", &w, &h));
}

// Round 23: gear menu > TV > Touch controls. Auto keeps today's rule (hidden
// with a controller or keyboard until a touch); Always on shows them on the
// iPad whatever is connected, only while the picture is on a TV.
RT_CHECK(tv_output_touch_controls_setting) {
    assert(!strcmp(rt_tv_touch_mode(NULL, 1, "always"), "1"));
    assert(!strcmp(rt_tv_touch_mode("auto", 1, "always"), "1"));
    assert(!rt_tv_touch_mode(NULL, 1, "auto") && !rt_tv_touch_mode(NULL, 1, NULL));
    assert(!rt_tv_touch_mode(NULL, 0, "always"));                        // not on a TV: as before
    assert(!strcmp(rt_tv_touch_mode("pad", 0, "always"), "pad"));
}

// Round 23: the picture takes the shape the TV shows (its bounds), not its
// pixel grid's: an anamorphic mode would stretch it (the AirPlay hand test).
RT_CHECK(tv_output_shape_follows_what_is_shown) {
    int w = 0, h = 0;
    assert(rt_tv_render_size_shown(1920, 1080, 16.0 / 9, "auto", &w, &h) && w == 1920 && h == 1080);   // square pixels
    assert(rt_tv_render_size_shown(1440, 1080, 16.0 / 9, "auto", &w, &h) && w == 1920 && h == 1080);   // anamorphic
    assert(rt_tv_render_size_shown(3840, 2160, 16.0 / 9, "1080", &w, &h) && w == 1920 && h == 1080);
    assert(rt_tv_render_size_shown(1920, 1080, 0, "auto", &w, &h) && w == 1920 && h == 1080);          // shape unknown
    assert(fabs((double)w / h - 16.0 / 9) < .01);
    assert(!rt_tv_render_size_shown(0, 1080, 1.7, "auto", &w, &h));
}
