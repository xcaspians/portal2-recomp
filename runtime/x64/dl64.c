// Dynamic loading shims of the x86-64 runtime: dlopen/dlsym/dlclose/dlerror,
// dladdr and dl_iterate_phdr over the lifted modules (loader64.c). A
// lifted module is opened by name (its DT_NEEDED modules first, then its
// constructors); a system library (libc.so.6, libm.so.6...) opens as a
// handle whose symbols are the runtime's shims.
#include "rt64.h"
#include "native_libs64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HANDLE_MAIN UINT64_C(0x1)          // dlopen(NULL)
#define HANDLE_SYSTEM UINT64_C(0x2)        // a library the runtime replaces with shims
#define HANDLE_NATIVE UINT64_C(0x100)      // + index: a native library standing in (native_libs64.h)

static _Thread_local char error_text[256];
static _Thread_local int error_pending;

static void set_error(const char *format, const char *name) {
    snprintf(error_text, sizeof error_text, format, name);
    error_pending = 1;
}

static int is_system_library(const char *name) {
    static const char *const system[] = {"libc.so", "libm.so", "libdl.so", "libpthread.so", "librt.so",
                                         "libgcc_s.so", "libstdc++.so", "ld-linux", "libuuid.so", NULL};
    const char *base = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
    for (int i = 0; system[i]; ++i)
        if (!strncmp(base, system[i], strlen(system[i]))) return 1;
    // Valve's Steam API: with the bridge on (steam64.c), its functions are shims
    // calling the macOS libsteam_api.dylib (P2CE piece D, round 23).
    if (!strncmp(base, "libsteam_api.so", 15) && getenv("P2_STEAM_API_DYLIB")) return 1;
    return 0;
}

static void s_dlopen(CPU64 *c) {
    const char *name = PTR(c, 0);
    if (!name) { RET(c, HANDLE_MAIN); return; }
    RT64Loaded *l = rt64_open(c, name);
    if (l) { RET(c, (uintptr_t)l); return; }
    if (is_system_library(name)) { RET(c, HANDLE_SYSTEM); return; }
    int native = rt64_native_library(name);
    if (native >= 0) { RET(c, HANDLE_NATIVE + (uint64_t)native); return; }
    set_error("%s: cannot open shared object file: not a lifted module", name);
    RET(c, 0);
}

static void s_dlsym(CPU64 *c) {
    uint64_t handle = ARG(c, 0);
    const char *name = PTR(c, 1);
    uint64_t address = 0;
    if (handle >= HANDLE_NATIVE && handle < HANDLE_NATIVE + 16) {
        // A native library: a shim of that name first (variadic functions), else its own symbol.
        address = rt64_find_shim(name) ? rt64_dynamic_import(name) : rt64_native_library_symbol((int)(handle - HANDLE_NATIVE), name);
        if (!address) set_error("undefined symbol: %s", name);
        RET(c, address);
        return;
    }
    if (handle != HANDLE_SYSTEM && handle != HANDLE_MAIN && handle != 0 && handle != UINT64_MAX) {
        // A lifted module (an RT64Loaded, whose first field is its module):
        // its own exports, then the global scope (its dependencies).
        const RT64Module *module = *(const RT64Module *const *)(uintptr_t)handle;
        address = rt64_export(module, name);
    }
    if (!address) address = rt64_lookup_global(name);
    if (!address) address = rt64_dynamic_import(name);
    if (!address) set_error("undefined symbol: %s", name);
    RET(c, address);
}

static void s_dlclose(CPU64 *c) { RET(c, 0); }   // modules stay loaded, as with RTLD_NODELETE

static void s_dlerror(CPU64 *c) {
    if (!error_pending) { RET(c, 0); return; }
    error_pending = 0;
    RET(c, (uintptr_t)error_text);
}

// Dl_info has the same layout on both systems: fname, fbase, sname, saddr.
static void s_dladdr(CPU64 *c) {
    uint64_t base;
    const RT64Module *m = rt64_module_at(ARG(c, 0), &base);
    if (!m || !ARG(c, 1)) { RET(c, 0); return; }
    uint64_t info = ARG(c, 1), address = ARG(c, 0), best = 0;
    const char *best_name = NULL;
    for (size_t i = 0; i < m->export_count; ++i) {
        uint64_t at = base + m->exports[i].offset;
        if (at <= address && at > best) { best = at; best_name = m->exports[i].name; }
    }
    W64(info, (uintptr_t)m->name); W64(info + 8, base);
    W64(info + 16, (uintptr_t)best_name); W64(info + 24, best);
    RET(c, 1);
}

// struct dl_phdr_info (64-bit): addr, name, phdr, phnum (u16), adds, subs, tls_modid, tls_data.
static void s_dl_iterate_phdr(CPU64 *c) {
    uint64_t callback = ARG(c, 0), data = ARG(c, 1);
    uint64_t saved[3] = {c->r[RDI], c->r[RSI], c->r[RDX]};
    int64_t result = 0;
    for (size_t i = 0; i < rt64_loaded_count() && !result; ++i) {
        uint64_t base;
        const RT64Module *m = rt64_loaded_module(i, &base);
        uint8_t info[64] = {0};
        uint64_t phdr = base + R64(base + 0x20);
        uint16_t phnum = (uint16_t)R16(base + 0x38);
        uint64_t loads = rt64_loaded_count();
        memcpy(info, &base, 8);
        uint64_t name = (uintptr_t)m->name;
        memcpy(info + 8, &name, 8);
        memcpy(info + 16, &phdr, 8);
        memcpy(info + 24, &phnum, 2);
        memcpy(info + 32, &loads, 8);
        c->r[RSP] -= 64;                       // the info lives on the guest stack
        uint64_t at = c->r[RSP];
        memcpy((void *)(uintptr_t)at, info, 64);
        c->r[RDI] = at; c->r[RSI] = 64; c->r[RDX] = data;
        c->r[RSP] -= 8;                        // keep the stack 16-byte aligned at the call
        rt64_call(c, callback);
        c->r[RSP] += 72;
        result = (int32_t)c->r[RAX];
    }
    c->r[RDI] = saved[0]; c->r[RSI] = saved[1]; c->r[RDX] = saved[2];
    RET(c, result);
}

static const RT64Shim shims[] = {
    {"dlopen", s_dlopen}, {"dlsym", s_dlsym}, {"dlvsym", s_dlsym}, {"dlclose", s_dlclose},
    {"dlerror", s_dlerror}, {"dladdr", s_dladdr}, {"dl_iterate_phdr", s_dl_iterate_phdr},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
