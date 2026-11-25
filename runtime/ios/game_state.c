// État du jeu pour les commandes tactiles : interface du jeu (menus, pause,
// écrans de chargement) affichée ou non.
//
// Aucun appel SDL ne le dit sur iOS : le moteur ne capture jamais la souris et
// appelle SDL_ShowCursor(1) à chaque image, en partie comme au menu. On demande
// donc au moteur : IEngineVGui (« VEngineVGui001 », obtenu par CreateInterface
// d'engine.so), méthode IsGameUIVisible, 4e entrée de sa table virtuelle (deux
// destructeurs, GetPanel, IsGameUIVisible ; vérifié dans le code traduit :
// m_pGameUI ? m_pGameUI->IsGameUIActive() : false).
//
// Il règle aussi, pour la manette virtuelle du stick tactile (touch_controls.m),
// la courbe de réponse du déplacement au joystick (joy_response_move) : voir
// plus bas.
//
// Appelé depuis SDL_PollEvent (sdl_ios.m), sur le thread principal du jeu,
// verrou invité tenu, au plus dix fois par seconde.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tv_output.h"
#include "device_ios.h"
#include "game_ios.h"
#include "modules.h"
#include "rt.h"

static uint32_t engine_vgui;      // objet IEngineVGui invité (0 : pas encore)
static int ui_visible = 1;        // menus au démarrage
static uint64_t next_query, next_lookup;

void rt_input_nudge_mouse(void);   // runtime/sdl_input.c
int rt_ios_game_ui_visible(void) { return ui_visible; }

// ---- Courbe de réponse du déplacement au joystick ------------------------
// La configuration Xbox du jeu (cfg/joy_configuration.cfg, config.cfg) règle
// joy_response_move à 5 : inclinaison réduite de moitié jusqu'à 60 %, entière
// au-delà, d'où un trou entre 30 et 60 % de la vitesse de marche. Mesuré dans
// le simulateur : vitesse = 175 × courbe((axe - 0,15) / 0,85), 0,15 étant
// joy_forwardthreshold/joy_sidethreshold. Pour le stick tactile, le jeu passe
// en réponse linéaire (0) tant que la manette virtuelle est là, et retrouve sa
// valeur quand elle part (vraie manette branchée).
//
// Variables lues et écrites par ICvar (libvstdlib.so ; « VEngineCvar007 »
// pour Portal 2, « VEngineCvar004 » pour HL2) : FindCommandBase, 14e entrée de
// sa table pour Portal 2 (IAppSystem en a 8, puis AllocateDLLIdentifier,
// Register/UnregisterConCommand, UnregisterConCommands, GetCommandLineValue ;
// même indice que l'outil SAR), 11e pour HL2 (game_ios.c). Le résultat
// est vérifié par son nom (ConCommandBase : vptr, m_pNext, m_bRegistered,
// m_pszName) avant tout usage ; la valeur est posée par IConVar::SetValue(const
// char *), 1re entrée de la table d'IConVar (à +24 dans ConVar), et relue dans
// m_pszString (+36) du parent (+28).
static uint32_t icvar;            // 0 : pas encore ; 1 : introuvable
static int linear_wanted;         // demandé par touch_controls.m
static char saved_response[16];   // valeur du jeu à rendre ("" : rien à rendre)
static int response = -1;         // valeur actuelle de joy_response_move (-1 : inconnue)
static int convar_broken;         // vérification échouée : ne plus rien tenter

void rt_ios_move_response_linear(int on) { linear_wanted = on; }
int rt_ios_move_response(void) { return response; }

