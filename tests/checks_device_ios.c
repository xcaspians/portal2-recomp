// The device report (runtime/ios/device_ios.c) that tools/ios_device.sh reads
// before a copy: model, memory and free space, in the shape
// tools/workshop_push.py parses.
#include "checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../runtime/ios/device_ios.c"

RT_CHECK(ios_device_report_has_model_memory_and_free_space) {
    setenv("P2_DEVICE_MODEL", "iPhone14,5", 1);
    setenv("P2_DEVICE_RAM_MB", "3700", 1);
    char dir[] = "/tmp/p2-device-XXXXXX", file[64], text[256] = {0};
    assert(mkdtemp(dir) != NULL);
    snprintf(file, sizeof(file), "%s/device-report.txt", dir);
    assert(rt_ios_device_report_write(file, dir));
    FILE *f = fopen(file, "r");
    assert(f != NULL);
    size_t n = fread(text, 1, sizeof(text) - 1, f);
    fclose(f);
    text[n] = 0;
    static const char head[] = "model=iPhone14,5\nram_mb=3700\nfree_bytes=";
    assert(!strncmp(text, head, strlen(head)));
    assert(strtoll(text + strlen(head), NULL, 10) > 0);
    unlink(file); rmdir(dir);
    unsetenv("P2_DEVICE_MODEL"); unsetenv("P2_DEVICE_RAM_MB");
    char model[64];
    rt_ios_device_model(model, sizeof(model));
    assert(model[0] != 0);                   // the Mac's own hw.machine here
    assert(rt_ios_device_ram_mb() > 1000);
}
