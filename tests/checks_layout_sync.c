// Round 26: custom layouts through iCloud, iPhone and iPad apart (runtime/ios/layout_sync.c).
#include "checks.h"
#include <string.h>
#include "../runtime/ios/layout_sync.c"

// The split rule: an iPad never takes an iPhone layout, nor the reverse.
RT_CHECK(layout_sync_iphone_and_ipad_never_mix) {
    assert(rt_layout_sync_key_of_kind("P2TouchLayout.pad", 1) && rt_layout_sync_key_of_kind("P2TouchLayout.pad.l4d2", 1));
    assert(!rt_layout_sync_key_of_kind("P2TouchLayout.phone", 1) && !rt_layout_sync_key_of_kind("P2TouchLayout.phone.hl2", 1));
    assert(rt_layout_sync_key_of_kind("P2TouchLayout.phone.hl2", 0) && !rt_layout_sync_key_of_kind("P2TouchLayout.pad", 0));
    assert(!rt_layout_sync_key_of_kind("P2TouchLayout.padx", 1) && !rt_layout_sync_key_of_kind("P2TouchLayout.pad.", 1));
    assert(strcmp(rt_layout_sync_file(1), rt_layout_sync_file(0)));

    rt_layout_set phone = {0}, pad = {0}, read = {0};
    assert(rt_layout_set_put(&phone, 0, "P2TouchLayout.phone", 100, "v1 jump=0.500,0.500,1.00"));
    assert(!rt_layout_set_put(&phone, 0, "P2TouchLayout.pad", 100, "v1 jump=0.100,0.100,1.00"));   // not an iPhone key
    assert(rt_layout_set_put(&pad, 1, "P2TouchLayout.pad.l4d2", 200, "v1 use=0.300,0.300,1.20"));
    char text[4096];
    assert(rt_layout_set_format(&phone, 0, text, sizeof(text)));
    // The iPhone file read on an iPad: nothing taken.
    assert(!rt_layout_set_parse(text, 1, &read) && read.count == 0);
    assert(rt_layout_set_parse(text, 0, &read) && read.count == 1);
    assert(!strcmp(rt_layout_set_get(&read, "P2TouchLayout.phone")->value, "v1 jump=0.500,0.500,1.00"));
    // A hand-edited iPad file with an iPhone line in it: the line is dropped.
    const char *mixed = "sauce-layouts v1 ipad\nP2TouchLayout.phone\t300\tv1 jump=0.900,0.900,1.00\n"
                        "P2TouchLayout.pad\t300\tv1 jump=0.200,0.200,1.00\n";
    assert(rt_layout_set_parse(mixed, 1, &read) && read.count == 1 && rt_layout_set_get(&read, "P2TouchLayout.pad"));
    // Merging an iPhone set into the iPad's changes nothing.
    assert(rt_layout_set_merge(&pad, &phone, 1) == 0 && pad.count == 1);
}

// Newer wins, per game layout; the file round-trips.
RT_CHECK(layout_sync_newer_wins) {
    rt_layout_set local = {0}, cloud = {0}, back = {0};
    rt_layout_set_put(&local, 1, "P2TouchLayout.pad", 100, "v1 jump=0.100,0.100,1.00");
    rt_layout_set_put(&local, 1, "P2TouchLayout.pad.l4d2", 500, "v1 use=0.500,0.500,1.00");
    rt_layout_set_put(&cloud, 1, "P2TouchLayout.pad", 200, "v1 jump=0.200,0.200,1.00");      // newer: taken
    rt_layout_set_put(&cloud, 1, "P2TouchLayout.pad.l4d2", 400, "v1 use=0.400,0.400,1.00");  // older: ignored
    rt_layout_set_put(&cloud, 1, "P2TouchLayout.pad.hl2", 50, "v1 fire=0.900,0.600,1.00");   // new game: taken
    assert(rt_layout_set_merge(&local, &cloud, 1) == 2);
    assert(!strcmp(rt_layout_set_get(&local, "P2TouchLayout.pad")->value, "v1 jump=0.200,0.200,1.00"));
    assert(!strcmp(rt_layout_set_get(&local, "P2TouchLayout.pad.l4d2")->value, "v1 use=0.500,0.500,1.00"));
    assert(rt_layout_set_get(&local, "P2TouchLayout.pad.hl2"));
    assert(rt_layout_set_merge(&local, &cloud, 1) == 0);   // again: nothing new
    char text[4096];
    assert(rt_layout_set_format(&local, 1, text, sizeof(text)) && rt_layout_set_parse(text, 1, &back));
    assert(back.count == 3 && rt_layout_set_get(&back, "P2TouchLayout.pad")->time == 200);
    // Damaged lines are skipped, not fatal.
    assert(rt_layout_set_parse("sauce-layouts v1 ipad\ngarbage\nP2TouchLayout.pad\tx\tv1\n", 1, &back) && back.count == 0);
}
