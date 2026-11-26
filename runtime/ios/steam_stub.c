// Stub Steam hôte pour le build iOS seulement (pas de client Steam sur iOS).
// Décision de l'utilisateur du 25 octobre 2026 : stub maison minimal, pour sa
// propre copie du jeu et un usage personnel ; jamais compilé dans le build Mac
// (qui parle au vrai client Steam).
//
// Il remplace libsteam_api.dylib derrière le pont runtime/steam.c, qui le
// trouve par dlsym (P2_STEAM_API_DYLIB désigne l'exécutable lui-même). Mode
// hors ligne : utilisateur connecté qui possède l'app, aucun callback, pas de
// succès, de statistiques, de nuage ni de multijoueur. Les fonctions plates
// sans réponse particulière sont générées (steam_stub_generated.c) et
// renvoient 0, false, NULL ou "".
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPORT __attribute__((visibility("default"), used))
// SteamErrMsg du SDK 1.59+ : char[k_cchMaxSteamErrMsg = 1024] (runtime/steam.c, STEAM_ERR_MSG).
#define STEAM_ERR_MSG_SIZE 1024

// ---- objets d'interface : une vtable de méthodes qui renvoient 0 ----
// Le pont n'appelle la vtable hôte que pour quelques interfaces hors API
// plate (GameCoordinator, GameStats, GetCSERIPPort) : « rien » leur convient.
static uint64_t zero_method(void) { return 0; }
// Assez d'emplacements pour la plus grande interface (ISteamUGC : ~90).
static void *zero_vtable[256];
typedef struct { void **vtable; char version[64]; } StubInterface;
static StubInterface interfaces[64];
static int interface_count;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void **steam_stub_vtable(const char *version);   // steam_stub_generated.c

void *steam_stub_interface(const char *version) {
    if (!version) return NULL;
    pthread_mutex_lock(&lock);
    if (!zero_vtable[0]) for (int i = 0; i < 256; ++i) zero_vtable[i] = (void *)zero_method;
    StubInterface *found = NULL;
    for (int i = 0; i < interface_count && !found; ++i)
        if (!strcmp(interfaces[i].version, version)) found = &interfaces[i];
    if (!found && interface_count < 64) {
        found = &interfaces[interface_count++];
        // SDK récents (HL2) : appels par emplacement, table générée par version.
        void **table = steam_stub_vtable(version);
        found->vtable = table ? table : zero_vtable;
        snprintf(found->version, sizeof(found->version), "%s", version);
        fprintf(stderr, "[steam-stub] interface %s\n", version);
    }
    pthread_mutex_unlock(&lock);
    return found;
}

// App possédée : SteamAppId, posée par ios/main_ios.c d'après le jeu du build
// (620 Portal 2, 220 Half-Life 2) ; 620 à défaut.
uint32_t steam_stub_app_id(void) {
    const char *id = getenv("SteamAppId");
    uint32_t value = id ? (uint32_t)strtoul(id, NULL, 10) : 0;
    return value ? value : 620u;
}

// The game's own app, or one Steam installed with a game of this app for this
// account: P2_STEAM_OWNED, the "SharedDepots" apps of the games' appmanifest on
// the Mac (tools/build_ios.py writes steam-owned.txt, ios/main_ios.c reads it).
// Half-Life 2's episodes (380, 420) and Lost Coast (340) come with it since the
// anniversary update, and its engine asks before switching to them.
bool steam_stub_owns(uint32_t app) {
    if (app == steam_stub_app_id()) return true;
    const char *list = getenv("P2_STEAM_OWNED");
    for (const char *p = list; p && *p;) {
        char *end = NULL;
        unsigned long id = strtoul(p, &end, 10);
        if (end == p) break;
        if (id == app) return true;
        p = *end ? end + 1 : end;
    }
    return false;
}

