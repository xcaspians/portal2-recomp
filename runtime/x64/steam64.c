// P2CE's Steam calls to Valve's own macOS libsteam_api.dylib (P2CE plan piece
// D, round 23). The Linux libsteam_api.so is not loaded: its exports resolve to
// these shims. No Steam emulation and nothing faked: the answers come from the
// Steam client running on the Mac, logged in by the player.
//   P2_STEAM_API_DYLIB  the dylib (Steamworks.NET 2024.8.0's, as for HL2 and Portal)
//   P2_STEAM_APPID      the game's Steam app (440000 for P2CE), as SteamAppId
// Flat functions pass through (native64.c). Three are done here instead:
//   - SteamInternal_ContextInit calls back into the game to fill its context;
//     a native library can't call lifted code, so the runtime does it;
//   - SteamAPI_RestartAppIfNecessary answers false: Steam is never asked to
//     relaunch anything;
//   - callbacks and call results go through Steam's manual dispatch: the
//     runtime runs the game's CCallbackBase::Run itself on its own thread.
// The interfaces' C++ methods are native code: lifted calls to them go
// through rt64_native_call (loader64.c, rt64_dispatch).
#include "rt64.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rt64_native_call(CPU64 *c, uint64_t fn);

static void *library(void) {
    static void *handle;
    static int tried;
    if (tried) return handle;
    tried = 1;
    const char *appid = getenv("P2_STEAM_APPID");
    if (appid && *appid) { setenv("SteamAppId", appid, 0); setenv("SteamGameId", appid, 0); }
    const char *path = getenv("P2_STEAM_API_DYLIB");
    if (!path || !*path) { fprintf(stderr, "steam64: P2_STEAM_API_DYLIB not set: Steam calls fail\n"); return NULL; }
    handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    fprintf(stderr, "steam64: %s %s\n", path, handle ? "loaded" : dlerror());
    return handle;
}
static void *host(const char *name) {
    void *h = library();
    void *fn = h ? dlsym(h, name) : NULL;
    if (!fn) fprintf(stderr, "steam64: %s missing in the macOS Steam library\n", name);
    return fn;
}
static uint64_t call0(const char *name) {   // host function without arguments, from C
    void *fn = host(name);
    return fn ? ((uint64_t (*)(void))fn)() : 0;
}

