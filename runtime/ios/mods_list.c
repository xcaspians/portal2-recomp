// Custom maps for the in-game menu (see mods_list.h).
#include "mods_list.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rt_mods_addon_folder(const char *name) {
    static const char prefix[] = "portal2_dlc";
    if (strncmp(name, prefix, sizeof(prefix) - 1)) return 0;
    const char *digits = name + sizeof(prefix) - 1;
    if (!*digits || *digits == '0') return 0;
    for (const char *p = digits; *p; ++p) if (!isdigit((unsigned char)*p)) return 0;
    return atoi(digits) >= 3;
}

static int typeable(const char *name, size_t length) {
    if (!length || name[0] == '/' || name[length - 1] == '/') return 0;
    for (size_t i = 0; i < length; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (!isalnum(c) && c != '_' && c != '-' && c != '.' && c != '/') return 0;
        if (c == '.' && i + 1 < length && name[i + 1] == '.') return 0;
        if (c == '/' && i + 1 < length && name[i + 1] == '/') return 0;
    }
    return 1;
}

// The part of the name the console takes, and its origin.
static int split(const char *relative, const char **name, size_t *length, char *origin, size_t size) {
    size_t n = strlen(relative);
    if (n < 5 || strcasecmp(relative + n - 4, ".bsp")) return 0;
    if (!strncmp(relative, "portal2/maps/custom/", 20)) {
        *name = relative + 13;   // "custom/..."
        snprintf(origin, size, "custom");
    } else if (!strncmp(relative, "portal2/maps/workshop/", 22)) {
        *name = relative + 13;   // "workshop/..."
        snprintf(origin, size, "workshop");
    } else {
        const char *slash = strchr(relative, '/');
        char folder[64];
        if (!slash || (size_t)(slash - relative) >= sizeof(folder)) return 0;
        memcpy(folder, relative, (size_t)(slash - relative)); folder[slash - relative] = 0;
        if (!rt_mods_addon_folder(folder) || strncmp(slash, "/maps/", 6)) return 0;
        *name = slash + 6;
        if (strchr(*name, '/')) return 0;   // the engine finds maps/<name>.bsp only
        snprintf(origin, size, "%s", folder);
    }
    *length = (size_t)(relative + n - 4 - *name);
    return typeable(*name, *length);
}

int rt_mods_map_name(const char *relative, char *out, size_t size) {
    const char *name; size_t length; char origin[64];
    if (!split(relative, &name, &length, origin, sizeof(origin)) || length >= size) return 0;
    memcpy(out, name, length); out[length] = 0;
    return 1;
}

void rt_mods_map_origin(const char *relative, char *out, size_t size) {
    const char *name; size_t length;
    if (!split(relative, &name, &length, out, size) && size) out[0] = 0;
}
