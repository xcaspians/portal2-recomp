// Live settings (see live_settings.h).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backtap_ios.h"
#include "gyro_ios.h"
#include "icloud_saves.h"
#include "live_settings.h"
#include "power_ios.h"
#include "settings.h"
#include "settings_menu.h"

// Last value applied per menu setting: a change in the Settings app is applied
// once, and our own writes are not applied twice.
static NSMutableDictionary<NSString *, NSString *> *applied;

const char *rt_ios_setting_value(const char *key, char *out, size_t size) {
    if (rt_ios_preference(key, out, size)) return out;
    const rt_menu_item *it = rt_menu_item_named(key);
    snprintf(out, size, "%s", it ? it->def : "");
    return out;
}

static void apply(const char *key, const char *value) {
    const rt_ios_setting *s = rt_ios_setting_named(key);
    if (!s) return;
    const char *env = rt_ios_setting_env_value(s, value);
    if (env) setenv(s->env, env, 1); else unsetenv(s->env);
    applied[@(key)] = @(value);
    fprintf(stderr, "[ios] réglage %s = %s (%s=%s)\n", key, value, s->env, env ? env : "défaut");
    if (!strncmp(key, "P2BackTap", 9)) rt_ios_backtap_reload();
    if (!strncmp(key, "P2Gyro", 6)) rt_ios_gyro_reload();
    if (!strcmp(key, "P2MaxFPS")) rt_ios_power_reload();
    if (!strcmp(key, "P2ICloudSaves")) rt_ios_icloud_reload();
    [NSNotificationCenter.defaultCenter postNotificationName:@"P2SettingsChanged" object:@(key)];
}

void rt_ios_setting_set(const char *key, const char *value) {
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    const rt_menu_item *it = rt_menu_item_named(key);
    NSString *name = @(key);
    if (it && it->kind == RT_MENU_TOGGLE) [defaults setBool:strcmp(value, "0") != 0 forKey:name];
    else if (it && it->kind == RT_MENU_SLIDER) [defaults setDouble:strtod(value, NULL) forKey:name];
    else [defaults setObject:@(value) forKey:name];
    apply(key, value);
}

void rt_ios_setting_reset(const char *key) {
    [NSUserDefaults.standardUserDefaults removeObjectForKey:@(key)];
    char value[64];
    apply(key, rt_ios_setting_value(key, value, sizeof(value)));
}

static void refresh(void) {
    char value[64];
    for (int i = 0; i < rt_menu_item_count; ++i) {
        const char *key = rt_menu_items[i].key;
        rt_ios_setting_value(key, value, sizeof(value));
        NSString *last = applied[@(key)];
        if (!last) { applied[@(key)] = @(value); continue; }   // first look: what the launch applied
        if (![last isEqualToString:@(value)]) apply(key, value);
    }
}

void rt_ios_settings_watch(void) {
    if (applied) return;
    applied = [NSMutableDictionary dictionary];
    refresh();
    [NSNotificationCenter.defaultCenter addObserverForName:NSUserDefaultsDidChangeNotification object:nil
                                                     queue:NSOperationQueue.mainQueue
                                                usingBlock:^(NSNotification *n) { (void)n; refresh(); }];
}
#endif
