// "Calibrate back taps" (in-game settings menu): a guided screen over the
// settings panel. Tap the back on the left x8, on the right x8, hold still 5 s;
// each tap shows at once (heard, and on which side). The values are then
// fitted (backtap_calibrate.c), shown with before and after counts, and saved
// for this device model; a test mode counts free taps with them. No portal is
// fired while the screen is up (backtap_ios.m, capture mode).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "back_tap.h"
#include "backtap_calibrate.h"
#include "backtap_calibration_ios.h"
#include "backtap_ios.h"

typedef enum { STEP_INTRO, STEP_LEFT, STEP_RIGHT, STEP_STILL, STEP_RESULT, STEP_TEST } Step;
enum { PER_SIDE = 8 };

@interface P2BackTapCalibration : UIView
@end

@implementation P2BackTapCalibration {
    void (*onClose)(void *);
    void *ctx;
    Step step;
    UILabel *title, *prompt, *feedback, *counts;
    UIStackView *buttons;
    rt_btcal_tap taps[2 * PER_SIDE + 8];
    int ntaps, left, right, testLeft, testRight;
    CFTimeInterval stillUntil;
    rt_btcal_fit fit;
    NSTimer *timer;
}

static UILabel *label(CGFloat size, UIFontWeight w, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.font = [UIFont systemFontOfSize:size weight:w];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.numberOfLines = 0;
    l.textAlignment = NSTextAlignmentCenter;
    return l;
}

