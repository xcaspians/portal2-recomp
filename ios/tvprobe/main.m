// Apple TV round 1 probe (round 23, docs/fork/APPLETV_RESEARCH.md): no game,
// nothing from Valve. Measures what a tvOS app on this box really gets:
//   - the memory limit (os_proc_available_memory at launch, then 64 MB steps
//     allocated and touched until close to it, each step logged and synced so
//     a kill by the system still leaves the last line);
//   - the folders it can write (Documents, Application Support, Caches, tmp),
//     the volume's free space, and a large write into Caches (then deleted);
//   - the GPU's recommended working set.
// Results: Library/Caches/tvprobe.txt (tools/tvos_probe.sh fetches it) and NSLog.
// Tunables (environment, devicectl --environment-variables): P2_PROBE_STEP_MB (64),
// P2_PROBE_STOP_MB (stop this far above the limit, 48), P2_PROBE_WRITE_GB (2).
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#include <mach/mach.h>
#include <os/proc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>

static FILE *out;
static void say(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void say(const char *fmt, ...) {
    char line[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    NSLog(@"[tvprobe] %s", line);
    if (out) { fprintf(out, "%s\n", line); fflush(out); fsync(fileno(out)); }
}
static long env_long(const char *name, long fallback) {
    const char *v = getenv(name);
    return v && *v ? strtol(v, NULL, 10) : fallback;
}
static uint64_t footprint(void) {
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    return task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&info, &count) == KERN_SUCCESS ? info.phys_footprint : 0;
}

static void folders(void) {
    NSFileManager *fm = NSFileManager.defaultManager;
    NSString *home = NSHomeDirectory();
    NSArray *dirs = @[@"Documents", @"Library/Application Support", @"Library/Caches", @"tmp"];
    NSData *mb = [NSMutableData dataWithLength:1 << 20];
    for (NSString *d in dirs) {
        NSString *dir = [home stringByAppendingPathComponent:d];
        NSError *e = nil;
        BOOL made = [fm createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:&e];
        NSString *file = [dir stringByAppendingPathComponent:@"tvprobe-write.bin"];
        BOOL wrote = made && [mb writeToFile:file options:NSDataWritingAtomic error:&e];
        say("folder %-28s %s%s", d.UTF8String, wrote ? "writable" : "NOT writable",
            wrote ? "" : [NSString stringWithFormat:@" (%@)", e.localizedDescription].UTF8String);
        [fm removeItemAtPath:file error:nil];
    }
    NSURL *caches = [NSURL fileURLWithPath:[home stringByAppendingPathComponent:@"Library/Caches"]];
    // tvOS has no "available for important / opportunistic usage" keys (unavailable in its SDK).
    NSDictionary *v = [caches resourceValuesForKeys:@[NSURLVolumeTotalCapacityKey, NSURLVolumeAvailableCapacityKey]
                                              error:nil];
    say("volume total %.2f GB, available %.2f GB", [v[NSURLVolumeTotalCapacityKey] doubleValue] / 1e9,
        [v[NSURLVolumeAvailableCapacityKey] doubleValue] / 1e9);
    // A large write into Caches, as game files would be (then removed).
    long gb = env_long("P2_PROBE_WRITE_GB", 2);
    if (gb <= 0) return;
    NSString *big = [caches.path stringByAppendingPathComponent:@"tvprobe-big.bin"];
    FILE *f = fopen(big.UTF8String, "wb");
    size_t chunk = 64u << 20;
    char *buf = calloc(1, chunk);
    double t0 = CACurrentMediaTime();
    long long done = 0;
    while (f && buf && done < gb * (1LL << 30)) {
        if (fwrite(buf, 1, chunk, f) != chunk) break;
        done += chunk;
    }
    int ok = f && fflush(f) == 0 && fsync(fileno(f)) == 0;
    if (f) fclose(f);
    double secs = CACurrentMediaTime() - t0;
    say("large write into Caches: %.2f GB of %ld GB %s, %.0f MB/s", done / 1e9, gb, ok ? "written" : "STOPPED",
        secs > 0 ? done / 1e6 / secs : 0);
    // Read it back once (as a level load would).
    f = fopen(big.UTF8String, "rb");
    t0 = CACurrentMediaTime();
    long long got = 0;
    size_t n;
    while (f && buf && (n = fread(buf, 1, chunk, f)) > 0) got += n;
    if (f) fclose(f);
    secs = CACurrentMediaTime() - t0;
    say("read back: %.2f GB, %.0f MB/s", got / 1e9, secs > 0 ? got / 1e6 / secs : 0);
    free(buf);
    [NSFileManager.defaultManager removeItemAtPath:big error:nil];
}

static void memory(void) {
    size_t step = (size_t)env_long("P2_PROBE_STEP_MB", 64) << 20;
    uint64_t stop = (uint64_t)env_long("P2_PROBE_STOP_MB", 48) << 20;
    say("memory at start: footprint %.0f MB, os_proc_available_memory %.0f MB, so the limit is about %.0f MB",
        footprint() / 1048576.0, os_proc_available_memory() / 1048576.0,
        (footprint() + os_proc_available_memory()) / 1048576.0);
    // Every block kept and written through a volatile pointer: the optimizer must
    // neither drop the allocation nor the page touches (a first version's were).
    static void *kept[4096];
    int blocks = 0;
    for (;;) {
        size_t left = os_proc_available_memory();
        if (left < stop + step || blocks >= 4096) break;
        volatile char *p = malloc(step);
        if (!p) { say("malloc failed at block %d", blocks); break; }
        for (size_t i = 0; i < step; i += 4096) p[i] = (char)(i >> 12);   // touch every page: real footprint
        kept[blocks++] = (void *)p;
        say("allocated %d MB, footprint %.0f MB, available %.0f MB", (int)((size_t)blocks * (step >> 20)),
            footprint() / 1048576.0, os_proc_available_memory() / 1048576.0);
    }
    say("memory result: peak footprint %.0f MB with %.0f MB still available (stopped %ld MB short, as asked)",
        footprint() / 1048576.0, os_proc_available_memory() / 1048576.0, env_long("P2_PROBE_STOP_MB", 48));
}

static void start_probe(UILabel *label) {
    static int started;
    if (started++) return;
    NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Caches/tvprobe.txt"];
    [NSFileManager.defaultManager createDirectoryAtPath:path.stringByDeletingLastPathComponent
                            withIntermediateDirectories:YES attributes:nil error:nil];
    out = fopen(path.UTF8String, "w");
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
        char model[64] = "";
        size_t size = sizeof(model);
        sysctlbyname("hw.machine", model, &size, NULL, 0);
        say("device %s, %s %s, physical memory %.2f GB", model, UIDevice.currentDevice.systemName.UTF8String,
            UIDevice.currentDevice.systemVersion.UTF8String, NSProcessInfo.processInfo.physicalMemory / 1e9);
        id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
        say("GPU %s, recommended working set %.0f MB", gpu.name.UTF8String, gpu.recommendedMaxWorkingSetSize / 1048576.0);
        folders();
        memory();
        say("done");
        dispatch_async(dispatch_get_main_queue(), ^{ label.text = @"Sauce probe: done."; });
    });
}