static uint32_t guest_text(const char *text) {
    uint32_t address = rt_alloc((uint32_t)strlen(text) + 1);
    if (address) memcpy(GPTR(address), text, strlen(text) + 1);
    return address;
}
static int plausible(uint32_t address) { return address >= 0x10000 && address < RT_IMPORT_BASE - 64; }
// ConVar invitée de ce nom, ou 0.
static uint32_t find_convar(CPU *c, const char *name) {
    if (!icvar) {
        const GuestModule *vstdlib = rt_find_module("libvstdlib.so");
        uint32_t create = vstdlib ? rt_module_symbol(vstdlib, "CreateInterface") : 0;
        uint32_t text = create ? guest_text(rt_ios_current_game()->cvar_interface) : 0;
        uint32_t args[2] = {text, 0};
        icvar = text ? rt_invoke(c, create, args, 2) : 0;
        rt_free(text);
        if (!icvar) icvar = 1;
    }
    if (icvar == 1) return 0;
    uint32_t text = guest_text(name);
    if (!text) return 0;
    uint32_t args[2] = {icvar, text};
    uint32_t base = rt_invoke(c, R32(R32(icvar) + rt_ios_current_game()->find_command_base * 4), args, 2);
    rt_free(text);
    if (!plausible(base) || !plausible(R32(base + 12)) || strcmp(GPTR(R32(base + 12)), name) ||
        !plausible(R32(base + 28))) return 0;
    return base;
}
static const char *convar_string(uint32_t convar) {
    uint32_t text = R32(R32(convar + 28) + 36);
    return plausible(text) ? GPTR(text) : "";
}
static int set_convar(CPU *c, uint32_t convar, const char *value) {
    uint32_t text = guest_text(value);
    if (!text) return 0;
    uint32_t args[2] = {convar + 24, text};
    rt_invoke(c, R32(R32(convar + 24)), args, 2);
    rt_free(text);
    return !strcmp(convar_string(convar), value);
}
static int level_known(void);   // below: a level has loaded, or this game can't tell
static void update_move_response(CPU *c) {
    if (convar_broken) return;
    uint32_t convar = find_convar(c, "joy_response_move");
    if (!convar) {
        // Missing in gameplay: not this game's variable. Portal 2 and Mel read
        // as "UI hidden" at start-up too, before client.so has registered its
        // variables: for them only once a level has loaded (level_known).
        if (icvar && !ui_visible && level_known()) {
            convar_broken = 1;
            fprintf(stderr, "[ios] joy_response_move introuvable : réponse du jeu gardée\n");
        }
        return;
    }
    const char *now = convar_string(convar);
    if (linear_wanted && strcmp(now, "0")) {
        // Pas pendant un menu : la configuration y est encore exécutée et
        // écraserait la valeur ; elle l'est avant l'entrée en partie.
        if (ui_visible) { response = now[0] ? (int)strtol(now, NULL, 10) : -1; return; }
        snprintf(saved_response, sizeof(saved_response), "%s", now);
        if (!set_convar(c, convar, "0")) {
            convar_broken = 1; saved_response[0] = 0;
            fprintf(stderr, "[ios] joy_response_move : écriture non vérifiée, réponse du jeu gardée\n");
        } else fprintf(stderr, "[ios] joy_response_move %s -> 0 (stick tactile linéaire)\n", saved_response);
    } else if (!linear_wanted && saved_response[0]) {
        if (!strcmp(now, "0")) set_convar(c, convar, saved_response);
        fprintf(stderr, "[ios] joy_response_move rendu : %s\n", convar_string(convar));
        saved_response[0] = 0;
    }
    now = convar_string(convar);
    response = now[0] ? (int)strtol(now, NULL, 10) : -1;
}

