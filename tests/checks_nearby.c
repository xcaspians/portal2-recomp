// Round 25: Play nearby's discovery and session rules (runtime/ios/nearby.c).
#include "checks.h"
#include <string.h>
#include "../runtime/ios/nearby.c"

RT_CHECK(nearby_txt) {
    unsigned char txt[64];
    size_t n = rt_nearby_txt_build(txt, sizeof(txt), "portal2");
    assert(n == 1 + 12 + 1 + 7);   // "game=portal2", "proto=1"
    char v[16];
    assert(rt_nearby_txt_get(txt, n, "game", v, sizeof(v)) && !strcmp(v, "portal2"));
    assert(rt_nearby_txt_get(txt, n, "proto", v, sizeof(v)) && !strcmp(v, "1"));
    assert(!rt_nearby_txt_get(txt, n, "gam", v, sizeof(v)));         // a key prefix isn't the key
    assert(!rt_nearby_txt_get(txt, n - 3, "proto", v, sizeof(v)));   // truncated record
    assert(!rt_nearby_txt_get(NULL, 0, "game", v, sizeof(v)));
}

RT_CHECK(nearby_discovery_list) {
    rt_nearby_list l = {0};
    snprintf(l.own, sizeof(l.own), "Caspian's iPad mini");
    unsigned char ok[64], other[64], old[32];
    size_t okn = rt_nearby_txt_build(ok, sizeof(ok), "portal2");
    size_t othern = rt_nearby_txt_build(other, sizeof(other), "l4d2");
    size_t oldn = 0;
    old[oldn++] = 12; memcpy(old + oldn, "game=portal2", 12); oldn += 12;
    old[oldn++] = 7;  memcpy(old + oldn, "proto=9", 7); oldn += 7;

    assert(!rt_nearby_add(&l, "Caspian's iPad mini", ok, okn, "portal2"));   // our own advertisement: never listed
    assert(rt_nearby_add(&l, "Caspian's iPhone", NULL, 0, "portal2"));        // seen, TXT not resolved yet
    assert(l.count == 1 && !l.hosts[0].usable);
    assert(rt_nearby_add(&l, "Caspian's iPhone", ok, okn, "portal2"));        // resolved: joinable
    assert(l.count == 1 && l.hosts[0].usable);
    assert(!rt_nearby_add(&l, "Caspian's iPhone", ok, okn, "portal2"));       // seen again on another interface: no duplicate
    assert(!rt_nearby_add(&l, "Caspian's iPhone", NULL, 0, "portal2"));       // a bare re-announce keeps the TXT
    assert(l.hosts[0].usable);
    assert(rt_nearby_add(&l, "Bedroom Mac", ok, okn, "portal2"));
    assert(rt_nearby_add(&l, "Den iPad", other, othern, "portal2"));          // another game: listed, not joinable
    assert(rt_nearby_add(&l, "Old Sauce", old, oldn, "portal2"));             // another protocol version: not joinable
    assert(l.count == 4);
    assert(!strcmp(l.hosts[0].name, "Bedroom Mac") && !strcmp(l.hosts[1].name, "Caspian's iPhone"));   // sorted
    assert(!l.hosts[2].usable && !l.hosts[3].usable);
    assert(rt_nearby_remove(&l, "Caspian's iPhone"));                         // stopped hosting: gone
    assert(!rt_nearby_remove(&l, "Caspian's iPhone"));
    assert(l.count == 3 && !strcmp(l.hosts[1].name, "Den iPad"));
    assert(!rt_nearby_add(&l, "", ok, okn, "portal2"));
    for (int i = 0; i < RT_NEARBY_MAX + 4; ++i) {
        char name[16]; snprintf(name, sizeof(name), "host %02d", i);
        rt_nearby_add(&l, name, ok, okn, "portal2");
    }
    assert(l.count == RT_NEARBY_MAX);                                         // bounded
}

RT_CHECK(nearby_server_info) {
    unsigned char q[32];
    assert(rt_nearby_info_query(q, sizeof(q)) == 25 && q[4] == 'T' && q[24] == 0);
    // The iPad Pro's real answer while hosting (round 25 item 1), one player of two.
    static const unsigned char reply[] = "\xff\xff\xff\xffI\x11Player\0mp_coop_lobby_3\0portal2\0Portal 2 Coop\0l\x02\x01\x02\0ll\0\0002.0.0.1";
    int players = -1, max = -1;
    char map[32];
    assert(rt_nearby_parse_info(reply, sizeof(reply) - 1, &players, &max, map, sizeof(map)));
    assert(players == 1 && max == 2 && !strcmp(map, "mp_coop_lobby_3"));
    assert(!rt_nearby_parse_info(reply, 20, &players, &max, map, sizeof(map)));   // cut short
    assert(!rt_nearby_parse_info(q, 25, &players, &max, map, sizeof(map)));       // the query isn't an answer
}

