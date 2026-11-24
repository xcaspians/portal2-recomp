// iCloud save sync (see icloud_saves.h).
//
// Local: Documents/<userdata>/<mod>/SAVE/<id>/ (portal2-userdata/portal2/SAVE/
// <SteamID of the stub>/). Remote: <iCloud container>/Documents/SAVE/<id>/.
// Only *.sav and *.tga move (rt_sync_wanted). For each file the decision comes
// from save_sync_rules.c, with the state of both sides at the last sync kept in
// Documents/<userdata>/icloud-sync.json:
//   upload / download: the changed side is copied over the other one; a local
//     file about to be replaced is first copied to
//     Documents/<userdata>/icloud-backups/<date>/<id>/ (14 days kept);
//   conflict: the other device's copy is kept under a new name with that
//     device's name and the time (both devices end up with both), and this
//     device's copy goes to iCloud under the original name;
//   a deletion is never synced.
// iCloud's own conflicts (two devices uploading the same name at once) are
// kept the same way. Copies keep the file's date (the game lists saves by
// date) and go through NSFileCoordinator, as iCloud Drive requires.
//
// When: at start, about five seconds after the game writes a save (the folder
// is checked every 10 s: the game rewrites files in place, which a folder
// watch would miss), when the app goes to the background and when it comes
// back. Everything runs on one serial queue, never on the game's thread.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <string.h>
#include "env_ios.h"
#include "game_ios.h"
#include "icloud_saves.h"
#include "save_sync_rules.h"

static NSString *const kLastSync = @"P2ICloudLastSync";
static dispatch_queue_t queue;
static dispatch_source_t timer;
static int running;              // setting on and started (queue)
static long long seen_print;     // local saves as of the last sync (queue)
static long long last_print;     // as of the last check, and when it changed
static CFTimeInterval changed_at;
static NSString *status = @"Off";

static void set_status(NSString *text) {
    @synchronized(kLastSync) { status = text; }
}
void rt_ios_icloud_status(char *out, size_t size) {
    NSString *text;
    @synchronized(kLastSync) { text = status; }
    NSDate *last = [NSUserDefaults.standardUserDefaults objectForKey:kLastSync];
    if ([text isEqualToString:@"Synced"] && [last isKindOfClass:NSDate.class]) {
        NSDateFormatter *f = [NSDateFormatter new];
        f.doesRelativeDateFormatting = YES;
        f.dateStyle = NSDateFormatterMediumStyle; f.timeStyle = NSDateFormatterShortStyle;
        text = [@"Last synced " stringByAppendingString:[f stringFromDate:last]];
    }
    snprintf(out, size, "%s", text.UTF8String);
}

static int wanted_here(void) {
    // Portal 2 only for now: Half-Life 2 and Portal keep their saves on the device.
    return !strcmp(rt_ios_current_game()->mod, "portal2") && rt_ios_env_flag("P2_ICLOUD_SAVES", 1);
}
static NSString *userdata(void) {
    NSString *docs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    return [docs stringByAppendingPathComponent:@(rt_ios_current_game()->userdata)];
}
static NSURL *local_root(void) {
    return [NSURL fileURLWithPath:[NSString pathWithComponents:@[userdata(), @(rt_ios_current_game()->mod), @"SAVE"]]
                      isDirectory:YES];
}

