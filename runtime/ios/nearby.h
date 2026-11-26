// Round 25: "Play nearby", Portal 2 co-op between devices on the same Wi-Fi
// (pure C rules, checked by tests/checks_nearby.c; the Bonjour and UIKit side is
// nearby_ios.m).
//
// A hosting Sauce advertises `_sauce-coop._tcp` (port: the game's server port,
// TXT game=portal2 proto=1); other copies browse for it and list the hosts by
// their Bonjour name, the device's name. Join connects the game to the host's
// address. While playing, each side asks the game server how many players it has
// (Source's A2S_INFO query over UDP, the one server browsers use): the host sees
// its friend arrive and leave, the guest notices a host that stopped answering.
#pragma once
#include <stddef.h>

#define RT_NEARBY_SERVICE "_sauce-coop._tcp"
#define RT_NEARBY_PROTO "1"
#define RT_NEARBY_MAX 16
#define RT_NEARBY_LOBBY "mp_coop_lobby_3"   // the co-op hub, where both players land

// ---- Discovery: the hosts found on the network ----
typedef struct {
    char name[64];    // Bonjour service name: the device's name ("Caspian's iPad Pro")
    char game[24];    // TXT game=
    int usable;       // same game and protocol: can be joined
} rt_nearby_host;
typedef struct {
    rt_nearby_host hosts[RT_NEARBY_MAX];
    int count;
    char own[64];     // our own advertisement's name, never listed
} rt_nearby_list;

// A TXT record (DNS-SD wire format: length-prefixed "key=value" strings): the
// value of `key` into out, 1 if present.
int rt_nearby_txt_get(const unsigned char *txt, size_t len, const char *key, char *out, size_t size);
// Builds the TXT record a host advertises (game, proto); returns its length.
size_t rt_nearby_txt_build(unsigned char *out, size_t size, const char *game);
// Browse callbacks: a service appeared (with its TXT once resolved, NULL before)
// or went away. Returns 1 when the list changed. Our own name is skipped; the
// list stays sorted by name, without duplicates.
int rt_nearby_add(rt_nearby_list *l, const char *name, const unsigned char *txt, size_t txt_len, const char *game);
int rt_nearby_remove(rt_nearby_list *l, const char *name);

// ---- The game server's answer (A2S_INFO) ----
// The query a client sends (25 bytes: FF FF FF FF 'T' "Source Engine Query\0").
size_t rt_nearby_info_query(unsigned char *out, size_t size);
// An A2S_INFO reply: players and max players (and the map), 1 if well formed.
int rt_nearby_parse_info(const unsigned char *reply, size_t len, int *players, int *max_players, char *map, size_t map_size);

// ---- The session: what each side shows as it goes ----
typedef enum { RT_NB_IDLE, RT_NB_HOST_STARTING, RT_NB_HOST_WAITING, RT_NB_HOST_PLAYING,
               RT_NB_JOIN_CONNECTING, RT_NB_JOIN_PLAYING } rt_nearby_phase;
typedef enum {
    RT_NB_NONE,
    RT_NB_ADVERTISE,      // host: the lobby is up: advertise, show "Waiting for a friend…"
    RT_NB_FRIEND_JOINED,  // host: a second player: hide the waiting message
    RT_NB_FRIEND_LEFT,    // host: the friend left or dropped: message, back to the menu
    RT_NB_HOST_FAILED,    // host: the lobby never loaded
    RT_NB_JOINED,         // guest: in the level
    RT_NB_JOIN_FAILED,    // guest: never got in
    RT_NB_HOST_LOST,      // guest: the host stopped answering: message, back to the menu
    RT_NB_ENDED,          // either: the player left the level themself: nothing to say
} rt_nearby_event;
typedef struct {
    rt_nearby_phase phase;
    double since;        // the phase started
    double level_since;  // the current level came up (0: not in a level)
    double out_since;    // left the level (0: in one, or never in one)
    double heard_at;     // the server's last answer
    double few_since;    // host: fewer than two players since (0: two)
    int players;         // the server's last count (-1: never heard)
    char peer[64];       // guest: the host's name
} rt_nearby_session;
// Timings (seconds).
#define RT_NB_HOST_START_LIMIT 120   // lobby load (the mini, the first launch's shader builds)
#define RT_NB_JOIN_LIMIT 60          // connect + map load on the guest
#define RT_NB_GRACE 10               // after a level comes up, before counting players
#define RT_NB_FRIEND_GONE 8          // host: one player for this long
#define RT_NB_SILENT 20              // guest in a level: no answer for this long
#define RT_NB_OUT_LIMIT 45           // out of a level (a map change) for this long
#define RT_NB_SILENT_OUT 30          // guest out of a level (the host loading a map): no answer for this long
void rt_nearby_start_host(rt_nearby_session *s, double now);
void rt_nearby_start_join(rt_nearby_session *s, double now, const char *peer);
void rt_nearby_stop(rt_nearby_session *s);
// The server answered (players >= 0) at `now`.
void rt_nearby_heard(rt_nearby_session *s, double now, int players);
// Once per frame or so: the event to act on (one at a time).
rt_nearby_event rt_nearby_step(rt_nearby_session *s, double now, int in_level);

// ---- Plain-English texts ----
const char *rt_nearby_denied_message(const char *app);   // local network access declined
// The message for an event (NULL: none), with the friend's or host's name.
const char *rt_nearby_message(rt_nearby_event e, const char *peer, char *out, size_t size);