// ---- Commandes console (P2_IOS_AUTOEXEC, ci-dessous) ----------------------
// Trouvées par ICvar (FindCommandBase, comme les variables), puis leur rappel
// appelé directement avec un CCommand invité : ConCommand garde le rappel à
// +24 et ses drapeaux à +32 (bit 1 : rappel à arguments, bit 2 : interface) ;
// CCommand = argc, taille d'argv[0], deux tampons de 512 octets, 64 pointeurs.
// Diagnostics seulement (getpos ; P2_IOS_AUTOEXEC pour le banc). Une commande
// du serveur (setpos…) passe par « cmd », qui l'envoie comme un joueur.
static void run_console_command(CPU *c, const char *line) {
    enum { ARGV = 64, LENGTH = 512, SIZE = 8 + 2 * LENGTH + 4 * ARGV };
    char name[64] = {0};
    sscanf(line, "%63s", name);
    if (!name[0] || strlen(line) >= LENGTH) return;
    if (!icvar) find_convar(c, "joy_response_move");   // initialise icvar
    if (icvar <= 1) return;
    uint32_t text = guest_text(name);
    if (!text) return;
    uint32_t args[2] = {icvar, text};
    uint32_t base = rt_invoke(c, R32(R32(icvar) + rt_ios_current_game()->find_command_base * 4), args, 2);
    rt_free(text);
    if (!plausible(base) || !plausible(R32(base + 12)) || strcmp(GPTR(R32(base + 12)), name)) {
        fprintf(stderr, "[ios] commande %s introuvable\n", name); return;
    }
    uint32_t callback = R32(base + 24); uint8_t bits = *(uint8_t *)GPTR(base + 32);
    if (!plausible(callback) || (bits & 4)) { fprintf(stderr, "[ios] commande %s : rappel inattendu\n", name); return; }
    uint32_t command = rt_alloc(SIZE);
    if (!command) return;
    memset(GPTR(command), 0, SIZE);
    char *full = GPTR(command + 8), *argv = GPTR(command + 8 + LENGTH);
    snprintf(full, LENGTH, "%s", line);
    // Mots séparés par des espaces, chacun terminé par un zéro dans m_pArgvBuffer.
    uint32_t argc = 0, at = 0;
    for (const char *p = line; *p && argc < ARGV;) {
        while (*p == ' ') ++p;
        if (!*p) break;
        W32(command + 8 + 2 * LENGTH + 4 * argc, command + 8 + LENGTH + at);
        while (*p && *p != ' ' && at < LENGTH - 1) argv[at++] = *p++;
        argv[at++] = 0; ++argc;
    }
    W32(command, argc); W32(command + 4, (uint32_t)strlen(name));
    fprintf(stderr, "[ios] commande %s\n", line);
    if (bits & 2) rt_invoke(c, callback, &command, 1); else rt_invoke(c, callback, NULL, 0);
    rt_free(command);
}
// P2_IOS_AUTOEXEC="var=valeur;commande args;pause 2;…" : exécuté 5 s après la
// première entrée en partie (banc : sv_cheats=1;cmd setpos …;cmd setang …) ;
// « pause N » attend N secondes avant la suite.
static void autoexec(CPU *c) {
    static int done; static uint64_t resume; static char plan[1024]; static char *next, *save;
    if (done || ui_visible) return;
    uint64_t now = SDL_GetTicks64();
    if (!next) {
        const char *e = getenv("P2_IOS_AUTOEXEC");
        if (!e || !*e) { done = 1; return; }
        snprintf(plan, sizeof(plan), "%s", e);
        next = strtok_r(plan, ";", &save);
        resume = now + 5000;
    }
    while (next && now >= resume) {
        char *item = next; next = strtok_r(NULL, ";", &save);
        while (*item == ' ') ++item;
        char *equal = strchr(item, '=');
        double seconds;
        if (sscanf(item, "pause %lf", &seconds) == 1) resume = now + (uint64_t)(seconds * 1000);
        else if (equal && !strchr(item, ' ')) {
            *equal = 0;
            uint32_t convar = find_convar(c, item);
            int ok = convar && set_convar(c, convar, equal + 1);
            fprintf(stderr, "[ios] %s = %s : %s\n", item, equal + 1, ok ? "posé" : "échec");
        } else run_console_command(c, item);
    }
    if (!next) done = 1;
}

// ---- In a level, and commands for the engine (in-game settings menu) -----
// The menu's gear shows only in the pause menu: game UI visible *and* a level
// loaded, which the engine says through IVEngineClient ("VEngineClient015",
// engine.so, Portal 2 only): IsInGame is entry 25 of its table,
// IsLevelMainMenuBackground 90 and ClientCmd_Unrestricted 108. These indices
// come from the symbols of the Mac edition's engine.dylib, which exports the
// same interface version (the Linux engine.so is stripped). Checked at run
// time: every entry must point into the code, both answers must be booleans,
// and gameplay (game UI hidden) must report a level; otherwise the menu never
// shows its gear rather than guessing.
// IVEngineClient per engine generation (slots read from the Mac editions'
// engine.dylib symbols, CEngineClient's vtable): Portal 2 and Mel's 2021
// build use VEngineClient015 (IsInGame 25, IsLevelMainMenuBackground 90,
// ClientCmd_Unrestricted 108); Half-Life 2 and Portal, Source 2013, use
// VEngineClient014 (26, 87, 106). Checked at run time (game_state.c below):
// a wrong answer turns the gear off rather than guessing.
typedef struct { const char *game, *version; int in_game, menu_background, cmd_unrestricted; } client_slots;
static const client_slots kClientSlots[] = {
    {"portal2", "VEngineClient015", 25, 90, 108},
    {"mel", "VEngineClient015", 25, 90, 108},
    {"hl2", "VEngineClient014", 26, 87, 106},
    {"portal1", "VEngineClient014", 26, 87, 106},
    // Left 4 Dead 2: VEngineClient013, from the Mac edition's engine.dylib
    // (CEngineClient's vtable: IsInGame 26, IsLevelMainMenuBackground 90,
    // ClientCmd_Unrestricted 106).
    {"l4d2", "VEngineClient013", 26, 90, 106},
};
static const client_slots *slots;
#define CLIENT_IS_IN_GAME (slots->in_game)
#define CLIENT_IS_MENU_BACKGROUND (slots->menu_background)
#define CLIENT_CMD_UNRESTRICTED (slots->cmd_unrestricted)
static uint32_t engine_client;    // 0: not yet; 1: unavailable (other game, failed check)
static int in_level;
static int level_mismatch;        // polls in gameplay that reported no level
static char command_queue[4][256];
static int command_count;

