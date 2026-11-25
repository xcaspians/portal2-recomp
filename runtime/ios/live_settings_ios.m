// Live settings (see live_settings.h).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backtap_ios.h"
#include "game_ios.h"
#include "device_defaults_ios.h"
#include "gyro_ios.h"
#include "icloud_saves.h"
#include "live_settings.h"
#include "power_ios.h"
#include "settings.h"
#include "settings_menu.h"
#include "touch_layout.h"
#include "tv_output.h"

// Last value applied per menu setting: a change in the Settings app is applied
// once, and our own writes are not applied twice.
static NSMutableDictionary<NSString *, NSString *> *applied;

const char *rt_ios_setting_value(const char *key, char *out, size_t size) {
    if (rt_ios_preference(key, out, size)) return out;
    const rt_menu_item *it = rt_menu_item_named(key);
    snprintf(out, size, "%s", it ? rt_menu_item_default(it, rt_ios_game_kind(rt_ios_current_game())) : "");
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
    if (!strcmp(key, "P2MaxFPS") || !strcmp(key, "P2TVMaxFPS")) rt_ios_power_reload();
    if (!strcmp(key, "P2TVOutput") || !strcmp(key, "P2TVResolution")) rt_ios_tv_refresh();   // round 20
    if (!strcmp(key, "P2ICloudSaves")) rt_ios_icloud_reload();
    [NSNotificationCenter.defaultCenter postNotificationName:@"P2SettingsChanged" object:@(key)];
}

void rt_ios_setting_set(const char *key, const char *value) {
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    const rt_menu_item *it = rt_menu_item_named(key);
    char own[160];
    NSString *name = @(rt_ios_pref_key(key, own, sizeof(own)));   // the running game's (game_ios.c)
    if (it && it->kind == RT_MENU_TOGGLE) [defaults setBool:strcmp(value, "0") != 0 forKey:name];
    else if (it && it->kind == RT_MENU_SLIDER) [defaults setDouble:strtod(value, NULL) forKey:name];
    else [defaults setObject:@(value) forKey:name];
    apply(key, value);
}

void rt_ios_setting_reset(const char *key) {
    // Picked for this device (device_defaults.txt): back to that, not the
    // code's default for the fastest phones.
    const char *recommended = rt_ios_device_recommended(key);
    if (recommended) { rt_ios_setting_set(key, recommended); return; }
    char own[160];
    [NSUserDefaults.standardUserDefaults removeObjectForKey:@(rt_ios_pref_key(key, own, sizeof(own)))];
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

// Round 11 made settings shared by every game (game_ios.c, rt_ios_pref_key):
// Portal 2's became the shared set. A hosted game's old settings ("mel.*")
// stay in the app's preferences, untouched, and are copied once to
// Documents/<userdata>/settings-before-shared.plist as a backup.
static void backup_hosted_settings(void) {
    const rt_ios_game *game = rt_ios_current_game();
    if (game == rt_ios_app_game()) return;
    NSString *docs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    NSString *dir = [docs stringByAppendingPathComponent:@(game->userdata)];
    NSString *path = [dir stringByAppendingPathComponent:@"settings-before-shared.plist"];
    if ([NSFileManager.defaultManager fileExistsAtPath:path]) return;
    NSString *prefix = [@(game->name) stringByAppendingString:@"."];
    NSMutableDictionary *old = [NSMutableDictionary dictionary];
    [NSUserDefaults.standardUserDefaults.dictionaryRepresentation enumerateKeysAndObjectsUsingBlock:^(NSString *k, id v, BOOL *stop) {
        (void)stop;
        if ([k hasPrefix:prefix]) old[k] = v;
    }];
    [NSFileManager.defaultManager createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
    BOOL ok = [old writeToFile:path atomically:YES];
    fprintf(stderr, "[ios] réglages partagés entre les jeux : %lu ancien(s) réglage(s) de %s gardé(s) dans %s%s\n",
            (unsigned long)old.count, game->name, path.UTF8String, ok ? "" : " (écriture impossible)");
}

// Round 19: a game's settings from its former standalone app (tools/ios_device.sh
// --game hl2|l4d2 migrate leaves them in Documents/<userdata>/settings-from-old-app.plist).
// Settings are shared since round 11, so what is the game's own is its button
// layout: its buttons the shared layout doesn't place yet take the old app's
// places (rt_touch_custom_import), once.
static void import_old_app_settings(void) {
    const rt_ios_game *game = rt_ios_current_game();
    if (game == rt_ios_app_game()) return;
    NSString *docs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    NSString *path = [[docs stringByAppendingPathComponent:@(game->userdata)]
                      stringByAppendingPathComponent:@"settings-from-old-app.plist"];
    NSDictionary *old = [NSDictionary dictionaryWithContentsOfFile:path];
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSString *mark = [NSString stringWithFormat:@"%s.P2OldAppSettingsImported", game->name];
    if (!old || [d boolForKey:mark]) return;
    int taken = 0;
    for (NSString *key in @[@"P2TouchLayout.pad", @"P2TouchLayout.phone"]) {   // shared keys (rt_ios_pref_key)
        NSString *was = old[key];
        if (![was isKindOfClass:NSString.class]) continue;
        rt_touch_custom_layout from, into;
        memset(&from, 0, sizeof(from));
        memset(&into, 0, sizeof(into));
        rt_touch_custom_parse(was.UTF8String, &from);
        NSString *now = [d stringForKey:key];
        if (now) rt_touch_custom_parse(now.UTF8String, &into);
        int n = rt_touch_custom_import(&into, &from, game->layout);
        char text[1024];
        if (n && rt_touch_custom_format(&into, text, sizeof(text))) {
            [d setObject:@(text) forKey:key];
            taken += n;
        }
    }
    [d setBool:YES forKey:mark];
    fprintf(stderr, "[ios] réglages de l'ancienne app de %s : %d bouton(s) de la disposition repris\n", game->name, taken);
}

void rt_ios_settings_watch(void) {
    if (applied) return;
    backup_hosted_settings();
    import_old_app_settings();
    applied = [NSMutableDictionary dictionary];
    refresh();
    [NSNotificationCenter.defaultCenter addObserverForName:NSUserDefaultsDidChangeNotification object:nil
                                                     queue:NSOperationQueue.mainQueue
                                                usingBlock:^(NSNotification *n) { (void)n; refresh(); }];
    // That notice is documented for changes made in this process: a change in
    // the Settings app is read again when the game comes back.
    [NSNotificationCenter.defaultCenter addObserverForName:UIApplicationWillEnterForegroundNotification object:nil
                                                     queue:NSOperationQueue.mainQueue
                                                usingBlock:^(NSNotification *n) {
        (void)n;
        [NSUserDefaults.standardUserDefaults synchronize];   // reread what another process wrote
        refresh();
    }];
}
#endif
