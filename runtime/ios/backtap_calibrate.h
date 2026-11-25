// Back-tap calibration (runtime/ios/backtap_calibrate.c, pure C, checked by
// tests/checks_backtap_calibrate.c): from taps the player made on purpose
// (left ×8, right ×8) and 5 s held still, per-device thresholds, the side
// weight and the wrong-side margin. Every rule is one sentence:
//   thresholds: 60 % of the player's softest taps (20th percentile), never
//     under 3 times the noise measured while holding still, never over the
//     default (calibration only makes taps easier to catch, not harder);
//   side weight: the mix of rotations (x against z) that puts the most taps
//     on their side, then the one that keeps them furthest from the line;
//   wrong-side margin: just above the strongest wrong-side tap seen, as long
//     as at least 3 good taps in 4 stay above it: a missed tap is better than
//     the wrong portal.
#pragma once
#include <stddef.h>
#include "back_tap.h"

typedef struct {
    float peak, jerk;     // g, g/s (rt_backtap_report)
    float rot_x, rot_z;   // rad/s
    int orient;           // ±1
    int expected;         // the side asked for, on screen: -1 left, +1 right
} rt_btcal_tap;

typedef struct { float a_rms, jerk_rms, w_rms; } rt_btcal_noise;   // held still

typedef struct {
    float accel_min, jerk_min, gyro_rot_min, side_x_weight, side_margin;
    float noise;          // g: a_rms held still
    int taps;             // taps given
    int before, after;    // fired on the right side with the defaults, with these values
    int wrong_before, wrong_after;
    int uncertain;        // held back by the margin (would have been a coin toss)
} rt_btcal_fit;

// Fits the values; 0 (fit untouched) when there aren't at least 4 taps per side.
int rt_btcal_fit_taps(const rt_btcal_tap *taps, int n, const rt_btcal_noise *noise, const rt_backtap_params *defaults,
                      rt_btcal_fit *out);
// Would these values fire this tap, and on which screen side (0: not fired)?
int rt_btcal_judge(const rt_btcal_tap *t, const rt_backtap_params *p);
// Stored form (the app's settings, one key per device model):
// "v1 accel=0.071 jerk=4.2 gyro_rot=0.21 side_x=0.85 margin=0.06 noise=0.006 when=1795500000".
int rt_btcal_format(const rt_btcal_fit *f, long long when, char *out, size_t size);
int rt_btcal_parse(const char *text, rt_btcal_fit *f, long long *when);
// The calibrated values over the defaults (before Tap sensitivity and the P2_BACKTAP_* variables).
void rt_btcal_apply(rt_backtap_params *p, const rt_btcal_fit *f);
