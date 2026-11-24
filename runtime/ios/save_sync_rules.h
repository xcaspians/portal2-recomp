// iCloud save sync (icloud_saves_ios.m): what to do with one save file, given
// its state on this device, in iCloud and at the last sync. Pure C, checked on
// the Mac by tests/checks_save_sync.c. The rules follow tools/save_sync.py
// (the cable sync): nothing is silently overwritten, deletions are never
// synced, and a save changed on both sides is kept twice.
#pragma once
#include <stddef.h>
#include <time.h>

typedef struct {
    int present;
    long long mtime;   // seconds
    long long size;
} rt_sync_file;

typedef struct {
    int known;               // synced before: both sides as they were then
    rt_sync_file local, remote;
} rt_sync_record;

typedef enum {
    RT_SYNC_NOTHING,         // in step, or deleted on one side (never synced)
    RT_SYNC_UPLOAD,          // new or changed here only
    RT_SYNC_DOWNLOAD,        // new or changed in iCloud only
    RT_SYNC_CONFLICT,        // changed on both sides, or different with no record: keep both
} rt_sync_action;

// Same file: same size, dates within 2 s (copies keep the date to the second).
int rt_sync_same(rt_sync_file a, rt_sync_file b);
rt_sync_action rt_sync_decide(rt_sync_file local, rt_sync_file remote, const rt_sync_record *record);

// Name of the other device's copy in a conflict: "quick.sav" from
// "Caspian's iPad mini" at 14:05 on 24 Nov 2026 ->
// "quick_Caspians-iPad-mini_2026-11-24_1405.sav" (letters, digits and '-'
// only, at most 24 characters for the device; "other-device" if empty).
// The game lists saves by date, so the newer of the two shows first.
void rt_sync_conflict_name(const char *name, const char *device, const struct tm *when, char *out, size_t size);
// Files the sync moves: the game's saves and their screenshots.
int rt_sync_wanted(const char *name);