#define PASS(name)                                                         \
    static void s_##name(CPU64 *c) {                                       \
        static void *fn;                                                   \
        if (!fn && !(fn = host(#name))) { RET(c, 0); return; }             \
        rt64_native_call(c, (uint64_t)(uintptr_t)fn);                      \
    }
PASS(SteamAPI_GetHSteamUser)
PASS(SteamAPI_GetHSteamPipe)
PASS(SteamAPI_SetTryCatchCallbacks)
PASS(SteamAPI_Shutdown)
PASS(SteamGameServer_GetHSteamUser)
PASS(SteamGameServer_GetHSteamPipe)
PASS(SteamGameServer_Shutdown)
PASS(SteamInternal_CreateInterface)
PASS(SteamInternal_FindOrCreateUserInterface)
PASS(SteamInternal_FindOrCreateGameServerInterface)
PASS(SteamInternal_GameServer_Init_V2)

// Init: the real one, then manual dispatch (callbacks delivered by RunCallbacks below).
static unsigned init_count;   // SteamInternal_ContextInit's counter: contexts refill after each init
static void s_SteamAPI_InitSafe(CPU64 *c) {
    static void *fn;
    if (!fn && !(fn = host("SteamAPI_InitSafe"))) { RET(c, 0); return; }
    rt64_native_call(c, (uint64_t)(uintptr_t)fn);
    int ok = (int)(c->r[RAX] & 0xff);
    if (ok) { ++init_count; call0("SteamAPI_ManualDispatch_Init"); }
    fprintf(stderr, "steam64: SteamAPI_InitSafe %s (app %s)\n", ok ? "ok" : "FAILED (is Steam running and logged in?)",
            getenv("SteamAppId") ? getenv("SteamAppId") : "?");
}
// SDK 1.59+ start (P2CE's filesystem): ESteamAPIInitResult SteamInternal_SteamAPI_Init(
// const char *versions, SteamErrMsg *message); 0 is OK.
static void s_SteamInternal_SteamAPI_Init(CPU64 *c) {
    static void *fn;
    if (!fn && !(fn = host("SteamInternal_SteamAPI_Init"))) { RET(c, 1); return; }
    uint64_t message = ARG(c, 1);
    rt64_native_call(c, (uint64_t)(uintptr_t)fn);
    int result = (int)(c->r[RAX] & 0xffffffff);
    if (!result) { ++init_count; call0("SteamAPI_ManualDispatch_Init"); }
    fprintf(stderr, "steam64: SteamInternal_SteamAPI_Init %s%s%s (app %s)\n", result ? "FAILED: " : "ok",
            result && message ? (const char *)(uintptr_t)message : "", result ? "" : "",
            getenv("SteamAppId") ? getenv("SteamAppId") : "?");
}
static void s_SteamAPI_RestartAppIfNecessary(CPU64 *c) {
    fprintf(stderr, "steam64: SteamAPI_RestartAppIfNecessary(%u): no (never relaunched through Steam)\n",
            (unsigned)ARG(c, 0));
    RET(c, 0);
}
// struct { void (*fill)(void *context); uintptr_t counter; context... }: filled
// again by the game's own function when the counter is behind (steam_api_internal.h).
static void s_SteamInternal_ContextInit(CPU64 *c) {
    uint64_t data = ARG(c, 0);
    if (R64(data + 8) != init_count) {
        uint64_t saved[16];
        memcpy(saved, c->r, sizeof saved);
        c->r[RDI] = data + 16;
        rt64_call(c, R64(data));
        memcpy(c->r, saved, sizeof saved);
        W64(data + 8, init_count);
    }
    RET(c, data + 16);
}

// ---- callbacks (CCallbackBase: vptr; uint8 flags at +8; int id at +12) ----
enum { MAX_REG = 256, FLAG_REGISTERED = 1, FLAG_GAMESERVER = 2, CALL_COMPLETED = 703 };
typedef struct { uint64_t object; int id; uint64_t call; int is_result; } Registration;
static Registration regs[MAX_REG];
static int reg_count;
static pthread_mutex_t reg_lock = PTHREAD_MUTEX_INITIALIZER;

static void add(uint64_t object, int id, uint64_t call, int is_result) {
    pthread_mutex_lock(&reg_lock);
    if (reg_count < MAX_REG) regs[reg_count++] = (Registration){object, id, call, is_result};
    pthread_mutex_unlock(&reg_lock);
}
static void drop(uint64_t object, uint64_t call, int is_result) {
    pthread_mutex_lock(&reg_lock);
    for (int i = 0; i < reg_count; ++i)
        if (regs[i].object == object && regs[i].is_result == is_result && (!is_result || regs[i].call == call)) {
            regs[i] = regs[--reg_count]; --i;
        }
    pthread_mutex_unlock(&reg_lock);
}
static void s_SteamAPI_RegisterCallback(CPU64 *c) {
    uint64_t object = ARG(c, 0);
    int id = (int)ARG(c, 1);
    W8(object + 8, R8(object + 8) | FLAG_REGISTERED);
    W32(object + 12, (uint32_t)id);
    add(object, id, 0, 0);
}
static void s_SteamAPI_UnregisterCallback(CPU64 *c) {
    uint64_t object = ARG(c, 0);
    W8(object + 8, R8(object + 8) & ~FLAG_REGISTERED);
    drop(object, 0, 0);
}
static void s_SteamAPI_RegisterCallResult(CPU64 *c) { add(ARG(c, 0), 0, ARG(c, 1), 1); }
static void s_SteamAPI_UnregisterCallResult(CPU64 *c) { drop(ARG(c, 0), ARG(c, 1), 1); }

// The game's virtual Run: slot 0 Run(void *param), slot 1 Run(void *param, bool failed, SteamAPICall_t).
static void run_guest(CPU64 *c, uint64_t object, int slot, uint64_t param, int failed, uint64_t call) {
    uint64_t saved[16];
    memcpy(saved, c->r, sizeof saved);
    c->r[RDI] = object; c->r[RSI] = param; c->r[RDX] = (uint64_t)failed; c->r[RCX] = call;
    rt64_call(c, R64(R64(object) + 8 * (uint64_t)slot));
    memcpy(c->r, saved, sizeof saved);
}
typedef struct { int32_t user; int32_t id; uint8_t *param; int32_t size; } CallbackMsg;
static void dispatch(CPU64 *c, int game_server) {
    static void *run_frame, *next, *free_last, *result;
    if (!run_frame) {
        run_frame = host("SteamAPI_ManualDispatch_RunFrame");
        next = host("SteamAPI_ManualDispatch_GetNextCallback");
        free_last = host("SteamAPI_ManualDispatch_FreeLastCallback");
        result = host("SteamAPI_ManualDispatch_GetAPICallResult");
        if (!run_frame || !next || !free_last || !result) return;
    }
    int32_t pipe = (int32_t)call0(game_server ? "SteamGameServer_GetHSteamPipe" : "SteamAPI_GetHSteamPipe");
    if (!pipe) return;
    ((void (*)(int32_t))run_frame)(pipe);
    CallbackMsg msg;
    while (((bool (*)(int32_t, CallbackMsg *))next)(pipe, &msg)) {
        if (msg.id == CALL_COMPLETED) {   // SteamAPICallCompleted_t: { SteamAPICall_t call; int id; uint32 size; }
            uint64_t call; int32_t id; uint32_t size;
            memcpy(&call, msg.param, 8); memcpy(&id, msg.param + 8, 4); memcpy(&size, msg.param + 12, 4);
            void *data = calloc(1, size ? size : 1);
            bool failed = false;
            if (data && ((bool (*)(int32_t, uint64_t, void *, int, int, bool *))result)(pipe, call, data, (int)size, id, &failed)) {
                for (int i = 0; i < reg_count; ++i)
                    if (regs[i].is_result && regs[i].call == call) {
                        uint64_t object = regs[i].object;
                        drop(object, call, 1);
                        run_guest(c, object, 1, (uint64_t)(uintptr_t)data, failed, call);
                        break;
                    }
            }
            free(data);
        } else {
            for (int i = 0; i < reg_count; ++i) {
                if (regs[i].is_result || regs[i].id != msg.id) continue;
                int server = (R8(regs[i].object + 8) & FLAG_GAMESERVER) != 0;
                if (server == game_server) run_guest(c, regs[i].object, 0, (uint64_t)(uintptr_t)msg.param, 0, 0);
            }
        }
        ((void (*)(int32_t))free_last)(pipe);
    }
}
static void s_SteamAPI_RunCallbacks(CPU64 *c) { dispatch(c, 0); }
static void s_SteamGameServer_RunCallbacks(CPU64 *c) { dispatch(c, 1); }

#define S(name) {#name, s_##name}
static const RT64Shim steam_shims[] = {
    S(SteamAPI_GetHSteamUser), S(SteamAPI_GetHSteamPipe), S(SteamAPI_SetTryCatchCallbacks), S(SteamAPI_Shutdown),
    S(SteamGameServer_GetHSteamUser), S(SteamGameServer_GetHSteamPipe), S(SteamGameServer_Shutdown),
    S(SteamInternal_CreateInterface), S(SteamInternal_FindOrCreateUserInterface),
    S(SteamInternal_FindOrCreateGameServerInterface), S(SteamInternal_GameServer_Init_V2),
    S(SteamAPI_InitSafe), S(SteamInternal_SteamAPI_Init), S(SteamAPI_RestartAppIfNecessary), S(SteamInternal_ContextInit),
    S(SteamAPI_RegisterCallback), S(SteamAPI_UnregisterCallback), S(SteamAPI_RegisterCallResult),
    S(SteamAPI_UnregisterCallResult), S(SteamAPI_RunCallbacks), S(SteamGameServer_RunCallbacks),
    {NULL, NULL},
};
RT64_SHIMS(steam_shims, NULL)
