// Game picker rules (see game_picker.h).
#include "game_picker.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static const char *in_data_dir(const rt_picker_game *g, const char *rest, char *out, size_t size) {
    if (g->data_dir && g->data_dir[0]) snprintf(out, size, "%s/%s", g->data_dir, rest);
    else snprintf(out, size, "%s", rest);
    return out;
}

const char *rt_picker_data_folder(const rt_picker_game *g, int which, char *out, size_t size) {
    char rest[128];
    switch (which) {
    case 0: snprintf(rest, sizeof(rest), "game/%s", g->mod); return in_data_dir(g, rest, out, size);
    case 1: return in_data_dir(g, "prep", out, size);
    case 2: return in_data_dir(g, "linux", out, size);
    default: return NULL;
    }
}

// Never Portal 2 (the others' base), never a game on another's data: removing an
// episode's files would remove Half-Life 2's (round 21).
int rt_picker_can_remove(const rt_picker_game *g) { return g && strcmp(g->name, "portal2") != 0 && !g->base; }

const char *rt_picker_owned(const rt_picker_game *g, int which, char *out, size_t size) {
    static const char *const folders[] = {"game", "prep", "linux"};
    if (which < 0 || which > 2) return NULL;
    return in_data_dir(g, folders[which], out, size);
}

const char *rt_picker_removable(const rt_picker_game *g, int which, char *out, size_t size) {
    return rt_picker_can_remove(g) ? rt_picker_owned(g, which, out, size) : NULL;
}

const char *rt_picker_slim_marker(const rt_picker_game *g, char *out, size_t size) {
    if (!g->data_dir || !g->data_dir[0]) return NULL;
    snprintf(out, size, "%s/.p2-slim", g->data_dir);
    return out;
}

int rt_picker_playable(int installed, int slim, int base_present) { return installed && (!slim || base_present); }

const char *rt_picker_autostart(const char *const *names, const int *installed, int count, const char *forced,
                                const char *remembered, int remember) {
    for (int i = 0; forced && forced[0] && i < count; ++i)
        if (!strcmp(names[i], forced)) return names[i];
    for (int i = 0; remember && remembered && i < count; ++i)
        if (!strcmp(names[i], remembered) && installed[i]) return names[i];
    if (count == 1) return names[0];   // nothing to choose
    return NULL;
}

// One line per series; a game joins by adding its name (round 23).
static const rt_picker_series series[] = {
    {"portal", "Portal", {"portal1", "portal2", "mel", "p2ce", "revolution", NULL}},
    {"halflife", "Half-Life", {"hl2", "ep1", "ep2", "lostcoast", NULL}},
    {"l4d", "Left 4 Dead", {"l4d1", "l4d2", NULL}},
};
int rt_picker_series_count(void) { return (int)(sizeof(series) / sizeof(*series)); }
const rt_picker_series *rt_picker_series_at(int i) {
    return i >= 0 && i < rt_picker_series_count() ? &series[i] : NULL;
}
const rt_picker_series *rt_picker_series_of(const char *game) {
    for (int i = 0; game && i < rt_picker_series_count(); ++i)
        for (int j = 0; series[i].games[j]; ++j)
            if (!strcmp(series[i].games[j], game)) return &series[i];
    return NULL;
}
const char *rt_picker_badge(const char *game) {
    static const char *const badges[][2] = {{"ep1", "EP1"}, {"ep2", "EP2"}, {"lostcoast", "LC"}};
    for (size_t i = 0; game && i < sizeof(badges) / sizeof(*badges); ++i)
        if (!strcmp(badges[i][0], game)) return badges[i][1];
    return NULL;
}

void rt_picker_size(long long bytes, char *out, size_t size) {
    if (bytes < 0) snprintf(out, size, "Not installed");
    else if (bytes >= 1000LL * 1000 * 1000) snprintf(out, size, "%.1f GB", (double)bytes / 1e9);
    else if (bytes >= 1000 * 1000) snprintf(out, size, "%.0f MB", (double)bytes / 1e6);
    else snprintf(out, size, "%lld KB", bytes / 1000);
}

int rt_picker_is_tap(double moved_x, double moved_y, int lifted_inside) {
    return lifted_inside && hypot(moved_x, moved_y) <= RT_PICKER_TAP_SLOP;
}
