// Custom maps the in-game menu's "Maps & Mods" list can start (mods_list.c,
// checked on the Mac by tests/checks_mods_list.c). See docs/mods.md.
#pragma once
#include <stddef.h>

// Console name of a map from its path relative to the game folder, written to
// out: "portal2/maps/custom/x.bsp" -> "custom/x",
// "portal2/maps/workshop/123/x.bsp" -> "workshop/123/x",
// "portal2_dlc3/maps/x.bsp" -> "x" (addons from portal2_dlc3 on; dlc1 and
// dlc2 are the game's). Returns 0 for anything else, and for names the console
// could misread (only letters, digits, '_', '-', '.', '/'; no ".."), so a file
// name can never carry a second command.
int rt_mods_map_name(const char *relative, char *out, size_t size);
// Where the map comes from, for the list: "custom", "workshop" or the addon's
// folder name, written to out; "" when rt_mods_map_name refuses it.
void rt_mods_map_origin(const char *relative, char *out, size_t size);
// 1 for an addon folder name the engine mounts after the game's: portal2_dlcN, N >= 3.
int rt_mods_addon_folder(const char *name);
