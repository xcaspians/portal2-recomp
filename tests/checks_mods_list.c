// Maps & Mods list of the in-game menu (runtime/ios/mods_list.c): which files
// become a map name for the console, and that no name can carry a command.
#include "checks.h"
#include <string.h>
#include "../runtime/ios/mods_list.c"

static int named(const char *relative, const char *expected) {
    char out[128];
    if (!rt_mods_map_name(relative, out, sizeof(out))) return expected == NULL;
    return expected && !strcmp(out, expected);
}

RT_CHECK(mods_list_map_names) {
    assert(named("portal2/maps/custom/sp_laser_maze.bsp", "custom/sp_laser_maze"));
    assert(named("portal2/maps/custom/pack/two.BSP", "custom/pack/two"));
    assert(named("portal2/maps/workshop/123456/mp_coop_x.bsp", "workshop/123456/mp_coop_x"));
    assert(named("portal2_dlc3/maps/my_chamber.bsp", "my_chamber"));
    assert(named("portal2_dlc12/maps/v1.2-final.bsp", "v1.2-final"));
    // the game's own maps and DLC are not "custom"
    assert(named("portal2/maps/sp_a1_intro1.bsp", NULL));
    assert(named("portal2_dlc1/maps/mp_coop_tripleaxis.bsp", NULL));
    assert(named("portal2_dlc2/maps/x.bsp", NULL));
    assert(named("portal2_dlc03/maps/x.bsp", NULL));
    assert(named("portal2_dlc3/maps/sub/x.bsp", NULL));   // the engine looks in maps/ only
    // not a map
    assert(named("portal2/maps/custom/notes.txt", NULL));
    assert(named("portal2/maps/custom/.bsp", NULL));
    assert(named("portal2/maps/custom/x.bsp.txt", NULL));
    // names the console could misread: a second command, quotes, spaces, escapes
    assert(named("portal2/maps/custom/x;quit.bsp", NULL));
    assert(named("portal2/maps/custom/x quit.bsp", NULL));
    assert(named("portal2/maps/custom/\"x\".bsp", NULL));
    assert(named("portal2/maps/custom/../../cfg/x.bsp", NULL));
    assert(named("portal2/maps/custom//x.bsp", NULL));
    char small[8];
    assert(!rt_mods_map_name("portal2/maps/custom/long_name.bsp", small, sizeof(small)));   // never cut short
}

RT_CHECK(mods_list_origins_and_addons) {
    char origin[64];
    rt_mods_map_origin("portal2/maps/custom/x.bsp", origin, sizeof(origin)); assert(!strcmp(origin, "custom"));
    rt_mods_map_origin("portal2/maps/workshop/1/x.bsp", origin, sizeof(origin)); assert(!strcmp(origin, "workshop"));
    rt_mods_map_origin("portal2_dlc4/maps/x.bsp", origin, sizeof(origin)); assert(!strcmp(origin, "portal2_dlc4"));
    rt_mods_map_origin("portal2/maps/sp_a1_intro1.bsp", origin, sizeof(origin)); assert(!strcmp(origin, ""));
    assert(rt_mods_addon_folder("portal2_dlc3") && rt_mods_addon_folder("portal2_dlc10"));
    assert(!rt_mods_addon_folder("portal2_dlc1") && !rt_mods_addon_folder("portal2_dlc2"));
    assert(!rt_mods_addon_folder("portal2_dlc") && !rt_mods_addon_folder("portal2_dlc3x"));
    assert(!rt_mods_addon_folder("portal2") && !rt_mods_addon_folder("portal2_dlc03"));
}
