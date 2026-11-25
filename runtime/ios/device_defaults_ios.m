// Defaults per device on iOS (see device_defaults_ios.h): the table from the
// app bundle (ios/device_defaults.txt), this device's model, memory and
// screen, and the app's preferences (shared with the Settings app and the
// in-game menu).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device_defaults.h"
#include "device_defaults_ios.h"
#include "device_ios.h"
#include "game_ios.h"
#include "live_settings.h"
#include "settings.h"

static rt_dd_table table;
static rt_dd_choice choice;
static int ready = -1;   // -1: not looked yet, 0: no table, 1: choice made

const rt_dd_choice *rt_ios_device_choice(void) {
    if (ready >= 0) return ready ? &choice : NULL;
    ready = 0;
    NSString *path = [NSBundle.mainBundle pathForResource:@"device_defaults" ofType:@"txt"];
    NSString *text = path ? [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil] : nil;
    if (!text || !rt_dd_parse(text.UTF8String, &table)) return NULL;
    char model[64];
    rt_ios_device_model(model, sizeof(model));
    __block NSInteger screen = 60;
    void (^read)(void) = ^{ screen = UIScreen.mainScreen.maximumFramesPerSecond; };
    if (NSThread.isMainThread) read(); else dispatch_sync(dispatch_get_main_queue(), read);
    ready = rt_dd_choose(&table, model, rt_ios_device_ram_mb(), (int)screen, &choice);
    return ready ? &choice : NULL;
}

static int get(void *ctx, const char *key, char *out, size_t size) { (void)ctx; return rt_ios_preference(key, out, size); }
static void set_pref(void *ctx, const char *key, const char *value) {
    (void)ctx;
    char own[160];
    CFStringRef k = CFStringCreateWithCString(NULL, rt_ios_pref_key(key, own, sizeof(own)), kCFStringEncodingUTF8);
    CFStringRef v = CFStringCreateWithCString(NULL, value, kCFStringEncodingUTF8);
    CFPreferencesSetAppValue(k, v, kCFPreferencesCurrentApplication);
    CFRelease(k); CFRelease(v);
}

int rt_ios_device_defaults_first_launch(char *summary, size_t size) {
    if (size) summary[0] = 0;
    const rt_dd_choice *c = rt_ios_device_choice();
    if (!c) { snprintf(summary, size, "pas de table device_defaults.txt dans l'app"); return 0; }
#if TARGET_OS_SIMULATOR
    // The simulator renders in software at ×1 unless told otherwise: its own
    // model's defaults (×2) would only slow every simulator check down. Only
    // P2_DEVICE_MODEL (a phone being imitated on purpose) picks them.
    if (!getenv("P2_DEVICE_MODEL")) {
        snprintf(summary, size, "simulateur (%s) : défauts de l'appareil non posés (P2_DEVICE_MODEL pour les essayer)", c->model);
        return 0;
    }
#endif
    int written = rt_dd_first_launch(c, get, set_pref, NULL);
    if (written) CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    snprintf(summary, size, "%s (%s, %u Mo%s), palier %s : résolution %s, MetalFX %s, %s i/s%s ; %d réglage(s) posé(s)",
             c->name, c->model, rt_ios_device_ram_mb(), c->low_memory ? ", mémoire réduite" : "", c->tier, c->scale,
             c->upscale, c->fps, c->known ? "" : " (modèle inconnu)", written);
    return written;
}

void rt_ios_device_defaults_use_recommended(void) {
    const rt_dd_choice *c = rt_ios_device_choice();
    if (!c) return;
    for (int i = 0; i < 3; ++i) rt_ios_setting_set(rt_dd_keys[i], rt_dd_value(c, rt_dd_keys[i]));   // live where it can be
    set_pref(NULL, RT_DD_MARKER, c->model);
}

const char *rt_ios_device_recommended(const char *key) {
    const rt_dd_choice *c = rt_ios_device_choice();
    return c ? rt_dd_value(c, key) : NULL;
}

int rt_ios_device_low_memory(void) {
    const rt_dd_choice *c = rt_ios_device_choice();
    return c && c->low_memory && !getenv("P2_NO_LOWMEM");
}
const char *rt_ios_device_low_memory_args(void) { return rt_ios_device_low_memory() ? table.lowmem_args : ""; }
const char *rt_ios_device_low_memory_metal(void) { return rt_ios_device_low_memory() ? table.lowmem_metal : ""; }
#endif
