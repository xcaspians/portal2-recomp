// Game picker rules (see game_picker.h).
#include "game_picker.h"
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

int rt_picker_can_remove(const rt_picker_game *g) { return g && strcmp(g->name, "portal2") != 0; }

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

void rt_picker_size(long long bytes, char *out, size_t size) {
    if (bytes < 0) snprintf(out, size, "Not installed");
    else if (bytes >= 1000LL * 1000 * 1000) snprintf(out, size, "%.1f GB", (double)bytes / 1e9);
    else if (bytes >= 1000 * 1000) snprintf(out, size, "%.0f MB", (double)bytes / 1e6);
    else snprintf(out, size, "%lld KB", bytes / 1000);
}