- (instancetype)initWithClose:(void (*)(void *))close ctx:(void *)c {
    if (!(self = [super initWithFrame:CGRectZero])) return nil;
    onClose = close; ctx = c;
    self.backgroundColor = [UIColor colorWithWhite:.06 alpha:1];
    self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    UIStackView *s = [UIStackView new];
    s.axis = UILayoutConstraintAxisVertical; s.spacing = 14; s.alignment = UIStackViewAlignmentCenter;
    s.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:s];
    [NSLayoutConstraint activateConstraints:@[
        [s.centerXAnchor constraintEqualToAnchor:self.centerXAnchor], [s.centerYAnchor constraintEqualToAnchor:self.centerYAnchor],
        [s.widthAnchor constraintEqualToAnchor:self.widthAnchor constant:-48]]];
    title = label(22, UIFontWeightBold, 1);
    prompt = label(18, UIFontWeightMedium, .95);
    feedback = label(30, UIFontWeightBold, 1);
    counts = label(15, UIFontWeightRegular, .7);
    buttons = [UIStackView new];
    buttons.spacing = 10;
    for (UIView *v in @[title, prompt, feedback, counts, buttons]) [s addArrangedSubview:v];
    [self show:STEP_INTRO];
    timer = [NSTimer scheduledTimerWithTimeInterval:1.0 / 30 repeats:YES block:^(NSTimer *t) { (void)t; [self tick]; }];
    return self;
}
- (void)removeFromSuperview {
    [timer invalidate];
    rt_ios_backtap_capture(0, NULL);
    [super removeFromSuperview];
}
- (void)button:(NSString *)name filled:(BOOL)filled action:(void (^)(void))action {
    UIButtonConfiguration *c = filled ? [UIButtonConfiguration filledButtonConfiguration] : [UIButtonConfiguration grayButtonConfiguration];
    c.title = name;
    c.baseForegroundColor = UIColor.whiteColor;
    c.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    [buttons addArrangedSubview:[UIButton buttonWithConfiguration:c primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; action();
    }]]];
}
static NSString *calibrated_on(void) {
    char text[256];
    rt_btcal_fit f;
    long long when;
    if (!rt_ios_backtap_calibration(text, sizeof(text)) || !rt_btcal_parse(text, &f, &when)) return nil;
    NSDateFormatter *df = [NSDateFormatter new];
    df.dateStyle = NSDateFormatterMediumStyle; df.timeStyle = NSDateFormatterShortStyle;
    return [df stringFromDate:[NSDate dateWithTimeIntervalSince1970:(double)when]];
}
- (void)show:(Step)s {
    step = s;
    for (UIView *v in buttons.arrangedSubviews) [v removeFromSuperview];
    feedback.text = @"";
    __weak P2BackTapCalibration *me = self;
    switch (s) {
    case STEP_INTRO: {
        rt_ios_backtap_capture(0, NULL);
        NSString *when = calibrated_on();
        title.text = @"Calibrate back taps";
        prompt.text = @"Hold the device as you play. You'll tap the back on the left 8 times, on the right 8 times, "
                       "then hold still for 5 seconds. No portal is fired meanwhile.";
        counts.text = when ? [NSString stringWithFormat:@"Calibrated on %@", when] : @"Using the defaults";
        [self button:@"Start" filled:YES action:^{ [me begin]; }];
        if (when) {
            [self button:@"Test" filled:NO action:^{ [me show:STEP_TEST]; }];
            [self button:@"Reset to defaults" filled:NO action:^{
                rt_ios_backtap_store_calibration(NULL);
                [me show:STEP_INTRO];
            }];
        }
        [self button:@"Close" filled:NO action:^{ [me close]; }];
        break;
    }
    case STEP_LEFT: case STEP_RIGHT: {
        title.text = s == STEP_LEFT ? @"Left side" : @"Right side";
        prompt.text = [NSString stringWithFormat:@"Tap the back on the %@ %d times, about a second apart.",
                       s == STEP_LEFT ? @"LEFT" : @"RIGHT", PER_SIDE];
        counts.text = [NSString stringWithFormat:@"%d / %d", s == STEP_LEFT ? left : right, PER_SIDE];
        [self button:@"Cancel" filled:NO action:^{ [me show:STEP_INTRO]; }];
        break;
    }
    case STEP_STILL: {
        title.text = @"Hold still";
        prompt.text = @"Keep the device still, as you hold it to play, for 5 seconds.";
        float a, j, w;
        rt_ios_backtap_capture_noise(&a, &j, &w, 1);
        stillUntil = CACurrentMediaTime() + 5;
        break;
    }
    case STEP_RESULT: [self showResult]; break;
    case STEP_TEST: {
        rt_backtap_params p;
        char text[256];
        rt_backtap_params_from_env_calibrated(&p, rt_ios_backtap_calibration(text, sizeof(text)));
        rt_ios_backtap_capture(1, &p);   // the values the game now uses
        testLeft = testRight = 0;
        title.text = @"Test";
        prompt.text = @"Tap the back anywhere, as in a game. Each portal the game would fire shows here.";
        counts.text = @"Left 0 · Right 0";
        [self button:@"Done" filled:YES action:^{ [me show:STEP_INTRO]; }];
        break;
    }
    }
}
- (void)begin {
    ntaps = left = right = 0;
    rt_ios_backtap_capture(1, NULL);
    [self show:STEP_LEFT];
}
- (void)tick {
    rt_backtap_report r;
    while (rt_ios_backtap_capture_take(&r)) {
        if (step == STEP_LEFT || step == STEP_RIGHT) {
            int expected = step == STEP_LEFT ? -1 : 1;
            if (ntaps < (int)(sizeof(taps) / sizeof(*taps)))
                taps[ntaps++] = (rt_btcal_tap){r.peak, r.jerk, r.rot[0], r.rot[2], r.orient ? r.orient : 1, expected};
            int *n = step == STEP_LEFT ? &left : &right;
            ++*n;
            feedback.text = r.side == expected ? @"✓ heard" : r.side ? @"✓ heard (on the other side for now)" : @"✓ heard";
            counts.text = [NSString stringWithFormat:@"%d / %d", *n, PER_SIDE];
            if (*n >= PER_SIDE) [self show:step == STEP_LEFT ? STEP_RIGHT : STEP_STILL];
        } else if (step == STEP_TEST && !r.reject) {
            if (r.side < 0) ++testLeft; else if (r.side > 0) ++testRight;
            feedback.text = r.side < 0 ? @"◀︎ Left (blue)" : r.side > 0 ? @"Right (orange) ▶︎" : @"";
            counts.text = [NSString stringWithFormat:@"Left %d · Right %d", testLeft, testRight];
        } else if (step == STEP_TEST && r.reject && !strcmp(r.reject, "côté incertain")) {
            feedback.text = @"Not sure of the side: no portal";
        }
    }
    if (step == STEP_STILL) {
        double left_s = stillUntil - CACurrentMediaTime();
        feedback.text = [NSString stringWithFormat:@"%.0f", left_s > 0 ? left_s : 0];
        if (left_s <= 0) {
            rt_btcal_noise noise;
            rt_ios_backtap_capture_noise(&noise.a_rms, &noise.jerk_rms, &noise.w_rms, 0);
            rt_ios_backtap_capture(0, NULL);
            rt_backtap_params def;
            rt_backtap_defaults(&def);
            if (!rt_btcal_fit_taps(taps, ntaps, &noise, &def, &fit)) memset(&fit, 0, sizeof(fit));
            [self show:STEP_RESULT];
        }
    }
}
- (void)showResult {
    __weak P2BackTapCalibration *me = self;
    title.text = @"Calibration";
    if (!fit.taps) {
        prompt.text = @"Not enough taps were heard on each side. Tap a little harder, and try again.";
        [self button:@"Try again" filled:YES action:^{ [me begin]; }];
        [self button:@"Cancel" filled:NO action:^{ [me show:STEP_INTRO]; }];
        return;
    }
    prompt.text = [NSString stringWithFormat:@"With the defaults, %d of your %d taps fired on the right side (%d on the wrong one).\n"
                   "With these values: %d, and %d on the wrong side.%@",
                   fit.before, fit.taps, fit.wrong_before, fit.after, fit.wrong_after,
                   fit.uncertain ? [NSString stringWithFormat:@"\n%d too close to call: no portal rather than the wrong one.", fit.uncertain] : @""];
    counts.text = [NSString stringWithFormat:@"Thresholds %.3f g, %.1f g/s, rotation %.2f rad/s · side weight %.2f · margin %.3f",
                   fit.accel_min, fit.jerk_min, fit.gyro_rot_min, fit.side_x_weight, fit.side_margin];
    [self button:@"Save and test" filled:YES action:^{ [me save]; }];
    [self button:@"Re-calibrate" filled:NO action:^{ [me begin]; }];
    [self button:@"Cancel" filled:NO action:^{ [me show:STEP_INTRO]; }];
}
- (void)save {
    char text[256];
    rt_btcal_format(&fit, (long long)time(NULL), text, sizeof(text));
    rt_ios_backtap_store_calibration(text);
    fprintf(stderr, "[ios] tapes au dos : calibration %s (avant %d/%d, après %d/%d)\n", text, fit.before, fit.taps, fit.after,
            fit.taps);
    [self show:STEP_TEST];
}
- (void)close {
    rt_ios_backtap_capture(0, NULL);
    onClose(ctx);
}
@end

void *rt_ios_backtap_calibration_create(void (*close)(void *), void *ctx) {
    return (__bridge_retained void *)[[P2BackTapCalibration alloc] initWithClose:close ctx:ctx];
}
const char *rt_ios_backtap_calibration_summary(char *out, size_t size) {
    NSString *when = calibrated_on();
    snprintf(out, size, "%s", when ? [NSString stringWithFormat:@"Calibrated on %@", when].UTF8String : "Using the defaults");
    return out;
}
#endif
