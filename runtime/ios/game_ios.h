// Jeu de ce build iOS : tools/build_ios.py écrit le profil (tools/games.py)
// dans la clé P2Game de l'Info.plist ; rien n'est deviné à l'exécution.
#pragma once

typedef enum { RT_IOS_LAYOUT_PORTAL2, RT_IOS_LAYOUT_HL2, RT_IOS_LAYOUT_L4D2, RT_IOS_LAYOUT_COUNT } rt_ios_layout;

typedef struct {
    const char *name;       // valeur de P2Game : "portal2", "portal1", "hl2"
    const char *mod;        // -game par défaut
    const char *userdata;   // dossier des sauvegardes dans Documents
    const char *app_id;     // SteamAppId du stub Steam (runtime/ios/steam_stub.c)
    int back_taps;          // tapes au dos par défaut (P2_BACKTAP absente)
    rt_ios_layout layout;   // disposition des commandes tactiles (touch_layout.c)
    // ICvar de libvstdlib.so (game_state.c) : version et entrée de
    // FindCommandBase dans sa table virtuelle.
    const char *cvar_interface;
    unsigned find_command_base;
    // Its data in Documents: "" for the app's own game (Documents/game, prep,
    // linux, where existing installs keep them), "mel" for a game hosted in the
    // Portal app (Documents/mel/game, ...). Saves stay in Documents/<userdata>.
    const char *data_dir;
    const char *title;      // shown in the game picker and the menu's version line
    // GPU device ID the Direct3D 9 bridge reports (P2_D3D9_DEVICE_ID), NULL for
    // libd3d9metal's own pick. That pick depends on the Apple chip, and the
    // engine's dxsupport.cfg reads it: the iPad mini's A20 Pro came out as
    // 0x034b, a GeForce FX (DirectX 9.0, gpu_level 0), so Left 4 Dead 2 lost its
    // shader combos (black hands). Only set where needed: a new ID makes the
    // engine reset a player's saved video settings once.
    const char *gpu_device_id;
} rt_ios_game;

// Profil de ce nom ; Portal 2 pour NULL, "" ou un nom inconnu (builds sans P2Game).
const rt_ios_game *rt_ios_game_named(const char *name);
// Profil de l'app en cours (Info.plist, lu une fois) ; Portal 2 hors iOS.
// Dans l'app Portal (plusieurs jeux), celui choisi au lancement (rt_ios_set_game).
const rt_ios_game *rt_ios_current_game(void);
// The game picked at launch (ios/main_ios.c); returns its profile.
const rt_ios_game *rt_ios_set_game(const char *name);
// The app's own game: the Info.plist's P2Game (its keys and folders are the
// historical ones). Another game's preference keys get "<name>." in front.
const rt_ios_game *rt_ios_app_game(void);
// Preference key of the running game. Settings are shared by every game in
// the app (touch layout, button size and opacity, back taps, gyro and look,
// frame rate, the gear menu's general settings): `key` itself, the app's own
// game's (Portal 2's) key. A per-game key (rt_ios_key_per_game) of a hosted
// game is "<game>.<key>". Saves, launch options and the version line are per
// game by their folders, not by keys.
const char *rt_ios_pref_key(const char *key, char *out, unsigned long size);
// 1 for a setting that only means something to one game (iCloud saves).
int rt_ios_key_per_game(const char *key);
