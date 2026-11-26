// Game picker rules (pure C, checked by tests/checks_game_picker.c): what is
// installed, where each game's files are, what "Remove game data" deletes and
// keeps, and which game starts without asking.
#pragma once
#include <stddef.h>

typedef struct {
    const char *name;       // "portal2", "mel"
    const char *mod;        // its folder in game/ ("portal2", "portal_stories")
    const char *data_dir;   // "" (Documents itself) or "mel" (Documents/mel)
    const char *userdata;   // saves and settings folder in Documents, never removed
    const char *base;       // a game on another's data (round 21: the episodes on Half-Life 2's), or NULL
} rt_picker_game;

// The folders that make a game installed, relative to Documents: game/<mod>,
// prep (and linux, which an install also copies). `which` 0..2; NULL past the end.
const char *rt_picker_data_folder(const rt_picker_game *g, int which, char *out, size_t size);
// The base game (Portal 2) can't be removed: the other games in the app run
// on its files. 1 for any other game.
int rt_picker_can_remove(const rt_picker_game *g);
#define RT_PICKER_BASE_NOTE "It's the base the other games run on, so it stays."
// A hosted game's slim pack (tools/mel_slim.py) reads Portal 2's archives:
// the marker that says so, relative to Documents ("mel/.p2-slim"; NULL for a
// game at the root of Documents), and Portal 2's file it needs.
const char *rt_picker_slim_marker(const rt_picker_game *g, char *out, size_t size);
#define RT_PICKER_BASE_FILE "game/portal2/pak01_dir.vpk"
#define RT_PICKER_NEEDS_BASE "This game uses Portal 2's files, and Portal 2 isn't on this device. Copy Portal 2 " \
    "from your Mac first (Natif Installer > Devices > Download), then open this game again."
// Can it start? Installed, and if slim, with Portal 2's files there.
int rt_picker_playable(int installed, int slim, int base_present);
// What "Remove game data" deletes, relative to Documents: game, prep, linux of
// that game's data folder (never its userdata, never another game's folders).
// `which` 0..2; NULL past the end, and always NULL for the base game.
const char *rt_picker_removable(const rt_picker_game *g, int which, char *out, size_t size);
// The same folders whatever the game (its size on the device): Portal 2's too,
// which nothing removes (its size read 0 KB when it was counted through
// rt_picker_removable).
const char *rt_picker_owned(const rt_picker_game *g, int which, char *out, size_t size);
// The game that starts without the picker: `forced` (P2_GAME_CHOICE) if it is
// one of `names`; else `remembered` if remember is on and it is installed; else
// the only installed game when just one is, and `names` has one entry; else
// NULL (show the picker). installed[i] belongs to names[i].
const char *rt_picker_autostart(const char *const *names, const int *installed, int count, const char *forced,
                                const char *remembered, int remember);
// "11.5 GB", "820 MB", "Not installed" style sizes.
void rt_picker_size(long long bytes, char *out, size_t size);

// ---- The launcher's series (round 23) ----
// The main screen shows series; a series shows its games in this order. Adding
// a game to a series is one line of the table in game_picker.c (a name the app
// doesn't hold is skipped, so P2CE, Revolution or Left 4 Dead can be listed
// before they exist).
typedef struct {
    const char *id;        // "portal"
    const char *title;     // "Portal"
    const char *games[8];  // game names (game_ios.c), NULL-terminated
} rt_picker_series;
int rt_picker_series_count(void);
const rt_picker_series *rt_picker_series_at(int i);
// The series a game belongs to, or NULL.
const rt_picker_series *rt_picker_series_of(const char *game);
// A short label over a cover that other games share ("EP1", "EP2", "LC"), or NULL.
const char *rt_picker_badge(const char *game);

// Round 26 ("Remember my game" turning itself on, seen on the iPad mini): the
// launcher's cards are UIKit controls in a scroll view, which by default lets
// a control keep a touch that turns into a drag, and a control fires when the
// finger lifts up to ~70 points outside it. A slow scroll started on the
// Remember pill flipped the setting. A card now fires only for a tap: the
// finger lifted inside the card, having moved at most RT_PICKER_TAP_SLOP points.
#define RT_PICKER_TAP_SLOP 12.0
int rt_picker_is_tap(double moved_x, double moved_y, int lifted_inside);
