// Round 24 bug 3: Portal 2's online co-op (Standard and Challenge Mode co-op)
// finds friends through Steam lobbies. On iPad and iPhone there is no Steam:
// the stub (steam_stub.c) can't create or list a lobby and no answer ever
// comes, so the game sat on "Searching for Friends..." forever. Now, once the
// game has waited P2_ONLINE_TIMEOUT seconds (default 8) without reaching a
// level, Sauce says why and what to try instead (online_notice_ios.m).
#pragma once

typedef struct { double asked_at; } rt_online_wait;
// The game asked Steam for a lobby (create, list or join) at `now` (seconds).
void rt_online_asked(rt_online_wait *w, double now);
// 1, once, when the notice is due: still waiting after `timeout` seconds and no
// level loaded (a level means co-op started: nothing to say).
int rt_online_due(rt_online_wait *w, double now, double timeout, int in_level);
const char *rt_online_title(void);
const char *rt_online_message(void);
