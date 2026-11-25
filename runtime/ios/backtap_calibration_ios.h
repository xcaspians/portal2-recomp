// "Calibrate back taps" screen (runtime/ios/backtap_calibration_ios.m).
#pragma once
#include <stddef.h>

// A UIView (retained, __bridge_transfer it) over the settings panel; close() when done.
void *rt_ios_backtap_calibration_create(void (*close)(void *), void *ctx);
// "Calibrated on <date>" or "Using the defaults", for the settings menu row.
const char *rt_ios_backtap_calibration_summary(char *out, size_t size);