int rt_ios_game_in_level(void) { return in_level; }
static int level_seen_once;
static int level_known(void) { return engine_client == 1 || level_seen_once; }
int rt_ios_game_paused(void) { return ui_visible && in_level; }
// A game menu is up, the main menu or the pause menu (the gear shows on both),
// for a game whose engine answers (engine_client checked).
int rt_ios_game_menu_shown(void) { return ui_visible && engine_client > 1; }
int rt_ios_game_commands_available(void) { return engine_client > 1; }
// Main thread (UIKit callbacks run inside SDL_PollEvent, like the poll below).
int rt_ios_game_command(const char *line) {
    if (engine_client <= 1 || command_count >= 4 || strlen(line) >= sizeof(command_queue[0])) return 0;
    snprintf(command_queue[command_count++], sizeof(command_queue[0]), "%s", line);
    return 1;
}

// P2_MEMORY_LOG=1 (device measurements): the process's memory footprint, now
// and at its peak (what iOS counts against the app's limit), every 10 s.
static void memory_log(uint64_t now) {
    static int on = -1;
    static uint64_t next;
    if (on < 0) on = rt_env_flag("P2_MEMORY_LOG");
    if (!on || now < next) return;
    next = now + 10000;
    long long current, peak, available;
    if (!rt_ios_memory_footprint(&current, &peak, &available)) return;
    fprintf(stderr, "[ios] mémoire : %lld Mo maintenant, pic %lld Mo, %lld Mo encore disponibles%s\n", current >> 20,
            peak >> 20, available >> 20, in_level ? " (en partie)" : "");
}

// P2_IOS_MENU_EXEC="<console command>" (device tests: tools/ios_device.sh mel
// slim switch loads a save this way): sent once, 3 s after the main menu is up
// with no level loaded, as a player would from the menu. A command-line +load
// runs before the menu on a slower device and the engine answers "file missing".
static void menu_exec(uint64_t now) {
    static int done;
    static uint64_t since;
    if (done) return;
    const char *line = getenv("P2_IOS_MENU_EXEC");
    if (!line || !*line) { done = 1; return; }
    if (!(ui_visible && engine_client > 1 && !in_level)) { since = 0; return; }
    if (!since) { since = now; return; }
    if (now - since < 3000) return;
    done = 1;
    fprintf(stderr, "[ios] commande du menu (P2_IOS_MENU_EXEC) : %s%s\n", line,
            rt_ios_game_command(line) ? "" : " refusée");
}

