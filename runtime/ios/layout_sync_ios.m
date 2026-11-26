// Round 26: custom button layouts through iCloud, per device kind (rules in
// layout_sync.c). The app's existing iCloud container (the saves' one,
// iCloud.<bundle id>): Documents/Layouts/iPad.txt or iPhone.txt, read and
// written through NSFileCoordinator. Without iCloud the layouts stay saved on
// the device, and the editor says so.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_ios.h"
#include "layout_sync.h"
#include "layout_sync_ios.h"
#include "touch_layout.h"

static NSString *const kTimes = @"P2TouchLayoutTimes";   // key -> seconds since 1970 when saved here
static dispatch_queue_t queue;
static NSString *status;

static BOOL is_ipad(void) { return UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad; }
static NSString *kind_name(void) { return is_ipad() ? @"iPads" : @"iPhones"; }

const char *rt_ios_layouts_sync_status(void) {
    @synchronized (kTimes) {
        return (status ?: @"Checking iCloud…").UTF8String;
    }
}
static void set_status(NSString *s) { @synchronized (kTimes) { status = s; } }

static NSString *layout_key(rt_ios_layout l) {
    char key[96], own[160];
    rt_touch_layout_store_key(l, is_ipad(), key, sizeof(key), NULL, 0);
    return @(rt_ios_pref_key(key, own, sizeof(own)));
}

// This device's layouts with their save times (a layout saved before round 26 has time 0).
static void local_set(rt_layout_set *s) {
    memset(s, 0, sizeof(*s));
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSDictionary *times = [d dictionaryForKey:kTimes];
    for (int l = 0; l < RT_IOS_LAYOUT_COUNT; ++l) {
        NSString *key = layout_key((rt_ios_layout)l), *value = [d stringForKey:key];
        if (value) rt_layout_set_put(s, is_ipad(), key.UTF8String, [times[key] doubleValue], value.UTF8String);
    }
}

void rt_ios_layouts_saved(rt_ios_layout layout) {
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSMutableDictionary *times = [[d dictionaryForKey:kTimes] mutableCopy] ?: [NSMutableDictionary dictionary];
    times[layout_key(layout)] = @(NSDate.date.timeIntervalSince1970);
    [d setObject:times forKey:kTimes];
    rt_ios_layouts_sync("disposition enregistrée");
}

static NSString *read_file(NSURL *url) {
    __block NSString *text = nil;
    NSError *error = nil;
    [[[NSFileCoordinator alloc] initWithFilePresenter:nil] coordinateReadingItemAtURL:url options:0 error:&error
                                                                          byAccessor:^(NSURL *u) {
        text = [NSString stringWithContentsOfURL:u encoding:NSUTF8StringEncoding error:nil];
    }];
    return text;
}
static BOOL write_file(NSURL *url, NSString *text) {
    __block BOOL ok = NO;
    NSError *error = nil;
    [NSFileManager.defaultManager createDirectoryAtURL:url.URLByDeletingLastPathComponent withIntermediateDirectories:YES
                                            attributes:nil error:nil];
    [[[NSFileCoordinator alloc] initWithFilePresenter:nil] coordinateWritingItemAtURL:url
                                                                              options:NSFileCoordinatorWritingForReplacing
                                                                                error:&error byAccessor:^(NSURL *u) {
        ok = [text writeToURL:u atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }];
    return ok;
}

static void sync_now(const char *why) {
    BOOL ipad = is_ipad();
    NSURL *container = [NSFileManager.defaultManager URLForUbiquityContainerIdentifier:nil];   // never on the main thread
    if (!container) {
        set_status(@"iCloud isn't available, so layouts are saved on this device only. "
                   "Sign in to iCloud to share them with your other devices.");
        fprintf(stderr, "[ios] dispositions iCloud (%s) : conteneur indisponible, gardées sur l'appareil\n", why);
        return;
    }
    NSURL *file = [[container URLByAppendingPathComponent:@"Documents" isDirectory:YES]
                   URLByAppendingPathComponent:@(rt_layout_sync_file(ipad))];
    NSString *state = nil;
    [file getResourceValue:&state forKey:NSURLUbiquitousItemDownloadingStatusKey error:nil];
    if (state && ![state isEqualToString:NSURLUbiquitousItemDownloadingStatusCurrent])
        [NSFileManager.defaultManager startDownloadingUbiquitousItemAtURL:file error:nil];
    // P2_LAYOUT_SYNC_TEST_FORGET=1 (device checks): this device's layouts dropped first, so
    // what comes back is iCloud's. Once per launch.
    static int forgot;
    if (!forgot++ && getenv("P2_LAYOUT_SYNC_TEST_FORGET")) {
        NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
        for (int l = 0; l < RT_IOS_LAYOUT_COUNT; ++l) [d removeObjectForKey:layout_key((rt_ios_layout)l)];
        [d removeObjectForKey:kTimes];
        fprintf(stderr, "[ios] dispositions iCloud : essai, dispositions de l'appareil oubliées\n");
    }
    static rt_layout_set local, cloud, back;   // ~9 KB each: off the stack
    local_set(&local);
    NSString *text = read_file(file);
    rt_layout_set_parse(text.UTF8String, ipad, &cloud);
    back = cloud;
    // From iCloud into this device: newer layouts of this kind.
    rt_layout_set before = local;
    int taken = rt_layout_set_merge(&local, &cloud, ipad);
    if (taken) {
        NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
        NSMutableDictionary *times = [[d dictionaryForKey:kTimes] mutableCopy] ?: [NSMutableDictionary dictionary];
        for (int i = 0; i < local.count; ++i) {
            const rt_layout_entry *was = rt_layout_set_get(&before, local.e[i].key);
            if (was && !strcmp(was->value, local.e[i].value)) continue;
            [d setObject:@(local.e[i].value) forKey:@(local.e[i].key)];
            times[@(local.e[i].key)] = @(local.e[i].time);
        }
        [d setObject:times forKey:kTimes];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSNotificationCenter.defaultCenter postNotificationName:@"P2SettingsChanged" object:@"P2TouchLayout"];
        });
    }
    // From this device into iCloud: what iCloud lacks or has older.
    int sent = rt_layout_set_merge(&back, &local, ipad);
    BOOL ok = YES;
    if (sent || (!text && local.count)) {
        static char out[RT_LAYOUT_SYNC_MAX * (RT_LAYOUT_SYNC_VALUE + 96) + 64];
        ok = rt_layout_set_format(&back, ipad, out, sizeof(out)) && write_file(file, @(out));
    }
    set_status(ok ? [NSString stringWithFormat:@"Shared with your other %@ through iCloud.", kind_name()]
                  : @"Couldn't write to iCloud just now; layouts are saved on this device and will be sent later.");
    fprintf(stderr, "[ios] dispositions iCloud (%s, %s) : %d reçue(s), %d envoyée(s)%s\n", why,
            rt_layout_sync_file(ipad), taken, ok ? sent : 0, ok ? "" : ", écriture impossible");
}

void rt_ios_layouts_sync(const char *why) {
    if (!queue) {
        queue = dispatch_queue_create("p2.layouts", DISPATCH_QUEUE_SERIAL);
        [NSNotificationCenter.defaultCenter addObserverForName:UIApplicationWillEnterForegroundNotification object:nil
                                                         queue:nil usingBlock:^(NSNotification *n) {
            (void)n; rt_ios_layouts_sync("retour");
        }];
    }
    char reason[64];
    snprintf(reason, sizeof(reason), "%s", why);
    NSString *r = @(reason);
    dispatch_async(queue, ^{ sync_now(r.UTF8String); });
}
#endif
