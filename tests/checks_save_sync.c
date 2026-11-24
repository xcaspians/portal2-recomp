// iCloud save sync rules (runtime/ios/save_sync_rules.c): never overwrite
// silently, never sync a deletion, keep both copies in a conflict.
#include "checks.h"
#include <string.h>
#include "../runtime/ios/save_sync_rules.c"

static rt_sync_file file(long long mtime, long long size) { return (rt_sync_file){1, mtime, size}; }
static const rt_sync_file none = {0, 0, 0};

RT_CHECK(save_sync_new_and_unchanged_files) {
    rt_sync_record never = {0};
    assert(rt_sync_decide(file(100, 5), none, &never) == RT_SYNC_UPLOAD);     // new here
    assert(rt_sync_decide(none, file(100, 5), &never) == RT_SYNC_DOWNLOAD);   // new in iCloud
    assert(rt_sync_decide(none, none, &never) == RT_SYNC_NOTHING);
    assert(rt_sync_decide(file(100, 5), file(101, 5), &never) == RT_SYNC_NOTHING);   // same, dates to 2 s
    assert(rt_sync_decide(file(100, 5), file(100, 5), NULL) == RT_SYNC_NOTHING);
    rt_sync_record synced = {1, file(100, 5), file(100, 5)};
    assert(rt_sync_decide(file(100, 5), file(100, 5), &synced) == RT_SYNC_NOTHING);
}

RT_CHECK(save_sync_changes_go_one_way) {
    rt_sync_record synced = {1, file(100, 5), file(100, 5)};
    assert(rt_sync_decide(file(200, 6), file(100, 5), &synced) == RT_SYNC_UPLOAD);    // saved here since
    assert(rt_sync_decide(file(100, 5), file(300, 7), &synced) == RT_SYNC_DOWNLOAD);  // saved on the other device
    assert(rt_sync_decide(file(100, 9), file(100, 5), &synced) == RT_SYNC_UPLOAD);    // same date, new size
}

RT_CHECK(save_sync_conflicts_keep_both) {
    rt_sync_record synced = {1, file(100, 5), file(100, 5)};
    // both devices saved "quick" since the last sync
    assert(rt_sync_decide(file(200, 6), file(300, 7), &synced) == RT_SYNC_CONFLICT);
    // two devices with a save of the same name, never synced: never pick one
    rt_sync_record never = {0};
    assert(rt_sync_decide(file(200, 6), file(300, 7), &never) == RT_SYNC_CONFLICT);
    assert(rt_sync_decide(file(300, 7), file(200, 6), NULL) == RT_SYNC_CONFLICT);
}

RT_CHECK(save_sync_deletions_stay_put) {
    rt_sync_record synced = {1, file(100, 5), file(100, 5)};
    // deleted in iCloud (on the other device), unchanged here: not re-uploaded, not deleted here
    assert(rt_sync_decide(file(100, 5), none, &synced) == RT_SYNC_NOTHING);
    // deleted here, unchanged in iCloud: not downloaded again, not deleted there
    assert(rt_sync_decide(none, file(100, 5), &synced) == RT_SYNC_NOTHING);
    // deleted on one side but saved again on the other: the new save travels
    assert(rt_sync_decide(file(200, 6), none, &synced) == RT_SYNC_UPLOAD);
    assert(rt_sync_decide(none, file(300, 7), &synced) == RT_SYNC_DOWNLOAD);
}

RT_CHECK(save_sync_conflict_names) {
    struct tm when = {0};
    when.tm_year = 2026 - 1900; when.tm_mon = 10; when.tm_mday = 24; when.tm_hour = 14; when.tm_min = 5;
    char out[128];
    rt_sync_conflict_name("quick.sav", "Jacksen's iPad mini", &when, out, sizeof(out));
    assert(!strcmp(out, "quick_Jacksens-iPad-mini_2026-11-24_1405.sav"));
    rt_sync_conflict_name("quick.tga", "Jacksen\xE2\x80\x99s iPhone", &when, out, sizeof(out));   // curly apostrophe
    assert(!strcmp(out, "quick_Jacksens-iPhone_2026-11-24_1405.tga"));
    rt_sync_conflict_name("autosave.sav", "", &when, out, sizeof(out));
    assert(!strcmp(out, "autosave_other-device_2026-11-24_1405.sav"));
    rt_sync_conflict_name("a.sav", "  ;;Very/Long Device Name That Goes On And On  ", &when, out, sizeof(out));
    assert(!strcmp(out, "a_Very-Long-Device-Name-Th_2026-11-24_1405.sav"));   // device cut at 24 characters
    assert(!strchr(out, '/') && !strchr(out, ' ') && !strchr(out, ';'));
}

RT_CHECK(save_sync_moves_saves_and_screenshots_only) {
    assert(rt_sync_wanted("quick.sav") && rt_sync_wanted("autosave01.SAV") && rt_sync_wanted("quick.tga"));
    assert(!rt_sync_wanted("quick.hl1") && !rt_sync_wanted("config.cfg") && !rt_sync_wanted(".sav"));
    assert(!rt_sync_wanted(".hidden.sav") && !rt_sync_wanted("steam_autocloud.vdf"));
}