static void client_unavailable(const char *why) {
    engine_client = 1; in_level = 0; command_count = 0;
    fprintf(stderr, "[ios] IVEngineClient : %s ; menu des réglages sans engrenage\n", why);
}
static void find_engine_client(CPU *c) {
    const char *name = rt_ios_game_kind(rt_ios_current_game());   // the episodes: Half-Life 2's engine
    for (size_t i = 0; i < sizeof(kClientSlots) / sizeof(*kClientSlots) && !slots; ++i)
        if (!strcmp(kClientSlots[i].game, name)) slots = &kClientSlots[i];
    if (!slots) { engine_client = 1; return; }
    const GuestModule *engine = rt_find_module("engine.so");
    uint32_t create = engine ? rt_module_symbol(engine, "CreateInterface") : 0;
    uint32_t text = create ? guest_text(slots->version) : 0;
    uint32_t args[2] = {text, 0};
    uint32_t client = text ? rt_invoke(c, create, args, 2) : 0;
    rt_free(text);
    if (!plausible(client) || !plausible(R32(client))) { client_unavailable("introuvable"); return; }
    uint32_t table = R32(client);
    const int used[] = {CLIENT_IS_IN_GAME, CLIENT_IS_MENU_BACKGROUND, CLIENT_CMD_UNRESTRICTED};
    for (size_t i = 0; i < sizeof(used) / sizeof(*used); ++i)
        if (!plausible(R32(table + used[i] * 4))) { client_unavailable("table inattendue"); return; }
    engine_client = client;
    fprintf(stderr, "[ios] état du jeu : IVEngineClient à %#x\n", engine_client);
}
static void update_level(CPU *c, int visible) {
    if (!engine_client && engine_vgui) find_engine_client(c);
    if (engine_client <= 1) return;
    uint32_t table = R32(engine_client);
    uint32_t game = rt_invoke(c, R32(table + CLIENT_IS_IN_GAME * 4), &engine_client, 1) & 0xff;
    uint32_t background = rt_invoke(c, R32(table + CLIENT_IS_MENU_BACKGROUND * 4), &engine_client, 1) & 0xff;
    if (game > 1 || background > 1) { client_unavailable("réponse non booléenne"); return; }
    // Two seconds of gameplay without a level: not the method we think. Only
    // counted once a level has loaded: before Portal 2's GameUI exists the UI
    // reads as hidden at start-up, which would turn the gear off for good.
    static int level_seen;
    level_seen |= game;
    level_mismatch = level_seen && !visible && !game ? level_mismatch + 1 : 0;
    if (level_mismatch > 20) { client_unavailable("partie sans niveau"); return; }
    int now = game && !background;
    if (now != in_level) fprintf(stderr, "[ios] niveau %s\n", now ? "chargé" : "fermé");
    level_seen_once |= now;
    if (now && !in_level) rt_ios_memory_log("niveau chargé");   // device_ios.c: for crashes on 4 GB phones
    in_level = now;
    for (int i = 0; i < command_count; ++i) {
        uint32_t text = guest_text(command_queue[i]);
        if (!text) continue;
        uint32_t cmd_args[2] = {engine_client, text};
        fprintf(stderr, "[ios] commande au moteur : %s\n", command_queue[i]);
        rt_invoke(c, R32(table + CLIENT_CMD_UNRESTRICTED * 4), cmd_args, 2);   // to the command buffer: next frame
        rt_free(text);
    }
    command_count = 0;
}

