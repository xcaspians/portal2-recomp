// A connected TV draws the game (round 20, tv_output.h). When an external
// screen appears (HDMI through USB-C, or AirPlay to an Apple TV) and the
// setting allows it, the game's Metal view moves into a window on that screen,
// its drawable takes the TV's size (rt_tv_render_size) and the engine switches
// its picture to that size (mat_setvideomode, windowed). The iPad keeps its own
// window with the touch controls over black: the pad view; with a controller
// in use it also shows a dim "Playing on TV". Unplugging, or turning the
// setting off, brings everything back, mid-game too.
//
// P2_TV_SIMULATE=1920x1080 (tests on an iPad without a TV): the iPad's own
// screen stands in for the TV, a 16:9 window across its top. With
// P2_TV_SIMULATE_CYCLE=N the stand-in is unplugged and plugged back every N
// seconds (hot-plug both ways, mid-game).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <GameController/GameController.h>
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include "power_ios.h"
#include "settings.h"
#include "settings_menu_ios.h"
#include "tv_output.h"

void *rt_ios_metal_view(void);   // metal_ios.m: the game's Metal view, once created
void rt_ios_view_set_scale(void *uiview, float wanted);   // sdl_ios.m
float rt_ios_render_scale(void);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"   // UIScreen.screens: no UIScene lifecycle here

static UIWindow *tv_window;          // on the TV (or across the iPad's top, simulated)
static __weak UIView *home;          // the Metal view's place on the iPad
static NSUInteger home_index;
static UILabel *pad_note;            // "Playing on TV", controller in use
static int tv_w, tv_h, tv_hz;        // the picture size given to the game, the TV's rate
static int mode_pending;             // a mat_setvideomode still to send (engine not ready)
static char mode_line[64];
static int simulated_off;            // P2_TV_SIMULATE_CYCLE: the stand-in unplugged for now

int rt_ios_tv_hz(void) { return tv_window ? (tv_hz > 0 ? tv_hz : 60) : 0; }

static UIScreen *external_screen(void) {
    for (UIScreen *s in UIScreen.screens)
        if (s != UIScreen.mainScreen) return s;
    return nil;
}

// Sends the picture size to the engine now, or as soon as it takes commands.
static void send_mode(int w, int h) {
    if (!rt_tv_mode_command(w, h, mode_line, sizeof(mode_line))) return;
    mode_pending = !rt_ios_game_commands_available() || !rt_ios_game_command(mode_line);
    fprintf(stderr, "[ios] TV : %s%s\n", mode_line, mode_pending ? " (en attente du moteur)" : "");
}

static void show_pad_note(BOOL on) {
    UIView *pad = home;
    if (!pad) return;
    if (on && !pad_note) {
        pad_note = [[UILabel alloc] init];
        pad_note.text = @"Playing on TV";
        pad_note.textColor = [UIColor colorWithWhite:1 alpha:.35];
        pad_note.font = [UIFont systemFontOfSize:28 weight:UIFontWeightSemibold];
        pad_note.textAlignment = NSTextAlignmentCenter;
        pad_note.userInteractionEnabled = NO;
    }
    if (on) {
        pad_note.frame = pad.bounds;
        pad_note.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        pad_note.hidden = GCController.controllers.count == 0;   // touch play: the controls only
        if (pad_note.superview != pad) [pad insertSubview:pad_note atIndex:0];
    } else {
        [pad_note removeFromSuperview];
    }
}

static void move_home(void) {
    UIView *view = (__bridge UIView *)rt_ios_metal_view();
    UIView *place = home;
    if (!tv_window) return;
    if (view && place) {
        [place insertSubview:view atIndex:MIN(home_index, place.subviews.count)];
        view.frame = place.bounds;
        rt_ios_view_set_scale((__bridge void *)view, rt_ios_render_scale());
    }
    show_pad_note(NO);
    tv_window.hidden = YES;
    tv_window = nil;
    tv_w = tv_h = tv_hz = 0;
    int w = 0, h = 0;
    rt_ios_render_size(&w, &h);   // the iPad's own picture again
    if (w > 0 && h > 0) send_mode(w, h);
    rt_ios_power_reload();
    fprintf(stderr, "[ios] TV : image revenue sur l'iPad\n");
}

