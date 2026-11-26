// Round 26: custom button layouts (Edit button layout) through iCloud, per
// device kind. iPhone layouts go to other iPhones, iPad layouts to other
// iPads, never across: an iPhone reads and writes only the iPhone file and
// the P2TouchLayout.phone* keys, an iPad only the iPad file and the
// P2TouchLayout.pad* keys. The file sits in the app's existing iCloud
// container (iCloud.<bundle id>, the saves' container), Documents/Layouts/.
// Each entry carries the time it was saved; the newer one wins. Rules here
// (checked on the Mac: tests/checks_layout_sync.c), files and preferences in
// layout_sync_ios.m.
#pragma once
#include <stddef.h>

#define RT_LAYOUT_SYNC_MAX 8          // game layouts per device kind (3 today)
#define RT_LAYOUT_SYNC_VALUE 1024     // a stored layout ("v1 jump=...")

typedef struct {
    char key[64];                     // "P2TouchLayout.pad.l4d2"
    double time;                      // seconds since 1970 when it was saved
    char value[RT_LAYOUT_SYNC_VALUE];
} rt_layout_entry;
typedef struct {
    int count;
    rt_layout_entry e[RT_LAYOUT_SYNC_MAX];
} rt_layout_set;

// The file of this device kind, under the container's Documents.
const char *rt_layout_sync_file(int ipad);
// The key belongs to this device kind: "P2TouchLayout.pad" or "P2TouchLayout.pad.<game>".
int rt_layout_sync_key_of_kind(const char *key, int ipad);
// Sets an entry (added or replaced); 0 if the key isn't this kind's or the set is full.
int rt_layout_set_put(rt_layout_set *s, int ipad, const char *key, double time, const char *value);
const rt_layout_entry *rt_layout_set_get(const rt_layout_set *s, const char *key);
// The file's text: "sauce-layouts v1 ipad|iphone" then "<key>\t<time>\t<value>" lines.
// Parsing keeps only this kind's entries, and nothing from the other kind's file.
int rt_layout_set_format(const rt_layout_set *s, int ipad, char *out, size_t size);
int rt_layout_set_parse(const char *text, int ipad, rt_layout_set *out);
// Newer entries of `from` into `into` (same key: the later time wins, a tie
// keeps `into`'s). Returns how many entries of `into` changed.
int rt_layout_set_merge(rt_layout_set *into, const rt_layout_set *from, int ipad);