// ---- The game's own Community Test Chambers dialog (Portal 2, Mel) ----
// Without Steam its list is empty and its way back is unclear, so the overlay
// opens Community Maps instead (settings_menu_ios.m). Found by reading the
// game's UI: VGUI's panel tree (VGUI_Panel009 in vgui2.so; slots from the
// Mac edition's vgui2.dylib: IsVisible 16, GetChildCount 18, GetChild 19,
// GetName 37) from IEngineVGui::GetPanel(PANEL_ROOT) (slot 2), looking for a
// visible panel named CommunityMapDialog (its name in the game's
// communitymapdialog.res). Nothing in the game is changed or faked; the
// dialog is then closed like a player would (Escape).
enum { PANEL_IS_VISIBLE = 16, PANEL_CHILD_COUNT = 18, PANEL_CHILD = 19, PANEL_NAME = 37 };
static uint32_t vgui_panel;        // VGUI_Panel009 (0: not yet; 1: unavailable)
static int community_wanted;       // the dialog was seen open: Community Maps should show
int rt_ios_game_take_community_request(void) { int w = community_wanted; community_wanted = 0; return w; }
static uint32_t panel_call(CPU *c, int slot, uint32_t *args, int n) {
    return rt_invoke(c, R32(R32(vgui_panel) + (uint32_t)slot * 4), args, (size_t)n);
}
static int find_panel(CPU *c, uint32_t vpanel, const char *name, int depth, int *budget) {
    if (depth > 7 || --*budget < 0) return 0;
    uint32_t a1[2] = {vgui_panel, vpanel};
    if (!(panel_call(c, PANEL_IS_VISIBLE, a1, 2) & 0xff)) return 0;
    uint32_t text = panel_call(c, PANEL_NAME, a1, 2);
    if (plausible(text) && !strcmp(GPTR(text), name)) return 1;
    int n = (int)panel_call(c, PANEL_CHILD_COUNT, a1, 2);
    if (n < 0 || n > 256) return 0;
    for (int i = 0; i < n; ++i) {
        uint32_t a2[3] = {vgui_panel, vpanel, (uint32_t)i};
        uint32_t child = panel_call(c, PANEL_CHILD, a2, 3);
        if (child && find_panel(c, child, name, depth + 1, budget)) return 1;
    }
    return 0;
}
static void watch_community_dialog(CPU *c, int visible) {
    static uint64_t next;
    static int was_open;
    uint64_t now = SDL_GetTicks64();
    // Community Maps is Portal 2's (Workshop app 620).
    if (!visible || engine_client <= 1 || vgui_panel == 1 || now < next || strcmp(rt_ios_current_game()->name, "portal2")) {
        if (!visible) was_open = 0;
        return;
    }
    next = now + 300;
    if (!vgui_panel) {
        const GuestModule *vgui = rt_find_module("vgui2.so");
        uint32_t create = vgui ? rt_module_symbol(vgui, "CreateInterface") : 0;
        uint32_t text = create ? guest_text("VGUI_Panel009") : 0;
        uint32_t args[2] = {text, 0};
        uint32_t found = text ? rt_invoke(c, create, args, 2) : 0;
        rt_free(text);
        vgui_panel = plausible(found) && plausible(R32(found)) ? found : 1;
        if (vgui_panel == 1) { fprintf(stderr, "[ios] VGUI_Panel009 introuvable : bouton Communauté du jeu laissé tel quel\n"); return; }
    }
    uint32_t args[2] = {engine_vgui, 0};   // PANEL_ROOT
    uint32_t root = rt_invoke(c, R32(R32(engine_vgui) + 2 * 4), args, 2);
    int budget = 600;
    int open = root && find_panel(c, root, "CommunityMapDialog", 0, &budget);
    if (open && !was_open) {
        fprintf(stderr, "[ios] dialogue Communauté du jeu : Community Maps à la place\n");
        community_wanted = 1;
        SDL_Event e;   // close the game's dialog the way a player would
        memset(&e, 0, sizeof(e));
        e.type = SDL_KEYDOWN; e.key.keysym.scancode = SDL_SCANCODE_ESCAPE; e.key.keysym.sym = SDLK_ESCAPE; e.key.state = SDL_PRESSED;
        SDL_PushEvent(&e);
        e.type = SDL_KEYUP; e.key.state = SDL_RELEASED;
        SDL_PushEvent(&e);
    }
    was_open = open;
}

void rt_ios_game_state_poll(void) {
    uint64_t now = SDL_GetTicks64();
    if (now < next_query) return;
    next_query = now + 100;
    rt_ios_tv_poll();   // a TV picture size the engine couldn't take yet (tv_output_ios.m)
    CPU *c = rt_foreign_thread_attach();   // thread invité courant : son CPU
    if (!c) return;
    if (!engine_vgui) {
        if (now < next_lookup) return;
        next_lookup = now + 1000;   // engine.so pas encore initialisé : réessayer
        const GuestModule *engine = rt_find_module("engine.so");
        uint32_t create = engine ? rt_module_symbol(engine, "CreateInterface") : 0;
        static uint32_t name;
        if (!name && (name = rt_alloc(sizeof("VEngineVGui001")))) memcpy(GPTR(name), "VEngineVGui001", sizeof("VEngineVGui001"));
        if (!create || !name) return;
        uint32_t args[2] = {name, 0};
        engine_vgui = rt_invoke(c, create, args, 2);
        if (engine_vgui) fprintf(stderr, "[ios] état du jeu : IEngineVGui à %#x\n", engine_vgui);
        if (!engine_vgui) return;
    }
    uint32_t method = R32(R32(engine_vgui) + 3 * 4);
    int visible = (rt_invoke(c, method, &engine_vgui, 1) & 0xff) != 0;
    if (visible != ui_visible) fprintf(stderr, "[ios] interface du jeu %s\n", visible ? "affichée" : "masquée");
    // En jeu, les commandes tactiles sont une manette : Source ouvre alors ses
    // menus en mode manette, curseur caché et souris ignorée. Un vrai
    // déplacement de souris le ramène en mode souris (runtime/sdl_input.c).
    if (visible && !ui_visible) rt_input_nudge_mouse();
    ui_visible = visible;
    update_level(c, visible);
    watch_community_dialog(c, visible);
    if (linear_wanted || saved_response[0]) update_move_response(c);
    autoexec(c);
    menu_exec(now);
    memory_log(now);
}
#endif