static void move_to_tv(UIScreen *screen, int sim_w, int sim_h) {
    UIView *view = (__bridge UIView *)rt_ios_metal_view();
    if (!view || tv_window) return;
    CGRect frame;
    int pw, ph;
    if (screen) {
        screen.overscanCompensation = UIScreenOverscanCompensationNone;
        frame = screen.bounds;
        CGSize px = screen.currentMode ? screen.currentMode.size : screen.nativeBounds.size;
        pw = (int)px.width; ph = (int)px.height;
        tv_hz = (int)screen.maximumFramesPerSecond;
    } else {   // simulated: a 16:9 (or the given shape) band across the iPad's top
        CGRect b = UIScreen.mainScreen.bounds;
        frame = CGRectMake(0, 0, b.size.width, b.size.width * sim_h / sim_w);
        pw = sim_w; ph = sim_h;
        tv_hz = (int)UIScreen.mainScreen.maximumFramesPerSecond;
    }
    int w = 0, h = 0;
    if (!rt_tv_render_size(pw, ph, getenv("P2_TV_RESOLUTION"), &w, &h)) return;
    tv_window = [[UIWindow alloc] initWithFrame:frame];
    if (screen) tv_window.screen = screen;
    else tv_window.windowLevel = UIWindowLevelNormal + 1;
    UIViewController *root = [[UIViewController alloc] init];
    root.view.backgroundColor = UIColor.blackColor;
    tv_window.rootViewController = root;
    home = view.superview;
    home_index = home ? [home.subviews indexOfObject:view] : 0;
    [root.view addSubview:view];
    view.frame = root.view.bounds;
    view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    rt_ios_view_set_scale((__bridge void *)view, (float)(w / frame.size.width));   // drawable = w x h
    tv_window.hidden = NO;
    tv_w = w; tv_h = h;
    show_pad_note(YES);
    send_mode(w, h);
    rt_ios_power_reload();   // the TV's frame cap (power_ios.m)
    fprintf(stderr, "[ios] TV : %s %dx%d pixels à %d Hz, image %dx%d\n", screen ? "écran externe" : "simulé (iPad)",
            pw, ph, tv_hz, w, h);
}

// Where the picture should be now: the TV when one is there and the setting allows it.
void rt_ios_tv_refresh(void) {
    if (!NSThread.isMainThread) { dispatch_async(dispatch_get_main_queue(), ^{ rt_ios_tv_refresh(); }); return; }
    int sw = 0, sh = 0;
    int simulated = !simulated_off && rt_tv_simulated(getenv("P2_TV_SIMULATE"), &sw, &sh);
    UIScreen *screen = simulated || getenv("P2_TV_SIMULATE") ? nil : external_screen();
    BOOL wanted = (screen || simulated) && rt_tv_enabled(getenv("P2_TV_OUTPUT"));
    if (tv_window && (!wanted || (screen && tv_window.screen != screen))) move_home();
    else if (tv_window && wanted) {   // still on the TV: again only if the picture size changes
        int pw = screen ? (int)(screen.currentMode ? screen.currentMode.size.width : screen.nativeBounds.size.width) : sw;
        int ph = screen ? (int)(screen.currentMode ? screen.currentMode.size.height : screen.nativeBounds.size.height) : sh;
        int w = 0, h = 0;
        if (rt_tv_render_size(pw, ph, getenv("P2_TV_RESOLUTION"), &w, &h) && (w != tv_w || h != tv_h)) move_home();
    }
    if (wanted && !tv_window) move_to_tv(screen, sw, sh);
}

// The Metal view on the TV window, filling it at the TV's picture size. SDL puts
// its views back into its own window when the engine toggles fullscreen or
// moves its window (Half-Life 2's and Left 4 Dead 2's engines do after a
// mode switch; Portal 2's doesn't): checked again on every poll.
static void keep_on_tv(void) {
    UIView *view = (__bridge UIView *)rt_ios_metal_view();
    UIView *root = tv_window.rootViewController.view;
    if (!view || !root || !tv_w) return;
    BOOL moved = view.superview != root;
    if (moved) {
        if (view.superview) { home = view.superview; home_index = [home.subviews indexOfObject:view]; }
        [root addSubview:view];
    }
    if (moved || !CGRectEqualToRect(view.frame, root.bounds)) {
        view.frame = root.bounds;
        view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        rt_ios_view_set_scale((__bridge void *)view, (float)(tv_w / root.bounds.size.width));
        fprintf(stderr, "[ios] TV : vue du jeu remise sur la TV%s\n", moved ? " (SDL l'avait reprise)" : "");
    }
}

// Each frame (game_state poll, main thread): a size the engine couldn't take
// yet, and the game's view still on the TV.
void rt_ios_tv_poll(void) {
    if (tv_window) keep_on_tv();
    if (mode_pending && rt_ios_game_commands_available() && rt_ios_game_command(mode_line)) {
        mode_pending = 0;
        fprintf(stderr, "[ios] TV : %s envoyée\n", mode_line);
    }
    if (pad_note && tv_window) pad_note.hidden = GCController.controllers.count == 0;
}

void rt_ios_tv_watch(void) {
    static int done;
    if (done++) return;
    NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
    for (NSNotificationName name in @[UIScreenDidConnectNotification, UIScreenDidDisconnectNotification,
                                      UIScreenModeDidChangeNotification])
        [center addObserverForName:name object:nil queue:NSOperationQueue.mainQueue
                        usingBlock:^(NSNotification *n) { (void)n; rt_ios_tv_refresh(); }];
    rt_ios_tv_refresh();
    int cycle = atoi(getenv("P2_TV_SIMULATE_CYCLE") ? getenv("P2_TV_SIMULATE_CYCLE") : "0");
    if (cycle > 0 && getenv("P2_TV_SIMULATE")) {
        [NSTimer scheduledTimerWithTimeInterval:cycle repeats:YES block:^(NSTimer *t) {
            (void)t;
            simulated_off = !simulated_off;
            fprintf(stderr, "[ios] TV simulée : %s\n", simulated_off ? "débranchée" : "rebranchée");
            rt_ios_tv_refresh();
        }];
    }
}

#pragma clang diagnostic pop
#endif
