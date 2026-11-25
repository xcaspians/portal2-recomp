// A connected TV's picture size and settings (see tv_output.h).
#include "tv_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int rt_tv_enabled(const char *value) {
    if (!value || !*value) return 1;   // on by default
    return strcmp(value, "0") && strcasecmp(value, "off") && strcasecmp(value, "false") && strcasecmp(value, "no");
}

static int even(double v) { int n = (int)(v + .5); return n - (n & 1); }

int rt_tv_render_size(int tv_w, int tv_h, const char *resolution, int *w, int *h) {
    if (tv_w <= 0 || tv_h <= 0) return 0;
    int lines = tv_h > 2160 ? 2160 : tv_h;
    if (resolution && *resolution && strcmp(resolution, "auto")) {
        char *end = NULL;
        long wanted = strtol(resolution, &end, 10);
        if (end && (!*end || !strcmp(end, "p")) && (wanted == 1080 || wanted == 1440 || wanted == 2160))
            lines = (int)wanted < tv_h ? (int)wanted : tv_h;
    }
    double aspect = (double)tv_w / tv_h;
    *h = even(lines);
    *w = even(lines * aspect);
    if (*w > tv_w) { *w = even(tv_w); *h = even(tv_w / aspect); }
    return *w > 0 && *h > 0;
}

int rt_tv_frame_cap(const char *value, int tv_hz) {
    int cap = tv_hz > 0 ? tv_hz : 60;
    if (value && *value && strcmp(value, "auto")) {
        char *end = NULL;
        long n = strtol(value, &end, 10);
        if (end && !*end && n >= 1) cap = (int)n;
    }
    return tv_hz > 0 && cap > tv_hz ? tv_hz : cap;
}

int rt_tv_mode_command(int w, int h, char *out, unsigned long size) {
    if (w <= 0 || h <= 0) return 0;
    int n = snprintf(out, size, "mat_setvideomode %d %d 1", w, h);
    return n > 0 && (unsigned long)n < size;
}

int rt_tv_simulated(const char *value, int *w, int *h) {
    if (!value || !*value) return 0;
    int a = 0, b = 0;
    char extra;
    if (sscanf(value, "%dx%d%c", &a, &b, &extra) != 2 || a < 320 || b < 200 || a > 7680 || b > 4320) return 0;
    *w = a; *h = b;
    return 1;
}
