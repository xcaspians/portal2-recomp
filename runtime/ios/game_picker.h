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
} rt_picker_game;

// The folders that make a game installed, relative to Documents: game/<mod>,
// prep (and linux, which an install also copies). `which` 0..2; NULL past the end.
const char *rt_picker_data_folder(const rt_picker_game *g, int which, char *out, size_t size);
// What "Remove game data" deletes, relative to Documents: game, prep, linux of
// that game's data folder (never its userdata, never another game's folders).
// `which` 0..2; NULL past the end.
const char *rt_picker_removable(const rt_picker_game *g, int which, char *out, size_t size);
// The game that starts without the picker: `forced` (P2_GAME_CHOICE) if it is
// one of `names`; else `remembered` if remember is on and it is installed; else
// the only installed game when just one is, and `names` has one entry; else
// NULL (show the picker). installed[i] belongs to names[i].
const char *rt_picker_autostart(const char *const *names, const int *installed, int count, const char *forced,
                                const char *remembered, int remember);
// "11.5 GB", "820 MB", "Not installed" style sizes.
void rt_picker_size(long long bytes, char *out, size_t size);
