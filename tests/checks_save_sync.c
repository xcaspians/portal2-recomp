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
    rt_sync_conflict_name("quick.sav", "Caspian's iPad mini", &when, out, sizeof(out));
    assert(!strcmp(out, "quick_Caspians-iPad-mini_2026-11-24_1405.sav"));
    rt_sync_conflict_name("quick.tga", "Caspian\xE2\x80\x99s iPhone", &when, out, sizeof(out));   // curly apostrophe
    assert(!strcmp(out, "quick_Caspians-iPhone_2026-11-24_1405.tga"));
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

// Two syncs in a row, as icloud_saves_ios.m runs them: decide, copy (which
// may fail), then rt_sync_after for the record. A side whose file is still
// downloading from iCloud is skipped by the caller (audit, round 4).
typedef struct { rt_sync_file local, remote; rt_sync_record rec; } Sides;
static rt_sync_action step(Sides *s, int copy_ok) {
    rt_sync_action a = rt_sync_decide(s->local, s->remote, &s->rec);
    if (copy_ok && a == RT_SYNC_UPLOAD) s->remote = s->local;
    if (copy_ok && a == RT_SYNC_DOWNLOAD) s->local = s->remote;
    s->rec = rt_sync_after(a, copy_ok, s->rec, s->local, s->remote);
    return a;
}

RT_CHECK(save_sync_local_deletion_stays_deleted_on_the_next_syncs) {
    Sides s = {none, file(100, 5), {1, file(100, 5), file(100, 5)}};   // deleted here since the last sync
    assert(step(&s, 1) == RT_SYNC_NOTHING);
    assert(step(&s, 1) == RT_SYNC_NOTHING);   // was DOWNLOAD: the deleted save came back
    assert(step(&s, 1) == RT_SYNC_NOTHING);
    s.remote = file(300, 7);                   // the other device saves it again: that one travels
    assert(step(&s, 1) == RT_SYNC_DOWNLOAD);
    Sides r = {file(100, 5), none, {1, file(100, 5), file(100, 5)}};   // deleted in iCloud
    assert(step(&r, 1) == RT_SYNC_NOTHING && step(&r, 1) == RT_SYNC_NOTHING);
}

RT_CHECK(save_sync_failed_copy_keeps_the_record) {
    // Uploading L2 fails (iCloud full); the other device then saves R3.
    Sides s = {file(200, 6), file(100, 5), {1, file(100, 5), file(100, 5)}};
    assert(step(&s, 0) == RT_SYNC_UPLOAD);
    s.remote = file(300, 7);
    // Was DOWNLOAD (L2, never synced, replaced): both changed, both kept.
    assert(step(&s, 1) == RT_SYNC_CONFLICT);
    // A failed first copy of a never-synced file leaves no record either.
    Sides n = {file(200, 6), none, {0}};
    assert(step(&n, 0) == RT_SYNC_UPLOAD && !n.rec.known);
}

RT_CHECK(save_sync_conflict_record_and_success) {
    Sides s = {file(200, 6), file(300, 7), {0}};
    assert(step(&s, 1) == RT_SYNC_CONFLICT);
    assert(s.rec.known && rt_sync_same(s.rec.local, file(200, 6)));
    rt_sync_record kept = {1, file(1, 1), file(1, 1)};
    rt_sync_record after = rt_sync_after(RT_SYNC_CONFLICT, 0, kept, file(200, 6), file(300, 7));
    assert(after.known && rt_sync_same(after.local, file(1, 1)) && rt_sync_same(after.remote, file(1, 1)));
}
