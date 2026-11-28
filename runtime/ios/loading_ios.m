// Sauce's setting-up screen (round 31). Between choosing a game and the
// engine's first frame the screen used to be pure black for seconds (longer
// on a game's first start): on an OLED iPhone, exactly what a phone that is
// off looks like. Now: the Sauce name and icon, a turning ring, a line saying
// what it's doing ("Setting up Left 4 Dead 2…"), and the version in a corner.
//
// The engine starts on the main thread and blocks it until its first frame,
// so nothing here can count on the run loop meanwhile: the window is laid out
// and committed to the render server at once (CATransaction flush), and the
// ring turns through a Core Animation animation, which the render server runs
// on its own. Once the game loop pumps events again, a timer sees the frames
// arrive (fps_overlay_presented) and fades the screen out.
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <stdlib.h>
#include <string.h>
#include "loading_ios.h"
#include "../fps_overlay.h"
#include "env_ios.h"

static UIWindow *window;
static UILabel *line;
static NSTimer *watch;
static NSString *booted_key;
static unsigned long frames_at_show;

static void commit(void) {
    [window layoutIfNeeded];
    [CATransaction flush];
}

static UIView *ring(void) {
    UIView *v = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 34, 34)];
    CAShapeLayer *arc = [CAShapeLayer layer];
    arc.frame = v.bounds;
    arc.path = [UIBezierPath bezierPathWithArcCenter:CGPointMake(17, 17) radius:14 startAngle:0 endAngle:M_PI * 1.5
                                           clockwise:YES].CGPath;
    arc.strokeColor = [UIColor colorWithRed:.98 green:.55 blue:.16 alpha:1].CGColor;   // the launcher's accent
    arc.fillColor = UIColor.clearColor.CGColor;
    arc.lineWidth = 3.5;
    arc.lineCap = kCALineCapRound;
    [v.layer addSublayer:arc];
    CABasicAnimation *turn = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
    turn.fromValue = @0;
    turn.toValue = @(2 * M_PI);
    turn.duration = 0.9;
    turn.repeatCount = HUGE_VALF;
    turn.removedOnCompletion = NO;
    [arc addAnimation:turn forKey:@"turn"];
    return v;
}

static UILabel *label(NSString *text, CGFloat size, UIFontWeight weight, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.text = text;
    l.font = [UIFont systemFontOfSize:size weight:weight];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.textAlignment = NSTextAlignmentCenter;
    l.numberOfLines = 0;
    l.translatesAutoresizingMaskIntoConstraints = NO;
    return l;
}

static void hide(void) {
    [watch invalidate];
    watch = nil;
    if (booted_key) [NSUserDefaults.standardUserDefaults setBool:YES forKey:booted_key];
    UIWindow *w = window;
    window = nil;
    line = nil;
    [UIView animateWithDuration:0.35 animations:^{ w.alpha = 0; } completion:^(BOOL done) {
        (void)done;
        w.hidden = YES;
        w.rootViewController = nil;
    }];
}

void rt_ios_loading_say(const char *text) {
    if (!line || !text) return;
    line.text = @(text);
    commit();
}

void rt_ios_loading_show(const char *game_name, const char *game_title) {
    if (window || !rt_ios_env_flag("P2_LOADING_SCREEN", 1)) return;
    NSString *title = @(game_title && *game_title ? game_title : "your game");
    booted_key = [@"P2GameBooted." stringByAppendingString:@(game_name ? game_name : "")];
    BOOL first = ![NSUserDefaults.standardUserDefaults boolForKey:booted_key];
    NSString *what = first ? [NSString stringWithFormat:@"Setting up %@…\nThe first start takes a little longer.", title]
                           : [NSString stringWithFormat:@"Starting %@…", title];

    window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    window.windowLevel = UIWindowLevelAlert + 1;   // over SDL's window until the game draws
    window.userInteractionEnabled = NO;            // touches go through to the game underneath
    UIViewController *vc = [UIViewController new];
    UIView *root = vc.view;
    root.backgroundColor = [UIColor colorWithRed:.035 green:.035 blue:.05 alpha:1];   // the launcher's, not black

    UIStackView *stack = [UIStackView new];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.alignment = UIStackViewAlignmentCenter;
    stack.spacing = 16;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    UIImage *icon = [UIImage imageNamed:@"AppIcon60x60"];
    if (icon) {
        UIImageView *iv = [[UIImageView alloc] initWithImage:icon];
        iv.layer.cornerRadius = 18;
        iv.layer.masksToBounds = YES;
        [iv.widthAnchor constraintEqualToConstant:80].active = YES;
        [iv.heightAnchor constraintEqualToConstant:80].active = YES;
        [stack addArrangedSubview:iv];
    }
    [stack addArrangedSubview:label(@"Sauce", 34, UIFontWeightBold, 1)];
    UIView *spinner = ring();
    spinner.translatesAutoresizingMaskIntoConstraints = NO;
    [spinner.widthAnchor constraintEqualToConstant:34].active = YES;
    [spinner.heightAnchor constraintEqualToConstant:34].active = YES;
    [stack addArrangedSubview:spinner];
    line = label(what, 16, UIFontWeightRegular, .75);
    [stack addArrangedSubview:line];
    [root addSubview:stack];

    NSDictionary *info = NSBundle.mainBundle.infoDictionary;
    NSString *version = info[@"P2SauceVersion"];
    if (!version.length) version = info[@"P2ForkVersion"];
    UILabel *corner = label(version.length ? [@"Sauce " stringByAppendingString:version] : @"Sauce", 12,
                            UIFontWeightMedium, .4);
    [root addSubview:corner];

    UILayoutGuide *safe = root.safeAreaLayoutGuide;
    [NSLayoutConstraint activateConstraints:@[
        [stack.centerXAnchor constraintEqualToAnchor:root.centerXAnchor],
        [stack.centerYAnchor constraintEqualToAnchor:root.centerYAnchor],
        [stack.leadingAnchor constraintGreaterThanOrEqualToAnchor:safe.leadingAnchor constant:24],
        [stack.trailingAnchor constraintLessThanOrEqualToAnchor:safe.trailingAnchor constant:-24],
        [corner.trailingAnchor constraintEqualToAnchor:safe.trailingAnchor constant:-16],
        [corner.bottomAnchor constraintEqualToAnchor:safe.bottomAnchor constant:-12],
    ]];
    window.rootViewController = vc;
    [window makeKeyAndVisible];
    commit();   // on screen before the engine blocks the main thread

    frames_at_show = fps_overlay_presented();
    watch = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer *t) {
        (void)t;
        if (fps_overlay_presented() >= frames_at_show + 3) hide();   // the game is drawing
    }];
    [NSRunLoop.mainRunLoop addTimer:watch forMode:NSRunLoopCommonModes];
    fprintf(stderr, "[ios] écran de démarrage : %s\n", what.UTF8String);
}