// ---- cycle de vie ----
EXPORT bool SteamAPI_InitSafe(void) { fprintf(stderr, "[steam-stub] hors ligne : stub iOS, app %u\n", steam_stub_app_id()); return true; }
// Left 4 Dead 2 starts through SteamAPI_Init and asks SteamAPI_IsSteamRunning:
// the same offline answers (a logged-in owner, no network).
EXPORT bool SteamAPI_Init(void) { return SteamAPI_InitSafe(); }
EXPORT bool SteamAPI_IsSteamRunning(void) { return true; }
// Initialisation du SDK 1.60+ (Half-Life 2) : ESteamAPIInitResult, 0 = OK.
EXPORT int32_t SteamInternal_SteamAPI_Init(const char *versions, char *message) {
    (void)versions; if (message) message[0] = 0;
    static int logged;
    if (!logged++) fprintf(stderr, "[steam-stub] hors ligne : stub iOS, app %u (SteamAPI_InitEx)\n", steam_stub_app_id());
    return 0;
}
EXPORT void SteamAPI_Shutdown(void) {}
EXPORT bool SteamAPI_RestartAppIfNecessary(uint32_t app) { (void)app; return false; }
EXPORT int32_t SteamAPI_GetHSteamPipe(void) { return 1; }
EXPORT int32_t SteamAPI_GetHSteamUser(void) { return 1; }
EXPORT void SteamAPI_SetTryCatchCallbacks(bool on) { (void)on; }
EXPORT const char *SteamAPI_GetSteamInstallPath(void) { return ""; }
EXPORT int32_t SteamGameServer_GetHSteamPipe(void) { return 0; }
EXPORT int32_t SteamGameServer_GetHSteamUser(void) { return 0; }
EXPORT uint32_t SteamGameServer_GetIPCCallCount(void) { return 0; }
EXPORT void SteamGameServer_Shutdown(void) {}
EXPORT bool SteamInternal_GameServer_Init(uint32_t ip, uint16_t a, uint16_t b, uint16_t q, int32_t mode, const char *v) {
    (void)ip; (void)a; (void)b; (void)q; (void)mode; (void)v; return false;
}
// Serveur de jeu Steam : refusé, comme l'ancienne version (1 = échec générique).
EXPORT int32_t SteamInternal_GameServer_Init_V2(uint32_t ip, uint16_t a, uint16_t q, int32_t mode, const char *v,
                                                const char *versions, char *message) {
    (void)ip; (void)a; (void)q; (void)mode; (void)v; (void)versions;
    if (message) snprintf(message, STEAM_ERR_MSG_SIZE, "no Steam game server on iOS");
    return 1;
}

// ---- callbacks : aucun, sauf les échecs de lobby (round 24) ----
// The game's lobby requests wait for an answer that can't come here. When the
// player leaves (online_notice_ios.m, its Back), each one gets Steam's own
// failure answer: k_EResultNoConnection for a lobby, no lobbies for a list.
// Nothing faked: there is no Steam connection on iOS. Host layout pack(4):
// LobbyCreated_t {EResult; uint64 lobby} 12 bytes, LobbyMatchList_t {uint32} 4.
enum { LOBBY_CREATED = 513, LOBBY_MATCH_LIST = 510, LOBBY_ENTER = 504, CALL_COMPLETED = 703, NO_CONNECTION = 3 };
typedef struct { uint64_t call; int32_t id, size; } Pending;
static Pending pending[8];
static int pending_count, released, delivering;
static uint64_t next_call = 0x51a0000001ull;
static uint8_t completed[16];   // SteamAPICallCompleted_t {SteamAPICall_t; int id; uint32 size}

static uint64_t fail_later(int32_t id, int32_t size) {
    pthread_mutex_lock(&lock);
    uint64_t call = 0;
    if (pending_count < 8) { call = next_call++; pending[pending_count++] = (Pending){call, id, size}; }
    pthread_mutex_unlock(&lock);
    return call;
}
// online_notice_ios.m: the player gave up; the game's requests fail now.
void steam_stub_online_give_up(void) {
    pthread_mutex_lock(&lock);
    released = pending_count > 0;
    pthread_mutex_unlock(&lock);
    fprintf(stderr, "[steam-stub] lobbies : échec renvoyé au jeu (pas de Steam sur iOS)\n");
}
EXPORT void SteamAPI_ManualDispatch_Init(void) {}
EXPORT void SteamAPI_ManualDispatch_RunFrame(int32_t pipe) { (void)pipe; }
typedef struct { int32_t user, id; uint8_t *param; int32_t size; } CallbackMsg;
EXPORT bool SteamAPI_ManualDispatch_GetNextCallback(int32_t pipe, void *message) {
    (void)pipe;
    pthread_mutex_lock(&lock);
    bool any = released && pending_count > 0;
    if (any) {
        Pending p = pending[0];
        memcpy(completed, &p.call, 8); memcpy(completed + 8, &p.id, 4); memcpy(completed + 12, &p.size, 4);
        *(CallbackMsg *)message = (CallbackMsg){1, CALL_COMPLETED, completed, 16};
        delivering = 1;
    }
    pthread_mutex_unlock(&lock);
    return any;
}
EXPORT void SteamAPI_ManualDispatch_FreeLastCallback(int32_t pipe) {
    (void)pipe;
    pthread_mutex_lock(&lock);
    if (delivering && pending_count > 0) {
        memmove(pending, pending + 1, sizeof(Pending) * (size_t)--pending_count);
        if (!pending_count) released = 0;
    }
    delivering = 0;
    pthread_mutex_unlock(&lock);
}
EXPORT bool SteamAPI_ManualDispatch_GetAPICallResult(int32_t pipe, uint64_t call, void *out, int32_t size, int32_t expected, bool *failed) {
    (void)pipe;
    pthread_mutex_lock(&lock);
    bool found = false;
    for (int i = 0; i < pending_count && !found; ++i) found = pending[i].call == call && pending[i].id == expected;
    pthread_mutex_unlock(&lock);
    if (failed) *failed = !found;
    if (!found || !out || size <= 0) return false;
    memset(out, 0, (size_t)size);
    if (expected == LOBBY_CREATED && size >= 4) { int32_t r = NO_CONNECTION; memcpy(out, &r, 4); }   // no lobby: no connection
    return true;   // LobbyMatchList_t: 0 lobbies
}

