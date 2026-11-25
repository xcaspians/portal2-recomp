// The device itself: model identifier, memory, free space (see device_ios.h).
#include "device_ios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

void rt_ios_device_model(char *out, size_t size) {
    if (!size) return;
    out[0] = 0;
    // P2_DEVICE_MODEL: tests and the simulator set up as another phone. The
    // simulator names the device it plays in SIMULATOR_MODEL_IDENTIFIER
    // (hw.machine is the Mac's "arm64" there).
    const char *forced = getenv("P2_DEVICE_MODEL");
    if (!forced || !*forced) forced = getenv("SIMULATOR_MODEL_IDENTIFIER");
    if (forced && *forced) { snprintf(out, size, "%s", forced); return; }
    size_t n = size;
    if (sysctlbyname("hw.machine", out, &n, NULL, 0) || !n) snprintf(out, size, "unknown");
    out[size - 1] = 0;
}

unsigned rt_ios_device_ram_mb(void) {
    const char *forced = getenv("P2_DEVICE_RAM_MB");
    if (forced && atoi(forced) > 0) return (unsigned)atoi(forced);
    uint64_t bytes = 0;
    size_t n = sizeof(bytes);
    if (sysctlbyname("hw.memsize", &bytes, &n, NULL, 0)) return 0;
    return (unsigned)(bytes >> 20);
}

#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#endif

long long rt_ios_free_bytes(const char *path) {
#if TARGET_OS_IPHONE
    // What the Files app and Settings > Storage show: purgeable space counts
    // ("important usage"), statfs alone would show less.
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)path, (CFIndex)strlen(path), true);
    if (url) {
        CFTypeRef value = NULL;
        long long bytes = -1;
        if (CFURLCopyResourcePropertyForKey(url, kCFURLVolumeAvailableCapacityForImportantUsageKey, &value, NULL) &&
            value && CFGetTypeID(value) == CFNumberGetTypeID())
            CFNumberGetValue((CFNumberRef)value, kCFNumberLongLongType, &bytes);
        if (value) CFRelease(value);
        CFRelease(url);
        if (bytes >= 0) return bytes;
    }
#endif
    struct statfs st;
    if (statfs(path, &st)) return -1;
    return (long long)st.f_bavail * (long long)st.f_bsize;
}

int rt_ios_device_report_write(const char *file, const char *documents) {
    FILE *f = fopen(file, "w");
    if (!f) return 0;
    char model[64];
    rt_ios_device_model(model, sizeof(model));
    long long free_bytes = rt_ios_free_bytes(documents);
    int ok = fprintf(f, "model=%s\nram_mb=%u\n", model, rt_ios_device_ram_mb()) > 0;
    if (free_bytes >= 0) ok &= fprintf(f, "free_bytes=%lld\n", free_bytes) > 0;
    ok &= fclose(f) == 0;
    return ok && free_bytes >= 0;
}