RT_CHECK(nearby_host_session) {
    rt_nearby_session s;
    rt_nearby_start_host(&s, 0);
    assert(rt_nearby_step(&s, 30, 0) == RT_NB_NONE);              // the lobby is loading
    assert(rt_nearby_step(&s, 40, 1) == RT_NB_ADVERTISE);         // up: advertise, "Waiting for a friend…"
    rt_nearby_heard(&s, 42, 1);
    assert(rt_nearby_step(&s, 100, 1) == RT_NB_NONE);             // alone: still waiting, no timeout
    rt_nearby_heard(&s, 101, 2);
    assert(rt_nearby_step(&s, 101, 1) == RT_NB_FRIEND_JOINED);
    rt_nearby_heard(&s, 120, 1);                                  // the friend's slot empties
    assert(rt_nearby_step(&s, 125, 1) == RT_NB_NONE);             // a blip: not yet
    rt_nearby_heard(&s, 126, 2);                                  // back (reconnected)
    assert(rt_nearby_step(&s, 140, 1) == RT_NB_NONE);
    // A co-op map change: the host loads, the friend reconnects after it.
    assert(rt_nearby_step(&s, 150, 0) == RT_NB_NONE);
    rt_nearby_heard(&s, 170, 1);
    assert(rt_nearby_step(&s, 172, 1) == RT_NB_NONE);             // the new level is up, the friend still loading
    assert(rt_nearby_step(&s, 185, 1) == RT_NB_NONE);             // grace (10 s) + 8 s not over
    rt_nearby_heard(&s, 186, 2);
    assert(rt_nearby_step(&s, 200, 1) == RT_NB_NONE);
    rt_nearby_heard(&s, 201, 1);                                  // gone for good
    assert(rt_nearby_step(&s, 209.5, 1) == RT_NB_FRIEND_LEFT);
    assert(s.phase == RT_NB_IDLE && rt_nearby_step(&s, 300, 1) == RT_NB_NONE);
    char text[256];
    assert(strstr(rt_nearby_message(RT_NB_FRIEND_LEFT, "", text, sizeof(text)), "back at the menu"));

    rt_nearby_start_host(&s, 0);                                        // the lobby never comes up
    assert(rt_nearby_step(&s, RT_NB_HOST_START_LIMIT + 1, 0) == RT_NB_HOST_FAILED);
    rt_nearby_start_host(&s, 0);                                        // the host quits to the menu while waiting
    assert(rt_nearby_step(&s, 10, 1) == RT_NB_ADVERTISE);
    assert(rt_nearby_step(&s, 20, 0) == RT_NB_NONE);
    assert(rt_nearby_step(&s, 20 + RT_NB_OUT_LIMIT + 1, 0) == RT_NB_ENDED);
}

RT_CHECK(nearby_join_session) {
    rt_nearby_session s;
    char text[256];
    rt_nearby_start_join(&s, 0, "Caspian's iPad Pro");
    assert(rt_nearby_step(&s, 20, 0) == RT_NB_NONE);              // connecting, loading the map
    assert(rt_nearby_step(&s, 30, 1) == RT_NB_JOINED);
    rt_nearby_heard(&s, 32, 2);
    assert(rt_nearby_step(&s, 50, 1) == RT_NB_NONE);              // 18 s without an answer: not yet
    rt_nearby_heard(&s, 51, 2);
    // A map change: out of the level, the host silent while it loads (under 30 s).
    assert(rt_nearby_step(&s, 60, 0) == RT_NB_NONE);
    assert(rt_nearby_step(&s, 85, 0) == RT_NB_NONE);
    rt_nearby_heard(&s, 86, 2);
    assert(rt_nearby_step(&s, 95, 1) == RT_NB_NONE);
    assert(rt_nearby_step(&s, 115, 1) == RT_NB_NONE);             // in the level since 95: 20 s counted from there
    assert(rt_nearby_step(&s, 116, 1) == RT_NB_HOST_LOST);        // the host stopped answering
    assert(strstr(rt_nearby_message(RT_NB_HOST_LOST, s.peer, text, sizeof(text)), "Caspian's iPad Pro"));

    rt_nearby_start_join(&s, 0, "Mac");                                 // the host went away: we're at the menu
    assert(rt_nearby_step(&s, 20, 1) == RT_NB_JOINED);
    rt_nearby_heard(&s, 25, 2);
    assert(rt_nearby_step(&s, 26, 0) == RT_NB_NONE);
    assert(rt_nearby_step(&s, 56.5, 0) == RT_NB_HOST_LOST);       // 30 s out of a level with no answer

    rt_nearby_start_join(&s, 0, "Mac");                                 // we left ourselves; the host still answers
    assert(rt_nearby_step(&s, 20, 1) == RT_NB_JOINED);
    assert(rt_nearby_step(&s, 30, 0) == RT_NB_NONE);
    for (double t = 31; t < 30 + RT_NB_OUT_LIMIT; t += 5) { rt_nearby_heard(&s, t, 1); assert(rt_nearby_step(&s, t, 0) == RT_NB_NONE); }
    rt_nearby_heard(&s, 76, 1);
    assert(rt_nearby_step(&s, 76, 0) == RT_NB_ENDED);             // quietly

    rt_nearby_start_join(&s, 0, "Mac");                                 // never got in
    assert(rt_nearby_step(&s, RT_NB_JOIN_LIMIT + 1, 0) == RT_NB_JOIN_FAILED);
    assert(strstr(rt_nearby_message(RT_NB_JOIN_FAILED, "Mac", text, sizeof(text)), "same Wi-Fi"));
    assert(!rt_nearby_message(RT_NB_JOINED, "Mac", text, sizeof(text)));
    assert(strstr(rt_nearby_denied_message("Sauce"), "Local Network") && strstr(rt_nearby_denied_message("Sauce"), "Settings"));
}