// tvOS 27's SDK uses the scene life cycle (Info.plist UIApplicationSceneManifest, project.yml).
@interface ProbeScene : UIResponder <UIWindowSceneDelegate>
@property(strong, nonatomic) UIWindow *window;
@end
@implementation ProbeScene
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)options {
    (void)session; (void)options;
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene *)scene];
    UIViewController *root = [[UIViewController alloc] init];
    UILabel *label = [[UILabel alloc] initWithFrame:self.window.bounds];
    label.text = @"Sauce probe: measuring memory and storage…";
    label.textAlignment = NSTextAlignmentCenter;
    label.textColor = UIColor.whiteColor;
    root.view.backgroundColor = UIColor.blackColor;
    [root.view addSubview:label];
    self.window.rootViewController = root;
    [self.window makeKeyAndVisible];
    start_probe(label);
}
@end

@interface ProbeDelegate : UIResponder <UIApplicationDelegate>
@end
@implementation ProbeDelegate
- (BOOL)application:(UIApplication *)app didFinishLaunchingWithOptions:(NSDictionary *)options {
    (void)app; (void)options;
    NSLog(@"[tvprobe] launched");
    return YES;
}
- (UISceneConfiguration *)application:(UIApplication *)app configurationForConnectingSceneSession:(UISceneSession *)session
                              options:(UISceneConnectionOptions *)options {
    (void)app; (void)options;
    UISceneConfiguration *c = [[UISceneConfiguration alloc] initWithName:@"Default" sessionRole:session.role];
    c.delegateClass = ProbeScene.class;
    return c;
}
@end

int main(int argc, char *argv[]) {
    @autoreleasepool { return UIApplicationMain(argc, argv, nil, NSStringFromClass(ProbeDelegate.class)); }
}
