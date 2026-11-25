// The Portal app's game picker (runtime/ios/game_picker.c): where each game's
// files are, what "Remove game data" deletes (never saves, never the other
// game's files), and which game starts without asking.
#include "checks.h"
#include <string.h>
#include "../runtime/ios/game_picker.c"

static const rt_picker_game p2 = {"portal2", "portal2", "", "portal2-userdata", NULL};
static const rt_picker_game mel = {"mel", "portal_stories", "mel", "mel-userdata", NULL};

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
    // Portal 2 is the base the other games run on: nothing of it is removable.
    assert(!rt_picker_can_remove(&p2) && rt_picker_can_remove(&mel));
    for (int i = -1; i < 4; ++i) assert(!rt_picker_removable(&p2, i, out, sizeof(out)));
    // Its size still counts its folders (the picker showed Portal 2 at 0 KB).
    assert(!strcmp(rt_picker_owned(&p2, 0, out, sizeof(out)), "game") &&
           !strcmp(rt_picker_owned(&p2, 2, out, sizeof(out)), "linux") && !rt_picker_owned(&p2, 3, out, sizeof(out)));
    assert(!strcmp(rt_picker_owned(&mel, 1, out, sizeof(out)), "mel/prep"));
    // Round 21: an episode plays from Half-Life 2's data: nothing of it is removable.
    const rt_picker_game ep1 = {"ep1", "hl2_complete", "hl2", "ep1-userdata", "hl2"};
    assert(!rt_picker_can_remove(&ep1) && !rt_picker_removable(&ep1, 0, out, sizeof(out)));
    assert(!strcmp(rt_picker_owned(&ep1, 0, out, sizeof(out)), "hl2/game"));
    assert(!strcmp(rt_picker_removable(&mel, 0, out, sizeof(out)), "mel/game"));
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

RT_CHECK(game_picker_slim_needs_portal2) {
    char out[64];
    assert(!strcmp(rt_picker_slim_marker(&mel, out, sizeof(out)), "mel/.p2-slim"));
    assert(!rt_picker_slim_marker(&p2, out, sizeof(out)));     // Portal 2 is the base itself
    assert(rt_picker_playable(1, 0, 0));                        // full copy: on its own
    assert(rt_picker_playable(1, 1, 1));
    assert(!rt_picker_playable(1, 1, 0));                       // slim without Portal 2: a sentence, not a crash
    assert(!rt_picker_playable(0, 0, 1));
}
