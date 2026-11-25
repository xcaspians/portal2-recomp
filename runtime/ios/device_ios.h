// The device itself (runtime/ios/device_ios.c): model identifier, memory and
// free space, for the first-launch defaults (device_defaults.c) and the
// device report that tools/ios_device.sh reads before a copy
// (P2_DEVICE_REPORT=1, ios/main_ios.c).
#pragma once
#include <stddef.h>

// "iPhone14,5", "iPad19,1"... P2_DEVICE_MODEL overrides it (tests, a
// simulator set up as another phone); the simulator's own model otherwise.
void rt_ios_device_model(char *out, size_t size);
// Physical memory in MB (P2_DEVICE_RAM_MB overrides it); 0 if unknown. iOS
// reports a little under the nominal size (a 4 GB phone: about 3,700).
unsigned rt_ios_device_ram_mb(void);
// Bytes free on the volume holding path, as Settings > Storage counts them; -1 if unknown.
long long rt_ios_free_bytes(const char *path);
// Writes "model=…\nram_mb=…\nfree_bytes=…\n" to file; 1 when complete.
int rt_ios_device_report_write(const char *file, const char *documents);
// This process's memory: footprint now and at its peak, and what is left
// before iOS ends the app (-1 if unknown); 0 if unreadable.
int rt_ios_memory_footprint(long long *now, long long *peak, long long *available);
// One log line with them ("[ios] mémoire (<why>) : ..."): at each level load,
// so a crash on a 4 GB phone can be diagnosed from game.log.
void rt_ios_memory_log(const char *why);
