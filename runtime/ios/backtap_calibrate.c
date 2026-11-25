// Back-tap calibration (see backtap_calibrate.h).
#include "backtap_calibrate.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmp_float(const void *a, const void *b) {
    float x = *(const float *)a, y = *(const float *)b;
    return (x > y) - (x < y);
}
// q-th quantile (0..1) of n values (copied, sorted).
static float quantile(const float *v, int n, float q) {
    if (n <= 0) return 0;
    float *s = malloc((size_t)n * sizeof(*s));
    if (!s) return v[0];
    memcpy(s, v, (size_t)n * sizeof(*s));
    qsort(s, (size_t)n, sizeof(*s), cmp_float);
    float r = s[(int)(q * (float)(n - 1) + 0.5f)];
    free(s);
    return r;
}
static float clampf(float v, float lo, float hi) { return v < lo ? lo : v > hi ? hi : v; }

// Side score in the device frame (back_tap.c), turned to the screen's side.
static float score(const rt_btcal_tap *t, float w) { return w * t->rot_x - (float)t->orient * t->rot_z; }
// > 0: on the expected side by that much; < 0: on the wrong side.
static float margin(const rt_btcal_tap *t, float w, int invert) {
    return (float)(t->expected * t->orient * (invert ? -1 : 1)) * score(t, w);
}

int rt_btcal_judge(const rt_btcal_tap *t, const rt_backtap_params *p) {
    int by_accel = t->peak >= p->accel_min && t->jerk >= p->jerk_min;
    int by_gyro = t->peak >= p->gyro_accel_min && t->jerk >= p->jerk_min && fabsf(t->rot_x) >= p->gyro_rot_min;
    if (!by_accel && !by_gyro) return 0;
    float s = score(t, p->side_x_weight);
    if (fabsf(s) < p->rot_min || fabsf(s) < p->side_margin) return 0;
    return (s > 0 ? 1 : -1) * t->orient * (p->invert ? -1 : 1);
}

static void count(const rt_btcal_tap *t, int n, const rt_backtap_params *p, int *right, int *wrong) {
    *right = *wrong = 0;
    for (int i = 0; i < n; ++i) {
        int side = rt_btcal_judge(&t[i], p);
        if (side == t[i].expected) ++*right;
        else if (side) ++*wrong;
    }
}

int rt_btcal_fit_taps(const rt_btcal_tap *t, int n, const rt_btcal_noise *noise, const rt_backtap_params *def,
                      rt_btcal_fit *out) {
    int left = 0, right = 0;
    for (int i = 0; i < n; ++i) left += t[i].expected < 0, right += t[i].expected > 0;
    if (left < 4 || right < 4) return 0;
    memset(out, 0, sizeof(*out));
    out->taps = n;
    out->noise = noise ? noise->a_rms : 0;
    float *peaks = malloc((size_t)n * sizeof(float)), *jerks = malloc((size_t)n * sizeof(float)),
          *rots = malloc((size_t)n * sizeof(float));
    if (!peaks || !jerks || !rots) { free(peaks); free(jerks); free(rots); return 0; }
    for (int i = 0; i < n; ++i) { peaks[i] = t[i].peak; jerks[i] = t[i].jerk; rots[i] = fabsf(t[i].rot_x); }
    // Thresholds: 60 % of the softest taps, over the noise, never above the defaults.
    float floor_a = noise ? 3 * noise->a_rms : 0, floor_j = noise ? 3 * noise->jerk_rms : 0,
          floor_w = noise ? 3 * noise->w_rms : 0;
    out->accel_min = clampf(0.6f * quantile(peaks, n, 0.2f), fmaxf(floor_a, 0.02f), def->accel_min);
    out->jerk_min = clampf(0.6f * quantile(jerks, n, 0.2f), fmaxf(floor_j, 1.0f), def->jerk_min);
    out->gyro_rot_min = clampf(0.6f * quantile(rots, n, 0.2f), fmaxf(floor_w, 0.08f), def->gyro_rot_min);
    free(peaks); free(jerks); free(rots);
    // Side weight: most taps on their side, then the widest smallest margin.
    float best_w = def->side_x_weight, best_min = -1e9f;
    int best_ok = -1;
    for (int k = 0; k <= 40; ++k) {
        float w = 0.05f * (float)k, low = 1e9f;
        int ok = 0;
        for (int i = 0; i < n; ++i) {
            float m = margin(&t[i], w, def->invert);
            if (m > 0) { ++ok; if (m < low) low = m; }
        }
        if (ok > best_ok || (ok == best_ok && low > best_min)) { best_ok = ok; best_min = low; best_w = w; }
    }
    out->side_x_weight = best_w;
    // Margin: above the strongest wrong-side tap, keeping 3 good taps in 4.
    float worst = 0, *good = malloc((size_t)n * sizeof(float));
    int ngood = 0;
    for (int i = 0; i < n; ++i) {
        float m = margin(&t[i], best_w, def->invert);
        if (m < 0 && -m > worst) worst = -m;
        if (m > 0 && good) good[ngood++] = m;
    }
    float cap = ngood ? quantile(good, ngood, 0.25f) : 0;
    free(good);
    out->side_margin = worst > 0 ? fminf(worst * 1.1f, cap) : 0;
    // Before and after, on these same taps.
    rt_backtap_params p = *def;
    count(t, n, &p, &out->before, &out->wrong_before);
    rt_btcal_apply(&p, out);
    count(t, n, &p, &out->after, &out->wrong_after);
    for (int i = 0; i < n; ++i) {
        float s = fabsf(score(&t[i], p.side_x_weight));
        out->uncertain += s >= p.rot_min && s < p.side_margin;
    }
    return 1;
}

int rt_btcal_format(const rt_btcal_fit *f, long long when, char *out, size_t size) {
    return snprintf(out, size, "v1 accel=%.4f jerk=%.3f gyro_rot=%.4f side_x=%.3f margin=%.4f noise=%.4f when=%lld",
                    f->accel_min, f->jerk_min, f->gyro_rot_min, f->side_x_weight, f->side_margin, f->noise, when);
}

int rt_btcal_parse(const char *text, rt_btcal_fit *f, long long *when) {
    memset(f, 0, sizeof(*f));
    *when = 0;
    if (!text || strncmp(text, "v1 ", 3)) return 0;
    int got = sscanf(text, "v1 accel=%f jerk=%f gyro_rot=%f side_x=%f margin=%f noise=%f when=%lld", &f->accel_min,
                     &f->jerk_min, &f->gyro_rot_min, &f->side_x_weight, &f->side_margin, &f->noise, when);
    if (got != 7) return 0;
    // Values a fit can give (a hand-edited or damaged setting falls back to the defaults).
    return f->accel_min > 0 && f->accel_min < 1 && f->jerk_min > 0 && f->jerk_min < 100 && f->gyro_rot_min > 0 &&
           f->gyro_rot_min < 5 && f->side_x_weight >= 0 && f->side_x_weight <= 2 && f->side_margin >= 0 &&
           f->side_margin < 5;
}

void rt_btcal_apply(rt_backtap_params *p, const rt_btcal_fit *f) {
    p->accel_min = f->accel_min;
    p->jerk_min = f->jerk_min;
    p->gyro_rot_min = f->gyro_rot_min;
    p->side_x_weight = f->side_x_weight;
    p->side_margin = f->side_margin;
}
