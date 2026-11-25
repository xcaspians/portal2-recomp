/* Généré par tools/gen_steam.py depuis les libsteam_api.dylib officielles (SDK 1.53a, 1.60,
 * 1.61), le libsteam_api.so i386 de Portal Stories: Mel (SDK 1.40, emplacements seulement)
 * et Steamworks.NET 20.1.0, 2024.8.0, 2025.161.0 (MIT). Ne pas modifier : relancer
 * le générateur. */
#include "steam.h"

void steam_manual_client_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
void steam_manual_utils_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppBuildId */
static void m0(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppBuildId"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppInstallDir */
static void m1(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppInstallDir"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppName */
static void m2(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppName"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetInstalledApps */
static void m3(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetInstalledApps"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 4u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetNumInstalledApps */
static void m4(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetNumInstalledApps"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::BGetDLCDataByIndex */
static void m5(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BGetDLCDataByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 1u);
    void *a3 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsAppInstalled */
static void m6(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsAppInstalled"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsCybercafe */
static void m7(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsCybercafe"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsDlcInstalled */
static void m8(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsDlcInstalled"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsLowViolence */
static void m9(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsLowViolence"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribed */
static void m10(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribed"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedApp */
static void m11(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedApp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFamilySharing */
static void m12(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedFromFamilySharing"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFreeWeekend */
static void m13(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedFromFreeWeekend"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsTimedTrial */
static void m14(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsTimedTrial"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsVACBanned */
static void m15(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsVACBanned"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppBuildId */
static void m16(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppBuildId"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppInstallDir */
static void m17(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppInstallDir"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppOwner */
static void m18(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppOwner"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAvailableGameLanguages */
static void m19(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAvailableGameLanguages"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentBetaName */
static void m20(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetCurrentBetaName"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentGameLanguage */
static void m21(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetCurrentGameLanguage"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDLCCount */
static void m22(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetDLCCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDlcDownloadProgress */
static void m23(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetDlcDownloadProgress"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    void *a2 = steam_out(c, R32(args + 8u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetEarliestPurchaseUnixTime */
static void m24(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetEarliestPurchaseUnixTime"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetFileDetails */
static void m25(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetFileDetails"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetInstalledDepots */
static void m26(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetInstalledDepots"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetLaunchCommandLine */
static void m27(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetLaunchCommandLine"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetLaunchQueryParam */
static void m28(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetLaunchQueryParam"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::InstallDLC */
static void m29(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_InstallDLC"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::MarkContentCorrupt */
static void m30(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_MarkContentCorrupt"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAllProofOfPurchaseKeys */
static void m31(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_RequestAllProofOfPurchaseKeys"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAppProofOfPurchaseKey */
static void m32(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_RequestAppProofOfPurchaseKey"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::UninstallDLC */
static void m33(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_UninstallDLC"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AddHeader */
static void m34(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_AddHeader"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AllowStartRequest */
static void m35(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_AllowStartRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CopyToClipboard */
static void m36(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_CopyToClipboard"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CreateBrowser */
static void m37(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_CreateBrowser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ExecuteJavascript */
static void m38(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_ExecuteJavascript"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Find */
static void m40(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, bool, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Find"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool a2 = (R8(args + 8u) != 0);
    bool a3 = (R8(args + 12u) != 0);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GetLinkAtPosition */
static void m41(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GetLinkAtPosition"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoBack */
static void m42(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GoBack"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoForward */
static void m43(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GoForward"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Init */
static void m44(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Init"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::JSDialogResponse */
static void m45(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_JSDialogResponse"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyChar */
static void m46(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyChar"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyDown */
static void m47(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyDown"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyUp */
static void m48(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyUp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::LoadURL */
static void m49(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_LoadURL"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDoubleClick */
static void m50(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseDoubleClick"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDown */
static void m51(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseDown"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseMove */
static void m52(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseMove"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseUp */
static void m53(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseUp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseWheel */
static void m54(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseWheel"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::OpenDeveloperTools */
static void m55(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_OpenDeveloperTools"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::PasteFromClipboard */
static void m56(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_PasteFromClipboard"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Reload */
static void m57(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Reload"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::RemoveBrowser */
static void m58(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_RemoveBrowser"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetBackgroundMode */
static void m59(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetBackgroundMode"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetCookie */
static void m60(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, const char *, const char *, uint32_t, bool, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetCookie"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    const char *a3 = steam_in_string(c, R32(args + 12u));
    uint32_t a4 = (uint32_t)R32(args + 16u);
    bool a5 = (R8(args + 20u) != 0);
    bool a6 = (R8(args + 24u) != 0);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetDPIScalingFactor */
static void m61(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetDPIScalingFactor"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    float a1 = RF(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetHorizontalScroll */
static void m62(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetHorizontalScroll"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetKeyFocus */
static void m63(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetKeyFocus"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetPageScaleFactor */
static void m64(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, float, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetPageScaleFactor"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    float a1 = RF(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetSize */
static void m65(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetVerticalScroll */
static void m66(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetVerticalScroll"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Shutdown */
static void m67(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopFind */
static void m68(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_StopFind"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopLoad */
static void m69(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_StopLoad"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ViewSource */
static void m70(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_ViewSource"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTTP_INTERFACE_VERSION002::CreateCookieContainer */
static void m71(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, bool);
    void *fn = steam_slot(c, self, 17u);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION002::CreateHTTPRequest */
static void m72(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 0u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION002::DeferHTTPRequest */
static void m73(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPDownloadProgressPct */
static void m74(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPRequestWasTimedOut */
static void m75(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 24u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseBodyData */
static void m76(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 12u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseBodySize */
static void m77(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseHeaderSize */
static void m78(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseHeaderValue */
static void m79(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 10u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::GetHTTPStreamingResponseBodyData */
static void m80(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::PrioritizeHTTPRequest */
static void m81(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::ReleaseCookieContainer */
static void m82(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::ReleaseHTTPRequest */
static void m83(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SendHTTPRequest */
static void m84(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SendHTTPRequestAndStreamResponse */
static void m85(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetCookie */
static void m86(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *, const char *);
    void *fn = steam_slot(c, self, 19u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    const char *a3 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestAbsoluteTimeoutMS */
static void m87(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 23u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestContextValue */
static void m88(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 1u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestCookieContainer */
static void m89(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestGetOrPostParameter */
static void m90(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestHeaderValue */
static void m91(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 3u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestNetworkActivityTimeout */
static void m92(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestRequiresVerifiedCertificate */
static void m94(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 22u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestUserAgentInfo */
static void m95(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateCookieContainer */
static void m96(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_CreateCookieContainer"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateHTTPRequest */
static void m97(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_CreateHTTPRequest"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::DeferHTTPRequest */
static void m98(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_DeferHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPDownloadProgressPct */
static void m99(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPDownloadProgressPct"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPRequestWasTimedOut */
static void m100(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPRequestWasTimedOut"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodyData */
static void m101(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseBodyData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodySize */
static void m102(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseBodySize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderSize */
static void m103(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseHeaderSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderValue */
static void m104(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseHeaderValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPStreamingResponseBodyData */
static void m105(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPStreamingResponseBodyData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::PrioritizeHTTPRequest */
static void m106(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_PrioritizeHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseCookieContainer */
static void m107(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_ReleaseCookieContainer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseHTTPRequest */
static void m108(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_ReleaseHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequest */
static void m109(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SendHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequestAndStreamResponse */
static void m110(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SendHTTPRequestAndStreamResponse"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetCookie */
static void m111(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetCookie"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    const char *a3 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestAbsoluteTimeoutMS */
static void m112(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestAbsoluteTimeoutMS"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestContextValue */
static void m113(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestContextValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestCookieContainer */
static void m114(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestCookieContainer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestGetOrPostParameter */
static void m115(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestGetOrPostParameter"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestHeaderValue */
static void m116(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestHeaderValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestNetworkActivityTimeout */
static void m117(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestNetworkActivityTimeout"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRequiresVerifiedCertificate */
static void m119(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestRequiresVerifiedCertificate"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestUserAgentInfo */
static void m120(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestUserAgentInfo"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::AddPromoItem */
static void m121(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_AddPromoItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::CheckResultSteamID */
static void m123(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_CheckResultSteamID"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::ConsumeItem */
static void m124(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_ConsumeItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::DeserializeResult */
static void m125(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_DeserializeResult"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::DestroyResult */
static void m126(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_DestroyResult"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMINVENTORY_INTERFACE_V003::GetAllItems */
static void m129(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetAllItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetItemPrice */
static void m133(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetItemPrice"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    void *a2 = steam_out(c, R32(args + 8u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetItemsByID */
static void m134(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetItemsByID"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetNumItemsWithPrices */
static void m136(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetNumItemsWithPrices"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultStatus */
static void m139(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetResultStatus"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultTimestamp */
static void m140(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetResultTimestamp"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GrantPromoItems */
static void m141(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GrantPromoItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::InspectItem */
static void m142(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_InspectItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::LoadItemDefinitions */
static void m143(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_LoadItemDefinitions"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::RemoveProperty */
static void m144(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RemoveProperty"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::RequestEligiblePromoItemDefinitionsIDs */
static void m145(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RequestEligiblePromoItemDefinitionsIDs"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::RequestPrices */
static void m146(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RequestPrices"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SendItemDropHeartbeat */
static void m147(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SendItemDropHeartbeat"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyBool */
static void m149(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyBool"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    bool a3 = (R8(args + 20u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyFloat */
static void m150(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    float a3 = RF(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyInt64 */
static void m151(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, int64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyInt64"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    int64_t a3 = (int64_t)R64(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyString */
static void m152(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyString"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    const char *a3 = steam_in_string(c, R32(args + 20u));
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::StartUpdateProperties */
static void m154(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_StartUpdateProperties"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SubmitUpdateProperties */
static void m155(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SubmitUpdateProperties"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::TransferItemQuantity */
static void m156(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_TransferItemQuantity"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t a3 = (uint64_t)R64(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::TriggerItemDrop */
static void m157(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_TriggerItemDrop"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BActivationSuccess */
static void m158(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_BActivationSuccess"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BIsCurrentMusicRemote */
static void m159(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_BIsCurrentMusicRemote"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryDidChange */
static void m160(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryIsAvailable */
static void m161(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryIsAvailable"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryWillChange */
static void m162(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::DeregisterSteamMusicRemote */
static void m163(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_DeregisterSteamMusicRemote"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableLooped */
static void m164(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableLooped"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayNext */
static void m165(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlayNext"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayPrevious */
static void m166(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlayPrevious"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlaylists */
static void m167(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlaylists"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableQueue */
static void m168(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableQueue"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableShuffled */
static void m169(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableShuffled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistDidChange */
static void m170(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_PlaylistDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistWillChange */
static void m171(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_PlaylistWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueDidChange */
static void m172(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_QueueDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueWillChange */
static void m173(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_QueueWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::RegisterSteamMusicRemote */
static void m174(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_RegisterSteamMusicRemote"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetPlaylistEntries */
static void m175(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_ResetPlaylistEntries"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetQueueEntries */
static void m176(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_ResetQueueEntries"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentPlaylistEntry */
static void m177(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetCurrentPlaylistEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentQueueEntry */
static void m178(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetCurrentQueueEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetDisplayName */
static void m179(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetDisplayName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPNGIcon_64x64 */
static void m180(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetPNGIcon_64x64"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPlaylistEntry */
static void m181(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetPlaylistEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetQueueEntry */
static void m182(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetQueueEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryCoverArt */
static void m183(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryCoverArt"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryElapsedSeconds */
static void m184(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryElapsedSeconds"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryText */
static void m185(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryText"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateLooped */
static void m186(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateLooped"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdatePlaybackStatus */
static void m187(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdatePlaybackStatus"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateShuffled */
static void m188(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateShuffled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateVolume */
static void m189(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateVolume"), __ATOMIC_RELAXED);
    float a0 = RF(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsEnabled */
static void m190(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_BIsEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsPlaying */
static void m191(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_BIsPlaying"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetPlaybackStatus */
static void m192(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_GetPlaybackStatus"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetVolume */
static void m193(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef float (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_GetVolume"), __ATOMIC_RELAXED);
    float r = ((F)fn)(self);
    rt_return_st0(c, r);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Pause */
static void m194(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_Pause"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Play */
static void m195(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_Play"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayNext */
static void m196(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_PlayNext"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayPrevious */
static void m197(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_PlayPrevious"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::SetVolume */
static void m198(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_SetVolume"), __ATOMIC_RELAXED);
    float a0 = RF(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppBlocked */
static void m199(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsAppBlocked"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppInBlockList */
static void m200(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsAppInBlockList"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureBlocked */
static void m201(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsFeatureBlocked"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureInBlockList */
static void m202(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsFeatureInBlockList"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockEnabled */
static void m203(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsParentalLockEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockLocked */
static void m204(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsParentalLockLocked"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BGetSessionClientResolution */
static void m205(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BSendRemotePlayTogetherInvite */
static void m206(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BStartRemotePlayTogether */
static void m207(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 6u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientFormFactor */
static void m208(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientName */
static void m209(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionCount */
static void m210(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionID */
static void m211(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionSteamID */
static void m212(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::CommitPublishedFileUpdate */
static void m213(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::CreatePublishedFileUpdateRequest */
static void m214(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::DeletePublishedFile */
static void m215(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumeratePublishedFilesByUserAction */
static void m216(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, uint32_t);
    void *fn = steam_slot(c, self, 52u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumerateUserPublishedFiles */
static void m218(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumerateUserSubscribedFiles */
static void m220(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 43u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileDelete */
static void m221(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 6u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileExists */
static void m222(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileForget */
static void m223(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FilePersisted */
static void m224(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileRead */
static void m225(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileReadAsync */
static void m226(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileReadAsyncComplete */
static void m227(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileShare */
static void m228(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWrite */
static void m229(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, int32_t);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteAsync */
static void m230(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamCancel */
static void m231(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamClose */
static void m232(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamOpen */
static void m233(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 9u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamWriteChunk */
static void m234(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetCachedUGCCount */
static void m235(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetCachedUGCHandle */
static void m236(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileCount */
static void m237(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileNameAndSize */
static void m238(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileSize */
static void m239(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileTimestamp */
static void m240(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 16u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetPublishedFileDetails */
static void m241(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetPublishedItemVoteDetails */
static void m242(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetQuota */
static void m243(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    void *fn = steam_slot(c, self, 20u);
    void *a0 = steam_out(c, R32(args + 0u), 8u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetSyncPlatforms */
static void m244(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 17u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUGCDetails */
static void m245(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, const char **, void *, void *);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2_slot = steam_out(c, R32(args + 12u), 4u); const char *a2_text = NULL;
    const char **a2 = a2_slot ? &a2_text : NULL;
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    void *a4 = steam_out(c, R32(args + 20u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    if (a2_slot) { uint32_t a2_guest = steam_guest_string(c, a2_text); memcpy(a2_slot, &a2_guest, 4); }
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUGCDownloadProgress */
static void m246(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUserPublishedItemVoteDetails */
static void m247(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::IsCloudEnabledForAccount */
static void m248(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::IsCloudEnabledForApp */
static void m249(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetCloudEnabledForApp */
static void m252(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 23u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetSyncPlatforms */
static void m253(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetUserPublishedFileAction */
static void m254(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 51u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::SubscribePublishedFile */
static void m255(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCDownload */
static void m256(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCDownloadToLocation */
static void m257(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCRead */
static void m258(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UnsubscribePublishedFile */
static void m259(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 44u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileDescription */
static void m260(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileFile */
static void m261(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFilePreviewFile */
static void m262(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileSetChangeDescription */
static void m263(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileTitle */
static void m265(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileVisibility */
static void m266(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdateUserPublishedItemVote */
static void m267(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch */
static void m268(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_BeginFileWriteBatch"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate */
static void m269(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_CommitPublishedFileUpdate"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest */
static void m270(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_CreatePublishedFileUpdateRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile */
static void m271(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_DeletePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch */
static void m272(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EndFileWriteBatch"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction */
static void m273(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumeratePublishedFilesByUserAction"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles */
static void m275(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumerateUserPublishedFiles"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles */
static void m277(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumerateUserSubscribedFiles"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete */
static void m278(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileDelete"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists */
static void m279(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileExists"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget */
static void m280(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileForget"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted */
static void m281(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FilePersisted"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead */
static void m282(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileRead"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync */
static void m283(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileReadAsync"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete */
static void m284(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileReadAsyncComplete"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare */
static void m285(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileShare"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite */
static void m286(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWrite"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync */
static void m287(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteAsync"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel */
static void m288(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamCancel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose */
static void m289(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamClose"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen */
static void m290(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamOpen"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk */
static void m291(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamWriteChunk"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount */
static void m292(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetCachedUGCCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle */
static void m293(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetCachedUGCHandle"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount */
static void m294(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize */
static void m295(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileNameAndSize"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize */
static void m296(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileSize"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp */
static void m297(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileTimestamp"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange */
static void m298(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetLocalFileChange"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount */
static void m299(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetLocalFileChangeCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails */
static void m300(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetPublishedFileDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails */
static void m301(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetPublishedItemVoteDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota */
static void m302(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetQuota"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 8u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms */
static void m303(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetSyncPlatforms"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails */
static void m304(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, const char **, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUGCDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2_slot = steam_out(c, R32(args + 12u), 4u); const char *a2_text = NULL;
    const char **a2 = a2_slot ? &a2_text : NULL;
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    void *a4 = steam_out(c, R32(args + 20u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    if (a2_slot) { uint32_t a2_guest = steam_guest_string(c, a2_text); memcpy(a2_slot, &a2_guest, 4); }
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDownloadProgress */
static void m305(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUGCDownloadProgress"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails */
static void m306(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUserPublishedItemVoteDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount */
static void m307(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_IsCloudEnabledForAccount"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp */
static void m308(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_IsCloudEnabledForApp"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp */
static void m309(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetCloudEnabledForApp"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms */
static void m310(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetSyncPlatforms"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction */
static void m311(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetUserPublishedFileAction"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile */
static void m312(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SubscribePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload */
static void m313(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCDownload"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation */
static void m314(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCDownloadToLocation"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead */
static void m315(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCRead"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile */
static void m316(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UnsubscribePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription */
static void m317(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile */
static void m318(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile */
static void m319(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFilePreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription */
static void m320(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileSetChangeDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle */
static void m322(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileTitle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility */
static void m323(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileVisibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote */
static void m324(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdateUserPublishedItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary */
static void m325(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, const char *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_AddScreenshotToLibrary"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary */
static void m326(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_AddVRScreenshotToLibrary"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots */
static void m327(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_HookScreenshots"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked */
static void m328(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_IsScreenshotsHooked"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation */
static void m329(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_SetLocation"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile */
static void m330(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TagPublishedFile"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser */
static void m331(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TagUser"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot */
static void m332(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TriggerScreenshot"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot */
static void m333(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_WriteScreenshot"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMTIMELINE_INTERFACE_V004::AddGamePhaseTag */
static void m334(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, const char *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMTIMELINE_INTERFACE_V004::AddInstantaneousTimelineEvent */
static void m335(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, int32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    int32_t a5 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::AddRangeTimelineEvent */
static void m336(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, float, int32_t);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    float a5 = RF(args + 20u);
    int32_t a6 = (int32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::ClearTimelineTooltip */
static void m337(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, float);
    void *fn = steam_slot(c, self, 1u);
    float a0 = RF(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::DoesEventRecordingExist */
static void m338(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::DoesGamePhaseRecordingExist */
static void m339(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::EndGamePhase */
static void m340(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    ((F)fn)(self);
}
/* STEAMTIMELINE_INTERFACE_V004::EndRangeTimelineEvent */
static void m341(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, float);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    float a1 = RF(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* STEAMTIMELINE_INTERFACE_V004::OpenOverlayToGamePhase */
static void m342(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 16u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::OpenOverlayToTimelineEvent */
static void m343(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::RemoveTimelineEvent */
static void m344(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetGamePhaseAttribute */
static void m345(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t a2 = (uint32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMTIMELINE_INTERFACE_V004::SetGamePhaseID */
static void m346(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 12u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetTimelineGameMode */
static void m347(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetTimelineTooltip */
static void m348(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, float);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMTIMELINE_INTERFACE_V004::StartGamePhase */
static void m349(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    ((F)fn)(self);
}
/* STEAMTIMELINE_INTERFACE_V004::StartRangeTimelineEvent */
static void m350(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, int32_t);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    int32_t a5 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::UpdateRangeTimelineEvent */
static void m351(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, const char *, const char *, const char *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    const char *a3 = steam_in_string(c, R32(args + 16u));
    uint32_t a4 = (uint32_t)R32(args + 20u);
    int32_t a5 = (int32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* STEAMUGC_INTERFACE_VERSION010::AddAppDependency */
static void m352(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 70u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::AddDependency */
static void m353(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 68u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::AddExcludedTag */
static void m354(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::AddItemKeyValueTag */
static void m355(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 43u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::AddItemPreviewFile */
static void m356(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    void *fn = steam_slot(c, self, 44u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::AddItemPreviewVideo */
static void m357(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::AddItemToFavorites */
static void m358(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 53u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::AddRequiredKeyValueTag */
static void m359(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::AddRequiredTag */
static void m360(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 14u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::BInitWorkshopForGameServer */
static void m361(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 63u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::CreateItem */
static void m362(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 32u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::CreateQueryAllUGCRequest */
static void m363(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::CreateQueryUGCDetailsRequest */
static void m364(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::CreateQueryUserUGCRequest */
static void m365(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 0u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    uint32_t a6 = (uint32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::DeleteItem */
static void m366(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 73u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::DownloadItem */
static void m367(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetAppDependencies */
static void m368(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 72u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::GetItemDownloadInfo */
static void m369(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetItemInstallInfo */
static void m370(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetItemState */
static void m371(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 59u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetItemUpdateProgress */
static void m372(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 50u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetNumSubscribedItems */
static void m373(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 57u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCAdditionalPreview */
static void m374(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    void *a5 = steam_out_buffer(c, R32(args + 24u), (int64_t)(int32_t)R32(args + 28u), 1u);
    uint32_t a6 = (uint32_t)R32(args + 28u);
    void *a7 = steam_out(c, R32(args + 32u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6, a7);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCChildren */
static void m375(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCKeyValueTag */
static void m376(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    void *a5 = steam_out_buffer(c, R32(args + 24u), (int64_t)(int32_t)R32(args + 28u), 1u);
    uint32_t a6 = (uint32_t)R32(args + 28u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCMetadata */
static void m377(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCNumAdditionalPreviews */
static void m378(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCNumKeyValueTags */
static void m379(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCPreviewURL */
static void m380(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCResult */
static void m381(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 9764u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetQueryUGCStatistic */
static void m382(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::GetSubscribedItems */
static void m383(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 58u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION010::GetUserItemVote */
static void m384(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::ReleaseQueryUGCRequest */
static void m385(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::RemoveAppDependency */
static void m386(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 71u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::RemoveDependency */
static void m387(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 69u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::RemoveItemFromFavorites */
static void m388(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 54u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::RemoveItemKeyValueTags */
static void m389(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::RemoveItemPreview */
static void m390(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::RequestUGCDetails */
static void m391(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::SendQueryUGCRequest */
static void m392(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::SetAllowCachedResponse */
static void m393(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetCloudFileNameFilter */
static void m394(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemContent */
static void m395(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemDescription */
static void m396(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemMetadata */
static void m397(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemPreview */
static void m398(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemTitle */
static void m400(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemUpdateLanguage */
static void m401(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetItemVisibility */
static void m402(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetLanguage */
static void m403(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetMatchAnyTag */
static void m404(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetRankedByTrendDays */
static void m405(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnAdditionalPreviews */
static void m406(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnChildren */
static void m407(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnKeyValueTags */
static void m408(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnLongDescription */
static void m409(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnMetadata */
static void m410(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnOnlyIDs */
static void m411(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnPlaytimeStats */
static void m412(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 23u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetReturnTotalOnly */
static void m413(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetSearchText */
static void m414(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::SetUserItemVote */
static void m415(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 51u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::StartItemUpdate */
static void m416(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 33u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::StartPlaytimeTracking */
static void m417(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 65u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::StopPlaytimeTracking */
static void m418(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 66u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::StopPlaytimeTrackingForAllItems */
static void m419(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 67u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::SubmitItemUpdate */
static void m420(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::SubscribeItem */
static void m421(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::SuspendDownloads */
static void m422(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 64u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION010::UnsubscribeItem */
static void m423(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION010::UpdateItemPreviewFile */
static void m424(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION010::UpdateItemPreviewVideo */
static void m425(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddAppDependency */
static void m426(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddAppDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddDependency */
static void m427(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddExcludedTag */
static void m428(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddExcludedTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag */
static void m429(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile */
static void m430(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemPreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo */
static void m431(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemPreviewVideo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites */
static void m432(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemToFavorites"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag */
static void m433(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddRequiredKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredTag */
static void m434(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddRequiredTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer */
static void m436(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_BInitWorkshopForGameServer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::CreateItem */
static void m437(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateItem"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor */
static void m438(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryAllUGCRequestCursor"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage */
static void m439(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryAllUGCRequestPage"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryUGCDetailsRequest */
static void m440(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryUGCDetailsRequest"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest */
static void m441(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryUserUGCRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    uint32_t a6 = (uint32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::DeleteItem */
static void m442(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_DeleteItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::DownloadItem */
static void m443(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_DownloadItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetAppDependencies */
static void m444(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetAppDependencies"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo */
static void m445(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemDownloadInfo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo */
static void m446(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemInstallInfo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemState */
static void m447(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemState"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress */
static void m448(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemUpdateProgress"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems */
static void m449(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetNumSubscribedItems"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag */
static void m450(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryFirstUGCKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren */
static void m451(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCChildren"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag */
static void m452(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    void *a5 = steam_out_buffer(c, R32(args + 24u), (int64_t)(int32_t)R32(args + 28u), 1u);
    uint32_t a6 = (uint32_t)R32(args + 28u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCMetadata */
static void m453(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews */
static void m454(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumAdditionalPreviews"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags */
static void m455(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags */
static void m456(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL */
static void m457(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCPreviewURL"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCResult */
static void m458(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCResult"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 9764u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic */
static void m459(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCStatistic"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag */
static void m460(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName */
static void m461(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCTagDisplayName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems */
static void m462(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetSubscribedItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetUserItemVote */
static void m463(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetUserItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus */
static void m464(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetWorkshopEULAStatus"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest */
static void m465(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_ReleaseQueryUGCRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags */
static void m466(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveAllItemKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency */
static void m467(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveAppDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveDependency */
static void m468(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites */
static void m469(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemFromFavorites"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags */
static void m470(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview */
static void m471(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemPreview"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails */
static void m472(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RequestUGCDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest */
static void m473(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SendQueryUGCRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse */
static void m474(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetAllowCachedResponse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload */
static void m475(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetAllowLegacyUpload"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter */
static void m476(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetCloudFileNameFilter"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemContent */
static void m477(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemContent"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemDescription */
static void m478(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemMetadata */
static void m479(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemPreview */
static void m480(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemPreview"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemTitle */
static void m482(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemTitle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage */
static void m483(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemUpdateLanguage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemVisibility */
static void m484(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemVisibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetLanguage */
static void m485(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetLanguage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag */
static void m486(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetMatchAnyTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays */
static void m487(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetRankedByTrendDays"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews */
static void m488(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnAdditionalPreviews"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnChildren */
static void m489(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnChildren"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags */
static void m490(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription */
static void m491(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnLongDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata */
static void m492(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs */
static void m493(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnOnlyIDs"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats */
static void m494(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnPlaytimeStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly */
static void m495(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnTotalOnly"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetSearchText */
static void m496(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetSearchText"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange */
static void m497(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetTimeCreatedDateRange"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange */
static void m498(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetTimeUpdatedDateRange"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetUserItemVote */
static void m499(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetUserItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA */
static void m500(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_ShowWorkshopEULA"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::StartItemUpdate */
static void m501(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StartItemUpdate"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::StartPlaytimeTracking */
static void m502(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StartPlaytimeTracking"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTracking */
static void m503(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StopPlaytimeTracking"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems */
static void m504(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StopPlaytimeTrackingForAllItems"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate */
static void m505(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SubmitItemUpdate"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubscribeItem */
static void m506(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SubscribeItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SuspendDownloads */
static void m507(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SuspendDownloads"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem */
static void m508(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UnsubscribeItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile */
static void m509(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UpdateItemPreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo */
static void m510(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UpdateItemPreviewVideo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddAppDependency */
static void m511(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 87u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddContentDescriptor */
static void m512(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 63u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddDependency */
static void m513(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 85u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddExcludedTag */
static void m514(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemKeyValueTag */
static void m515(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 57u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemPreviewFile */
static void m516(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    void *fn = steam_slot(c, self, 58u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemPreviewVideo */
static void m517(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 59u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemToFavorites */
static void m518(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 70u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddRequiredKeyValueTag */
static void m519(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddRequiredTag */
static void m520(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::BInitWorkshopForGameServer */
static void m522(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 80u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::CreateItem */
static void m523(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 44u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestCursor */
static void m524(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestPage */
static void m525(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryUGCDetailsRequest */
static void m526(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryUserUGCRequest */
static void m527(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 0u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    uint32_t a6 = (uint32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::DeleteItem */
static void m528(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 90u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::DownloadItem */
static void m529(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 79u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetAppDependencies */
static void m530(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 89u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemDownloadInfo */
static void m531(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 78u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemInstallInfo */
static void m532(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 77u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemState */
static void m533(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 76u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemUpdateProgress */
static void m534(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 67u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetNumSubscribedItems */
static void m535(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 74u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetNumSupportedGameVersions */
static void m536(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryFirstUGCKeyValueTag */
static void m537(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCChildren */
static void m538(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCContentDescriptors */
static void m539(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 4u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCKeyValueTag */
static void m540(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    void *a5 = steam_out_buffer(c, R32(args + 24u), (int64_t)(int32_t)R32(args + 28u), 1u);
    uint32_t a6 = (uint32_t)R32(args + 28u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCMetadata */
static void m541(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumAdditionalPreviews */
static void m542(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumKeyValueTags */
static void m543(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumTags */
static void m544(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCPreviewURL */
static void m545(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCResult */
static void m546(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 9772u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCStatistic */
static void m547(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTag */
static void m548(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTagDisplayName */
static void m549(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetSubscribedItems */
static void m550(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 75u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetUserContentDescriptorPreferences */
static void m552(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 93u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 4u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetUserItemVote */
static void m553(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 69u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::GetWorkshopEULAStatus */
static void m554(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 92u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::ReleaseQueryUGCRequest */
static void m555(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveAllItemKeyValueTags */
static void m556(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveAppDependency */
static void m557(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 88u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveContentDescriptor */
static void m558(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 64u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveDependency */
static void m559(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 86u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemFromFavorites */
static void m560(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 71u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemKeyValueTags */
static void m561(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemPreview */
static void m562(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RequestUGCDetails */
static void m563(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 43u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SendQueryUGCRequest */
static void m564(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SetAdminQuery */
static void m565(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetAllowCachedResponse */
static void m566(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetAllowLegacyUpload */
static void m567(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetCloudFileNameFilter */
static void m568(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemContent */
static void m569(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemDescription */
static void m570(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemMetadata */
static void m571(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemPreview */
static void m572(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 53u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemTitle */
static void m574(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemUpdateLanguage */
static void m575(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemVisibility */
static void m576(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 50u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetLanguage */
static void m577(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetMatchAnyTag */
static void m578(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetRankedByTrendDays */
static void m579(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetRequiredGameVersions */
static void m580(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 65u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnAdditionalPreviews */
static void m581(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnChildren */
static void m582(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnKeyValueTags */
static void m583(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnLongDescription */
static void m584(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnMetadata */
static void m585(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnOnlyIDs */
static void m586(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnPlaytimeStats */
static void m587(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnTotalOnly */
static void m588(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetSearchText */
static void m589(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetTimeCreatedDateRange */
static void m590(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetTimeUpdatedDateRange */
static void m591(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetUserItemVote */
static void m592(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 68u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::ShowWorkshopEULA */
static void m593(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 91u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::StartItemUpdate */
static void m594(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 45u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::StartPlaytimeTracking */
static void m595(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 82u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTracking */
static void m596(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 83u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTrackingForAllItems */
static void m597(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 84u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SubmitItemUpdate */
static void m598(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 66u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SubscribeItem */
static void m599(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 72u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SuspendDownloads */
static void m600(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 81u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION020::UnsubscribeItem */
static void m601(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 73u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewFile */
static void m602(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewVideo */
static void m603(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::AttachLeaderboardUGC */
static void m604(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::ClearAchievement */
static void m605(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::DownloadLeaderboardEntries */
static void m606(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::DownloadLeaderboardEntriesForUsers */
static void m607(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::FindLeaderboard */
static void m608(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::FindOrCreateLeaderboard */
static void m609(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievement */
static void m610(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 6u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementAchievedPercent */
static void m611(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 37u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementAndUnlockTime */
static void m612(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    void *fn = steam_slot(c, self, 9u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementDisplayAttribute */
static void m613(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 12u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementIcon */
static void m614(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 11u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementName */
static void m615(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetDownloadedLeaderboardEntry */
static void m616(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, void *, int32_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 28u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStat */
static void m617(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 39u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStat0 */
static void m618(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 40u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStatHistory */
static void m619(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStatHistory0 */
static void m620(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 42u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardDisplayType */
static void m621(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardEntryCount */
static void m622(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardName */
static void m623(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardSortMethod */
static void m624(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetNumAchievements */
static void m627(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 14u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetNumberOfCurrentPlayers */
static void m628(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetStat */
static void m629(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetStat0 */
static void m630(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetUserAchievement */
static void m631(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetUserAchievementAndUnlockTime */
static void m632(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *, void *);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetUserStat */
static void m633(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::GetUserStat0 */
static void m634(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::IndicateAchievementProgress */
static void m635(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::RequestCurrentStats */
static void m636(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::RequestGlobalAchievementPercentages */
static void m637(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::RequestGlobalStats */
static void m638(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::RequestUserStats */
static void m639(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::ResetAllStats */
static void m640(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 21u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::SetAchievement */
static void m641(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::SetStat */
static void m642(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::SetStat0 */
static void m643(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::StoreStats */
static void m644(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::UpdateAvgRateStat */
static void m645(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float, double);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    double a2 = RD(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION011::UploadLeaderboardScore */
static void m646(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_in_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC */
static void m647(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_AttachLeaderboardUGC"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement */
static void m648(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_ClearAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries */
static void m649(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_DownloadLeaderboardEntries"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers */
static void m650(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_DownloadLeaderboardEntriesForUsers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard */
static void m651(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_FindLeaderboard"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard */
static void m652(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_FindOrCreateLeaderboard"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement */
static void m653(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent */
static void m654(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementAchievedPercent"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime */
static void m655(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementAndUnlockTime"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute */
static void m656(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementDisplayAttribute"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon */
static void m657(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementIcon"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName */
static void m658(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementName"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat */
static void m659(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementProgressLimitsFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32 */
static void m660(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementProgressLimitsInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry */
static void m661(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetDownloadedLeaderboardEntry"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 28u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble */
static void m662(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatDouble"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble */
static void m663(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatHistoryDouble"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64 */
static void m664(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatHistoryInt64"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64 */
static void m665(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatInt64"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType */
static void m666(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardDisplayType"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount */
static void m667(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardEntryCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName */
static void m668(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod */
static void m669(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardSortMethod"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements */
static void m672(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetNumAchievements"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers */
static void m673(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetNumberOfCurrentPlayers"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat */
static void m674(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetStatFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32 */
static void m675(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetStatInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement */
static void m676(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime */
static void m677(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserAchievementAndUnlockTime"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat */
static void m678(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32 */
static void m679(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress */
static void m680(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_IndicateAchievementProgress"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats */
static void m681(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestCurrentStats"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages */
static void m682(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestGlobalAchievementPercentages"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats */
static void m683(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestGlobalStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats */
static void m684(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats */
static void m685(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_ResetAllStats"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement */
static void m686(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat */
static void m687(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetStatFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32 */
static void m688(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetStatInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats */
static void m689(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_StoreStats"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat */
static void m690(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float, double);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_UpdateAvgRateStat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    double a2 = RD(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore */
static void m691(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_UploadLeaderboardScore"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_in_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMVIDEO_INTERFACE_V002::GetOPFSettings */
static void m692(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_GetOPFSettings"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::GetVideoURL */
static void m694(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_GetVideoURL"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::IsBroadcasting */
static void m695(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_IsBroadcasting"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMVIDEO_INTERFACE_V007::GetOPFSettings */
static void m696(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V007::GetVideoURL */
static void m698(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 0u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V007::IsBroadcasting */
static void m699(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient017::BReleaseSteamPipe */
static void m700(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient017::BShutdownIfAllPipesClosed */
static void m701(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient017::ConnectToGlobalUser */
static void m702(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient017::CreateLocalUser */
static void m703(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient017::CreateSteamPipe */
static void m704(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient017::GetIPCCallCount */
static void m705(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 20u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient017::GetISteamAppList */
static void m706(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 27u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamApps */
static void m707(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 15u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamController */
static void m708(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamFriends */
static void m709(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamGameServer */
static void m710(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamGameServerStats */
static void m711(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 14u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamGenericInterface */
static void m712(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamHTMLSurface */
static void m713(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 30u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamHTTP */
static void m714(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 23u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamInventory */
static void m715(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamMatchmaking */
static void m716(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamMatchmakingServers */
static void m717(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 11u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamMusic */
static void m718(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 28u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamMusicRemote */
static void m719(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamNetworking */
static void m720(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 16u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamParentalSettings */
static void m721(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamRemoteStorage */
static void m722(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 17u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamScreenshots */
static void m723(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamUGC */
static void m724(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 26u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamUser */
static void m726(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamUserStats */
static void m727(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::GetISteamUtils */
static void m728(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient017::GetISteamVideo */
static void m729(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient017::ReleaseUser */
static void m730(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamClient020::BReleaseSteamPipe */
static void m733(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_BReleaseSteamPipe"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::BShutdownIfAllPipesClosed */
static void m734(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_BShutdownIfAllPipesClosed"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::ConnectToGlobalUser */
static void m735(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_ConnectToGlobalUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateLocalUser */
static void m736(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_CreateLocalUser"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateSteamPipe */
static void m737(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_CreateSteamPipe"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetIPCCallCount */
static void m738(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetIPCCallCount"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetISteamAppList */
static void m739(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamAppList"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamApps */
static void m740(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamApps"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamController */
static void m741(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamController"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamFriends */
static void m742(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamFriends"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameSearch */
static void m743(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameSearch"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServer */
static void m744(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameServer"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServerStats */
static void m745(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameServerStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGenericInterface */
static void m746(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGenericInterface"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTMLSurface */
static void m747(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamHTMLSurface"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTTP */
static void m748(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamHTTP"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInput */
static void m749(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInventory */
static void m750(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamInventory"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmaking */
static void m751(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMatchmaking"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmakingServers */
static void m752(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMatchmakingServers"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusic */
static void m753(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMusic"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusicRemote */
static void m754(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMusicRemote"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamNetworking */
static void m755(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamNetworking"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParentalSettings */
static void m756(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamParentalSettings"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParties */
static void m757(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamParties"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemotePlay */
static void m758(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamRemotePlay"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemoteStorage */
static void m759(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamRemoteStorage"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamScreenshots */
static void m760(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamScreenshots"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUGC */
static void m761(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUGC"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUser */
static void m762(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUserStats */
static void m763(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUserStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUtils */
static void m764(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUtils"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient020::GetISteamVideo */
static void m765(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamVideo"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::ReleaseUser */
static void m766(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_ReleaseUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamClient021::BReleaseSteamPipe */
static void m769(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient021::BShutdownIfAllPipesClosed */
static void m770(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient021::ConnectToGlobalUser */
static void m771(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient021::CreateLocalUser */
static void m772(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient021::CreateSteamPipe */
static void m773(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient021::GetIPCCallCount */
static void m774(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient021::GetISteamApps */
static void m775(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 15u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamController */
static void m776(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamFriends */
static void m777(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameSearch */
static void m778(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameServer */
static void m779(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameServerStats */
static void m780(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 14u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGenericInterface */
static void m781(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamHTMLSurface */
static void m782(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamHTTP */
static void m783(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamInput */
static void m784(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamInventory */
static void m785(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMatchmaking */
static void m786(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMatchmakingServers */
static void m787(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 11u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMusic */
static void m788(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 27u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMusicRemote */
static void m789(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 28u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamNetworking */
static void m790(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 16u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamParentalSettings */
static void m791(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamParties */
static void m792(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 37u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamRemotePlay */
static void m793(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamRemoteStorage */
static void m794(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 17u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamScreenshots */
static void m795(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUGC */
static void m796(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 26u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUser */
static void m797(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUserStats */
static void m798(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUtils */
static void m799(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient021::GetISteamVideo */
static void m800(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::ReleaseUser */
static void m801(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamController005::ActivateActionSet */
static void m804(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController005::GetActionSetHandle */
static void m805(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController005::GetAnalogActionHandle */
static void m807(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 11u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController005::GetAnalogActionOrigins */
static void m808(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController005::GetConnectedControllers */
static void m809(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController005::GetControllerForGamepadIndex */
static void m810(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 20u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController005::GetCurrentActionSet */
static void m811(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController005::GetDigitalActionHandle */
static void m813(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController005::GetDigitalActionOrigins */
static void m814(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController005::GetGamepadIndexForController */
static void m815(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController005::GetGlyphForActionOrigin */
static void m816(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController005::GetStringForActionOrigin */
static void m818(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 24u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController005::Init */
static void m819(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController005::RunFrame */
static void m820(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    ((F)fn)(self);
}
/* SteamController005::SetLEDColor */
static void m821(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamController005::ShowBindingPanel */
static void m823(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamController005::Shutdown */
static void m825(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController005::StopAnalogActionMomentum */
static void m826(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 14u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController005::TriggerHapticPulse */
static void m827(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamController005::TriggerRepeatedHapticPulse */
static void m828(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamController005::TriggerVibration */
static void m829(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamController008::ActivateActionSet */
static void m830(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ActivateActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::ActivateActionSetLayer */
static void m831(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ActivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateActionSetLayer */
static void m832(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_DeactivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateAllActionSetLayers */
static void m833(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_DeactivateAllActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamController008::GetActionOriginFromXboxOrigin */
static void m834(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActionOriginFromXboxOrigin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetActionSetHandle */
static void m835(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActionSetHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetActiveActionSetLayers */
static void m836(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActiveActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetAnalogActionHandle */
static void m837(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetAnalogActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetAnalogActionOrigins */
static void m838(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetAnalogActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetConnectedControllers */
static void m839(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetConnectedControllers"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetControllerBindingRevision */
static void m840(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetControllerBindingRevision"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::GetControllerForGamepadIndex */
static void m841(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetControllerForGamepadIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetCurrentActionSet */
static void m842(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetCurrentActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionHandle */
static void m843(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetDigitalActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionOrigins */
static void m844(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetDigitalActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGamepadIndexForController */
static void m845(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGamepadIndexForController"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGlyphForActionOrigin */
static void m846(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGlyphForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetGlyphForXboxOrigin */
static void m847(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGlyphForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetInputTypeForHandle */
static void m848(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetInputTypeForHandle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetStringForActionOrigin */
static void m849(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetStringForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetStringForXboxOrigin */
static void m850(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetStringForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::Init */
static void m851(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_Init"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::RunFrame */
static void m852(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_RunFrame"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamController008::SetLEDColor */
static void m853(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_SetLEDColor"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamController008::ShowBindingPanel */
static void m854(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ShowBindingPanel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::Shutdown */
static void m855(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::StopAnalogActionMomentum */
static void m856(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_StopAnalogActionMomentum"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::TranslateActionOrigin */
static void m857(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TranslateActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::TriggerHapticPulse */
static void m858(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamController008::TriggerRepeatedHapticPulse */
static void m859(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerRepeatedHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamController008::TriggerVibration */
static void m860(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerVibration"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends015::ActivateGameOverlay */
static void m861(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 28u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends015::ActivateGameOverlayInviteDialog */
static void m862(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends015::ActivateGameOverlayToStore */
static void m863(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends015::ActivateGameOverlayToUser */
static void m864(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, uint64_t);
    void *fn = steam_slot(c, self, 29u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t a1 = (uint64_t)R64(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends015::ActivateGameOverlayToWebPage */
static void m865(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 30u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends015::ClearRichPresence */
static void m866(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 44u);
    ((F)fn)(self);
}
/* SteamFriends015::CloseClanChatWindowInSteam */
static void m867(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 63u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::DownloadClanActivityCounts */
static void m868(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 23u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::EnumerateFollowingList */
static void m869(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 69u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::GetChatMemberByIndex */
static void m870(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 57u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetClanActivityCounts */
static void m871(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::GetClanByIndex */
static void m872(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetClanChatMemberCount */
static void m873(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetClanChatMessage */
static void m874(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 59u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    void *a5 = steam_out(c, R32(args + 24u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetClanCount */
static void m875(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetClanName */
static void m876(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetClanOfficerByIndex */
static void m877(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetClanOfficerCount */
static void m878(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetClanOwner */
static void m879(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetClanTag */
static void m880(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetCoplayFriend */
static void m881(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 51u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetCoplayFriendCount */
static void m882(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 50u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFollowerCount */
static void m883(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 67u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::GetFriendByIndex */
static void m884(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetFriendCoplayGame */
static void m885(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 53u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendCoplayTime */
static void m886(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendCount */
static void m887(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendCountFromSource */
static void m888(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendFromSourceByIndex */
static void m889(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends015::GetFriendGamePlayed */
static void m890(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 24u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::GetFriendMessage */
static void m891(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 66u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendPersonaName */
static void m892(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetFriendPersonaNameHistory */
static void m893(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetFriendPersonaState */
static void m894(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendRelationship */
static void m895(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendRichPresence */
static void m896(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetFriendRichPresenceKeyByIndex */
static void m897(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetFriendRichPresenceKeyCount */
static void m898(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendSteamLevel */
static void m899(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendsGroupCount */
static void m900(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendsGroupIDByIndex */
static void m901(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int16_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int16_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendsGroupMembersCount */
static void m902(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int16_t);
    void *fn = steam_slot(c, self, 15u);
    int16_t a0 = (int16_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetFriendsGroupMembersList */
static void m903(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int16_t, void *, int32_t);
    void *fn = steam_slot(c, self, 16u);
    int16_t a0 = (int16_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends015::GetFriendsGroupName */
static void m904(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int16_t);
    void *fn = steam_slot(c, self, 14u);
    int16_t a0 = (int16_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetLargeFriendAvatar */
static void m905(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetMediumFriendAvatar */
static void m906(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetPersonaName */
static void m907(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetPersonaState */
static void m908(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetPlayerNickname */
static void m909(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends015::GetSmallFriendAvatar */
static void m910(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::GetUserRestrictions */
static void m911(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 42u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends015::HasFriend */
static void m912(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::InviteUserToGame */
static void m913(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::IsClanChatAdmin */
static void m914(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::IsClanChatWindowOpenInSteam */
static void m915(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::IsFollowing */
static void m916(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 68u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::IsUserInSource */
static void m917(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::JoinClanChatRoom */
static void m918(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::LeaveClanChatRoom */
static void m919(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::OpenClanChatWindowInSteam */
static void m920(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::ReplyToFriendMessage */
static void m921(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 65u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::RequestClanOfficerList */
static void m922(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::RequestFriendRichPresence */
static void m923(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends015::RequestUserInformation */
static void m924(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::SendClanChatMessage */
static void m925(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 58u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::SetInGameVoiceSpeaking */
static void m926(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends015::SetListenForFriendsMessages */
static void m927(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 64u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends015::SetPersonaName */
static void m928(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends015::SetPlayedWith */
static void m929(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends015::SetRichPresence */
static void m930(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 43u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::ActivateGameOverlay */
static void m931(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlay"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialog */
static void m932(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialog"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialogConnectString */
static void m933(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialogConnectString"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog */
static void m934(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayRemotePlayTogetherInviteDialog"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayToStore */
static void m935(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToStore"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToUser */
static void m936(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToUser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t a1 = (uint64_t)R64(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToWebPage */
static void m937(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ClearRichPresence */
static void m938(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ClearRichPresence"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamFriends017::CloseClanChatWindowInSteam */
static void m939(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_CloseClanChatWindowInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::DownloadClanActivityCounts */
static void m940(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_DownloadClanActivityCounts"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::EnumerateFollowingList */
static void m941(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_EnumerateFollowingList"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetChatMemberByIndex */
static void m942(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetChatMemberByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanActivityCounts */
static void m943(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanActivityCounts"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetClanByIndex */
static void m944(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanChatMemberCount */
static void m945(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanChatMemberCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanChatMessage */
static void m946(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanChatMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    void *a5 = steam_out(c, R32(args + 24u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanCount */
static void m947(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanName */
static void m948(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetClanOfficerByIndex */
static void m949(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOfficerByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanOfficerCount */
static void m950(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOfficerCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanOwner */
static void m951(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanTag */
static void m952(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetCoplayFriend */
static void m953(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetCoplayFriend"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetCoplayFriendCount */
static void m954(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetCoplayFriendCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFollowerCount */
static void m955(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFollowerCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetFriendByIndex */
static void m956(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendCoplayGame */
static void m957(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCoplayGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCoplayTime */
static void m958(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCoplayTime"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCount */
static void m959(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCountFromSource */
static void m960(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCountFromSource"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendFromSourceByIndex */
static void m961(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendFromSourceByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendGamePlayed */
static void m962(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendGamePlayed"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 24u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetFriendMessage */
static void m963(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendPersonaName */
static void m964(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaNameHistory */
static void m965(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaNameHistory"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaState */
static void m966(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaState"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRelationship */
static void m967(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRelationship"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRichPresence */
static void m968(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresence"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyByIndex */
static void m969(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresenceKeyByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyCount */
static void m970(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresenceKeyCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendSteamLevel */
static void m971(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendSteamLevel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupCount */
static void m972(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupIDByIndex */
static void m973(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int16_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupIDByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int16_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersCount */
static void m974(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupMembersCount"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersList */
static void m975(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int16_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupMembersList"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends017::GetFriendsGroupName */
static void m976(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupName"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetLargeFriendAvatar */
static void m977(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetLargeFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetMediumFriendAvatar */
static void m978(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetMediumFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetNumChatsWithUnreadPriorityMessages */
static void m979(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetNumChatsWithUnreadPriorityMessages"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPersonaName */
static void m980(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPersonaName"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetPersonaState */
static void m981(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPersonaState"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPlayerNickname */
static void m982(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPlayerNickname"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetSmallFriendAvatar */
static void m983(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetSmallFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetUserRestrictions */
static void m984(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetUserRestrictions"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::HasFriend */
static void m985(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_HasFriend"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::InviteUserToGame */
static void m986(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_InviteUserToGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatAdmin */
static void m987(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanChatAdmin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatWindowOpenInSteam */
static void m988(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanChatWindowOpenInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanOfficialGameGroup */
static void m989(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanOfficialGameGroup"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanPublic */
static void m990(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanPublic"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsFollowing */
static void m991(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsFollowing"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::IsUserInSource */
static void m992(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsUserInSource"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::JoinClanChatRoom */
static void m993(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_JoinClanChatRoom"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::LeaveClanChatRoom */
static void m994(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_LeaveClanChatRoom"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::OpenClanChatWindowInSteam */
static void m995(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_OpenClanChatWindowInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RegisterProtocolInOverlayBrowser */
static void m996(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RegisterProtocolInOverlayBrowser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::ReplyToFriendMessage */
static void m997(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ReplyToFriendMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RequestClanOfficerList */
static void m998(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestClanOfficerList"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::RequestFriendRichPresence */
static void m999(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestFriendRichPresence"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::RequestUserInformation */
static void m1000(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestUserInformation"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SendClanChatMessage */
static void m1001(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SendClanChatMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetInGameVoiceSpeaking */
static void m1002(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetInGameVoiceSpeaking"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::SetListenForFriendsMessages */
static void m1003(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetListenForFriendsMessages"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetPersonaName */
static void m1004(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetPersonaName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::SetPlayedWith */
static void m1005(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetPlayedWith"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::SetRichPresence */
static void m1006(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetRichPresence"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::AssociateWithClan */
static void m1007(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer012::BLoggedOn */
static void m1008(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::BSecure */
static void m1009(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::BUpdateUserData */
static void m1010(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::BeginAuthSession */
static void m1011(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 29u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer012::CancelAuthTicket */
static void m1012(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 31u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::ClearAllKeyValues */
static void m1013(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    ((F)fn)(self);
}
/* SteamGameServer012::ComputeNewPlayerCompatibility */
static void m1014(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 43u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer012::CreateUnauthenticatedUserConnection */
static void m1015(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 25u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer012::EnableHeartbeats */
static void m1016(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 39u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::EndAuthSession */
static void m1017(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::GetAuthSessionTicket */
static void m1019(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 28u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer012::GetGameplayStats */
static void m1020(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 34u);
    ((F)fn)(self);
}
/* SteamGameServer012::GetNextOutgoingPacket */
static void m1021(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 38u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer012::GetServerReputation */
static void m1023(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 35u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer012::GetSteamID */
static void m1024(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer012::HandleIncomingPacket */
static void m1025(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 37u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::LogOff */
static void m1027(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamGameServer012::LogOn */
static void m1028(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::LogOnAnonymous */
static void m1029(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    ((F)fn)(self);
}
/* SteamGameServer012::RequestUserGroupStatus */
static void m1030(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::SendUserConnectAndAuthenticate */
static void m1031(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 24u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer012::SendUserDisconnect */
static void m1032(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetBotPlayerCount */
static void m1033(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetDedicatedServer */
static void m1034(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 4u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetGameData */
static void m1035(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetGameDescription */
static void m1036(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetGameTags */
static void m1037(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetKeyValue */
static void m1039(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 20u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer012::SetMapName */
static void m1040(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetMaxPlayerCount */
static void m1041(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetModDir */
static void m1042(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetPasswordProtected */
static void m1043(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 16u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetProduct */
static void m1044(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetRegion */
static void m1045(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetServerName */
static void m1046(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetSpectatorPort */
static void m1047(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    void *fn = steam_slot(c, self, 17u);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer012::SetSpectatorServerName */
static void m1048(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 18u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer012::UserHasLicenseForApp */
static void m1049(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer012::WasRestartRequested */
static void m1050(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::AssociateWithClan */
static void m1051(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_AssociateWithClan"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::BLoggedOn */
static void m1052(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BLoggedOn"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BSecure */
static void m1053(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BSecure"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BUpdateUserData */
static void m1054(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BUpdateUserData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BeginAuthSession */
static void m1055(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BeginAuthSession"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::CancelAuthTicket */
static void m1056(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_CancelAuthTicket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::ClearAllKeyValues */
static void m1057(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_ClearAllKeyValues"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::ComputeNewPlayerCompatibility */
static void m1058(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_ComputeNewPlayerCompatibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::CreateUnauthenticatedUserConnection */
static void m1059(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_CreateUnauthenticatedUserConnection"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::EndAuthSession */
static void m1060(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_EndAuthSession"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::GetAuthSessionTicket */
static void m1061(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetAuthSessionTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetGameplayStats */
static void m1062(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetGameplayStats"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::GetNextOutgoingPacket */
static void m1063(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetNextOutgoingPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetServerReputation */
static void m1064(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetServerReputation"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::GetSteamID */
static void m1065(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetSteamID"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::HandleIncomingPacket */
static void m1066(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_HandleIncomingPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::LogOff */
static void m1067(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOff"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::LogOn */
static void m1068(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOn"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::LogOnAnonymous */
static void m1069(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOnAnonymous"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::RequestUserGroupStatus */
static void m1070(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_RequestUserGroupStatus"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED */
static void m1071(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserDisconnect_DEPRECATED */
static void m1072(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SendUserDisconnect_DEPRECATED"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetAdvertiseServerActive */
static void m1073(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetAdvertiseServerActive"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetBotPlayerCount */
static void m1074(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetBotPlayerCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetDedicatedServer */
static void m1075(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetDedicatedServer"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameData */
static void m1076(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameData"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameDescription */
static void m1077(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameDescription"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameTags */
static void m1078(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameTags"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetKeyValue */
static void m1079(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetKeyValue"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer014::SetMapName */
static void m1080(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetMapName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetMaxPlayerCount */
static void m1081(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetMaxPlayerCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetModDir */
static void m1082(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetModDir"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetPasswordProtected */
static void m1083(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetPasswordProtected"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetProduct */
static void m1084(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetProduct"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetRegion */
static void m1085(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetRegion"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetServerName */
static void m1086(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetServerName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorPort */
static void m1087(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetSpectatorPort"), __ATOMIC_RELAXED);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorServerName */
static void m1088(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetSpectatorServerName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::UserHasLicenseForApp */
static void m1089(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_UserHasLicenseForApp"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::WasRestartRequested */
static void m1090(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_WasRestartRequested"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::AssociateWithClan */
static void m1091(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::BLoggedOn */
static void m1092(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BSecure */
static void m1093(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BUpdateUserData */
static void m1094(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BeginAuthSession */
static void m1095(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::CancelAuthTicket */
static void m1096(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 28u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::ClearAllKeyValues */
static void m1097(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    ((F)fn)(self);
}
/* SteamGameServer015::ComputeNewPlayerCompatibility */
static void m1098(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::CreateUnauthenticatedUserConnection */
static void m1099(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 39u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer015::EndAuthSession */
static void m1100(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::GetGameplayStats */
static void m1102(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    ((F)fn)(self);
}
/* SteamGameServer015::GetNextOutgoingPacket */
static void m1103(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 35u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::GetServerReputation */
static void m1104(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::GetSteamID */
static void m1105(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer015::HandleIncomingPacket */
static void m1106(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 34u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::LogOff */
static void m1107(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamGameServer015::LogOn */
static void m1108(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::LogOnAnonymous */
static void m1109(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    ((F)fn)(self);
}
/* SteamGameServer015::RequestUserGroupStatus */
static void m1110(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::SendUserConnectAndAuthenticate_DEPRECATED */
static void m1111(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 38u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::SendUserDisconnect_DEPRECATED */
static void m1112(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetAdvertiseServerActive */
static void m1113(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 24u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetBotPlayerCount */
static void m1114(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetDedicatedServer */
static void m1115(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 4u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameData */
static void m1116(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameDescription */
static void m1117(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameTags */
static void m1118(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetKeyValue */
static void m1119(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 20u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer015::SetMapName */
static void m1120(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetMaxPlayerCount */
static void m1121(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetModDir */
static void m1122(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetPasswordProtected */
static void m1123(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 16u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetProduct */
static void m1124(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetRegion */
static void m1125(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetServerName */
static void m1126(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetSpectatorPort */
static void m1127(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    void *fn = steam_slot(c, self, 17u);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetSpectatorServerName */
static void m1128(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 18u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::UserHasLicenseForApp */
static void m1129(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::WasRestartRequested */
static void m1130(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::ClearUserAchievement */
static void m1131(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_ClearUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserAchievement */
static void m1132(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatFloat */
static void m1133(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatInt32 */
static void m1134(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::RequestUserStats */
static void m1135(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_RequestUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::SetUserAchievement */
static void m1136(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatFloat */
static void m1137(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatInt32 */
static void m1138(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::StoreUserStats */
static void m1139(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_StoreUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::UpdateUserAvgRateStat */
static void m1140(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float, double);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_UpdateUserAvgRateStat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    double a3 = RD(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::ActivateActionSet */
static void m1141(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ActivateActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::ActivateActionSetLayer */
static void m1142(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ActivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::BNewDataAvailable */
static void m1143(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_BNewDataAvailable"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::BWaitForData */
static void m1144(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_BWaitForData"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::DeactivateActionSetLayer */
static void m1145(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_DeactivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::DeactivateAllActionSetLayers */
static void m1146(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_DeactivateAllActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamInput006::EnableDeviceCallbacks */
static void m1148(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_EnableDeviceCallbacks"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamInput006::GetActionOriginFromXboxOrigin */
static void m1149(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActionOriginFromXboxOrigin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetActionSetHandle */
static void m1150(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActionSetHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetActiveActionSetLayers */
static void m1151(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActiveActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetAnalogActionHandle */
static void m1152(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetAnalogActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetAnalogActionOrigins */
static void m1153(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetAnalogActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetConnectedControllers */
static void m1154(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetConnectedControllers"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetControllerForGamepadIndex */
static void m1155(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetControllerForGamepadIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetCurrentActionSet */
static void m1156(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetCurrentActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDeviceBindingRevision */
static void m1157(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDeviceBindingRevision"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::GetDigitalActionHandle */
static void m1158(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDigitalActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDigitalActionOrigins */
static void m1159(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDigitalActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGamepadIndexForController */
static void m1160(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGamepadIndexForController"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGlyphForActionOrigin_Legacy */
static void m1161(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphForActionOrigin_Legacy"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphForXboxOrigin */
static void m1162(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphPNGForActionOrigin */
static void m1163(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphPNGForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphSVGForActionOrigin */
static void m1164(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphSVGForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetInputTypeForHandle */
static void m1165(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetInputTypeForHandle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetRemotePlaySessionID */
static void m1166(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetRemotePlaySessionID"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetSessionInputConfigurationSettings */
static void m1167(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint16_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetSessionInputConfigurationSettings"), __ATOMIC_RELAXED);
    uint16_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetStringForActionOrigin */
static void m1168(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForAnalogActionName */
static void m1169(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForAnalogActionName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForDigitalActionName */
static void m1170(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForDigitalActionName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForXboxOrigin */
static void m1171(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::Init */
static void m1172(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Init"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Legacy_TriggerHapticPulse */
static void m1173(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Legacy_TriggerHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::Legacy_TriggerRepeatedHapticPulse */
static void m1174(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Legacy_TriggerRepeatedHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamInput006::RunFrame */
static void m1175(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_RunFrame"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamInput006::SetInputActionManifestFilePath */
static void m1176(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_SetInputActionManifestFilePath"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::SetLEDColor */
static void m1177(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_SetLEDColor"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamInput006::ShowBindingPanel */
static void m1178(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ShowBindingPanel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Shutdown */
static void m1179(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::StopAnalogActionMomentum */
static void m1180(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_StopAnalogActionMomentum"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::TranslateActionOrigin */
static void m1181(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TranslateActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::TriggerVibration */
static void m1183(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TriggerVibration"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::TriggerVibrationExtended */
static void m1184(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TriggerVibrationExtended"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamMatchGameSearch001::AcceptGame */
static void m1185(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_AcceptGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::AddGameSearchParams */
static void m1186(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_AddGameSearchParams"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::CancelRequestPlayersForGame */
static void m1187(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_CancelRequestPlayersForGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::DeclineGame */
static void m1188(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_DeclineGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGame */
static void m1189(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_EndGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGameSearch */
static void m1190(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_EndGameSearch"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::HostConfirmGameStart */
static void m1191(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_HostConfirmGameStart"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RequestPlayersForGame */
static void m1192(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_RequestPlayersForGame"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RetrieveConnectionDetails */
static void m1193(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_RetrieveConnectionDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameSolo */
static void m1194(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SearchForGameSolo"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameWithLobby */
static void m1195(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SearchForGameWithLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetConnectionDetails */
static void m1196(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SetConnectionDetails"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetGameHostParams */
static void m1197(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SetGameHostParams"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SubmitPlayerResult */
static void m1198(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SubmitPlayerResult"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    int32_t a2 = (int32_t)R32(args + 16u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::AddFavoriteGame */
static void m1199(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddFavoriteGame"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint16_t a2 = (uint16_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::AddRequestLobbyListCompatibleMembersFilter */
static void m1200(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListCompatibleMembersFilter"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListDistanceFilter */
static void m1201(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListDistanceFilter"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable */
static void m1202(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListFilterSlotsAvailable"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListNearValueFilter */
static void m1203(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListNearValueFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamMatchMaking009::AddRequestLobbyListNumericalFilter */
static void m1204(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListNumericalFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::AddRequestLobbyListResultCountFilter */
static void m1205(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListResultCountFilter"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListStringFilter */
static void m1206(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListStringFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::CreateLobby */
static void m1207(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_CreateLobby"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::DeleteLobbyData */
static void m1208(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_DeleteLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetFavoriteGame */
static void m1209(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetFavoriteGame"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    void *a4 = steam_out(c, R32(args + 16u), 2u);
    void *a5 = steam_out(c, R32(args + 20u), 4u);
    void *a6 = steam_out(c, R32(args + 24u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetFavoriteGameCount */
static void m1210(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetFavoriteGameCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyByIndex */
static void m1211(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyChatEntry */
static void m1212(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyChatEntry"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    int32_t a4 = (int32_t)R32(args + 20u);
    void *a5 = steam_out(c, R32(args + 24u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyData */
static void m1213(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyDataByIndex */
static void m1214(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyDataByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out_buffer(c, R32(args + 20u), (int64_t)(int32_t)R32(args + 24u), 1u);
    int32_t a5 = (int32_t)R32(args + 24u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetLobbyDataCount */
static void m1215(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyDataCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyGameServer */
static void m1216(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyGameServer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 2u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetLobbyMemberByIndex */
static void m1217(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyMemberData */
static void m1218(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyMemberLimit */
static void m1219(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberLimit"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyOwner */
static void m1220(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetNumLobbyMembers */
static void m1221(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetNumLobbyMembers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::InviteUserToLobby */
static void m1222(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_InviteUserToLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::JoinLobby */
static void m1223(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_JoinLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::LeaveLobby */
static void m1224(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_LeaveLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::RemoveFavoriteGame */
static void m1225(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RemoveFavoriteGame"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint16_t a2 = (uint16_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyData */
static void m1226(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RequestLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyList */
static void m1227(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RequestLobbyList"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::SendLobbyChatMsg */
static void m1228(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SendLobbyChatMsg"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLinkedLobby */
static void m1229(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLinkedLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyData */
static void m1230(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyGameServer */
static void m1231(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyGameServer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint64_t a3 = (uint64_t)R64(args + 16u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* SteamMatchMaking009::SetLobbyJoinable */
static void m1232(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyJoinable"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyMemberData */
static void m1233(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyMemberData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::SetLobbyMemberLimit */
static void m1234(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyMemberLimit"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyOwner */
static void m1235(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyType */
static void m1236(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyType"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMakingServers002::CancelQuery */
static void m1237(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_CancelQuery"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    ((F)fn)(self, a0);
}
/* SteamMatchMakingServers002::CancelServerQuery */
static void m1238(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_CancelServerQuery"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMakingServers002::GetServerCount */
static void m1239(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_GetServerCount"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMakingServers002::IsRefreshing */
static void m1241(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_IsRefreshing"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMakingServers002::RefreshQuery */
static void m1244(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_RefreshQuery"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    ((F)fn)(self, a0);
}
/* SteamMatchMakingServers002::RefreshServer */
static void m1245(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_RefreshServer"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamMatchMakingServers002::ReleaseRequest */
static void m1246(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_ReleaseRequest"), __ATOMIC_RELAXED);
    if (R32(args + 0u)) rt_trap(c, c->eip, "Steam: server list requests are not bridged");
    void *a0 = NULL;
    ((F)fn)(self, a0);
}
/* SteamNetworking005::AcceptP2PSessionWithUser */
static void m1254(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::AllowP2PPacketRelay */
static void m1255(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 7u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::CloseP2PChannelWithUser */
static void m1256(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::CloseP2PSessionWithUser */
static void m1257(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::CreateP2PConnectionSocket */
static void m1260(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, int32_t, int32_t, bool);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool a3 = (R8(args + 16u) != 0);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamNetworking005::DestroyListenSocket */
static void m1261(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 12u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::DestroySocket */
static void m1262(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::GetMaxPacketSize */
static void m1264(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 21u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking005::GetSocketConnectionType */
static void m1266(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking005::IsDataAvailable */
static void m1268(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 16u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::IsDataAvailableOnSocket */
static void m1269(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::IsP2PPacketAvailable */
static void m1270(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::ReadP2PPacket */
static void m1271(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t, void *, void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::RetrieveData */
static void m1272(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::RetrieveDataFromSocket */
static void m1273(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::SendDataOnSocket */
static void m1274(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking005::SendP2PPacket */
static void m1275(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 0u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::AcceptP2PSessionWithUser */
static void m1276(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::AllowP2PPacketRelay */
static void m1277(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_AllowP2PPacketRelay"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PChannelWithUser */
static void m1278(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CloseP2PChannelWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PSessionWithUser */
static void m1279(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CloseP2PSessionWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CreateP2PConnectionSocket */
static void m1280(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, int32_t, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CreateP2PConnectionSocket"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool a3 = (R8(args + 16u) != 0);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::DestroyListenSocket */
static void m1281(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_DestroyListenSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::DestroySocket */
static void m1282(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_DestroySocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::GetMaxPacketSize */
static void m1284(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_GetMaxPacketSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::GetSocketConnectionType */
static void m1286(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_GetSocketConnectionType"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::IsDataAvailable */
static void m1288(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsDataAvailable"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsDataAvailableOnSocket */
static void m1289(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsDataAvailableOnSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsP2PPacketAvailable */
static void m1290(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsP2PPacketAvailable"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::ReadP2PPacket */
static void m1291(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_ReadP2PPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveData */
static void m1292(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_RetrieveData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveDataFromSocket */
static void m1293(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_RetrieveDataFromSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendDataOnSocket */
static void m1294(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_SendDataOnSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendP2PPacket */
static void m1295(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_SendP2PPacket"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::AcceptConnection */
static void m1302(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::BeginAsyncRequestFakeIP */
static void m1303(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 42u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CloseConnection */
static void m1304(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, int32_t, const char *, bool);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CloseListenSocket */
static void m1305(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CreatePollGroup */
static void m1316(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::DestroyPollGroup */
static void m1318(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 25u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::FlushMessagesOnConnection */
static void m1320(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetConnectionName */
static void m1324(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::GetConnectionUserData */
static void m1326(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamNetworkingSockets012::GetDetailedConnectionStatus */
static void m1327(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerAddress */
static void m1330(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 33u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerPOPID */
static void m1331(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 32u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerPort */
static void m1332(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint16_t (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    uint16_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetListenSocketAddress */
static void m1334(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 18u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 2u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::GetRemoteFakeIPForConnection */
static void m1335(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 45u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 2u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::InitAuthentication */
static void m1336(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::RunCallbacks */
static void m1342(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 41u);
    ((F)fn)(self);
}
/* SteamNetworkingSockets012::SendMessageToConnection */
static void m1343(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::SetConnectionName */
static void m1346(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamNetworkingSockets012::SetConnectionPollGroup */
static void m1347(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 26u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::SetConnectionUserData */
static void m1348(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, int64_t);
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int64_t a1 = (int64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingUtils004::CheckPingDataUpToDate */
static void m1350(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, float);
    void *fn = steam_slot(c, self, 7u);
    float a0 = RF(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingUtils004::GetConfigValueInfo */
static void m1355(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamNetworkingUtils004::GetDirectPingToPOP */
static void m1356(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetIPv4FakeIPType */
static void m1357(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetLocalTimestamp */
static void m1359(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    int64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamNetworkingUtils004::GetPOPCount */
static void m1360(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetPOPList */
static void m1361(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetPingToDataCenter */
static void m1362(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::IterateGenericEditableConfigValues */
static void m1365(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::SteamNetworkingIPAddr_GetFakeIPType */
static void m1369(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out(c, R32(args + 0u), 2u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::SteamNetworkingIPAddr_ParseString */
static void m1370(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_out(c, R32(args + 0u), 2u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::CancelReservation */
static void m1372(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamParties002::ChangeNumOpenSlots */
static void m1373(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::DestroyBeacon */
static void m1375(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::GetBeaconByIndex */
static void m1377(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::GetNumActiveBeacons */
static void m1379(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamParties002::GetNumAvailableBeaconLocations */
static void m1380(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 4u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::JoinParty */
static void m1381(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::OnReservationCompleted */
static void m1382(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser019::AdvertiseGame */
static void m1383(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser019::BIsBehindNAT */
static void m1384(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BIsPhoneIdentifying */
static void m1385(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BIsPhoneRequiringVerification */
static void m1386(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BIsPhoneVerified */
static void m1387(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 25u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BIsTwoFactorEnabled */
static void m1388(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BLoggedOn */
static void m1389(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::BeginAuthSession */
static void m1390(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 14u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser019::CancelAuthTicket */
static void m1391(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 16u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser019::DecompressVoice */
static void m1392(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser019::EndAuthSession */
static void m1393(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser019::GetAuthSessionTicket */
static void m1394(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 13u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser019::GetEncryptedAppTicket */
static void m1396(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::GetGameBadgeLevel */
static void m1397(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 22u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser019::GetHSteamUser */
static void m1398(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser019::GetPlayerSteamLevel */
static void m1399(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser019::GetSteamID */
static void m1400(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser019::GetUserDataFolder */
static void m1401(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser019::GetVoiceOptimalSampleRate */
static void m1403(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser019::InitiateGameConnection */
static void m1404(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    bool a5 = (R8(args + 24u) != 0);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser019::RequestEncryptedAppTicket */
static void m1405(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 20u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser019::RequestStoreAuthURL */
static void m1406(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 24u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser019::StartVoiceRecording */
static void m1407(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamUser019::StopVoiceRecording */
static void m1408(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    ((F)fn)(self);
}
/* SteamUser019::TerminateGameConnection */
static void m1409(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser019::TrackAppUsageEvent */
static void m1410(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser019::UserHasLicenseForApp */
static void m1411(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser021::AdvertiseGame */
static void m1412(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_AdvertiseGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser021::BIsBehindNAT */
static void m1413(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsBehindNAT"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneIdentifying */
static void m1414(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneIdentifying"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneRequiringVerification */
static void m1415(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneRequiringVerification"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneVerified */
static void m1416(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneVerified"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsTwoFactorEnabled */
static void m1417(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsTwoFactorEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BLoggedOn */
static void m1418(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BLoggedOn"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BSetDurationControlOnlineState */
static void m1419(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BSetDurationControlOnlineState"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BeginAuthSession */
static void m1420(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BeginAuthSession"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::CancelAuthTicket */
static void m1421(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_CancelAuthTicket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::DecompressVoice */
static void m1422(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_DecompressVoice"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser021::EndAuthSession */
static void m1423(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_EndAuthSession"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::GetAuthSessionTicket */
static void m1424(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetAuthSessionTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetDurationControl */
static void m1426(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetDurationControl"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetEncryptedAppTicket */
static void m1427(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetEncryptedAppTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetGameBadgeLevel */
static void m1428(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetGameBadgeLevel"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetHSteamUser */
static void m1429(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetHSteamUser"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetMarketEligibility */
static void m1430(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetMarketEligibility"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetPlayerSteamLevel */
static void m1431(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetPlayerSteamLevel"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetSteamID */
static void m1432(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetSteamID"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser021::GetUserDataFolder */
static void m1433(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetUserDataFolder"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetVoiceOptimalSampleRate */
static void m1434(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetVoiceOptimalSampleRate"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::InitiateGameConnection_DEPRECATED */
static void m1435(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    bool a5 = (R8(args + 24u) != 0);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser021::RequestEncryptedAppTicket */
static void m1436(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_RequestEncryptedAppTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::RequestStoreAuthURL */
static void m1437(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_RequestStoreAuthURL"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::StartVoiceRecording */
static void m1438(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_StartVoiceRecording"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamUser021::StopVoiceRecording */
static void m1439(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_StopVoiceRecording"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamUser021::TerminateGameConnection_DEPRECATED */
static void m1440(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_TerminateGameConnection_DEPRECATED"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser021::UserHasLicenseForApp */
static void m1441(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_UserHasLicenseForApp"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser023::AdvertiseGame */
static void m1442(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser023::BIsBehindNAT */
static void m1443(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneIdentifying */
static void m1444(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneRequiringVerification */
static void m1445(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 29u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneVerified */
static void m1446(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsTwoFactorEnabled */
static void m1447(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BLoggedOn */
static void m1448(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BSetDurationControlOnlineState */
static void m1449(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 32u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BeginAuthSession */
static void m1450(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 15u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser023::CancelAuthTicket */
static void m1451(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser023::DecompressVoice */
static void m1452(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser023::EndAuthSession */
static void m1453(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser023::GetAuthTicketForWebApi */
static void m1455(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetDurationControl */
static void m1457(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::GetEncryptedAppTicket */
static void m1458(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::GetGameBadgeLevel */
static void m1459(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 23u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetHSteamUser */
static void m1460(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetMarketEligibility */
static void m1461(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::GetPlayerSteamLevel */
static void m1462(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetSteamID */
static void m1463(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser023::GetUserDataFolder */
static void m1464(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::GetVoiceOptimalSampleRate */
static void m1465(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::InitiateGameConnection_DEPRECATED */
static void m1466(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    bool a5 = (R8(args + 24u) != 0);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser023::RequestEncryptedAppTicket */
static void m1467(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::RequestStoreAuthURL */
static void m1468(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 25u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::StartVoiceRecording */
static void m1469(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamUser023::StopVoiceRecording */
static void m1470(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    ((F)fn)(self);
}
/* SteamUser023::TerminateGameConnection_DEPRECATED */
static void m1471(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser023::UserHasLicenseForApp */
static void m1472(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::BOverlayNeedsPresent */
static void m1473(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::CheckFileSignature */
static void m1474(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 19u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUtils009::GetAPICallFailureReason */
static void m1475(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetAPICallResult */
static void m1476(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::GetAppID */
static void m1477(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetConnectedUniverse */
static void m1479(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetCurrentBatteryPower */
static void m1480(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint8_t (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    uint8_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetEnteredGamepadTextInput */
static void m1481(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::GetEnteredGamepadTextLength */
static void m1482(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetIPCCallCount */
static void m1483(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetIPCountry */
static void m1484(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils009::GetImageRGBA */
static void m1485(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::GetImageSize */
static void m1486(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::GetSecondsSinceAppActive */
static void m1487(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetSecondsSinceComputerActive */
static void m1488(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetServerRealTime */
static void m1489(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils009::GetSteamUILanguage */
static void m1490(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils009::IsAPICallCompleted */
static void m1491(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::IsOverlayEnabled */
static void m1492(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 17u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::IsSteamInBigPictureMode */
static void m1493(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::IsSteamRunningInVR */
static void m1494(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::IsVRHeadsetStreamingEnabled */
static void m1495(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::SetOverlayNotificationInset */
static void m1496(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUtils009::SetOverlayNotificationPosition */
static void m1497(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUtils009::SetVRHeadsetStreamingEnabled */
static void m1498(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 29u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils009::ShowGamepadTextInput */
static void m1500(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 20u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils009::StartVRDashboard */
static void m1501(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    ((F)fn)(self);
}
/* SteamUtils010::BOverlayNeedsPresent */
static void m1502(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_BOverlayNeedsPresent"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::CheckFileSignature */
static void m1503(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_CheckFileSignature"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUtils010::DismissFloatingGamepadTextInput */
static void m1504(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_DismissFloatingGamepadTextInput"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::FilterText */
static void m1505(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, uint64_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_FilterText"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallFailureReason */
static void m1506(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAPICallFailureReason"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallResult */
static void m1507(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAPICallResult"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetAppID */
static void m1508(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAppID"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetConnectedUniverse */
static void m1509(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetConnectedUniverse"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetCurrentBatteryPower */
static void m1510(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint8_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetCurrentBatteryPower"), __ATOMIC_RELAXED);
    uint8_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetEnteredGamepadTextInput */
static void m1511(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetEnteredGamepadTextInput"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetEnteredGamepadTextLength */
static void m1512(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetEnteredGamepadTextLength"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCCallCount */
static void m1513(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPCCallCount"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCountry */
static void m1514(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPCountry"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::GetIPv6ConnectivityState */
static void m1515(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPv6ConnectivityState"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetImageRGBA */
static void m1516(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetImageRGBA"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetImageSize */
static void m1517(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetImageSize"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetSecondsSinceAppActive */
static void m1518(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSecondsSinceAppActive"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSecondsSinceComputerActive */
static void m1519(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSecondsSinceComputerActive"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetServerRealTime */
static void m1520(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetServerRealTime"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSteamUILanguage */
static void m1521(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSteamUILanguage"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::InitFilterText */
static void m1522(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_InitFilterText"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsAPICallCompleted */
static void m1523(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsAPICallCompleted"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsOverlayEnabled */
static void m1524(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsOverlayEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamChinaLauncher */
static void m1525(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamChinaLauncher"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamInBigPictureMode */
static void m1526(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamInBigPictureMode"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningInVR */
static void m1527(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamRunningInVR"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningOnSteamDeck */
static void m1528(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamRunningOnSteamDeck"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsVRHeadsetStreamingEnabled */
static void m1529(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsVRHeadsetStreamingEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::SetGameLauncherMode */
static void m1530(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetGameLauncherMode"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetOverlayNotificationInset */
static void m1531(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetOverlayNotificationInset"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUtils010::SetOverlayNotificationPosition */
static void m1532(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetOverlayNotificationPosition"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetVRHeadsetStreamingEnabled */
static void m1533(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetVRHeadsetStreamingEnabled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::ShowFloatingGamepadTextInput */
static void m1535(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_ShowFloatingGamepadTextInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::ShowGamepadTextInput */
static void m1536(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_ShowGamepadTextInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::StartVRDashboard */
static void m1537(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_StartVRDashboard"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
const SteamInterfaceInfo steam_interfaces[] = {
    {"SteamClient020", "ISteamClient", 41u},
    {"SteamUser021", "ISteamUser", 32u},
    {"SteamFriends017", "ISteamFriends", 76u},
    {"SteamUtils010", "ISteamUtils", 38u},
    {"SteamMatchMaking009", "ISteamMatchmaking", 38u},
    {"SteamMatchGameSearch001", "ISteamGameSearch", 14u},
    {"SteamMatchMakingServers002", "ISteamMatchmakingServers", 17u},
    {"STEAMUSERSTATS_INTERFACE_VERSION012", "ISteamUserStats", 45u},
    {"STEAMAPPS_INTERFACE_VERSION008", "ISteamApps", 29u},
    {"SteamNetworking006", "ISteamNetworking", 22u},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016", "ISteamRemoteStorage", 59u},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003", "ISteamScreenshots", 9u},
    {"STEAMHTTP_INTERFACE_VERSION003", "ISteamHTTP", 25u},
    {"SteamController008", "ISteamController", 34u},
    {"STEAMUGC_INTERFACE_VERSION016", "ISteamUGC", 86u},
    {"STEAMAPPLIST_INTERFACE_VERSION001", "ISteamAppList", 5u},
    {"STEAMMUSIC_INTERFACE_VERSION001", "ISteamMusic", 9u},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001", "ISteamMusicRemote", 32u},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005", "ISteamHTMLSurface", 39u},
    {"STEAMINVENTORY_INTERFACE_V003", "ISteamInventory", 38u},
    {"STEAMVIDEO_INTERFACE_V002", "ISteamVideo", 4u},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001", "ISteamParentalSettings", 6u},
    {"SteamInput006", "ISteamInput", 47u},
    {"SteamGameServer014", "ISteamGameServer", 42u},
    {"SteamGameServerStats001", "ISteamGameServerStats", 10u},
    {"SteamClient021", "ISteamClient", 39u},
    {"SteamUser023", "ISteamUser", 33u},
    {"STEAMUGC_INTERFACE_VERSION020", "ISteamUGC", 94u},
    {"STEAMVIDEO_INTERFACE_V007", "ISteamVideo", 4u},
    {"SteamGameServer015", "ISteamGameServer", 42u},
    {"SteamParties002", "ISteamParties", 11u},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002", "ISteamRemotePlay", 8u},
    {"SteamNetworkingUtils004", "ISteamNetworkingUtils", 25u},
    {"SteamNetworkingSockets012", "ISteamNetworkingSockets", 47u},
    {"SteamNetworkingMessages002", "ISteamNetworkingMessages", 6u},
    {"STEAMTIMELINE_INTERFACE_V004", "ISteamTimeline", 18u},
    {"SteamClient017", "ISteamClient", 37u},
    {"SteamUser019", "ISteamUser", 29u},
    {"SteamFriends015", "ISteamFriends", 70u},
    {"SteamUtils009", "ISteamUtils", 30u},
    {"STEAMUSERSTATS_INTERFACE_VERSION011", "ISteamUserStats", 43u},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014", "ISteamRemoteStorage", 55u},
    {"STEAMUGC_INTERFACE_VERSION010", "ISteamUGC", 74u},
    {"STEAMHTTP_INTERFACE_VERSION002", "ISteamHTTP", 25u},
    {"SteamController005", "ISteamController", 26u},
    {"SteamGameServer012", "ISteamGameServer", 44u},
    {"SteamNetworking005", "ISteamNetworking", 22u},
};
const size_t steam_interface_count = 47;
const SteamMethodInfo steam_methods[] = {
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppBuildId", 15u, 4u, 0, m0, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppInstallDir", 15u, 3u, 0, m1, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppName", 15u, 2u, 0, m2, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetInstalledApps", 15u, 1u, 0, m3, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetNumInstalledApps", 15u, 0u, 0, m4, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BGetDLCDataByIndex", 8u, 11u, 0, m5, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsAppInstalled", 8u, 19u, 0, m6, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsCybercafe", 8u, 2u, 0, m7, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsDlcInstalled", 8u, 7u, 0, m8, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsLowViolence", 8u, 1u, 0, m9, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribed", 8u, 0u, 0, m10, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedApp", 8u, 6u, 0, m11, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFamilySharing", 8u, 27u, 0, m12, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFreeWeekend", 8u, 9u, 0, m13, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsTimedTrial", 8u, 28u, 0, m14, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsVACBanned", 8u, 3u, 0, m15, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppBuildId", 8u, 23u, 0, m16, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppInstallDir", 8u, 18u, 0, m17, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppOwner", 8u, 20u, 1, m18, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAvailableGameLanguages", 8u, 5u, 0, m19, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetCurrentBetaName", 8u, 15u, 0, m20, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetCurrentGameLanguage", 8u, 4u, 0, m21, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetDLCCount", 8u, 10u, 0, m22, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetDlcDownloadProgress", 8u, 22u, 0, m23, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetEarliestPurchaseUnixTime", 8u, 8u, 0, m24, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetFileDetails", 8u, 25u, 0, m25, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetInstalledDepots", 8u, 17u, 0, m26, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetLaunchCommandLine", 8u, 26u, 0, m27, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetLaunchQueryParam", 8u, 21u, 0, m28, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::InstallDLC", 8u, 12u, 0, m29, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::MarkContentCorrupt", 8u, 16u, 0, m30, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::RequestAllProofOfPurchaseKeys", 8u, 24u, 0, m31, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::RequestAppProofOfPurchaseKey", 8u, 14u, 0, m32, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::UninstallDLC", 8u, 13u, 0, m33, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::AddHeader", 18u, 12u, 0, m34, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::AllowStartRequest", 18u, 36u, 0, m35, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::CopyToClipboard", 18u, 26u, 0, m36, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::CreateBrowser", 18u, 4u, 0, m37, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::ExecuteJavascript", 18u, 13u, 0, m38, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::FileLoadDialogResponse", 18u, 38u, 0, NULL, "tampon IntPtr pchSelectedFiles sans taille"},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Find", 18u, 28u, 0, m40, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GetLinkAtPosition", 18u, 30u, 0, m41, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoBack", 18u, 10u, 0, m42, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoForward", 18u, 11u, 0, m43, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Init", 18u, 2u, 0, m44, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::JSDialogResponse", 18u, 37u, 0, m45, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyChar", 18u, 21u, 0, m46, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyDown", 18u, 19u, 0, m47, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyUp", 18u, 20u, 0, m48, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::LoadURL", 18u, 6u, 0, m49, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDoubleClick", 18u, 16u, 0, m50, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDown", 18u, 15u, 0, m51, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseMove", 18u, 17u, 0, m52, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseUp", 18u, 14u, 0, m53, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseWheel", 18u, 18u, 0, m54, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::OpenDeveloperTools", 18u, 35u, 0, m55, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::PasteFromClipboard", 18u, 27u, 0, m56, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Reload", 18u, 9u, 0, m57, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::RemoveBrowser", 18u, 5u, 0, m58, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetBackgroundMode", 18u, 33u, 0, m59, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetCookie", 18u, 31u, 0, m60, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetDPIScalingFactor", 18u, 34u, 0, m61, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetHorizontalScroll", 18u, 22u, 0, m62, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetKeyFocus", 18u, 24u, 0, m63, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetPageScaleFactor", 18u, 32u, 0, m64, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetSize", 18u, 7u, 0, m65, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetVerticalScroll", 18u, 23u, 0, m66, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Shutdown", 18u, 3u, 0, m67, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopFind", 18u, 29u, 0, m68, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopLoad", 18u, 8u, 0, m69, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::ViewSource", 18u, 25u, 0, m70, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::CreateCookieContainer", 43u, 17u, 0, m71, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::CreateHTTPRequest", 43u, 0u, 0, m72, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::DeferHTTPRequest", 43u, 7u, 0, m73, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPDownloadProgressPct", 43u, 15u, 0, m74, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPRequestWasTimedOut", 43u, 24u, 0, m75, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseBodyData", 43u, 12u, 0, m76, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseBodySize", 43u, 11u, 0, m77, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseHeaderSize", 43u, 9u, 0, m78, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPResponseHeaderValue", 43u, 10u, 0, m79, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::GetHTTPStreamingResponseBodyData", 43u, 13u, 0, m80, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::PrioritizeHTTPRequest", 43u, 8u, 0, m81, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::ReleaseCookieContainer", 43u, 18u, 0, m82, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::ReleaseHTTPRequest", 43u, 14u, 0, m83, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SendHTTPRequest", 43u, 5u, 0, m84, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SendHTTPRequestAndStreamResponse", 43u, 6u, 0, m85, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetCookie", 43u, 19u, 0, m86, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestAbsoluteTimeoutMS", 43u, 23u, 0, m87, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestContextValue", 43u, 1u, 0, m88, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestCookieContainer", 43u, 20u, 0, m89, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestGetOrPostParameter", 43u, 4u, 0, m90, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestHeaderValue", 43u, 3u, 0, m91, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestNetworkActivityTimeout", 43u, 2u, 0, m92, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestRawPostBody", 43u, 16u, 0, NULL, "tampon byte[] pubBody sans taille"},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestRequiresVerifiedCertificate", 43u, 22u, 0, m94, NULL},
    {"STEAMHTTP_INTERFACE_VERSION002::SetHTTPRequestUserAgentInfo", 43u, 21u, 0, m95, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::CreateCookieContainer", 12u, 17u, 0, m96, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::CreateHTTPRequest", 12u, 0u, 0, m97, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::DeferHTTPRequest", 12u, 7u, 0, m98, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPDownloadProgressPct", 12u, 15u, 0, m99, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPRequestWasTimedOut", 12u, 24u, 0, m100, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodyData", 12u, 12u, 0, m101, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodySize", 12u, 11u, 0, m102, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderSize", 12u, 9u, 0, m103, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderValue", 12u, 10u, 0, m104, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPStreamingResponseBodyData", 12u, 13u, 0, m105, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::PrioritizeHTTPRequest", 12u, 8u, 0, m106, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::ReleaseCookieContainer", 12u, 18u, 0, m107, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::ReleaseHTTPRequest", 12u, 14u, 0, m108, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequest", 12u, 5u, 0, m109, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequestAndStreamResponse", 12u, 6u, 0, m110, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetCookie", 12u, 19u, 0, m111, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestAbsoluteTimeoutMS", 12u, 23u, 0, m112, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestContextValue", 12u, 1u, 0, m113, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestCookieContainer", 12u, 20u, 0, m114, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestGetOrPostParameter", 12u, 4u, 0, m115, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestHeaderValue", 12u, 3u, 0, m116, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestNetworkActivityTimeout", 12u, 2u, 0, m117, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRawPostBody", 12u, 16u, 0, NULL, "tampon byte[] pubBody sans taille"},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRequiresVerifiedCertificate", 12u, 22u, 0, m119, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestUserAgentInfo", 12u, 21u, 0, m120, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::AddPromoItem", 19u, 12u, 0, m121, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::AddPromoItems", 19u, 13u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::CheckResultSteamID", 19u, 4u, 0, m123, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::ConsumeItem", 19u, 14u, 0, m124, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::DeserializeResult", 19u, 9u, 0, m125, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::DestroyResult", 19u, 5u, 0, m126, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::ExchangeItems", 19u, 15u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayGenerate sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GenerateItems", 19u, 10u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetAllItems", 19u, 6u, 0, m129, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetEligiblePromoItemDefinitionIDs", 19u, 24u, 0, NULL, "tampon inout SteamItemDef_t[] pItemDefIDs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemDefinitionIDs", 19u, 21u, 0, NULL, "tampon inout SteamItemDef_t[] pItemDefIDs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemDefinitionProperty", 19u, 22u, 0, NULL, "tampon IntPtr pchValueBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemPrice", 19u, 29u, 0, m133, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemsByID", 19u, 7u, 0, m134, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemsWithPrices", 19u, 28u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetNumItemsWithPrices", 19u, 27u, 0, m136, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultItemProperty", 19u, 2u, 0, NULL, "tampon IntPtr pchValueBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultItems", 19u, 1u, 0, NULL, "tableau inout SteamItemDetails_t[] pOutItemsArray"},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultStatus", 19u, 0u, 0, m139, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultTimestamp", 19u, 3u, 0, m140, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GrantPromoItems", 19u, 11u, 0, m141, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::InspectItem", 19u, 37u, 0, m142, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::LoadItemDefinitions", 19u, 20u, 0, m143, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RemoveProperty", 19u, 31u, 0, m144, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RequestEligiblePromoItemDefinitionsIDs", 19u, 23u, 0, m145, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RequestPrices", 19u, 26u, 0, m146, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SendItemDropHeartbeat", 19u, 17u, 0, m147, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SerializeResult", 19u, 8u, 0, NULL, "tampon byte[] pOutBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyBool", 19u, 33u, 0, m149, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyFloat", 19u, 35u, 0, m150, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyInt64", 19u, 34u, 0, m151, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyString", 19u, 32u, 0, m152, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::StartPurchase", 19u, 25u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::StartUpdateProperties", 19u, 30u, 0, m154, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SubmitUpdateProperties", 19u, 36u, 0, m155, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::TransferItemQuantity", 19u, 16u, 0, m156, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::TriggerItemDrop", 19u, 18u, 0, m157, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::BActivationSuccess", 17u, 3u, 0, m158, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::BIsCurrentMusicRemote", 17u, 2u, 0, m159, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryDidChange", 17u, 21u, 0, m160, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryIsAvailable", 17u, 17u, 0, m161, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryWillChange", 17u, 16u, 0, m162, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::DeregisterSteamMusicRemote", 17u, 1u, 0, m163, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableLooped", 17u, 9u, 0, m164, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayNext", 17u, 7u, 0, m165, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayPrevious", 17u, 6u, 0, m166, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlaylists", 17u, 11u, 0, m167, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableQueue", 17u, 10u, 0, m168, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableShuffled", 17u, 8u, 0, m169, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistDidChange", 17u, 31u, 0, m170, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistWillChange", 17u, 27u, 0, m171, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueDidChange", 17u, 26u, 0, m172, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueWillChange", 17u, 22u, 0, m173, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::RegisterSteamMusicRemote", 17u, 0u, 0, m174, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetPlaylistEntries", 17u, 28u, 0, m175, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetQueueEntries", 17u, 23u, 0, m176, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentPlaylistEntry", 17u, 30u, 0, m177, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentQueueEntry", 17u, 25u, 0, m178, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetDisplayName", 17u, 4u, 0, m179, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPNGIcon_64x64", 17u, 5u, 0, m180, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPlaylistEntry", 17u, 29u, 0, m181, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetQueueEntry", 17u, 24u, 0, m182, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryCoverArt", 17u, 20u, 0, m183, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryElapsedSeconds", 17u, 19u, 0, m184, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryText", 17u, 18u, 0, m185, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateLooped", 17u, 14u, 0, m186, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdatePlaybackStatus", 17u, 12u, 0, m187, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateShuffled", 17u, 13u, 0, m188, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateVolume", 17u, 15u, 0, m189, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::BIsEnabled", 16u, 0u, 0, m190, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::BIsPlaying", 16u, 1u, 0, m191, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::GetPlaybackStatus", 16u, 2u, 0, m192, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::GetVolume", 16u, 8u, 0, m193, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::Pause", 16u, 4u, 0, m194, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::Play", 16u, 3u, 0, m195, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::PlayNext", 16u, 6u, 0, m196, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::PlayPrevious", 16u, 5u, 0, m197, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::SetVolume", 16u, 7u, 0, m198, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppBlocked", 21u, 2u, 0, m199, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppInBlockList", 21u, 3u, 0, m200, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureBlocked", 21u, 4u, 0, m201, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureInBlockList", 21u, 5u, 0, m202, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockEnabled", 21u, 0u, 0, m203, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockLocked", 21u, 1u, 0, m204, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BGetSessionClientResolution", 31u, 5u, 0, m205, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BSendRemotePlayTogetherInvite", 31u, 7u, 0, m206, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BStartRemotePlayTogether", 31u, 6u, 0, m207, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientFormFactor", 31u, 4u, 0, m208, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientName", 31u, 3u, 0, m209, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionCount", 31u, 0u, 0, m210, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionID", 31u, 1u, 0, m211, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionSteamID", 31u, 2u, 1, m212, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::CommitPublishedFileUpdate", 41u, 38u, 0, m213, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::CreatePublishedFileUpdateRequest", 41u, 31u, 0, m214, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::DeletePublishedFile", 41u, 40u, 0, m215, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumeratePublishedFilesByUserAction", 41u, 52u, 0, m216, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumeratePublishedWorkshopFiles", 41u, 53u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumerateUserPublishedFiles", 41u, 41u, 0, m218, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumerateUserSharedWorkshopFiles", 41u, 49u, 0, NULL, "tampon IntPtr pRequiredTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::EnumerateUserSubscribedFiles", 41u, 43u, 0, m220, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileDelete", 41u, 6u, 0, m221, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileExists", 41u, 13u, 0, m222, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileForget", 41u, 5u, 0, m223, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FilePersisted", 41u, 14u, 0, m224, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileRead", 41u, 1u, 0, m225, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileReadAsync", 41u, 3u, 0, m226, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileReadAsyncComplete", 41u, 4u, 0, m227, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileShare", 41u, 7u, 0, m228, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWrite", 41u, 0u, 0, m229, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteAsync", 41u, 2u, 0, m230, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamCancel", 41u, 12u, 0, m231, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamClose", 41u, 11u, 0, m232, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamOpen", 41u, 9u, 0, m233, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::FileWriteStreamWriteChunk", 41u, 10u, 0, m234, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetCachedUGCCount", 41u, 28u, 0, m235, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetCachedUGCHandle", 41u, 29u, 0, m236, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileCount", 41u, 18u, 0, m237, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileNameAndSize", 41u, 19u, 0, m238, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileSize", 41u, 15u, 0, m239, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetFileTimestamp", 41u, 16u, 0, m240, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetPublishedFileDetails", 41u, 39u, 0, m241, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetPublishedItemVoteDetails", 41u, 46u, 0, m242, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetQuota", 41u, 20u, 0, m243, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetSyncPlatforms", 41u, 17u, 0, m244, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUGCDetails", 41u, 26u, 0, m245, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUGCDownloadProgress", 41u, 25u, 0, m246, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::GetUserPublishedItemVoteDetails", 41u, 48u, 0, m247, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::IsCloudEnabledForAccount", 41u, 21u, 0, m248, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::IsCloudEnabledForApp", 41u, 22u, 0, m249, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::PublishVideo", 41u, 50u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::PublishWorkshopFile", 41u, 30u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetCloudEnabledForApp", 41u, 23u, 0, m252, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetSyncPlatforms", 41u, 8u, 0, m253, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::SetUserPublishedFileAction", 41u, 51u, 0, m254, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::SubscribePublishedFile", 41u, 42u, 0, m255, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCDownload", 41u, 24u, 0, m256, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCDownloadToLocation", 41u, 54u, 0, m257, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UGCRead", 41u, 27u, 0, m258, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UnsubscribePublishedFile", 41u, 44u, 0, m259, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileDescription", 41u, 35u, 0, m260, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileFile", 41u, 32u, 0, m261, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFilePreviewFile", 41u, 33u, 0, m262, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileSetChangeDescription", 41u, 45u, 0, m263, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileTags", 41u, 37u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileTitle", 41u, 34u, 0, m265, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdatePublishedFileVisibility", 41u, 36u, 0, m266, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION014::UpdateUserPublishedItemVote", 41u, 47u, 0, m267, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch", 10u, 57u, 0, m268, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate", 10u, 38u, 0, m269, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest", 10u, 31u, 0, m270, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile", 10u, 40u, 0, m271, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch", 10u, 58u, 0, m272, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction", 10u, 52u, 0, m273, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedWorkshopFiles", 10u, 53u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles", 10u, 41u, 0, m275, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSharedWorkshopFiles", 10u, 49u, 0, NULL, "tampon IntPtr pRequiredTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles", 10u, 43u, 0, m277, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete", 10u, 6u, 0, m278, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists", 10u, 13u, 0, m279, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget", 10u, 5u, 0, m280, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted", 10u, 14u, 0, m281, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead", 10u, 1u, 0, m282, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync", 10u, 3u, 0, m283, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete", 10u, 4u, 0, m284, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare", 10u, 7u, 0, m285, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite", 10u, 0u, 0, m286, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync", 10u, 2u, 0, m287, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel", 10u, 12u, 0, m288, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose", 10u, 11u, 0, m289, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen", 10u, 9u, 0, m290, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk", 10u, 10u, 0, m291, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount", 10u, 28u, 0, m292, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle", 10u, 29u, 0, m293, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount", 10u, 18u, 0, m294, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize", 10u, 19u, 0, m295, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize", 10u, 15u, 0, m296, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp", 10u, 16u, 0, m297, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange", 10u, 56u, 0, m298, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount", 10u, 55u, 0, m299, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails", 10u, 39u, 0, m300, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails", 10u, 46u, 0, m301, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota", 10u, 20u, 0, m302, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms", 10u, 17u, 0, m303, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails", 10u, 26u, 0, m304, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDownloadProgress", 10u, 25u, 0, m305, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails", 10u, 48u, 0, m306, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount", 10u, 21u, 0, m307, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp", 10u, 22u, 0, m308, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp", 10u, 23u, 0, m309, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms", 10u, 8u, 0, m310, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction", 10u, 51u, 0, m311, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile", 10u, 42u, 0, m312, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload", 10u, 24u, 0, m313, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation", 10u, 54u, 0, m314, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead", 10u, 27u, 0, m315, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile", 10u, 44u, 0, m316, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription", 10u, 35u, 0, m317, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile", 10u, 32u, 0, m318, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile", 10u, 33u, 0, m319, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription", 10u, 45u, 0, m320, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTags", 10u, 37u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle", 10u, 34u, 0, m322, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility", 10u, 36u, 0, m323, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote", 10u, 47u, 0, m324, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary", 11u, 1u, 0, m325, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary", 11u, 8u, 0, m326, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots", 11u, 3u, 0, m327, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked", 11u, 7u, 0, m328, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation", 11u, 4u, 0, m329, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile", 11u, 6u, 0, m330, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser", 11u, 5u, 0, m331, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot", 11u, 2u, 0, m332, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot", 11u, 0u, 0, m333, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddGamePhaseTag", 35u, 14u, 0, m334, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddInstantaneousTimelineEvent", 35u, 3u, 0, m335, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddRangeTimelineEvent", 35u, 4u, 0, m336, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::ClearTimelineTooltip", 35u, 1u, 0, m337, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::DoesEventRecordingExist", 35u, 9u, 0, m338, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::DoesGamePhaseRecordingExist", 35u, 13u, 0, m339, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::EndGamePhase", 35u, 11u, 0, m340, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::EndRangeTimelineEvent", 35u, 7u, 0, m341, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::OpenOverlayToGamePhase", 35u, 16u, 0, m342, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::OpenOverlayToTimelineEvent", 35u, 17u, 0, m343, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::RemoveTimelineEvent", 35u, 8u, 0, m344, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetGamePhaseAttribute", 35u, 15u, 0, m345, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetGamePhaseID", 35u, 12u, 0, m346, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetTimelineGameMode", 35u, 2u, 0, m347, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetTimelineTooltip", 35u, 0u, 0, m348, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::StartGamePhase", 35u, 10u, 0, m349, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::StartRangeTimelineEvent", 35u, 5u, 0, m350, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::UpdateRangeTimelineEvent", 35u, 6u, 0, m351, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddAppDependency", 42u, 70u, 0, m352, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddDependency", 42u, 68u, 0, m353, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddExcludedTag", 42u, 15u, 0, m354, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddItemKeyValueTag", 42u, 43u, 0, m355, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddItemPreviewFile", 42u, 44u, 0, m356, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddItemPreviewVideo", 42u, 45u, 0, m357, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddItemToFavorites", 42u, 53u, 0, m358, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddRequiredKeyValueTag", 42u, 30u, 0, m359, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::AddRequiredTag", 42u, 14u, 0, m360, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::BInitWorkshopForGameServer", 42u, 63u, 0, m361, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::CreateItem", 42u, 32u, 0, m362, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::CreateQueryAllUGCRequest", 42u, 1u, 0, m363, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::CreateQueryUGCDetailsRequest", 42u, 2u, 0, m364, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::CreateQueryUserUGCRequest", 42u, 0u, 0, m365, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::DeleteItem", 42u, 73u, 0, m366, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::DownloadItem", 42u, 62u, 0, m367, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetAppDependencies", 42u, 72u, 0, m368, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetItemDownloadInfo", 42u, 61u, 0, m369, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetItemInstallInfo", 42u, 60u, 0, m370, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetItemState", 42u, 59u, 0, m371, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetItemUpdateProgress", 42u, 50u, 0, m372, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetNumSubscribedItems", 42u, 57u, 0, m373, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCAdditionalPreview", 42u, 10u, 0, m374, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCChildren", 42u, 7u, 0, m375, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCKeyValueTag", 42u, 12u, 0, m376, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCMetadata", 42u, 6u, 0, m377, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCNumAdditionalPreviews", 42u, 9u, 0, m378, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCNumKeyValueTags", 42u, 11u, 0, m379, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCPreviewURL", 42u, 5u, 0, m380, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCResult", 42u, 4u, 0, m381, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetQueryUGCStatistic", 42u, 8u, 0, m382, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetSubscribedItems", 42u, 58u, 0, m383, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::GetUserItemVote", 42u, 52u, 0, m384, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::ReleaseQueryUGCRequest", 42u, 13u, 0, m385, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RemoveAppDependency", 42u, 71u, 0, m386, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RemoveDependency", 42u, 69u, 0, m387, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RemoveItemFromFavorites", 42u, 54u, 0, m388, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RemoveItemKeyValueTags", 42u, 42u, 0, m389, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RemoveItemPreview", 42u, 48u, 0, m390, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::RequestUGCDetails", 42u, 31u, 0, m391, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SendQueryUGCRequest", 42u, 3u, 0, m392, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetAllowCachedResponse", 42u, 25u, 0, m393, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetCloudFileNameFilter", 42u, 26u, 0, m394, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemContent", 42u, 40u, 0, m395, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemDescription", 42u, 35u, 0, m396, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemMetadata", 42u, 37u, 0, m397, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemPreview", 42u, 41u, 0, m398, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemTags", 42u, 39u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemTitle", 42u, 34u, 0, m400, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemUpdateLanguage", 42u, 36u, 0, m401, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetItemVisibility", 42u, 38u, 0, m402, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetLanguage", 42u, 24u, 0, m403, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetMatchAnyTag", 42u, 27u, 0, m404, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetRankedByTrendDays", 42u, 29u, 0, m405, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnAdditionalPreviews", 42u, 21u, 0, m406, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnChildren", 42u, 20u, 0, m407, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnKeyValueTags", 42u, 17u, 0, m408, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnLongDescription", 42u, 18u, 0, m409, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnMetadata", 42u, 19u, 0, m410, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnOnlyIDs", 42u, 16u, 0, m411, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnPlaytimeStats", 42u, 23u, 0, m412, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetReturnTotalOnly", 42u, 22u, 0, m413, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetSearchText", 42u, 28u, 0, m414, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SetUserItemVote", 42u, 51u, 0, m415, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::StartItemUpdate", 42u, 33u, 0, m416, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::StartPlaytimeTracking", 42u, 65u, 0, m417, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::StopPlaytimeTracking", 42u, 66u, 0, m418, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::StopPlaytimeTrackingForAllItems", 42u, 67u, 0, m419, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SubmitItemUpdate", 42u, 49u, 0, m420, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SubscribeItem", 42u, 55u, 0, m421, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::SuspendDownloads", 42u, 64u, 0, m422, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::UnsubscribeItem", 42u, 56u, 0, m423, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::UpdateItemPreviewFile", 42u, 46u, 0, m424, NULL},
    {"STEAMUGC_INTERFACE_VERSION010::UpdateItemPreviewVideo", 42u, 47u, 0, m425, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddAppDependency", 14u, 80u, 0, m426, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddDependency", 14u, 78u, 0, m427, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddExcludedTag", 14u, 21u, 0, m428, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag", 14u, 53u, 0, m429, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile", 14u, 54u, 0, m430, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo", 14u, 55u, 0, m431, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites", 14u, 63u, 0, m432, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag", 14u, 38u, 0, m433, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTag", 14u, 19u, 0, m434, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTagGroup", 14u, 20u, 0, NULL, "tampon IntPtr pTagGroups sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer", 14u, 73u, 0, m436, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateItem", 14u, 40u, 0, m437, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor", 14u, 2u, 0, m438, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage", 14u, 1u, 0, m439, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUGCDetailsRequest", 14u, 3u, 0, m440, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest", 14u, 0u, 0, m441, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DeleteItem", 14u, 83u, 0, m442, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DownloadItem", 14u, 72u, 0, m443, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetAppDependencies", 14u, 82u, 0, m444, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo", 14u, 71u, 0, m445, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo", 14u, 70u, 0, m446, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemState", 14u, 69u, 0, m447, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress", 14u, 60u, 0, m448, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems", 14u, 67u, 0, m449, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag", 14u, 17u, 0, m450, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren", 14u, 11u, 0, m451, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag", 14u, 16u, 0, m452, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCMetadata", 14u, 10u, 0, m453, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews", 14u, 13u, 0, m454, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags", 14u, 15u, 0, m455, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags", 14u, 6u, 0, m456, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL", 14u, 9u, 0, m457, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCResult", 14u, 5u, 0, m458, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic", 14u, 12u, 0, m459, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag", 14u, 7u, 0, m460, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName", 14u, 8u, 0, m461, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems", 14u, 68u, 0, m462, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetUserItemVote", 14u, 62u, 0, m463, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus", 14u, 85u, 0, m464, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest", 14u, 18u, 0, m465, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags", 14u, 51u, 0, m466, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency", 14u, 81u, 0, m467, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveDependency", 14u, 79u, 0, m468, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites", 14u, 64u, 0, m469, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags", 14u, 52u, 0, m470, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview", 14u, 58u, 0, m471, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails", 14u, 39u, 0, m472, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest", 14u, 4u, 0, m473, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse", 14u, 31u, 0, m474, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload", 14u, 50u, 0, m475, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter", 14u, 32u, 0, m476, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemContent", 14u, 48u, 0, m477, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemDescription", 14u, 43u, 0, m478, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemMetadata", 14u, 45u, 0, m479, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemPreview", 14u, 49u, 0, m480, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTags", 14u, 47u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTitle", 14u, 42u, 0, m482, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage", 14u, 44u, 0, m483, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemVisibility", 14u, 46u, 0, m484, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetLanguage", 14u, 30u, 0, m485, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag", 14u, 33u, 0, m486, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays", 14u, 35u, 0, m487, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews", 14u, 27u, 0, m488, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnChildren", 14u, 26u, 0, m489, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags", 14u, 23u, 0, m490, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription", 14u, 24u, 0, m491, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata", 14u, 25u, 0, m492, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs", 14u, 22u, 0, m493, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats", 14u, 29u, 0, m494, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly", 14u, 28u, 0, m495, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetSearchText", 14u, 34u, 0, m496, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange", 14u, 36u, 0, m497, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange", 14u, 37u, 0, m498, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetUserItemVote", 14u, 61u, 0, m499, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA", 14u, 84u, 0, m500, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartItemUpdate", 14u, 41u, 0, m501, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartPlaytimeTracking", 14u, 75u, 0, m502, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTracking", 14u, 76u, 0, m503, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems", 14u, 77u, 0, m504, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate", 14u, 59u, 0, m505, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubscribeItem", 14u, 65u, 0, m506, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SuspendDownloads", 14u, 74u, 0, m507, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem", 14u, 66u, 0, m508, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile", 14u, 56u, 0, m509, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo", 14u, 57u, 0, m510, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddAppDependency", 27u, 87u, 0, m511, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddContentDescriptor", 27u, 63u, 0, m512, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddDependency", 27u, 85u, 0, m513, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddExcludedTag", 27u, 24u, 0, m514, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemKeyValueTag", 27u, 57u, 0, m515, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemPreviewFile", 27u, 58u, 0, m516, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemPreviewVideo", 27u, 59u, 0, m517, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemToFavorites", 27u, 70u, 0, m518, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredKeyValueTag", 27u, 42u, 0, m519, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredTag", 27u, 22u, 0, m520, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredTagGroup", 27u, 23u, 0, NULL, "tampon IntPtr pTagGroups sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::BInitWorkshopForGameServer", 27u, 80u, 0, m522, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateItem", 27u, 44u, 0, m523, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestCursor", 27u, 2u, 0, m524, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestPage", 27u, 1u, 0, m525, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryUGCDetailsRequest", 27u, 3u, 0, m526, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryUserUGCRequest", 27u, 0u, 0, m527, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::DeleteItem", 27u, 90u, 0, m528, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::DownloadItem", 27u, 79u, 0, m529, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetAppDependencies", 27u, 89u, 0, m530, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemDownloadInfo", 27u, 78u, 0, m531, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemInstallInfo", 27u, 77u, 0, m532, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemState", 27u, 76u, 0, m533, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemUpdateProgress", 27u, 67u, 0, m534, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetNumSubscribedItems", 27u, 74u, 0, m535, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetNumSupportedGameVersions", 27u, 18u, 0, m536, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryFirstUGCKeyValueTag", 27u, 17u, 0, m537, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCChildren", 27u, 11u, 0, m538, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCContentDescriptors", 27u, 20u, 0, m539, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCKeyValueTag", 27u, 16u, 0, m540, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCMetadata", 27u, 10u, 0, m541, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumAdditionalPreviews", 27u, 13u, 0, m542, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumKeyValueTags", 27u, 15u, 0, m543, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumTags", 27u, 6u, 0, m544, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCPreviewURL", 27u, 9u, 0, m545, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCResult", 27u, 5u, 0, m546, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCStatistic", 27u, 12u, 0, m547, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTag", 27u, 7u, 0, m548, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTagDisplayName", 27u, 8u, 0, m549, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetSubscribedItems", 27u, 75u, 0, m550, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetSupportedGameVersionData", 27u, 19u, 0, NULL, "tampon IntPtr pchGameBranchMin sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::GetUserContentDescriptorPreferences", 27u, 93u, 0, m552, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetUserItemVote", 27u, 69u, 0, m553, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetWorkshopEULAStatus", 27u, 92u, 0, m554, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::ReleaseQueryUGCRequest", 27u, 21u, 0, m555, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveAllItemKeyValueTags", 27u, 55u, 0, m556, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveAppDependency", 27u, 88u, 0, m557, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveContentDescriptor", 27u, 64u, 0, m558, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveDependency", 27u, 86u, 0, m559, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemFromFavorites", 27u, 71u, 0, m560, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemKeyValueTags", 27u, 56u, 0, m561, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemPreview", 27u, 62u, 0, m562, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RequestUGCDetails", 27u, 43u, 0, m563, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SendQueryUGCRequest", 27u, 4u, 0, m564, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAdminQuery", 27u, 35u, 0, m565, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAllowCachedResponse", 27u, 34u, 0, m566, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAllowLegacyUpload", 27u, 54u, 0, m567, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetCloudFileNameFilter", 27u, 36u, 0, m568, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemContent", 27u, 52u, 0, m569, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemDescription", 27u, 47u, 0, m570, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemMetadata", 27u, 49u, 0, m571, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemPreview", 27u, 53u, 0, m572, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemTags", 27u, 51u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemTitle", 27u, 46u, 0, m574, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemUpdateLanguage", 27u, 48u, 0, m575, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemVisibility", 27u, 50u, 0, m576, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetLanguage", 27u, 33u, 0, m577, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetMatchAnyTag", 27u, 37u, 0, m578, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetRankedByTrendDays", 27u, 39u, 0, m579, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetRequiredGameVersions", 27u, 65u, 0, m580, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnAdditionalPreviews", 27u, 30u, 0, m581, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnChildren", 27u, 29u, 0, m582, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnKeyValueTags", 27u, 26u, 0, m583, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnLongDescription", 27u, 27u, 0, m584, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnMetadata", 27u, 28u, 0, m585, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnOnlyIDs", 27u, 25u, 0, m586, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnPlaytimeStats", 27u, 32u, 0, m587, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnTotalOnly", 27u, 31u, 0, m588, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetSearchText", 27u, 38u, 0, m589, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetTimeCreatedDateRange", 27u, 40u, 0, m590, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetTimeUpdatedDateRange", 27u, 41u, 0, m591, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetUserItemVote", 27u, 68u, 0, m592, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::ShowWorkshopEULA", 27u, 91u, 0, m593, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StartItemUpdate", 27u, 45u, 0, m594, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StartPlaytimeTracking", 27u, 82u, 0, m595, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTracking", 27u, 83u, 0, m596, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTrackingForAllItems", 27u, 84u, 0, m597, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SubmitItemUpdate", 27u, 66u, 0, m598, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SubscribeItem", 27u, 72u, 0, m599, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SuspendDownloads", 27u, 81u, 0, m600, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UnsubscribeItem", 27u, 73u, 0, m601, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewFile", 27u, 60u, 0, m602, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewVideo", 27u, 61u, 0, m603, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::AttachLeaderboardUGC", 40u, 32u, 0, m604, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::ClearAchievement", 40u, 8u, 0, m605, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::DownloadLeaderboardEntries", 40u, 28u, 0, m606, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::DownloadLeaderboardEntriesForUsers", 40u, 29u, 0, m607, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::FindLeaderboard", 40u, 23u, 0, m608, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::FindOrCreateLeaderboard", 40u, 22u, 0, m609, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievement", 40u, 6u, 0, m610, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementAchievedPercent", 40u, 37u, 0, m611, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementAndUnlockTime", 40u, 9u, 0, m612, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementDisplayAttribute", 40u, 12u, 0, m613, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementIcon", 40u, 11u, 0, m614, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetAchievementName", 40u, 15u, 0, m615, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetDownloadedLeaderboardEntry", 40u, 30u, 0, m616, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStat", 40u, 39u, 0, m617, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStat0", 40u, 40u, 0, m618, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStatHistory", 40u, 41u, 0, m619, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetGlobalStatHistory0", 40u, 42u, 0, m620, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardDisplayType", 40u, 27u, 0, m621, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardEntryCount", 40u, 25u, 0, m622, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardName", 40u, 24u, 0, m623, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetLeaderboardSortMethod", 40u, 26u, 0, m624, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetMostAchievedAchievementInfo", 40u, 35u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetNextMostAchievedAchievementInfo", 40u, 36u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetNumAchievements", 40u, 14u, 0, m627, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetNumberOfCurrentPlayers", 40u, 33u, 0, m628, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetStat", 40u, 1u, 0, m629, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetStat0", 40u, 2u, 0, m630, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetUserAchievement", 40u, 19u, 0, m631, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetUserAchievementAndUnlockTime", 40u, 20u, 0, m632, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetUserStat", 40u, 17u, 0, m633, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::GetUserStat0", 40u, 18u, 0, m634, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::IndicateAchievementProgress", 40u, 13u, 0, m635, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::RequestCurrentStats", 40u, 0u, 0, m636, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::RequestGlobalAchievementPercentages", 40u, 34u, 0, m637, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::RequestGlobalStats", 40u, 38u, 0, m638, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::RequestUserStats", 40u, 16u, 0, m639, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::ResetAllStats", 40u, 21u, 0, m640, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::SetAchievement", 40u, 7u, 0, m641, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::SetStat", 40u, 3u, 0, m642, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::SetStat0", 40u, 4u, 0, m643, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::StoreStats", 40u, 10u, 0, m644, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::UpdateAvgRateStat", 40u, 5u, 0, m645, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION011::UploadLeaderboardScore", 40u, 31u, 0, m646, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC", 7u, 32u, 0, m647, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement", 7u, 8u, 0, m648, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries", 7u, 28u, 0, m649, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers", 7u, 29u, 0, m650, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard", 7u, 23u, 0, m651, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard", 7u, 22u, 0, m652, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement", 7u, 6u, 0, m653, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent", 7u, 37u, 0, m654, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime", 7u, 9u, 0, m655, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute", 7u, 12u, 0, m656, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon", 7u, 11u, 0, m657, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName", 7u, 15u, 0, m658, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat", 7u, 44u, 0, m659, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32", 7u, 43u, 0, m660, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry", 7u, 30u, 0, m661, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble", 7u, 40u, 0, m662, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble", 7u, 42u, 0, m663, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64", 7u, 41u, 0, m664, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64", 7u, 39u, 0, m665, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType", 7u, 27u, 0, m666, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount", 7u, 25u, 0, m667, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName", 7u, 24u, 0, m668, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod", 7u, 26u, 0, m669, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetMostAchievedAchievementInfo", 7u, 35u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNextMostAchievedAchievementInfo", 7u, 36u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements", 7u, 14u, 0, m672, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers", 7u, 33u, 0, m673, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat", 7u, 2u, 0, m674, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32", 7u, 1u, 0, m675, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement", 7u, 19u, 0, m676, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime", 7u, 20u, 0, m677, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat", 7u, 18u, 0, m678, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32", 7u, 17u, 0, m679, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress", 7u, 13u, 0, m680, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats", 7u, 0u, 0, m681, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages", 7u, 34u, 0, m682, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats", 7u, 38u, 0, m683, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats", 7u, 16u, 0, m684, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats", 7u, 21u, 0, m685, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement", 7u, 7u, 0, m686, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat", 7u, 4u, 0, m687, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32", 7u, 3u, 0, m688, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats", 7u, 10u, 0, m689, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat", 7u, 5u, 0, m690, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore", 7u, 31u, 0, m691, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFSettings", 20u, 2u, 0, m692, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFStringForApp", 20u, 3u, 0, NULL, "tampon IntPtr pchBuffer sans taille"},
    {"STEAMVIDEO_INTERFACE_V002::GetVideoURL", 20u, 0u, 0, m694, NULL},
    {"STEAMVIDEO_INTERFACE_V002::IsBroadcasting", 20u, 1u, 0, m695, NULL},
    {"STEAMVIDEO_INTERFACE_V007::GetOPFSettings", 28u, 2u, 0, m696, NULL},
    {"STEAMVIDEO_INTERFACE_V007::GetOPFStringForApp", 28u, 3u, 0, NULL, "tampon IntPtr pchBuffer sans taille"},
    {"STEAMVIDEO_INTERFACE_V007::GetVideoURL", 28u, 0u, 0, m698, NULL},
    {"STEAMVIDEO_INTERFACE_V007::IsBroadcasting", 28u, 1u, 0, m699, NULL},
    {"SteamClient017::BReleaseSteamPipe", 36u, 1u, 0, m700, NULL},
    {"SteamClient017::BShutdownIfAllPipesClosed", 36u, 22u, 0, m701, NULL},
    {"SteamClient017::ConnectToGlobalUser", 36u, 2u, 0, m702, NULL},
    {"SteamClient017::CreateLocalUser", 36u, 3u, 0, m703, NULL},
    {"SteamClient017::CreateSteamPipe", 36u, 0u, 0, m704, NULL},
    {"SteamClient017::GetIPCCallCount", 36u, 20u, 0, m705, NULL},
    {"SteamClient017::GetISteamAppList", 36u, 27u, 0, m706, NULL},
    {"SteamClient017::GetISteamApps", 36u, 15u, 0, m707, NULL},
    {"SteamClient017::GetISteamController", 36u, 25u, 0, m708, NULL},
    {"SteamClient017::GetISteamFriends", 36u, 8u, 0, m709, NULL},
    {"SteamClient017::GetISteamGameServer", 36u, 6u, 0, m710, NULL},
    {"SteamClient017::GetISteamGameServerStats", 36u, 14u, 0, m711, NULL},
    {"SteamClient017::GetISteamGenericInterface", 36u, 12u, 0, m712, NULL},
    {"SteamClient017::GetISteamHTMLSurface", 36u, 30u, 0, m713, NULL},
    {"SteamClient017::GetISteamHTTP", 36u, 23u, 0, m714, NULL},
    {"SteamClient017::GetISteamInventory", 36u, 34u, 0, m715, NULL},
    {"SteamClient017::GetISteamMatchmaking", 36u, 10u, 0, m716, NULL},
    {"SteamClient017::GetISteamMatchmakingServers", 36u, 11u, 0, m717, NULL},
    {"SteamClient017::GetISteamMusic", 36u, 28u, 0, m718, NULL},
    {"SteamClient017::GetISteamMusicRemote", 36u, 29u, 0, m719, NULL},
    {"SteamClient017::GetISteamNetworking", 36u, 16u, 0, m720, NULL},
    {"SteamClient017::GetISteamParentalSettings", 36u, 36u, 0, m721, NULL},
    {"SteamClient017::GetISteamRemoteStorage", 36u, 17u, 0, m722, NULL},
    {"SteamClient017::GetISteamScreenshots", 36u, 18u, 0, m723, NULL},
    {"SteamClient017::GetISteamUGC", 36u, 26u, 0, m724, NULL},
    {"SteamClient017::GetISteamUnifiedMessages", 36u, 24u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamClient017::GetISteamUser", 36u, 5u, 0, m726, NULL},
    {"SteamClient017::GetISteamUserStats", 36u, 13u, 0, m727, NULL},
    {"SteamClient017::GetISteamUtils", 36u, 9u, 0, m728, NULL},
    {"SteamClient017::GetISteamVideo", 36u, 35u, 0, m729, NULL},
    {"SteamClient017::ReleaseUser", 36u, 4u, 0, m730, NULL},
    {"SteamClient017::SetLocalIPBinding", 36u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient017::SetWarningMessageHook", 36u, 21u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamClient020::BReleaseSteamPipe", 0u, 1u, 0, m733, NULL},
    {"SteamClient020::BShutdownIfAllPipesClosed", 0u, 23u, 0, m734, NULL},
    {"SteamClient020::ConnectToGlobalUser", 0u, 2u, 0, m735, NULL},
    {"SteamClient020::CreateLocalUser", 0u, 3u, 0, m736, NULL},
    {"SteamClient020::CreateSteamPipe", 0u, 0u, 0, m737, NULL},
    {"SteamClient020::GetIPCCallCount", 0u, 21u, 0, m738, NULL},
    {"SteamClient020::GetISteamAppList", 0u, 28u, 0, m739, NULL},
    {"SteamClient020::GetISteamApps", 0u, 15u, 0, m740, NULL},
    {"SteamClient020::GetISteamController", 0u, 26u, 0, m741, NULL},
    {"SteamClient020::GetISteamFriends", 0u, 8u, 0, m742, NULL},
    {"SteamClient020::GetISteamGameSearch", 0u, 19u, 0, m743, NULL},
    {"SteamClient020::GetISteamGameServer", 0u, 6u, 0, m744, NULL},
    {"SteamClient020::GetISteamGameServerStats", 0u, 14u, 0, m745, NULL},
    {"SteamClient020::GetISteamGenericInterface", 0u, 12u, 0, m746, NULL},
    {"SteamClient020::GetISteamHTMLSurface", 0u, 31u, 0, m747, NULL},
    {"SteamClient020::GetISteamHTTP", 0u, 24u, 0, m748, NULL},
    {"SteamClient020::GetISteamInput", 0u, 38u, 0, m749, NULL},
    {"SteamClient020::GetISteamInventory", 0u, 35u, 0, m750, NULL},
    {"SteamClient020::GetISteamMatchmaking", 0u, 10u, 0, m751, NULL},
    {"SteamClient020::GetISteamMatchmakingServers", 0u, 11u, 0, m752, NULL},
    {"SteamClient020::GetISteamMusic", 0u, 29u, 0, m753, NULL},
    {"SteamClient020::GetISteamMusicRemote", 0u, 30u, 0, m754, NULL},
    {"SteamClient020::GetISteamNetworking", 0u, 16u, 0, m755, NULL},
    {"SteamClient020::GetISteamParentalSettings", 0u, 37u, 0, m756, NULL},
    {"SteamClient020::GetISteamParties", 0u, 39u, 0, m757, NULL},
    {"SteamClient020::GetISteamRemotePlay", 0u, 40u, 0, m758, NULL},
    {"SteamClient020::GetISteamRemoteStorage", 0u, 17u, 0, m759, NULL},
    {"SteamClient020::GetISteamScreenshots", 0u, 18u, 0, m760, NULL},
    {"SteamClient020::GetISteamUGC", 0u, 27u, 0, m761, NULL},
    {"SteamClient020::GetISteamUser", 0u, 5u, 0, m762, NULL},
    {"SteamClient020::GetISteamUserStats", 0u, 13u, 0, m763, NULL},
    {"SteamClient020::GetISteamUtils", 0u, 9u, 0, m764, NULL},
    {"SteamClient020::GetISteamVideo", 0u, 36u, 0, m765, NULL},
    {"SteamClient020::ReleaseUser", 0u, 4u, 0, m766, NULL},
    {"SteamClient020::SetLocalIPBinding", 0u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient020::SetWarningMessageHook", 0u, 22u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamClient021::BReleaseSteamPipe", 25u, 1u, 0, m769, NULL},
    {"SteamClient021::BShutdownIfAllPipesClosed", 25u, 23u, 0, m770, NULL},
    {"SteamClient021::ConnectToGlobalUser", 25u, 2u, 0, m771, NULL},
    {"SteamClient021::CreateLocalUser", 25u, 3u, 0, m772, NULL},
    {"SteamClient021::CreateSteamPipe", 25u, 0u, 0, m773, NULL},
    {"SteamClient021::GetIPCCallCount", 25u, 21u, 0, m774, NULL},
    {"SteamClient021::GetISteamApps", 25u, 15u, 0, m775, NULL},
    {"SteamClient021::GetISteamController", 25u, 25u, 0, m776, NULL},
    {"SteamClient021::GetISteamFriends", 25u, 8u, 0, m777, NULL},
    {"SteamClient021::GetISteamGameSearch", 25u, 19u, 0, m778, NULL},
    {"SteamClient021::GetISteamGameServer", 25u, 6u, 0, m779, NULL},
    {"SteamClient021::GetISteamGameServerStats", 25u, 14u, 0, m780, NULL},
    {"SteamClient021::GetISteamGenericInterface", 25u, 12u, 0, m781, NULL},
    {"SteamClient021::GetISteamHTMLSurface", 25u, 29u, 0, m782, NULL},
    {"SteamClient021::GetISteamHTTP", 25u, 24u, 0, m783, NULL},
    {"SteamClient021::GetISteamInput", 25u, 36u, 0, m784, NULL},
    {"SteamClient021::GetISteamInventory", 25u, 33u, 0, m785, NULL},
    {"SteamClient021::GetISteamMatchmaking", 25u, 10u, 0, m786, NULL},
    {"SteamClient021::GetISteamMatchmakingServers", 25u, 11u, 0, m787, NULL},
    {"SteamClient021::GetISteamMusic", 25u, 27u, 0, m788, NULL},
    {"SteamClient021::GetISteamMusicRemote", 25u, 28u, 0, m789, NULL},
    {"SteamClient021::GetISteamNetworking", 25u, 16u, 0, m790, NULL},
    {"SteamClient021::GetISteamParentalSettings", 25u, 35u, 0, m791, NULL},
    {"SteamClient021::GetISteamParties", 25u, 37u, 0, m792, NULL},
    {"SteamClient021::GetISteamRemotePlay", 25u, 38u, 0, m793, NULL},
    {"SteamClient021::GetISteamRemoteStorage", 25u, 17u, 0, m794, NULL},
    {"SteamClient021::GetISteamScreenshots", 25u, 18u, 0, m795, NULL},
    {"SteamClient021::GetISteamUGC", 25u, 26u, 0, m796, NULL},
    {"SteamClient021::GetISteamUser", 25u, 5u, 0, m797, NULL},
    {"SteamClient021::GetISteamUserStats", 25u, 13u, 0, m798, NULL},
    {"SteamClient021::GetISteamUtils", 25u, 9u, 0, m799, NULL},
    {"SteamClient021::GetISteamVideo", 25u, 34u, 0, m800, NULL},
    {"SteamClient021::ReleaseUser", 25u, 4u, 0, m801, NULL},
    {"SteamClient021::SetLocalIPBinding", 25u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient021::SetWarningMessageHook", 25u, 22u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamController005::ActivateActionSet", 44u, 6u, 0, m804, NULL},
    {"SteamController005::GetActionSetHandle", 44u, 5u, 0, m805, NULL},
    {"SteamController005::GetAnalogActionData", 44u, 12u, 0, NULL, "signature C# (5 mots de pile) != libsteam_api.so (6)"},
    {"SteamController005::GetAnalogActionHandle", 44u, 11u, 0, m807, NULL},
    {"SteamController005::GetAnalogActionOrigins", 44u, 13u, 0, m808, NULL},
    {"SteamController005::GetConnectedControllers", 44u, 3u, 0, m809, NULL},
    {"SteamController005::GetControllerForGamepadIndex", 44u, 20u, 0, m810, NULL},
    {"SteamController005::GetCurrentActionSet", 44u, 7u, 0, m811, NULL},
    {"SteamController005::GetDigitalActionData", 44u, 9u, 0, NULL, "signature C# (5 mots de pile) != libsteam_api.so (6)"},
    {"SteamController005::GetDigitalActionHandle", 44u, 8u, 0, m813, NULL},
    {"SteamController005::GetDigitalActionOrigins", 44u, 10u, 0, m814, NULL},
    {"SteamController005::GetGamepadIndexForController", 44u, 19u, 0, m815, NULL},
    {"SteamController005::GetGlyphForActionOrigin", 44u, 25u, 0, m816, NULL},
    {"SteamController005::GetMotionData", 44u, 21u, 0, NULL, "signature C# (3 mots de pile) != libsteam_api.so (4)"},
    {"SteamController005::GetStringForActionOrigin", 44u, 24u, 0, m818, NULL},
    {"SteamController005::Init", 44u, 0u, 0, m819, NULL},
    {"SteamController005::RunFrame", 44u, 2u, 0, m820, NULL},
    {"SteamController005::SetLEDColor", 44u, 18u, 0, m821, NULL},
    {"SteamController005::ShowAnalogActionOrigins", 44u, 23u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamController005::ShowBindingPanel", 44u, 4u, 0, m823, NULL},
    {"SteamController005::ShowDigitalActionOrigins", 44u, 22u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamController005::Shutdown", 44u, 1u, 0, m825, NULL},
    {"SteamController005::StopAnalogActionMomentum", 44u, 14u, 0, m826, NULL},
    {"SteamController005::TriggerHapticPulse", 44u, 15u, 0, m827, NULL},
    {"SteamController005::TriggerRepeatedHapticPulse", 44u, 16u, 0, m828, NULL},
    {"SteamController005::TriggerVibration", 44u, 17u, 0, m829, NULL},
    {"SteamController008::ActivateActionSet", 13u, 5u, 0, m830, NULL},
    {"SteamController008::ActivateActionSetLayer", 13u, 7u, 0, m831, NULL},
    {"SteamController008::DeactivateActionSetLayer", 13u, 8u, 0, m832, NULL},
    {"SteamController008::DeactivateAllActionSetLayers", 13u, 9u, 0, m833, NULL},
    {"SteamController008::GetActionOriginFromXboxOrigin", 13u, 31u, 0, m834, NULL},
    {"SteamController008::GetActionSetHandle", 13u, 4u, 0, m835, NULL},
    {"SteamController008::GetActiveActionSetLayers", 13u, 10u, 0, m836, NULL},
    {"SteamController008::GetAnalogActionHandle", 13u, 14u, 0, m837, NULL},
    {"SteamController008::GetAnalogActionOrigins", 13u, 16u, 0, m838, NULL},
    {"SteamController008::GetConnectedControllers", 13u, 3u, 0, m839, NULL},
    {"SteamController008::GetControllerBindingRevision", 13u, 33u, 0, m840, NULL},
    {"SteamController008::GetControllerForGamepadIndex", 13u, 27u, 0, m841, NULL},
    {"SteamController008::GetCurrentActionSet", 13u, 6u, 0, m842, NULL},
    {"SteamController008::GetDigitalActionHandle", 13u, 11u, 0, m843, NULL},
    {"SteamController008::GetDigitalActionOrigins", 13u, 13u, 0, m844, NULL},
    {"SteamController008::GetGamepadIndexForController", 13u, 28u, 0, m845, NULL},
    {"SteamController008::GetGlyphForActionOrigin", 13u, 17u, 0, m846, NULL},
    {"SteamController008::GetGlyphForXboxOrigin", 13u, 30u, 0, m847, NULL},
    {"SteamController008::GetInputTypeForHandle", 13u, 26u, 0, m848, NULL},
    {"SteamController008::GetStringForActionOrigin", 13u, 18u, 0, m849, NULL},
    {"SteamController008::GetStringForXboxOrigin", 13u, 29u, 0, m850, NULL},
    {"SteamController008::Init", 13u, 0u, 0, m851, NULL},
    {"SteamController008::RunFrame", 13u, 2u, 0, m852, NULL},
    {"SteamController008::SetLEDColor", 13u, 24u, 0, m853, NULL},
    {"SteamController008::ShowBindingPanel", 13u, 25u, 0, m854, NULL},
    {"SteamController008::Shutdown", 13u, 1u, 0, m855, NULL},
    {"SteamController008::StopAnalogActionMomentum", 13u, 19u, 0, m856, NULL},
    {"SteamController008::TranslateActionOrigin", 13u, 32u, 0, m857, NULL},
    {"SteamController008::TriggerHapticPulse", 13u, 21u, 0, m858, NULL},
    {"SteamController008::TriggerRepeatedHapticPulse", 13u, 22u, 0, m859, NULL},
    {"SteamController008::TriggerVibration", 13u, 23u, 0, m860, NULL},
    {"SteamFriends015::ActivateGameOverlay", 38u, 28u, 0, m861, NULL},
    {"SteamFriends015::ActivateGameOverlayInviteDialog", 38u, 33u, 0, m862, NULL},
    {"SteamFriends015::ActivateGameOverlayToStore", 38u, 31u, 0, m863, NULL},
    {"SteamFriends015::ActivateGameOverlayToUser", 38u, 29u, 0, m864, NULL},
    {"SteamFriends015::ActivateGameOverlayToWebPage", 38u, 30u, 0, m865, NULL},
    {"SteamFriends015::ClearRichPresence", 38u, 44u, 0, m866, NULL},
    {"SteamFriends015::CloseClanChatWindowInSteam", 38u, 63u, 0, m867, NULL},
    {"SteamFriends015::DownloadClanActivityCounts", 38u, 23u, 0, m868, NULL},
    {"SteamFriends015::EnumerateFollowingList", 38u, 69u, 0, m869, NULL},
    {"SteamFriends015::GetChatMemberByIndex", 38u, 57u, 1, m870, NULL},
    {"SteamFriends015::GetClanActivityCounts", 38u, 22u, 0, m871, NULL},
    {"SteamFriends015::GetClanByIndex", 38u, 19u, 1, m872, NULL},
    {"SteamFriends015::GetClanChatMemberCount", 38u, 56u, 0, m873, NULL},
    {"SteamFriends015::GetClanChatMessage", 38u, 59u, 0, m874, NULL},
    {"SteamFriends015::GetClanCount", 38u, 18u, 0, m875, NULL},
    {"SteamFriends015::GetClanName", 38u, 20u, 0, m876, NULL},
    {"SteamFriends015::GetClanOfficerByIndex", 38u, 41u, 1, m877, NULL},
    {"SteamFriends015::GetClanOfficerCount", 38u, 40u, 0, m878, NULL},
    {"SteamFriends015::GetClanOwner", 38u, 39u, 1, m879, NULL},
    {"SteamFriends015::GetClanTag", 38u, 21u, 0, m880, NULL},
    {"SteamFriends015::GetCoplayFriend", 38u, 51u, 1, m881, NULL},
    {"SteamFriends015::GetCoplayFriendCount", 38u, 50u, 0, m882, NULL},
    {"SteamFriends015::GetFollowerCount", 38u, 67u, 0, m883, NULL},
    {"SteamFriends015::GetFriendByIndex", 38u, 4u, 1, m884, NULL},
    {"SteamFriends015::GetFriendCoplayGame", 38u, 53u, 0, m885, NULL},
    {"SteamFriends015::GetFriendCoplayTime", 38u, 52u, 0, m886, NULL},
    {"SteamFriends015::GetFriendCount", 38u, 3u, 0, m887, NULL},
    {"SteamFriends015::GetFriendCountFromSource", 38u, 24u, 0, m888, NULL},
    {"SteamFriends015::GetFriendFromSourceByIndex", 38u, 25u, 1, m889, NULL},
    {"SteamFriends015::GetFriendGamePlayed", 38u, 8u, 0, m890, NULL},
    {"SteamFriends015::GetFriendMessage", 38u, 66u, 0, m891, NULL},
    {"SteamFriends015::GetFriendPersonaName", 38u, 7u, 0, m892, NULL},
    {"SteamFriends015::GetFriendPersonaNameHistory", 38u, 9u, 0, m893, NULL},
    {"SteamFriends015::GetFriendPersonaState", 38u, 6u, 0, m894, NULL},
    {"SteamFriends015::GetFriendRelationship", 38u, 5u, 0, m895, NULL},
    {"SteamFriends015::GetFriendRichPresence", 38u, 45u, 0, m896, NULL},
    {"SteamFriends015::GetFriendRichPresenceKeyByIndex", 38u, 47u, 0, m897, NULL},
    {"SteamFriends015::GetFriendRichPresenceKeyCount", 38u, 46u, 0, m898, NULL},
    {"SteamFriends015::GetFriendSteamLevel", 38u, 10u, 0, m899, NULL},
    {"SteamFriends015::GetFriendsGroupCount", 38u, 12u, 0, m900, NULL},
    {"SteamFriends015::GetFriendsGroupIDByIndex", 38u, 13u, 0, m901, NULL},
    {"SteamFriends015::GetFriendsGroupMembersCount", 38u, 15u, 0, m902, NULL},
    {"SteamFriends015::GetFriendsGroupMembersList", 38u, 16u, 0, m903, NULL},
    {"SteamFriends015::GetFriendsGroupName", 38u, 14u, 0, m904, NULL},
    {"SteamFriends015::GetLargeFriendAvatar", 38u, 36u, 0, m905, NULL},
    {"SteamFriends015::GetMediumFriendAvatar", 38u, 35u, 0, m906, NULL},
    {"SteamFriends015::GetPersonaName", 38u, 0u, 0, m907, NULL},
    {"SteamFriends015::GetPersonaState", 38u, 2u, 0, m908, NULL},
    {"SteamFriends015::GetPlayerNickname", 38u, 11u, 0, m909, NULL},
    {"SteamFriends015::GetSmallFriendAvatar", 38u, 34u, 0, m910, NULL},
    {"SteamFriends015::GetUserRestrictions", 38u, 42u, 0, m911, NULL},
    {"SteamFriends015::HasFriend", 38u, 17u, 0, m912, NULL},
    {"SteamFriends015::InviteUserToGame", 38u, 49u, 0, m913, NULL},
    {"SteamFriends015::IsClanChatAdmin", 38u, 60u, 0, m914, NULL},
    {"SteamFriends015::IsClanChatWindowOpenInSteam", 38u, 61u, 0, m915, NULL},
    {"SteamFriends015::IsFollowing", 38u, 68u, 0, m916, NULL},
    {"SteamFriends015::IsUserInSource", 38u, 26u, 0, m917, NULL},
    {"SteamFriends015::JoinClanChatRoom", 38u, 54u, 0, m918, NULL},
    {"SteamFriends015::LeaveClanChatRoom", 38u, 55u, 0, m919, NULL},
    {"SteamFriends015::OpenClanChatWindowInSteam", 38u, 62u, 0, m920, NULL},
    {"SteamFriends015::ReplyToFriendMessage", 38u, 65u, 0, m921, NULL},
    {"SteamFriends015::RequestClanOfficerList", 38u, 38u, 0, m922, NULL},
    {"SteamFriends015::RequestFriendRichPresence", 38u, 48u, 0, m923, NULL},
    {"SteamFriends015::RequestUserInformation", 38u, 37u, 0, m924, NULL},
    {"SteamFriends015::SendClanChatMessage", 38u, 58u, 0, m925, NULL},
    {"SteamFriends015::SetInGameVoiceSpeaking", 38u, 27u, 0, m926, NULL},
    {"SteamFriends015::SetListenForFriendsMessages", 38u, 64u, 0, m927, NULL},
    {"SteamFriends015::SetPersonaName", 38u, 1u, 0, m928, NULL},
    {"SteamFriends015::SetPlayedWith", 38u, 32u, 0, m929, NULL},
    {"SteamFriends015::SetRichPresence", 38u, 43u, 0, m930, NULL},
    {"SteamFriends017::ActivateGameOverlay", 2u, 28u, 0, m931, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialog", 2u, 33u, 0, m932, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialogConnectString", 2u, 75u, 0, m933, NULL},
    {"SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog", 2u, 73u, 0, m934, NULL},
    {"SteamFriends017::ActivateGameOverlayToStore", 2u, 31u, 0, m935, NULL},
    {"SteamFriends017::ActivateGameOverlayToUser", 2u, 29u, 0, m936, NULL},
    {"SteamFriends017::ActivateGameOverlayToWebPage", 2u, 30u, 0, m937, NULL},
    {"SteamFriends017::ClearRichPresence", 2u, 44u, 0, m938, NULL},
    {"SteamFriends017::CloseClanChatWindowInSteam", 2u, 63u, 0, m939, NULL},
    {"SteamFriends017::DownloadClanActivityCounts", 2u, 23u, 0, m940, NULL},
    {"SteamFriends017::EnumerateFollowingList", 2u, 69u, 0, m941, NULL},
    {"SteamFriends017::GetChatMemberByIndex", 2u, 57u, 1, m942, NULL},
    {"SteamFriends017::GetClanActivityCounts", 2u, 22u, 0, m943, NULL},
    {"SteamFriends017::GetClanByIndex", 2u, 19u, 1, m944, NULL},
    {"SteamFriends017::GetClanChatMemberCount", 2u, 56u, 0, m945, NULL},
    {"SteamFriends017::GetClanChatMessage", 2u, 59u, 0, m946, NULL},
    {"SteamFriends017::GetClanCount", 2u, 18u, 0, m947, NULL},
    {"SteamFriends017::GetClanName", 2u, 20u, 0, m948, NULL},
    {"SteamFriends017::GetClanOfficerByIndex", 2u, 41u, 1, m949, NULL},
    {"SteamFriends017::GetClanOfficerCount", 2u, 40u, 0, m950, NULL},
    {"SteamFriends017::GetClanOwner", 2u, 39u, 1, m951, NULL},
    {"SteamFriends017::GetClanTag", 2u, 21u, 0, m952, NULL},
    {"SteamFriends017::GetCoplayFriend", 2u, 51u, 1, m953, NULL},
    {"SteamFriends017::GetCoplayFriendCount", 2u, 50u, 0, m954, NULL},
    {"SteamFriends017::GetFollowerCount", 2u, 67u, 0, m955, NULL},
    {"SteamFriends017::GetFriendByIndex", 2u, 4u, 1, m956, NULL},
    {"SteamFriends017::GetFriendCoplayGame", 2u, 53u, 0, m957, NULL},
    {"SteamFriends017::GetFriendCoplayTime", 2u, 52u, 0, m958, NULL},
    {"SteamFriends017::GetFriendCount", 2u, 3u, 0, m959, NULL},
    {"SteamFriends017::GetFriendCountFromSource", 2u, 24u, 0, m960, NULL},
    {"SteamFriends017::GetFriendFromSourceByIndex", 2u, 25u, 1, m961, NULL},
    {"SteamFriends017::GetFriendGamePlayed", 2u, 8u, 0, m962, NULL},
    {"SteamFriends017::GetFriendMessage", 2u, 66u, 0, m963, NULL},
    {"SteamFriends017::GetFriendPersonaName", 2u, 7u, 0, m964, NULL},
    {"SteamFriends017::GetFriendPersonaNameHistory", 2u, 9u, 0, m965, NULL},
    {"SteamFriends017::GetFriendPersonaState", 2u, 6u, 0, m966, NULL},
    {"SteamFriends017::GetFriendRelationship", 2u, 5u, 0, m967, NULL},
    {"SteamFriends017::GetFriendRichPresence", 2u, 45u, 0, m968, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyByIndex", 2u, 47u, 0, m969, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyCount", 2u, 46u, 0, m970, NULL},
    {"SteamFriends017::GetFriendSteamLevel", 2u, 10u, 0, m971, NULL},
    {"SteamFriends017::GetFriendsGroupCount", 2u, 12u, 0, m972, NULL},
    {"SteamFriends017::GetFriendsGroupIDByIndex", 2u, 13u, 0, m973, NULL},
    {"SteamFriends017::GetFriendsGroupMembersCount", 2u, 15u, 0, m974, NULL},
    {"SteamFriends017::GetFriendsGroupMembersList", 2u, 16u, 0, m975, NULL},
    {"SteamFriends017::GetFriendsGroupName", 2u, 14u, 0, m976, NULL},
    {"SteamFriends017::GetLargeFriendAvatar", 2u, 36u, 0, m977, NULL},
    {"SteamFriends017::GetMediumFriendAvatar", 2u, 35u, 0, m978, NULL},
    {"SteamFriends017::GetNumChatsWithUnreadPriorityMessages", 2u, 72u, 0, m979, NULL},
    {"SteamFriends017::GetPersonaName", 2u, 0u, 0, m980, NULL},
    {"SteamFriends017::GetPersonaState", 2u, 2u, 0, m981, NULL},
    {"SteamFriends017::GetPlayerNickname", 2u, 11u, 0, m982, NULL},
    {"SteamFriends017::GetSmallFriendAvatar", 2u, 34u, 0, m983, NULL},
    {"SteamFriends017::GetUserRestrictions", 2u, 42u, 0, m984, NULL},
    {"SteamFriends017::HasFriend", 2u, 17u, 0, m985, NULL},
    {"SteamFriends017::InviteUserToGame", 2u, 49u, 0, m986, NULL},
    {"SteamFriends017::IsClanChatAdmin", 2u, 60u, 0, m987, NULL},
    {"SteamFriends017::IsClanChatWindowOpenInSteam", 2u, 61u, 0, m988, NULL},
    {"SteamFriends017::IsClanOfficialGameGroup", 2u, 71u, 0, m989, NULL},
    {"SteamFriends017::IsClanPublic", 2u, 70u, 0, m990, NULL},
    {"SteamFriends017::IsFollowing", 2u, 68u, 0, m991, NULL},
    {"SteamFriends017::IsUserInSource", 2u, 26u, 0, m992, NULL},
    {"SteamFriends017::JoinClanChatRoom", 2u, 54u, 0, m993, NULL},
    {"SteamFriends017::LeaveClanChatRoom", 2u, 55u, 0, m994, NULL},
    {"SteamFriends017::OpenClanChatWindowInSteam", 2u, 62u, 0, m995, NULL},
    {"SteamFriends017::RegisterProtocolInOverlayBrowser", 2u, 74u, 0, m996, NULL},
    {"SteamFriends017::ReplyToFriendMessage", 2u, 65u, 0, m997, NULL},
    {"SteamFriends017::RequestClanOfficerList", 2u, 38u, 0, m998, NULL},
    {"SteamFriends017::RequestFriendRichPresence", 2u, 48u, 0, m999, NULL},
    {"SteamFriends017::RequestUserInformation", 2u, 37u, 0, m1000, NULL},
    {"SteamFriends017::SendClanChatMessage", 2u, 58u, 0, m1001, NULL},
    {"SteamFriends017::SetInGameVoiceSpeaking", 2u, 27u, 0, m1002, NULL},
    {"SteamFriends017::SetListenForFriendsMessages", 2u, 64u, 0, m1003, NULL},
    {"SteamFriends017::SetPersonaName", 2u, 1u, 0, m1004, NULL},
    {"SteamFriends017::SetPlayedWith", 2u, 32u, 0, m1005, NULL},
    {"SteamFriends017::SetRichPresence", 2u, 43u, 0, m1006, NULL},
    {"SteamGameServer012::AssociateWithClan", 45u, 42u, 0, m1007, NULL},
    {"SteamGameServer012::BLoggedOn", 45u, 8u, 0, m1008, NULL},
    {"SteamGameServer012::BSecure", 45u, 9u, 0, m1009, NULL},
    {"SteamGameServer012::BUpdateUserData", 45u, 27u, 0, m1010, NULL},
    {"SteamGameServer012::BeginAuthSession", 45u, 29u, 0, m1011, NULL},
    {"SteamGameServer012::CancelAuthTicket", 45u, 31u, 0, m1012, NULL},
    {"SteamGameServer012::ClearAllKeyValues", 45u, 19u, 0, m1013, NULL},
    {"SteamGameServer012::ComputeNewPlayerCompatibility", 45u, 43u, 0, m1014, NULL},
    {"SteamGameServer012::CreateUnauthenticatedUserConnection", 45u, 25u, 1, m1015, NULL},
    {"SteamGameServer012::EnableHeartbeats", 45u, 39u, 0, m1016, NULL},
    {"SteamGameServer012::EndAuthSession", 45u, 30u, 0, m1017, NULL},
    {"SteamGameServer012::ForceHeartbeat", 45u, 41u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamGameServer012::GetAuthSessionTicket", 45u, 28u, 0, m1019, NULL},
    {"SteamGameServer012::GetGameplayStats", 45u, 34u, 0, m1020, NULL},
    {"SteamGameServer012::GetNextOutgoingPacket", 45u, 38u, 0, m1021, NULL},
    {"SteamGameServer012::GetPublicIP", 45u, 36u, 0, NULL, "retour SteamIPAddress_t (SteamIPAddress_t)"},
    {"SteamGameServer012::GetServerReputation", 45u, 35u, 0, m1023, NULL},
    {"SteamGameServer012::GetSteamID", 45u, 10u, 1, m1024, NULL},
    {"SteamGameServer012::HandleIncomingPacket", 45u, 37u, 0, m1025, NULL},
    {"SteamGameServer012::InitGameServer", 45u, 0u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamGameServer012::LogOff", 45u, 7u, 0, m1027, NULL},
    {"SteamGameServer012::LogOn", 45u, 5u, 0, m1028, NULL},
    {"SteamGameServer012::LogOnAnonymous", 45u, 6u, 0, m1029, NULL},
    {"SteamGameServer012::RequestUserGroupStatus", 45u, 33u, 0, m1030, NULL},
    {"SteamGameServer012::SendUserConnectAndAuthenticate", 45u, 24u, 0, m1031, NULL},
    {"SteamGameServer012::SendUserDisconnect", 45u, 26u, 0, m1032, NULL},
    {"SteamGameServer012::SetBotPlayerCount", 45u, 13u, 0, m1033, NULL},
    {"SteamGameServer012::SetDedicatedServer", 45u, 4u, 0, m1034, NULL},
    {"SteamGameServer012::SetGameData", 45u, 22u, 0, m1035, NULL},
    {"SteamGameServer012::SetGameDescription", 45u, 2u, 0, m1036, NULL},
    {"SteamGameServer012::SetGameTags", 45u, 21u, 0, m1037, NULL},
    {"SteamGameServer012::SetHeartbeatInterval", 45u, 40u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamGameServer012::SetKeyValue", 45u, 20u, 0, m1039, NULL},
    {"SteamGameServer012::SetMapName", 45u, 15u, 0, m1040, NULL},
    {"SteamGameServer012::SetMaxPlayerCount", 45u, 12u, 0, m1041, NULL},
    {"SteamGameServer012::SetModDir", 45u, 3u, 0, m1042, NULL},
    {"SteamGameServer012::SetPasswordProtected", 45u, 16u, 0, m1043, NULL},
    {"SteamGameServer012::SetProduct", 45u, 1u, 0, m1044, NULL},
    {"SteamGameServer012::SetRegion", 45u, 23u, 0, m1045, NULL},
    {"SteamGameServer012::SetServerName", 45u, 14u, 0, m1046, NULL},
    {"SteamGameServer012::SetSpectatorPort", 45u, 17u, 0, m1047, NULL},
    {"SteamGameServer012::SetSpectatorServerName", 45u, 18u, 0, m1048, NULL},
    {"SteamGameServer012::UserHasLicenseForApp", 45u, 32u, 0, m1049, NULL},
    {"SteamGameServer012::WasRestartRequested", 45u, 11u, 0, m1050, NULL},
    {"SteamGameServer014::AssociateWithClan", 23u, 36u, 0, m1051, NULL},
    {"SteamGameServer014::BLoggedOn", 23u, 8u, 0, m1052, NULL},
    {"SteamGameServer014::BSecure", 23u, 9u, 0, m1053, NULL},
    {"SteamGameServer014::BUpdateUserData", 23u, 41u, 0, m1054, NULL},
    {"SteamGameServer014::BeginAuthSession", 23u, 26u, 0, m1055, NULL},
    {"SteamGameServer014::CancelAuthTicket", 23u, 28u, 0, m1056, NULL},
    {"SteamGameServer014::ClearAllKeyValues", 23u, 19u, 0, m1057, NULL},
    {"SteamGameServer014::ComputeNewPlayerCompatibility", 23u, 37u, 0, m1058, NULL},
    {"SteamGameServer014::CreateUnauthenticatedUserConnection", 23u, 39u, 1, m1059, NULL},
    {"SteamGameServer014::EndAuthSession", 23u, 27u, 0, m1060, NULL},
    {"SteamGameServer014::GetAuthSessionTicket", 23u, 25u, 0, m1061, NULL},
    {"SteamGameServer014::GetGameplayStats", 23u, 31u, 0, m1062, NULL},
    {"SteamGameServer014::GetNextOutgoingPacket", 23u, 35u, 0, m1063, NULL},
    {"SteamGameServer014::GetServerReputation", 23u, 32u, 0, m1064, NULL},
    {"SteamGameServer014::GetSteamID", 23u, 10u, 1, m1065, NULL},
    {"SteamGameServer014::HandleIncomingPacket", 23u, 34u, 0, m1066, NULL},
    {"SteamGameServer014::LogOff", 23u, 7u, 0, m1067, NULL},
    {"SteamGameServer014::LogOn", 23u, 5u, 0, m1068, NULL},
    {"SteamGameServer014::LogOnAnonymous", 23u, 6u, 0, m1069, NULL},
    {"SteamGameServer014::RequestUserGroupStatus", 23u, 30u, 0, m1070, NULL},
    {"SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED", 23u, 38u, 0, m1071, NULL},
    {"SteamGameServer014::SendUserDisconnect_DEPRECATED", 23u, 40u, 0, m1072, NULL},
    {"SteamGameServer014::SetAdvertiseServerActive", 23u, 24u, 0, m1073, NULL},
    {"SteamGameServer014::SetBotPlayerCount", 23u, 13u, 0, m1074, NULL},
    {"SteamGameServer014::SetDedicatedServer", 23u, 4u, 0, m1075, NULL},
    {"SteamGameServer014::SetGameData", 23u, 22u, 0, m1076, NULL},
    {"SteamGameServer014::SetGameDescription", 23u, 2u, 0, m1077, NULL},
    {"SteamGameServer014::SetGameTags", 23u, 21u, 0, m1078, NULL},
    {"SteamGameServer014::SetKeyValue", 23u, 20u, 0, m1079, NULL},
    {"SteamGameServer014::SetMapName", 23u, 15u, 0, m1080, NULL},
    {"SteamGameServer014::SetMaxPlayerCount", 23u, 12u, 0, m1081, NULL},
    {"SteamGameServer014::SetModDir", 23u, 3u, 0, m1082, NULL},
    {"SteamGameServer014::SetPasswordProtected", 23u, 16u, 0, m1083, NULL},
    {"SteamGameServer014::SetProduct", 23u, 1u, 0, m1084, NULL},
    {"SteamGameServer014::SetRegion", 23u, 23u, 0, m1085, NULL},
    {"SteamGameServer014::SetServerName", 23u, 14u, 0, m1086, NULL},
    {"SteamGameServer014::SetSpectatorPort", 23u, 17u, 0, m1087, NULL},
    {"SteamGameServer014::SetSpectatorServerName", 23u, 18u, 0, m1088, NULL},
    {"SteamGameServer014::UserHasLicenseForApp", 23u, 29u, 0, m1089, NULL},
    {"SteamGameServer014::WasRestartRequested", 23u, 11u, 0, m1090, NULL},
    {"SteamGameServer015::AssociateWithClan", 29u, 36u, 0, m1091, NULL},
    {"SteamGameServer015::BLoggedOn", 29u, 8u, 0, m1092, NULL},
    {"SteamGameServer015::BSecure", 29u, 9u, 0, m1093, NULL},
    {"SteamGameServer015::BUpdateUserData", 29u, 41u, 0, m1094, NULL},
    {"SteamGameServer015::BeginAuthSession", 29u, 26u, 0, m1095, NULL},
    {"SteamGameServer015::CancelAuthTicket", 29u, 28u, 0, m1096, NULL},
    {"SteamGameServer015::ClearAllKeyValues", 29u, 19u, 0, m1097, NULL},
    {"SteamGameServer015::ComputeNewPlayerCompatibility", 29u, 37u, 0, m1098, NULL},
    {"SteamGameServer015::CreateUnauthenticatedUserConnection", 29u, 39u, 1, m1099, NULL},
    {"SteamGameServer015::EndAuthSession", 29u, 27u, 0, m1100, NULL},
    {"SteamGameServer015::GetAuthSessionTicket", 29u, 25u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pSnid"},
    {"SteamGameServer015::GetGameplayStats", 29u, 31u, 0, m1102, NULL},
    {"SteamGameServer015::GetNextOutgoingPacket", 29u, 35u, 0, m1103, NULL},
    {"SteamGameServer015::GetServerReputation", 29u, 32u, 0, m1104, NULL},
    {"SteamGameServer015::GetSteamID", 29u, 10u, 1, m1105, NULL},
    {"SteamGameServer015::HandleIncomingPacket", 29u, 34u, 0, m1106, NULL},
    {"SteamGameServer015::LogOff", 29u, 7u, 0, m1107, NULL},
    {"SteamGameServer015::LogOn", 29u, 5u, 0, m1108, NULL},
    {"SteamGameServer015::LogOnAnonymous", 29u, 6u, 0, m1109, NULL},
    {"SteamGameServer015::RequestUserGroupStatus", 29u, 30u, 0, m1110, NULL},
    {"SteamGameServer015::SendUserConnectAndAuthenticate_DEPRECATED", 29u, 38u, 0, m1111, NULL},
    {"SteamGameServer015::SendUserDisconnect_DEPRECATED", 29u, 40u, 0, m1112, NULL},
    {"SteamGameServer015::SetAdvertiseServerActive", 29u, 24u, 0, m1113, NULL},
    {"SteamGameServer015::SetBotPlayerCount", 29u, 13u, 0, m1114, NULL},
    {"SteamGameServer015::SetDedicatedServer", 29u, 4u, 0, m1115, NULL},
    {"SteamGameServer015::SetGameData", 29u, 22u, 0, m1116, NULL},
    {"SteamGameServer015::SetGameDescription", 29u, 2u, 0, m1117, NULL},
    {"SteamGameServer015::SetGameTags", 29u, 21u, 0, m1118, NULL},
    {"SteamGameServer015::SetKeyValue", 29u, 20u, 0, m1119, NULL},
    {"SteamGameServer015::SetMapName", 29u, 15u, 0, m1120, NULL},
    {"SteamGameServer015::SetMaxPlayerCount", 29u, 12u, 0, m1121, NULL},
    {"SteamGameServer015::SetModDir", 29u, 3u, 0, m1122, NULL},
    {"SteamGameServer015::SetPasswordProtected", 29u, 16u, 0, m1123, NULL},
    {"SteamGameServer015::SetProduct", 29u, 1u, 0, m1124, NULL},
    {"SteamGameServer015::SetRegion", 29u, 23u, 0, m1125, NULL},
    {"SteamGameServer015::SetServerName", 29u, 14u, 0, m1126, NULL},
    {"SteamGameServer015::SetSpectatorPort", 29u, 17u, 0, m1127, NULL},
    {"SteamGameServer015::SetSpectatorServerName", 29u, 18u, 0, m1128, NULL},
    {"SteamGameServer015::UserHasLicenseForApp", 29u, 29u, 0, m1129, NULL},
    {"SteamGameServer015::WasRestartRequested", 29u, 11u, 0, m1130, NULL},
    {"SteamGameServerStats001::ClearUserAchievement", 24u, 8u, 0, m1131, NULL},
    {"SteamGameServerStats001::GetUserAchievement", 24u, 3u, 0, m1132, NULL},
    {"SteamGameServerStats001::GetUserStatFloat", 24u, 2u, 0, m1133, NULL},
    {"SteamGameServerStats001::GetUserStatInt32", 24u, 1u, 0, m1134, NULL},
    {"SteamGameServerStats001::RequestUserStats", 24u, 0u, 0, m1135, NULL},
    {"SteamGameServerStats001::SetUserAchievement", 24u, 7u, 0, m1136, NULL},
    {"SteamGameServerStats001::SetUserStatFloat", 24u, 5u, 0, m1137, NULL},
    {"SteamGameServerStats001::SetUserStatInt32", 24u, 4u, 0, m1138, NULL},
    {"SteamGameServerStats001::StoreUserStats", 24u, 9u, 0, m1139, NULL},
    {"SteamGameServerStats001::UpdateUserAvgRateStat", 24u, 6u, 0, m1140, NULL},
    {"SteamInput006::ActivateActionSet", 22u, 10u, 0, m1141, NULL},
    {"SteamInput006::ActivateActionSetLayer", 22u, 12u, 0, m1142, NULL},
    {"SteamInput006::BNewDataAvailable", 22u, 5u, 0, m1143, NULL},
    {"SteamInput006::BWaitForData", 22u, 4u, 0, m1144, NULL},
    {"SteamInput006::DeactivateActionSetLayer", 22u, 13u, 0, m1145, NULL},
    {"SteamInput006::DeactivateAllActionSetLayers", 22u, 14u, 0, m1146, NULL},
    {"SteamInput006::EnableActionEventCallbacks", 22u, 8u, 0, NULL, "param\u00e8tre SteamInputActionEventCallbackPointer pCallback"},
    {"SteamInput006::EnableDeviceCallbacks", 22u, 7u, 0, m1148, NULL},
    {"SteamInput006::GetActionOriginFromXboxOrigin", 22u, 42u, 0, m1149, NULL},
    {"SteamInput006::GetActionSetHandle", 22u, 9u, 0, m1150, NULL},
    {"SteamInput006::GetActiveActionSetLayers", 22u, 15u, 0, m1151, NULL},
    {"SteamInput006::GetAnalogActionHandle", 22u, 20u, 0, m1152, NULL},
    {"SteamInput006::GetAnalogActionOrigins", 22u, 22u, 0, m1153, NULL},
    {"SteamInput006::GetConnectedControllers", 22u, 6u, 0, m1154, NULL},
    {"SteamInput006::GetControllerForGamepadIndex", 22u, 38u, 0, m1155, NULL},
    {"SteamInput006::GetCurrentActionSet", 22u, 11u, 0, m1156, NULL},
    {"SteamInput006::GetDeviceBindingRevision", 22u, 44u, 0, m1157, NULL},
    {"SteamInput006::GetDigitalActionHandle", 22u, 16u, 0, m1158, NULL},
    {"SteamInput006::GetDigitalActionOrigins", 22u, 18u, 0, m1159, NULL},
    {"SteamInput006::GetGamepadIndexForController", 22u, 39u, 0, m1160, NULL},
    {"SteamInput006::GetGlyphForActionOrigin_Legacy", 22u, 25u, 0, m1161, NULL},
    {"SteamInput006::GetGlyphForXboxOrigin", 22u, 41u, 0, m1162, NULL},
    {"SteamInput006::GetGlyphPNGForActionOrigin", 22u, 23u, 0, m1163, NULL},
    {"SteamInput006::GetGlyphSVGForActionOrigin", 22u, 24u, 0, m1164, NULL},
    {"SteamInput006::GetInputTypeForHandle", 22u, 37u, 0, m1165, NULL},
    {"SteamInput006::GetRemotePlaySessionID", 22u, 45u, 0, m1166, NULL},
    {"SteamInput006::GetSessionInputConfigurationSettings", 22u, 46u, 0, m1167, NULL},
    {"SteamInput006::GetStringForActionOrigin", 22u, 26u, 0, m1168, NULL},
    {"SteamInput006::GetStringForAnalogActionName", 22u, 27u, 0, m1169, NULL},
    {"SteamInput006::GetStringForDigitalActionName", 22u, 19u, 0, m1170, NULL},
    {"SteamInput006::GetStringForXboxOrigin", 22u, 40u, 0, m1171, NULL},
    {"SteamInput006::Init", 22u, 0u, 0, m1172, NULL},
    {"SteamInput006::Legacy_TriggerHapticPulse", 22u, 34u, 0, m1173, NULL},
    {"SteamInput006::Legacy_TriggerRepeatedHapticPulse", 22u, 35u, 0, m1174, NULL},
    {"SteamInput006::RunFrame", 22u, 3u, 0, m1175, NULL},
    {"SteamInput006::SetInputActionManifestFilePath", 22u, 2u, 0, m1176, NULL},
    {"SteamInput006::SetLEDColor", 22u, 33u, 0, m1177, NULL},
    {"SteamInput006::ShowBindingPanel", 22u, 36u, 0, m1178, NULL},
    {"SteamInput006::Shutdown", 22u, 1u, 0, m1179, NULL},
    {"SteamInput006::StopAnalogActionMomentum", 22u, 28u, 0, m1180, NULL},
    {"SteamInput006::TranslateActionOrigin", 22u, 43u, 0, m1181, NULL},
    {"SteamInput006::TriggerSimpleHapticEvent", 22u, 32u, 0, NULL, "param\u00e8tre char nGainDB"},
    {"SteamInput006::TriggerVibration", 22u, 30u, 0, m1183, NULL},
    {"SteamInput006::TriggerVibrationExtended", 22u, 31u, 0, m1184, NULL},
    {"SteamMatchGameSearch001::AcceptGame", 5u, 3u, 0, m1185, NULL},
    {"SteamMatchGameSearch001::AddGameSearchParams", 5u, 0u, 0, m1186, NULL},
    {"SteamMatchGameSearch001::CancelRequestPlayersForGame", 5u, 11u, 0, m1187, NULL},
    {"SteamMatchGameSearch001::DeclineGame", 5u, 4u, 0, m1188, NULL},
    {"SteamMatchGameSearch001::EndGame", 5u, 13u, 0, m1189, NULL},
    {"SteamMatchGameSearch001::EndGameSearch", 5u, 6u, 0, m1190, NULL},
    {"SteamMatchGameSearch001::HostConfirmGameStart", 5u, 10u, 0, m1191, NULL},
    {"SteamMatchGameSearch001::RequestPlayersForGame", 5u, 9u, 0, m1192, NULL},
    {"SteamMatchGameSearch001::RetrieveConnectionDetails", 5u, 5u, 0, m1193, NULL},
    {"SteamMatchGameSearch001::SearchForGameSolo", 5u, 2u, 0, m1194, NULL},
    {"SteamMatchGameSearch001::SearchForGameWithLobby", 5u, 1u, 0, m1195, NULL},
    {"SteamMatchGameSearch001::SetConnectionDetails", 5u, 8u, 0, m1196, NULL},
    {"SteamMatchGameSearch001::SetGameHostParams", 5u, 7u, 0, m1197, NULL},
    {"SteamMatchGameSearch001::SubmitPlayerResult", 5u, 12u, 0, m1198, NULL},
    {"SteamMatchMaking009::AddFavoriteGame", 4u, 2u, 0, m1199, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListCompatibleMembersFilter", 4u, 11u, 0, m1200, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListDistanceFilter", 4u, 9u, 0, m1201, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable", 4u, 8u, 0, m1202, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNearValueFilter", 4u, 7u, 0, m1203, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNumericalFilter", 4u, 6u, 0, m1204, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListResultCountFilter", 4u, 10u, 0, m1205, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListStringFilter", 4u, 5u, 0, m1206, NULL},
    {"SteamMatchMaking009::CreateLobby", 4u, 13u, 0, m1207, NULL},
    {"SteamMatchMaking009::DeleteLobbyData", 4u, 23u, 0, m1208, NULL},
    {"SteamMatchMaking009::GetFavoriteGame", 4u, 1u, 0, m1209, NULL},
    {"SteamMatchMaking009::GetFavoriteGameCount", 4u, 0u, 0, m1210, NULL},
    {"SteamMatchMaking009::GetLobbyByIndex", 4u, 12u, 1, m1211, NULL},
    {"SteamMatchMaking009::GetLobbyChatEntry", 4u, 27u, 0, m1212, NULL},
    {"SteamMatchMaking009::GetLobbyData", 4u, 19u, 0, m1213, NULL},
    {"SteamMatchMaking009::GetLobbyDataByIndex", 4u, 22u, 0, m1214, NULL},
    {"SteamMatchMaking009::GetLobbyDataCount", 4u, 21u, 0, m1215, NULL},
    {"SteamMatchMaking009::GetLobbyGameServer", 4u, 30u, 0, m1216, NULL},
    {"SteamMatchMaking009::GetLobbyMemberByIndex", 4u, 18u, 1, m1217, NULL},
    {"SteamMatchMaking009::GetLobbyMemberData", 4u, 24u, 0, m1218, NULL},
    {"SteamMatchMaking009::GetLobbyMemberLimit", 4u, 32u, 0, m1219, NULL},
    {"SteamMatchMaking009::GetLobbyOwner", 4u, 35u, 1, m1220, NULL},
    {"SteamMatchMaking009::GetNumLobbyMembers", 4u, 17u, 0, m1221, NULL},
    {"SteamMatchMaking009::InviteUserToLobby", 4u, 16u, 0, m1222, NULL},
    {"SteamMatchMaking009::JoinLobby", 4u, 14u, 0, m1223, NULL},
    {"SteamMatchMaking009::LeaveLobby", 4u, 15u, 0, m1224, NULL},
    {"SteamMatchMaking009::RemoveFavoriteGame", 4u, 3u, 0, m1225, NULL},
    {"SteamMatchMaking009::RequestLobbyData", 4u, 28u, 0, m1226, NULL},
    {"SteamMatchMaking009::RequestLobbyList", 4u, 4u, 0, m1227, NULL},
    {"SteamMatchMaking009::SendLobbyChatMsg", 4u, 26u, 0, m1228, NULL},
    {"SteamMatchMaking009::SetLinkedLobby", 4u, 37u, 0, m1229, NULL},
    {"SteamMatchMaking009::SetLobbyData", 4u, 20u, 0, m1230, NULL},
    {"SteamMatchMaking009::SetLobbyGameServer", 4u, 29u, 0, m1231, NULL},
    {"SteamMatchMaking009::SetLobbyJoinable", 4u, 34u, 0, m1232, NULL},
    {"SteamMatchMaking009::SetLobbyMemberData", 4u, 25u, 0, m1233, NULL},
    {"SteamMatchMaking009::SetLobbyMemberLimit", 4u, 31u, 0, m1234, NULL},
    {"SteamMatchMaking009::SetLobbyOwner", 4u, 36u, 0, m1235, NULL},
    {"SteamMatchMaking009::SetLobbyType", 4u, 33u, 0, m1236, NULL},
    {"SteamMatchMakingServers002::CancelQuery", 6u, 8u, 0, m1237, NULL},
    {"SteamMatchMakingServers002::CancelServerQuery", 6u, 16u, 0, m1238, NULL},
    {"SteamMatchMakingServers002::GetServerCount", 6u, 11u, 0, m1239, NULL},
    {"SteamMatchMakingServers002::GetServerDetails", 6u, 7u, 0, NULL, "retour IntPtr (gameserveritem_t)"},
    {"SteamMatchMakingServers002::IsRefreshing", 6u, 10u, 0, m1241, NULL},
    {"SteamMatchMakingServers002::PingServer", 6u, 13u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::PlayerDetails", 6u, 14u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::RefreshQuery", 6u, 9u, 0, m1244, NULL},
    {"SteamMatchMakingServers002::RefreshServer", 6u, 12u, 0, m1245, NULL},
    {"SteamMatchMakingServers002::ReleaseRequest", 6u, 6u, 0, m1246, NULL},
    {"SteamMatchMakingServers002::RequestFavoritesServerList", 6u, 3u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestFriendsServerList", 6u, 2u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestHistoryServerList", 6u, 4u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestInternetServerList", 6u, 0u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestLANServerList", 6u, 1u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::RequestSpectatorServerList", 6u, 5u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::ServerRules", 6u, 15u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamNetworking005::AcceptP2PSessionWithUser", 46u, 3u, 0, m1254, NULL},
    {"SteamNetworking005::AllowP2PPacketRelay", 46u, 7u, 0, m1255, NULL},
    {"SteamNetworking005::CloseP2PChannelWithUser", 46u, 5u, 0, m1256, NULL},
    {"SteamNetworking005::CloseP2PSessionWithUser", 46u, 4u, 0, m1257, NULL},
    {"SteamNetworking005::CreateConnectionSocket", 46u, 10u, 0, NULL, "param\u00e8tre SteamIPAddress_t nIP"},
    {"SteamNetworking005::CreateListenSocket", 46u, 8u, 0, NULL, "param\u00e8tre SteamIPAddress_t nIP"},
    {"SteamNetworking005::CreateP2PConnectionSocket", 46u, 9u, 0, m1260, NULL},
    {"SteamNetworking005::DestroyListenSocket", 46u, 12u, 0, m1261, NULL},
    {"SteamNetworking005::DestroySocket", 46u, 11u, 0, m1262, NULL},
    {"SteamNetworking005::GetListenSocketInfo", 46u, 19u, 0, NULL, "param\u00e8tre out SteamIPAddress_t pnIP"},
    {"SteamNetworking005::GetMaxPacketSize", 46u, 21u, 0, m1264, NULL},
    {"SteamNetworking005::GetP2PSessionState", 46u, 6u, 0, NULL, "param\u00e8tre out P2PSessionState_t pConnectionState"},
    {"SteamNetworking005::GetSocketConnectionType", 46u, 20u, 0, m1266, NULL},
    {"SteamNetworking005::GetSocketInfo", 46u, 18u, 0, NULL, "param\u00e8tre out SteamIPAddress_t punIPRemote"},
    {"SteamNetworking005::IsDataAvailable", 46u, 16u, 0, m1268, NULL},
    {"SteamNetworking005::IsDataAvailableOnSocket", 46u, 14u, 0, m1269, NULL},
    {"SteamNetworking005::IsP2PPacketAvailable", 46u, 1u, 0, m1270, NULL},
    {"SteamNetworking005::ReadP2PPacket", 46u, 2u, 0, m1271, NULL},
    {"SteamNetworking005::RetrieveData", 46u, 17u, 0, m1272, NULL},
    {"SteamNetworking005::RetrieveDataFromSocket", 46u, 15u, 0, m1273, NULL},
    {"SteamNetworking005::SendDataOnSocket", 46u, 13u, 0, m1274, NULL},
    {"SteamNetworking005::SendP2PPacket", 46u, 0u, 0, m1275, NULL},
    {"SteamNetworking006::AcceptP2PSessionWithUser", 9u, 3u, 0, m1276, NULL},
    {"SteamNetworking006::AllowP2PPacketRelay", 9u, 7u, 0, m1277, NULL},
    {"SteamNetworking006::CloseP2PChannelWithUser", 9u, 5u, 0, m1278, NULL},
    {"SteamNetworking006::CloseP2PSessionWithUser", 9u, 4u, 0, m1279, NULL},
    {"SteamNetworking006::CreateP2PConnectionSocket", 9u, 9u, 0, m1280, NULL},
    {"SteamNetworking006::DestroyListenSocket", 9u, 12u, 0, m1281, NULL},
    {"SteamNetworking006::DestroySocket", 9u, 11u, 0, m1282, NULL},
    {"SteamNetworking006::GetListenSocketInfo", 9u, 19u, 0, NULL, "param\u00e8tre out SteamIPAddress_t pnIP"},
    {"SteamNetworking006::GetMaxPacketSize", 9u, 21u, 0, m1284, NULL},
    {"SteamNetworking006::GetP2PSessionState", 9u, 6u, 0, NULL, "param\u00e8tre out P2PSessionState_t pConnectionState"},
    {"SteamNetworking006::GetSocketConnectionType", 9u, 20u, 0, m1286, NULL},
    {"SteamNetworking006::GetSocketInfo", 9u, 18u, 0, NULL, "param\u00e8tre out SteamIPAddress_t punIPRemote"},
    {"SteamNetworking006::IsDataAvailable", 9u, 16u, 0, m1288, NULL},
    {"SteamNetworking006::IsDataAvailableOnSocket", 9u, 14u, 0, m1289, NULL},
    {"SteamNetworking006::IsP2PPacketAvailable", 9u, 1u, 0, m1290, NULL},
    {"SteamNetworking006::ReadP2PPacket", 9u, 2u, 0, m1291, NULL},
    {"SteamNetworking006::RetrieveData", 9u, 17u, 0, m1292, NULL},
    {"SteamNetworking006::RetrieveDataFromSocket", 9u, 15u, 0, m1293, NULL},
    {"SteamNetworking006::SendDataOnSocket", 9u, 13u, 0, m1294, NULL},
    {"SteamNetworking006::SendP2PPacket", 9u, 0u, 0, m1295, NULL},
    {"SteamNetworkingMessages002::AcceptSessionWithUser", 34u, 2u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::CloseChannelWithUser", 34u, 4u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::CloseSessionWithUser", 34u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::GetSessionConnectionInfo", 34u, 5u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::ReceiveMessagesOnChannel", 34u, 1u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingMessages002::SendMessageToUser", 34u, 0u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingSockets012::AcceptConnection", 33u, 4u, 0, m1302, NULL},
    {"SteamNetworkingSockets012::BeginAsyncRequestFakeIP", 33u, 42u, 0, m1303, NULL},
    {"SteamNetworkingSockets012::CloseConnection", 33u, 5u, 0, m1304, NULL},
    {"SteamNetworkingSockets012::CloseListenSocket", 33u, 6u, 0, m1305, NULL},
    {"SteamNetworkingSockets012::ConfigureConnectionLanes", 33u, 20u, 0, NULL, "tampon inout int[] pLanePriorities sans taille"},
    {"SteamNetworkingSockets012::ConnectByIPAddress", 33u, 1u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::ConnectP2P", 33u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingSockets012::ConnectP2PCustomSignaling", 33u, 36u, 0, NULL, "param\u00e8tre out ISteamNetworkingConnectionSignaling pSignaling"},
    {"SteamNetworkingSockets012::ConnectToHostedDedicatedServer", 33u, 30u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityTarget"},
    {"SteamNetworkingSockets012::CreateFakeUDPPort", 33u, 46u, 0, NULL, "retour IntPtr (IntPtr)"},
    {"SteamNetworkingSockets012::CreateHostedDedicatedServerListenSocket", 33u, 34u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketIP", 33u, 0u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketP2P", 33u, 2u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketP2PFakeIP", 33u, 44u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreatePollGroup", 33u, 24u, 0, m1316, NULL},
    {"SteamNetworkingSockets012::CreateSocketPair", 33u, 19u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pIdentity1"},
    {"SteamNetworkingSockets012::DestroyPollGroup", 33u, 25u, 0, m1318, NULL},
    {"SteamNetworkingSockets012::FindRelayAuthTicketForServer", 33u, 29u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityGameServer"},
    {"SteamNetworkingSockets012::FlushMessagesOnConnection", 33u, 13u, 0, m1320, NULL},
    {"SteamNetworkingSockets012::GetAuthenticationStatus", 33u, 23u, 0, NULL, "param\u00e8tre out SteamNetAuthenticationStatus_t pDetails"},
    {"SteamNetworkingSockets012::GetCertificateRequest", 33u, 38u, 0, NULL, "tampon IntPtr pBlob sans taille"},
    {"SteamNetworkingSockets012::GetConnectionInfo", 33u, 15u, 0, NULL, "param\u00e8tre out SteamNetConnectionInfo_t pInfo"},
    {"SteamNetworkingSockets012::GetConnectionName", 33u, 10u, 0, m1324, NULL},
    {"SteamNetworkingSockets012::GetConnectionRealTimeStatus", 33u, 16u, 0, NULL, "param\u00e8tre ref SteamNetConnectionRealTimeStatus_t pStatus"},
    {"SteamNetworkingSockets012::GetConnectionUserData", 33u, 8u, 0, m1326, NULL},
    {"SteamNetworkingSockets012::GetDetailedConnectionStatus", 33u, 17u, 0, m1327, NULL},
    {"SteamNetworkingSockets012::GetFakeIP", 33u, 43u, 0, NULL, "param\u00e8tre out SteamNetworkingFakeIPResult_t pInfo"},
    {"SteamNetworkingSockets012::GetGameCoordinatorServerLogin", 33u, 35u, 0, NULL, "tampon IntPtr pLoginInfo sans taille"},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerAddress", 33u, 33u, 0, m1330, NULL},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerPOPID", 33u, 32u, 0, m1331, NULL},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerPort", 33u, 31u, 0, m1332, NULL},
    {"SteamNetworkingSockets012::GetIdentity", 33u, 21u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pIdentity"},
    {"SteamNetworkingSockets012::GetListenSocketAddress", 33u, 18u, 0, m1334, NULL},
    {"SteamNetworkingSockets012::GetRemoteFakeIPForConnection", 33u, 45u, 0, m1335, NULL},
    {"SteamNetworkingSockets012::InitAuthentication", 33u, 22u, 0, m1336, NULL},
    {"SteamNetworkingSockets012::ReceiveMessagesOnConnection", 33u, 14u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingSockets012::ReceiveMessagesOnPollGroup", 33u, 27u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingSockets012::ReceivedP2PCustomSignal", 33u, 37u, 0, NULL, "param\u00e8tre out ISteamNetworkingSignalingRecvContext pContext"},
    {"SteamNetworkingSockets012::ReceivedRelayAuthTicket", 33u, 28u, 0, NULL, "param\u00e8tre out SteamDatagramRelayAuthTicket pOutParsedTicket"},
    {"SteamNetworkingSockets012::ResetIdentity", 33u, 40u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pIdentity"},
    {"SteamNetworkingSockets012::RunCallbacks", 33u, 41u, 0, m1342, NULL},
    {"SteamNetworkingSockets012::SendMessageToConnection", 33u, 11u, 0, m1343, NULL},
    {"SteamNetworkingSockets012::SendMessages", 33u, 12u, 0, NULL, "tableau inout IntPtr[] pMessages"},
    {"SteamNetworkingSockets012::SetCertificate", 33u, 39u, 0, NULL, "param\u00e8tre out SteamNetworkingErrMsg errMsg"},
    {"SteamNetworkingSockets012::SetConnectionName", 33u, 9u, 0, m1346, NULL},
    {"SteamNetworkingSockets012::SetConnectionPollGroup", 33u, 26u, 0, m1347, NULL},
    {"SteamNetworkingSockets012::SetConnectionUserData", 33u, 7u, 0, m1348, NULL},
    {"SteamNetworkingUtils004::AllocateMessage", 32u, 0u, 0, NULL, "retour IntPtr (IntPtr)"},
    {"SteamNetworkingUtils004::CheckPingDataUpToDate", 32u, 7u, 0, m1350, NULL},
    {"SteamNetworkingUtils004::ConvertPingLocationToString", 32u, 5u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t location"},
    {"SteamNetworkingUtils004::EstimatePingTimeBetweenTwoLocations", 32u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t location1"},
    {"SteamNetworkingUtils004::EstimatePingTimeFromLocalHost", 32u, 4u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t remoteLocation"},
    {"SteamNetworkingUtils004::GetConfigValue", 32u, 17u, 0, NULL, "tampon IntPtr scopeObj sans taille"},
    {"SteamNetworkingUtils004::GetConfigValueInfo", 32u, 18u, 0, m1355, NULL},
    {"SteamNetworkingUtils004::GetDirectPingToPOP", 32u, 9u, 0, m1356, NULL},
    {"SteamNetworkingUtils004::GetIPv4FakeIPType", 32u, 14u, 0, m1357, NULL},
    {"SteamNetworkingUtils004::GetLocalPingLocation", 32u, 2u, 0, NULL, "param\u00e8tre out SteamNetworkPingLocation_t result"},
    {"SteamNetworkingUtils004::GetLocalTimestamp", 32u, 12u, 0, m1359, NULL},
    {"SteamNetworkingUtils004::GetPOPCount", 32u, 10u, 0, m1360, NULL},
    {"SteamNetworkingUtils004::GetPOPList", 32u, 11u, 0, m1361, NULL},
    {"SteamNetworkingUtils004::GetPingToDataCenter", 32u, 8u, 0, m1362, NULL},
    {"SteamNetworkingUtils004::GetRealIdentityForFakeIP", 32u, 15u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pOutRealIdentity"},
    {"SteamNetworkingUtils004::GetRelayNetworkStatus", 32u, 1u, 0, NULL, "param\u00e8tre out SteamRelayNetworkStatus_t pDetails"},
    {"SteamNetworkingUtils004::IterateGenericEditableConfigValues", 32u, 19u, 0, m1365, NULL},
    {"SteamNetworkingUtils004::ParsePingLocationString", 32u, 6u, 0, NULL, "param\u00e8tre out SteamNetworkPingLocation_t result"},
    {"SteamNetworkingUtils004::SetConfigValue", 32u, 16u, 0, NULL, "tampon IntPtr scopeObj sans taille"},
    {"SteamNetworkingUtils004::SetDebugOutputFunction", 32u, 13u, 0, NULL, "param\u00e8tre FSteamNetworkingSocketsDebugOutput pfnFunc"},
    {"SteamNetworkingUtils004::SteamNetworkingIPAddr_GetFakeIPType", 32u, 22u, 0, m1369, NULL},
    {"SteamNetworkingUtils004::SteamNetworkingIPAddr_ParseString", 32u, 21u, 0, m1370, NULL},
    {"SteamNetworkingUtils004::SteamNetworkingIdentity_ParseString", 32u, 24u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pIdentity"},
    {"SteamParties002::CancelReservation", 30u, 8u, 0, m1372, NULL},
    {"SteamParties002::ChangeNumOpenSlots", 30u, 9u, 0, m1373, NULL},
    {"SteamParties002::CreateBeacon", 30u, 6u, 0, NULL, "param\u00e8tre ref SteamPartyBeaconLocation_t pBeaconLocation"},
    {"SteamParties002::DestroyBeacon", 30u, 10u, 0, m1375, NULL},
    {"SteamParties002::GetAvailableBeaconLocations", 30u, 5u, 0, NULL, "tableau inout SteamPartyBeaconLocation_t[] pLocationList"},
    {"SteamParties002::GetBeaconByIndex", 30u, 1u, 0, m1377, NULL},
    {"SteamParties002::GetBeaconDetails", 30u, 2u, 0, NULL, "param\u00e8tre out SteamPartyBeaconLocation_t pLocation"},
    {"SteamParties002::GetNumActiveBeacons", 30u, 0u, 0, m1379, NULL},
    {"SteamParties002::GetNumAvailableBeaconLocations", 30u, 4u, 0, m1380, NULL},
    {"SteamParties002::JoinParty", 30u, 3u, 0, m1381, NULL},
    {"SteamParties002::OnReservationCompleted", 30u, 7u, 0, m1382, NULL},
    {"SteamUser019::AdvertiseGame", 37u, 19u, 0, m1383, NULL},
    {"SteamUser019::BIsBehindNAT", 37u, 18u, 0, m1384, NULL},
    {"SteamUser019::BIsPhoneIdentifying", 37u, 27u, 0, m1385, NULL},
    {"SteamUser019::BIsPhoneRequiringVerification", 37u, 28u, 0, m1386, NULL},
    {"SteamUser019::BIsPhoneVerified", 37u, 25u, 0, m1387, NULL},
    {"SteamUser019::BIsTwoFactorEnabled", 37u, 26u, 0, m1388, NULL},
    {"SteamUser019::BLoggedOn", 37u, 1u, 0, m1389, NULL},
    {"SteamUser019::BeginAuthSession", 37u, 14u, 0, m1390, NULL},
    {"SteamUser019::CancelAuthTicket", 37u, 16u, 0, m1391, NULL},
    {"SteamUser019::DecompressVoice", 37u, 11u, 0, m1392, NULL},
    {"SteamUser019::EndAuthSession", 37u, 15u, 0, m1393, NULL},
    {"SteamUser019::GetAuthSessionTicket", 37u, 13u, 0, m1394, NULL},
    {"SteamUser019::GetAvailableVoice", 37u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser019::GetEncryptedAppTicket", 37u, 21u, 0, m1396, NULL},
    {"SteamUser019::GetGameBadgeLevel", 37u, 22u, 0, m1397, NULL},
    {"SteamUser019::GetHSteamUser", 37u, 0u, 0, m1398, NULL},
    {"SteamUser019::GetPlayerSteamLevel", 37u, 23u, 0, m1399, NULL},
    {"SteamUser019::GetSteamID", 37u, 2u, 1, m1400, NULL},
    {"SteamUser019::GetUserDataFolder", 37u, 6u, 0, m1401, NULL},
    {"SteamUser019::GetVoice", 37u, 10u, 0, NULL, "tampon IntPtr nUncompressBytesWritten_Deprecated sans taille"},
    {"SteamUser019::GetVoiceOptimalSampleRate", 37u, 12u, 0, m1403, NULL},
    {"SteamUser019::InitiateGameConnection", 37u, 3u, 0, m1404, NULL},
    {"SteamUser019::RequestEncryptedAppTicket", 37u, 20u, 0, m1405, NULL},
    {"SteamUser019::RequestStoreAuthURL", 37u, 24u, 0, m1406, NULL},
    {"SteamUser019::StartVoiceRecording", 37u, 7u, 0, m1407, NULL},
    {"SteamUser019::StopVoiceRecording", 37u, 8u, 0, m1408, NULL},
    {"SteamUser019::TerminateGameConnection", 37u, 4u, 0, m1409, NULL},
    {"SteamUser019::TrackAppUsageEvent", 37u, 5u, 0, m1410, NULL},
    {"SteamUser019::UserHasLicenseForApp", 37u, 17u, 0, m1411, NULL},
    {"SteamUser021::AdvertiseGame", 1u, 19u, 0, m1412, NULL},
    {"SteamUser021::BIsBehindNAT", 1u, 18u, 0, m1413, NULL},
    {"SteamUser021::BIsPhoneIdentifying", 1u, 27u, 0, m1414, NULL},
    {"SteamUser021::BIsPhoneRequiringVerification", 1u, 28u, 0, m1415, NULL},
    {"SteamUser021::BIsPhoneVerified", 1u, 25u, 0, m1416, NULL},
    {"SteamUser021::BIsTwoFactorEnabled", 1u, 26u, 0, m1417, NULL},
    {"SteamUser021::BLoggedOn", 1u, 1u, 0, m1418, NULL},
    {"SteamUser021::BSetDurationControlOnlineState", 1u, 31u, 0, m1419, NULL},
    {"SteamUser021::BeginAuthSession", 1u, 14u, 0, m1420, NULL},
    {"SteamUser021::CancelAuthTicket", 1u, 16u, 0, m1421, NULL},
    {"SteamUser021::DecompressVoice", 1u, 11u, 0, m1422, NULL},
    {"SteamUser021::EndAuthSession", 1u, 15u, 0, m1423, NULL},
    {"SteamUser021::GetAuthSessionTicket", 1u, 13u, 0, m1424, NULL},
    {"SteamUser021::GetAvailableVoice", 1u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser021::GetDurationControl", 1u, 30u, 0, m1426, NULL},
    {"SteamUser021::GetEncryptedAppTicket", 1u, 21u, 0, m1427, NULL},
    {"SteamUser021::GetGameBadgeLevel", 1u, 22u, 0, m1428, NULL},
    {"SteamUser021::GetHSteamUser", 1u, 0u, 0, m1429, NULL},
    {"SteamUser021::GetMarketEligibility", 1u, 29u, 0, m1430, NULL},
    {"SteamUser021::GetPlayerSteamLevel", 1u, 23u, 0, m1431, NULL},
    {"SteamUser021::GetSteamID", 1u, 2u, 1, m1432, NULL},
    {"SteamUser021::GetUserDataFolder", 1u, 6u, 0, m1433, NULL},
    {"SteamUser021::GetVoiceOptimalSampleRate", 1u, 12u, 0, m1434, NULL},
    {"SteamUser021::InitiateGameConnection_DEPRECATED", 1u, 3u, 0, m1435, NULL},
    {"SteamUser021::RequestEncryptedAppTicket", 1u, 20u, 0, m1436, NULL},
    {"SteamUser021::RequestStoreAuthURL", 1u, 24u, 0, m1437, NULL},
    {"SteamUser021::StartVoiceRecording", 1u, 7u, 0, m1438, NULL},
    {"SteamUser021::StopVoiceRecording", 1u, 8u, 0, m1439, NULL},
    {"SteamUser021::TerminateGameConnection_DEPRECATED", 1u, 4u, 0, m1440, NULL},
    {"SteamUser021::UserHasLicenseForApp", 1u, 17u, 0, m1441, NULL},
    {"SteamUser023::AdvertiseGame", 26u, 20u, 0, m1442, NULL},
    {"SteamUser023::BIsBehindNAT", 26u, 19u, 0, m1443, NULL},
    {"SteamUser023::BIsPhoneIdentifying", 26u, 28u, 0, m1444, NULL},
    {"SteamUser023::BIsPhoneRequiringVerification", 26u, 29u, 0, m1445, NULL},
    {"SteamUser023::BIsPhoneVerified", 26u, 26u, 0, m1446, NULL},
    {"SteamUser023::BIsTwoFactorEnabled", 26u, 27u, 0, m1447, NULL},
    {"SteamUser023::BLoggedOn", 26u, 1u, 0, m1448, NULL},
    {"SteamUser023::BSetDurationControlOnlineState", 26u, 32u, 0, m1449, NULL},
    {"SteamUser023::BeginAuthSession", 26u, 15u, 0, m1450, NULL},
    {"SteamUser023::CancelAuthTicket", 26u, 17u, 0, m1451, NULL},
    {"SteamUser023::DecompressVoice", 26u, 11u, 0, m1452, NULL},
    {"SteamUser023::EndAuthSession", 26u, 16u, 0, m1453, NULL},
    {"SteamUser023::GetAuthSessionTicket", 26u, 13u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pSteamNetworkingIdentity"},
    {"SteamUser023::GetAuthTicketForWebApi", 26u, 14u, 0, m1455, NULL},
    {"SteamUser023::GetAvailableVoice", 26u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser023::GetDurationControl", 26u, 31u, 0, m1457, NULL},
    {"SteamUser023::GetEncryptedAppTicket", 26u, 22u, 0, m1458, NULL},
    {"SteamUser023::GetGameBadgeLevel", 26u, 23u, 0, m1459, NULL},
    {"SteamUser023::GetHSteamUser", 26u, 0u, 0, m1460, NULL},
    {"SteamUser023::GetMarketEligibility", 26u, 30u, 0, m1461, NULL},
    {"SteamUser023::GetPlayerSteamLevel", 26u, 24u, 0, m1462, NULL},
    {"SteamUser023::GetSteamID", 26u, 2u, 1, m1463, NULL},
    {"SteamUser023::GetUserDataFolder", 26u, 6u, 0, m1464, NULL},
    {"SteamUser023::GetVoiceOptimalSampleRate", 26u, 12u, 0, m1465, NULL},
    {"SteamUser023::InitiateGameConnection_DEPRECATED", 26u, 3u, 0, m1466, NULL},
    {"SteamUser023::RequestEncryptedAppTicket", 26u, 21u, 0, m1467, NULL},
    {"SteamUser023::RequestStoreAuthURL", 26u, 25u, 0, m1468, NULL},
    {"SteamUser023::StartVoiceRecording", 26u, 7u, 0, m1469, NULL},
    {"SteamUser023::StopVoiceRecording", 26u, 8u, 0, m1470, NULL},
    {"SteamUser023::TerminateGameConnection_DEPRECATED", 26u, 4u, 0, m1471, NULL},
    {"SteamUser023::UserHasLicenseForApp", 26u, 18u, 0, m1472, NULL},
    {"SteamUtils009::BOverlayNeedsPresent", 39u, 18u, 0, m1473, NULL},
    {"SteamUtils009::CheckFileSignature", 39u, 19u, 0, m1474, NULL},
    {"SteamUtils009::GetAPICallFailureReason", 39u, 12u, 0, m1475, NULL},
    {"SteamUtils009::GetAPICallResult", 39u, 13u, 0, m1476, NULL},
    {"SteamUtils009::GetAppID", 39u, 9u, 0, m1477, NULL},
    {"SteamUtils009::GetCSERIPPort", 39u, 7u, 0, NULL, "signature absente de Steamworks.NET"},
    {"SteamUtils009::GetConnectedUniverse", 39u, 2u, 0, m1479, NULL},
    {"SteamUtils009::GetCurrentBatteryPower", 39u, 8u, 0, m1480, NULL},
    {"SteamUtils009::GetEnteredGamepadTextInput", 39u, 22u, 0, m1481, NULL},
    {"SteamUtils009::GetEnteredGamepadTextLength", 39u, 21u, 0, m1482, NULL},
    {"SteamUtils009::GetIPCCallCount", 39u, 15u, 0, m1483, NULL},
    {"SteamUtils009::GetIPCountry", 39u, 4u, 0, m1484, NULL},
    {"SteamUtils009::GetImageRGBA", 39u, 6u, 0, m1485, NULL},
    {"SteamUtils009::GetImageSize", 39u, 5u, 0, m1486, NULL},
    {"SteamUtils009::GetSecondsSinceAppActive", 39u, 0u, 0, m1487, NULL},
    {"SteamUtils009::GetSecondsSinceComputerActive", 39u, 1u, 0, m1488, NULL},
    {"SteamUtils009::GetServerRealTime", 39u, 3u, 0, m1489, NULL},
    {"SteamUtils009::GetSteamUILanguage", 39u, 23u, 0, m1490, NULL},
    {"SteamUtils009::IsAPICallCompleted", 39u, 11u, 0, m1491, NULL},
    {"SteamUtils009::IsOverlayEnabled", 39u, 17u, 0, m1492, NULL},
    {"SteamUtils009::IsSteamInBigPictureMode", 39u, 26u, 0, m1493, NULL},
    {"SteamUtils009::IsSteamRunningInVR", 39u, 24u, 0, m1494, NULL},
    {"SteamUtils009::IsVRHeadsetStreamingEnabled", 39u, 28u, 0, m1495, NULL},
    {"SteamUtils009::SetOverlayNotificationInset", 39u, 25u, 0, m1496, NULL},
    {"SteamUtils009::SetOverlayNotificationPosition", 39u, 10u, 0, m1497, NULL},
    {"SteamUtils009::SetVRHeadsetStreamingEnabled", 39u, 29u, 0, m1498, NULL},
    {"SteamUtils009::SetWarningMessageHook", 39u, 16u, 0, steam_manual_utils_warning_hook, NULL},
    {"SteamUtils009::ShowGamepadTextInput", 39u, 20u, 0, m1500, NULL},
    {"SteamUtils009::StartVRDashboard", 39u, 27u, 0, m1501, NULL},
    {"SteamUtils010::BOverlayNeedsPresent", 3u, 18u, 0, m1502, NULL},
    {"SteamUtils010::CheckFileSignature", 3u, 19u, 0, m1503, NULL},
    {"SteamUtils010::DismissFloatingGamepadTextInput", 3u, 37u, 0, m1504, NULL},
    {"SteamUtils010::FilterText", 3u, 32u, 0, m1505, NULL},
    {"SteamUtils010::GetAPICallFailureReason", 3u, 12u, 0, m1506, NULL},
    {"SteamUtils010::GetAPICallResult", 3u, 13u, 0, m1507, NULL},
    {"SteamUtils010::GetAppID", 3u, 9u, 0, m1508, NULL},
    {"SteamUtils010::GetConnectedUniverse", 3u, 2u, 0, m1509, NULL},
    {"SteamUtils010::GetCurrentBatteryPower", 3u, 8u, 0, m1510, NULL},
    {"SteamUtils010::GetEnteredGamepadTextInput", 3u, 22u, 0, m1511, NULL},
    {"SteamUtils010::GetEnteredGamepadTextLength", 3u, 21u, 0, m1512, NULL},
    {"SteamUtils010::GetIPCCallCount", 3u, 15u, 0, m1513, NULL},
    {"SteamUtils010::GetIPCountry", 3u, 4u, 0, m1514, NULL},
    {"SteamUtils010::GetIPv6ConnectivityState", 3u, 33u, 0, m1515, NULL},
    {"SteamUtils010::GetImageRGBA", 3u, 6u, 0, m1516, NULL},
    {"SteamUtils010::GetImageSize", 3u, 5u, 0, m1517, NULL},
    {"SteamUtils010::GetSecondsSinceAppActive", 3u, 0u, 0, m1518, NULL},
    {"SteamUtils010::GetSecondsSinceComputerActive", 3u, 1u, 0, m1519, NULL},
    {"SteamUtils010::GetServerRealTime", 3u, 3u, 0, m1520, NULL},
    {"SteamUtils010::GetSteamUILanguage", 3u, 23u, 0, m1521, NULL},
    {"SteamUtils010::InitFilterText", 3u, 31u, 0, m1522, NULL},
    {"SteamUtils010::IsAPICallCompleted", 3u, 11u, 0, m1523, NULL},
    {"SteamUtils010::IsOverlayEnabled", 3u, 17u, 0, m1524, NULL},
    {"SteamUtils010::IsSteamChinaLauncher", 3u, 30u, 0, m1525, NULL},
    {"SteamUtils010::IsSteamInBigPictureMode", 3u, 26u, 0, m1526, NULL},
    {"SteamUtils010::IsSteamRunningInVR", 3u, 24u, 0, m1527, NULL},
    {"SteamUtils010::IsSteamRunningOnSteamDeck", 3u, 34u, 0, m1528, NULL},
    {"SteamUtils010::IsVRHeadsetStreamingEnabled", 3u, 28u, 0, m1529, NULL},
    {"SteamUtils010::SetGameLauncherMode", 3u, 36u, 0, m1530, NULL},
    {"SteamUtils010::SetOverlayNotificationInset", 3u, 25u, 0, m1531, NULL},
    {"SteamUtils010::SetOverlayNotificationPosition", 3u, 10u, 0, m1532, NULL},
    {"SteamUtils010::SetVRHeadsetStreamingEnabled", 3u, 29u, 0, m1533, NULL},
    {"SteamUtils010::SetWarningMessageHook", 3u, 16u, 0, steam_manual_utils_warning_hook, NULL},
    {"SteamUtils010::ShowFloatingGamepadTextInput", 3u, 35u, 0, m1535, NULL},
    {"SteamUtils010::ShowGamepadTextInput", 3u, 20u, 0, m1536, NULL},
    {"SteamUtils010::StartVRDashboard", 3u, 27u, 0, m1537, NULL},
};
const size_t steam_method_count = 1538;
