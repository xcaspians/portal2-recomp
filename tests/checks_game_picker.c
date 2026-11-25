// The Portal app's game picker (runtime/ios/game_picker.c): where each game's
// files are, what "Remove game data" deletes (never saves, never the other
// game's files), and which game starts without asking.
#include "checks.h"
#include <string.h>
#include "../runtime/ios/game_picker.c"

static const rt_picker_game p2 = {"portal2", "portal2", "", "portal2-userdata"};
static const rt_picker_game mel = {"mel", "portal_stories", "mel", "mel-userdata"};

RT_CHECK(game_picker_folders_per_game) {
    char out[128];
    // Portal 2's data stays where existing installs have it.
    assert(!strcmp(rt_picker_data_folder(&p2, 0, out, sizeof(out)), "game/portal2"));
    assert(!strcmp(rt_picker_data_folder(&p2, 1, out, sizeof(out)), "prep"));
    assert(!strcmp(rt_picker_data_folder(&mel, 0, out, sizeof(out)), "mel/game/portal_stories"));
    assert(!strcmp(rt_picker_data_folder(&mel, 2, out, sizeof(out)), "mel/linux"));
    assert(!rt_picker_data_folder(&mel, 3, out, sizeof(out)));
}

RT_CHECK(game_picker_remove_keeps_saves_and_other_games) {
    char out[128];
    for (int i = 0; rt_picker_removable(&mel, i, out, sizeof(out)); ++i) {
        assert(!strncmp(out, "mel/", 4));                   // only Mel's own folder
        assert(!strstr(out, "userdata") && !strstr(out, "SAVE"));
    }
    for (int i = 0; rt_picker_removable(&p2, i, out, sizeof(out)); ++i) {
        assert(strcmp(out, "mel") && strncmp(out, "mel/", 4) && !strstr(out, "userdata"));
    }
    assert(!strcmp(rt_picker_removable(&p2, 0, out, sizeof(out)), "game"));
    assert(!rt_picker_removable(&p2, 3, out, sizeof(out)));
}

RT_CHECK(game_picker_autostart) {
    const char *names[] = {"portal2", "mel"};
    int both[] = {1, 1}, only_p2[] = {1, 0};
    assert(!rt_picker_autostart(names, both, 2, NULL, NULL, 0));                     // picker
    assert(!strcmp(rt_picker_autostart(names, both, 2, "mel", NULL, 0), "mel"));     // P2_GAME_CHOICE
    assert(!rt_picker_autostart(names, both, 2, "hl2", NULL, 0));                    // unknown: picker
    assert(!strcmp(rt_picker_autostart(names, both, 2, NULL, "mel", 1), "mel"));     // remembered
    assert(!rt_picker_autostart(names, both, 2, NULL, "mel", 0));                    // remember off
    assert(!rt_picker_autostart(names, only_p2, 2, NULL, "mel", 1));                 // removed since: picker
    const char *one[] = {"portal2"};
    assert(!strcmp(rt_picker_autostart(one, only_p2, 1, NULL, NULL, 0), "portal2")); // one game: no picker
    char s[32];
    rt_picker_size(11500000000LL, s, sizeof(s)); assert(!strcmp(s, "11.5 GB"));
    rt_picker_size(820000000LL, s, sizeof(s)); assert(!strcmp(s, "820 MB"));
    rt_picker_size(-1, s, sizeof(s)); assert(!strcmp(s, "Not installed"));
}