// ---- lobbies (round 24): no Steam to ask, never an answer ----
// The game's online co-op waits for a lobby: online_notice_ios.m explains,
// after a timeout, that it needs Steam, and its Back fails the requests
// (steam_stub_online_give_up above). JoinLobby (invites): handle 0, invalid.
void steam_stub_online_requested(const char *what);   // online_notice_ios.m
// Portal 2 (620) only: the other games keep the old answer (handle 0) and their offline flow.
EXPORT uint64_t SteamAPI_ISteamMatchmaking_CreateLobby(void *self, int32_t type, int32_t members) {
    (void)self; (void)type; (void)members;
    if (steam_stub_app_id() != 620) return 0;
    steam_stub_online_requested("CreateLobby");
    return fail_later(LOBBY_CREATED, 12);
}
EXPORT uint64_t SteamAPI_ISteamMatchmaking_RequestLobbyList(void *self) {
    (void)self;
    if (steam_stub_app_id() != 620) return 0;
    steam_stub_online_requested("RequestLobbyList");
    return fail_later(LOBBY_MATCH_LIST, 4);
}
EXPORT uint64_t SteamAPI_ISteamMatchmaking_JoinLobby(void *self, uint64_t lobby) {
    (void)self; (void)lobby;
    if (steam_stub_app_id() == 620) steam_stub_online_requested("JoinLobby");
    return 0;
}

// ---- tickets d'authentification ----
// En solo, le client se connecte quand même au serveur local et exige un
// ticket non vide (« STEAM certificate length error 0/2048 »). Ticket
// factice : rien ne lit son contenu (le serveur local du stub accepte tout),
// seule sa longueur compte ; jamais envoyé ailleurs.
int32_t steam_stub_ticket(void *buffer, int32_t capacity) {
    static const unsigned char magic[16] = "P2-iOS-offline";
    if (!buffer || capacity < (int32_t)sizeof(magic) + 8) return 0;
    memcpy(buffer, magic, sizeof(magic));
    uint64_t id = 76561197960265729ull;
    memcpy((unsigned char *)buffer + sizeof(magic), &id, sizeof(id));
    return (int32_t)sizeof(magic) + 8;
}

// ---- crochets d'avertissement (appelés par steam.c hors code généré) ----
EXPORT void SteamAPI_ISteamClient_SetWarningMessageHook(void *self, void *hook) { (void)self; (void)hook; }
EXPORT void SteamAPI_ISteamUtils_SetWarningMessageHook(void *self, void *hook) { (void)self; (void)hook; }

// ---- accès aux interfaces ----
// SteamInternal_ContextInit(ctx) : ctx = {pFn, compteur, contexte} ; pFn
// remplit le contexte quand le compteur global a changé.
typedef struct { void (*fn)(void *); uintptr_t counter; void *context[32]; } Context;
EXPORT void *SteamInternal_ContextInit(void *data) {
    Context *ctx = data;
    if (ctx->counter != 1) { ctx->counter = 1; ctx->fn(ctx->context); }
    return ctx->context;
}
EXPORT void *SteamInternal_CreateInterface(const char *version) { return steam_stub_interface(version); }
EXPORT void *SteamInternal_FindOrCreateUserInterface(int32_t user, const char *version) {
    (void)user; return steam_stub_interface(version);
}
EXPORT void *SteamInternal_FindOrCreateGameServerInterface(int32_t user, const char *version) {
    (void)user; return steam_stub_interface(version);
}
#endif