// ---- files ----
static rt_sync_file stat_url(NSURL *url) {
    NSDictionary *a = [NSFileManager.defaultManager attributesOfItemAtPath:url.path error:nil];
    if (!a) return (rt_sync_file){0, 0, 0};
    return (rt_sync_file){1, (long long)a.fileModificationDate.timeIntervalSince1970, (long long)a.fileSize};
}
static NSMutableDictionary<NSString *, NSValue *> *list_local(NSURL *dir) {
    NSMutableDictionary *found = [NSMutableDictionary dictionary];
    for (NSString *name in [NSFileManager.defaultManager contentsOfDirectoryAtPath:dir.path error:nil]) {
        if (!rt_sync_wanted(name.UTF8String)) continue;
        rt_sync_file f = stat_url([dir URLByAppendingPathComponent:name]);
        if (f.present) found[name] = [NSValue valueWithBytes:&f objCType:@encode(rt_sync_file)];
    }
    return found;
}
// Remote files ready to read; those still in iCloud are asked for and counted
// in *waiting (the next sync gets them).
static NSMutableDictionary<NSString *, NSValue *> *list_remote(NSURL *dir, int *waiting) {
    NSFileManager *files = NSFileManager.defaultManager;
    NSMutableDictionary *found = [NSMutableDictionary dictionary];
    NSArray *keys = @[NSURLUbiquitousItemDownloadingStatusKey];
    for (NSURL *url in [files contentsOfDirectoryAtURL:dir includingPropertiesForKeys:keys options:0 error:nil]) {
        NSString *name = url.lastPathComponent;
        if ([name hasPrefix:@"."] && [name hasSuffix:@".icloud"]) {   // placeholder of a file not downloaded
            NSString *real = [name substringWithRange:NSMakeRange(1, name.length - 8)];
            if (!rt_sync_wanted(real.UTF8String)) continue;
            [files startDownloadingUbiquitousItemAtURL:[dir URLByAppendingPathComponent:real] error:nil];
            ++*waiting;
            continue;
        }
        if (!rt_sync_wanted(name.UTF8String)) continue;
        NSString *state = nil;
        [url getResourceValue:&state forKey:NSURLUbiquitousItemDownloadingStatusKey error:nil];
        if (state && ![state isEqualToString:NSURLUbiquitousItemDownloadingStatusCurrent]) {
            [files startDownloadingUbiquitousItemAtURL:url error:nil];
            ++*waiting;
            continue;
        }
        rt_sync_file f = stat_url(url);
        if (f.present) found[name] = [NSValue valueWithBytes:&f objCType:@encode(rt_sync_file)];
    }
    return found;
}
static rt_sync_file entry(NSDictionary<NSString *, NSValue *> *d, NSString *name) {
    rt_sync_file f = {0, 0, 0};
    [d[name] getValue:&f size:sizeof(f)];
    return f;
}
// Copies with the source's date, through a hidden temporary file, coordinated.
static BOOL copy_file(NSURL *from, NSURL *to) {
    NSFileManager *files = NSFileManager.defaultManager;
    NSFileCoordinator *coordinator = [[NSFileCoordinator alloc] initWithFilePresenter:nil];
    __block BOOL ok = NO;
    NSError *error = nil;
    [coordinator coordinateReadingItemAtURL:from options:NSFileCoordinatorReadingWithoutChanges
                           writingItemAtURL:to options:NSFileCoordinatorWritingForReplacing error:&error
                                 byAccessor:^(NSURL *source, NSURL *destination) {
        NSDate *date = [files attributesOfItemAtPath:source.path error:nil].fileModificationDate;
        NSURL *tmp = [destination.URLByDeletingLastPathComponent
                      URLByAppendingPathComponent:[@".p2sync-" stringByAppendingString:destination.lastPathComponent]];
        [files removeItemAtURL:tmp error:nil];
        if (![files copyItemAtURL:source toURL:tmp error:nil]) return;
        if ([files fileExistsAtPath:destination.path])
            ok = [files replaceItemAtURL:destination withItemAtURL:tmp backupItemName:nil options:0
                        resultingItemURL:nil error:nil];
        else ok = [files moveItemAtURL:tmp toURL:destination error:nil];
        if (ok && date) [files setAttributes:@{NSFileModificationDate: date} ofItemAtPath:destination.path error:nil];
        [files removeItemAtURL:tmp error:nil];
    }];
    if (!ok) fprintf(stderr, "[ios] iCloud : copie %s -> %s échouée%s%s\n", from.lastPathComponent.UTF8String,
                     to.path.UTF8String, error ? " : " : "", error ? error.localizedDescription.UTF8String : "");
    return ok;
}
static void backup_local(NSURL *file, NSString *save_id) {
    NSFileManager *files = NSFileManager.defaultManager;
    if (![files fileExistsAtPath:file.path]) return;
    NSDateFormatter *f = [NSDateFormatter new];
    f.dateFormat = @"yyyy-MM-dd";
    NSString *root = [userdata() stringByAppendingPathComponent:@"icloud-backups"];
    NSString *dir = [NSString pathWithComponents:@[root, [f stringFromDate:NSDate.date], save_id]];
    [files createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
    NSString *copy = [dir stringByAppendingPathComponent:file.lastPathComponent];
    [files removeItemAtPath:copy error:nil];
    [files copyItemAtPath:file.path toPath:copy error:nil];
    // Days older than two weeks go.
    NSDate *limit = [NSDate dateWithTimeIntervalSinceNow:-14 * 86400];
    for (NSString *day in [files contentsOfDirectoryAtPath:root error:nil]) {
        NSDate *when = [f dateFromString:day];
        if (when && [when compare:limit] == NSOrderedAscending)
            [files removeItemAtPath:[root stringByAppendingPathComponent:day] error:nil];
    }
}
static NSString *conflict_name(NSString *name, NSString *device, NSDate *when) {
    time_t t = (time_t)(when ?: NSDate.date).timeIntervalSince1970;
    struct tm local;
    localtime_r(&t, &local);
    char out[256];
    rt_sync_conflict_name(name.UTF8String, device.UTF8String, &local, out, sizeof(out));
    return @(out);
}
// iCloud's own conflicts: each other version kept under its device's name.
static int keep_icloud_conflicts(NSURL *remote_dir) {
    int kept = 0;
    for (NSURL *url in [NSFileManager.defaultManager contentsOfDirectoryAtURL:remote_dir includingPropertiesForKeys:nil
                                                                       options:0 error:nil]) {
        if (!rt_sync_wanted(url.lastPathComponent.UTF8String)) continue;
        NSArray<NSFileVersion *> *versions = [NSFileVersion unresolvedConflictVersionsOfItemAtURL:url];
        for (NSFileVersion *v in versions) {
            NSString *name = conflict_name(url.lastPathComponent, v.localizedNameOfSavingComputer, v.modificationDate);
            if (copy_file(v.URL, [remote_dir URLByAppendingPathComponent:name])) ++kept;
            v.resolved = YES;
        }
        if (versions.count) [NSFileVersion removeOtherVersionsOfItemAtURL:url error:nil];
    }
    return kept;
}

// ---- record of the last sync ----
static NSString *record_path(void) { return [userdata() stringByAppendingPathComponent:@"icloud-sync.json"]; }
static NSMutableDictionary *load_record(void) {
    NSData *data = [NSData dataWithContentsOfFile:record_path()];
    id parsed = data ? [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil] : nil;
    return [parsed isKindOfClass:NSMutableDictionary.class] ? parsed : [NSMutableDictionary dictionary];
}
static rt_sync_record record_of(NSDictionary *record, NSString *key) {
    NSArray *v = record[key];
    if (![v isKindOfClass:NSArray.class] || v.count != 4) return (rt_sync_record){0};
    long long lm = [v[0] longLongValue], ls = [v[1] longLongValue], rm = [v[2] longLongValue], rs = [v[3] longLongValue];
    return (rt_sync_record){1, {ls >= 0, lm, ls}, {rs >= 0, rm, rs}};
}
static void remember(NSMutableDictionary *record, NSString *key, rt_sync_file local, rt_sync_file remote) {
    record[key] = @[@(local.mtime), @(local.present ? local.size : -1), @(remote.mtime), @(remote.present ? remote.size : -1)];
}

// ---- one sync (queue) ----
static long long fingerprint(void) {
    long long sum = 0;
    NSURL *root = local_root();
    for (NSString *save_id in [NSFileManager.defaultManager contentsOfDirectoryAtPath:root.path error:nil])
        for (NSValue *v in list_local([root URLByAppendingPathComponent:save_id]).allValues) {
            rt_sync_file f;
            [v getValue:&f size:sizeof(f)];
            sum = sum * 31 + f.mtime * 7 + f.size;
        }
    return sum;
}
static void sync_now(const char *why);
static void retry_later(void) {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 15 * NSEC_PER_SEC), queue, ^{ sync_now("téléchargements iCloud"); });
}
static void sync_now(const char *why) {
    if (!running) return;
    NSFileManager *files = NSFileManager.defaultManager;
    NSURL *container = [files URLForUbiquityContainerIdentifier:nil];   // never on the main thread
    if (!container) {
        set_status(@"iCloud isn't available: sign in to iCloud, or iCloud isn't set up for this app yet");
        fprintf(stderr, "[ios] iCloud : conteneur indisponible (%s)\n", why);
        return;
    }
    set_status(@"Syncing…");
    NSURL *remote_root = [container URLByAppendingPathComponent:@"Documents/SAVE" isDirectory:YES];
    NSURL *local = local_root();
    [files createDirectoryAtURL:remote_root withIntermediateDirectories:YES attributes:nil error:nil];
    [files createDirectoryAtURL:local withIntermediateDirectories:YES attributes:nil error:nil];
    NSMutableSet<NSString *> *ids = [NSMutableSet set];
    for (NSURL *root in @[local, remote_root])
        for (NSString *name in [files contentsOfDirectoryAtPath:root.path error:nil]) {
            BOOL dir = NO;
            if (![name hasPrefix:@"."] && [files fileExistsAtPath:[root.path stringByAppendingPathComponent:name]
                                                      isDirectory:&dir] && dir) [ids addObject:name];
        }
    NSMutableDictionary *record = load_record();
    int up = 0, down = 0, kept = 0, waiting = 0;
    for (NSString *save_id in ids) {
        NSURL *l = [local URLByAppendingPathComponent:save_id isDirectory:YES];
        NSURL *r = [remote_root URLByAppendingPathComponent:save_id isDirectory:YES];
        [files createDirectoryAtURL:l withIntermediateDirectories:YES attributes:nil error:nil];
        [files createDirectoryAtURL:r withIntermediateDirectories:YES attributes:nil error:nil];
        kept += keep_icloud_conflicts(r);
        NSDictionary *lf = list_local(l), *rf = list_remote(r, &waiting);
        NSMutableSet<NSString *> *names = [NSMutableSet setWithArray:lf.allKeys];
        [names addObjectsFromArray:rf.allKeys];
        for (NSString *name in [names.allObjects sortedArrayUsingSelector:@selector(compare:)]) {
            NSString *key = [save_id stringByAppendingPathComponent:name];
            NSURL *lu = [l URLByAppendingPathComponent:name], *ru = [r URLByAppendingPathComponent:name];
            rt_sync_record before = record_of(record, key);
            switch (rt_sync_decide(entry(lf, name), entry(rf, name), &before)) {
            case RT_SYNC_NOTHING: break;
            case RT_SYNC_UPLOAD: up += copy_file(lu, ru); break;
            case RT_SYNC_DOWNLOAD:
                backup_local(lu, save_id);
                down += copy_file(ru, lu);
                break;
            case RT_SYNC_CONFLICT: {
                // The other device's copy under its own name, on both sides; ours keeps the name.
                NSFileVersion *theirs = [NSFileVersion currentVersionOfItemAtURL:ru];
                NSString *other = conflict_name(name, theirs.localizedNameOfSavingComputer, theirs.modificationDate);
                NSURL *ol = [l URLByAppendingPathComponent:other], *orr = [r URLByAppendingPathComponent:other];
                if (copy_file(ru, ol) && copy_file(ol, orr) && copy_file(lu, ru)) {
                    ++kept;
                    remember(record, [save_id stringByAppendingPathComponent:other], stat_url(ol), stat_url(orr));
                    fprintf(stderr, "[ios] iCloud : conflit sur %s, l'autre copie gardée sous %s\n", name.UTF8String,
                            other.UTF8String);
                }
                break;
            }
            }
            rt_sync_file now_local = stat_url(lu), now_remote = rf[name] || [files fileExistsAtPath:ru.path]
                ? stat_url(ru) : (rt_sync_file){0, 0, 0};
            // A file still in iCloud stays as recorded until it arrives.
            if (!rf[name] && !now_remote.present && before.known) now_remote = before.remote;
            remember(record, key, now_local, now_remote);
        }
    }
    NSData *json = [NSJSONSerialization dataWithJSONObject:record options:NSJSONWritingPrettyPrinted error:nil];
    [json writeToFile:record_path() atomically:YES];
    seen_print = fingerprint();
    [NSUserDefaults.standardUserDefaults setObject:NSDate.date forKey:kLastSync];
    set_status(waiting ? @"Waiting for files from iCloud…" : @"Synced");
    fprintf(stderr, "[ios] iCloud (%s) : %d envoyées, %d reçues, %d conflits gardés en double, %d en attente\n", why, up,
            down, kept, waiting);
    if (waiting) retry_later();
}

// ---- start, stop, triggers ----
static void start_running(void) {
    running = 1;
    seen_print = last_print = 0;
    dispatch_async(queue, ^{ sync_now("lancement"); });
    if (timer) return;
    timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
    dispatch_source_set_timer(timer, dispatch_time(DISPATCH_TIME_NOW, 10 * NSEC_PER_SEC), 10 * NSEC_PER_SEC, NSEC_PER_SEC);
    dispatch_source_set_event_handler(timer, ^{
        if (!running) return;
        long long print = fingerprint();
        CFTimeInterval now = CACurrentMediaTime();
        if (print != last_print) { last_print = print; changed_at = now; return; }
        // A save written since the last sync, and left alone for five seconds.
        if (print != seen_print && now - changed_at >= 5) sync_now("sauvegarde");
    });
    dispatch_resume(timer);
}

void rt_ios_icloud_start(void) {
    if (queue) return;
    queue = dispatch_queue_create("p2.icloud", DISPATCH_QUEUE_SERIAL);
    NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
    [center addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil
                    usingBlock:^(NSNotification *n) {
        (void)n;
        if (!running) return;
        __block UIBackgroundTaskIdentifier task = UIBackgroundTaskInvalid;
        task = [UIApplication.sharedApplication beginBackgroundTaskWithName:@"iCloud saves" expirationHandler:^{
            [UIApplication.sharedApplication endBackgroundTask:task];
        }];
        dispatch_async(queue, ^{
            sync_now("arrière-plan");
            dispatch_async(dispatch_get_main_queue(), ^{ [UIApplication.sharedApplication endBackgroundTask:task]; });
        });
    }];
    [center addObserverForName:UIApplicationWillEnterForegroundNotification object:nil queue:nil
                    usingBlock:^(NSNotification *n) {
        (void)n;
        if (running) dispatch_async(queue, ^{ sync_now("retour"); });
    }];
    if (wanted_here()) start_running();
}

void rt_ios_icloud_reload(void) {
    if (!queue) return;   // not started yet: the start reads the setting
    if (wanted_here() && !running) start_running();
    else if (!wanted_here() && running) {
        running = 0;   // the timer stays, idle
        set_status(@"Off");
    }
}
#endif
