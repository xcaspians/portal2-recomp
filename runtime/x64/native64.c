// Calls from lifted x86-64 code into native arm64 code (P2CE piece D, round
// 23): Valve's macOS libsteam_api.dylib and the steamclient.dylib it loads.
// Guest and host share one address space (rt64.h), so pointers pass as they
// are; only the calling convention differs. System V x86-64 passes integers in
// rdi, rsi, rdx, rcx, r8, r9 then on the stack, and floating point in
// xmm0-xmm7; Apple arm64 passes integers in x0-x7 and floating point in d0-d7.
// The trampoline loads all of both (the 7th and 8th integers from the guest's
// stack), calls, and hands back x0/x1 and d0/d1 as rax/rdx and xmm0/xmm1.
// Round 27: more than eight integer arguments, and structures returned
// through a hidden pointer (rdi on x86-64, x8 on arm64). Neither can be told
// from the registers alone, and steamclient.dylib is stripped (no method
// names), so a call's signature is registered by its function's address
// (rt64_native_set_sig, or rt64_native_set_vtable_sig for an interface's
// slot). Without one, the integer arguments past the eighth are taken as 8-byte
// values (pointers, 64-bit integers, CSteamID), up to RT64_NATIVE_MAX_STACK.
// Arguments past the registers go on the native stack packed by their natural
// size, as Apple's arm64 ABI does (a 4-byte argument takes 4 bytes there, 8 on
// x86-64). Floating-point arguments past the eighth aren't covered.
#include "rt64.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#if defined(__aarch64__)
// (ints[8], doubles[8], function, out[4], stack, stack bytes (16-aligned), x8):
// out = {x0, x1, d0 bits, d1 bits}. The stack area is copied below sp for the callee.
__attribute__((naked)) static void native_trampoline(const uint64_t *ints, const double *fps, uint64_t fn, uint64_t *out,
                                                     const uint8_t *stack, uint64_t stack_bytes, uint64_t x8) {
    __asm__ volatile(
        "stp x29, x30, [sp, #-32]!\n"
        "mov x29, sp\n"
        "str x3, [sp, #16]\n"
        "mov x16, x2\n"
        "mov x17, x0\n"
        "sub sp, sp, x5\n"
        "mov x9, #0\n"
        "1:\n"
        "cmp x9, x5\n"
        "b.ge 2f\n"
        "ldr x10, [x4, x9]\n"
        "str x10, [sp, x9]\n"
        "add x9, x9, #8\n"
        "b 1b\n"
        "2:\n"
        "mov x8, x6\n"
        "ldp d0, d1, [x1]\n"
        "ldp d2, d3, [x1, #16]\n"
        "ldp d4, d5, [x1, #32]\n"
        "ldp d6, d7, [x1, #48]\n"
        "ldp x0, x1, [x17]\n"
        "ldp x2, x3, [x17, #16]\n"
        "ldp x4, x5, [x17, #32]\n"
        "ldp x6, x7, [x17, #48]\n"
        "blr x16\n"
        "mov sp, x29\n"
        "ldr x9, [sp, #16]\n"
        "stp x0, x1, [x9]\n"
        "stp d0, d1, [x9, #16]\n"
        "ldp x29, x30, [sp], #32\n"
        "ret\n");
}
#endif

// ---- Signatures (round 27) ----
static struct { uint64_t fn; rt64_native_sig sig; } sigs[64];
static int sig_count;
int rt64_native_set_sig(uint64_t fn, const rt64_native_sig *sig) {
    for (int i = 0; i < sig_count; ++i)
        if (sigs[i].fn == fn) { sigs[i].sig = *sig; return 1; }
    if (sig_count >= (int)(sizeof(sigs) / sizeof(*sigs)) || sig->stack_args > RT64_NATIVE_MAX_STACK) return 0;
    sigs[sig_count].fn = fn;
    sigs[sig_count++].sig = *sig;
    return 1;
}
int rt64_native_set_vtable_sig(uint64_t object, int slot, const rt64_native_sig *sig) {
    if (!object || slot < 0) return 0;
    uint64_t vtable = *(const uint64_t *)(uintptr_t)object;
    return vtable && rt64_native_set_sig(((const uint64_t *)(uintptr_t)vtable)[slot], sig);
}
const rt64_native_sig *rt64_native_sig_of(uint64_t fn) {
    for (int i = 0; i < sig_count; ++i)
        if (sigs[i].fn == fn) return &sigs[i].sig;
    return NULL;
}

// The arm64 call for System V arguments: ints[8] for x0-x7, the native stack
// area, x8. Pure arithmetic on the guest's values (tests/x64_checks.c).
size_t rt64_native_layout(const uint64_t gp[6], const uint64_t *guest_stack, const rt64_native_sig *sig, uint64_t ints[8],
                          uint8_t *stack, size_t stack_cap, uint64_t *x8) {
    static const rt64_native_sig plain = {0, RT64_NATIVE_MAX_STACK, {0}};
    if (!sig) sig = &plain;
    // The integer arguments in order: six registers, then the guest's stack slots.
    uint64_t args[6 + 2 + RT64_NATIVE_MAX_STACK];
    int n = 0;
    for (int i = 0; i < 6; ++i) args[n++] = gp[i];
    for (int i = 0; i < 2 + sig->stack_args; ++i) args[n++] = guest_stack[i];
    int first = 0;
    *x8 = 0;
    if (sig->hidden_return) { *x8 = args[0]; first = 1; }   // rdi carries the result's address
    for (int i = 0; i < 8; ++i) ints[i] = args[first + i];
    // Past x7: packed by natural size on the native stack.
    size_t at = 0;
    for (int i = 0; i < sig->stack_args && first + 8 + i < n; ++i) {
        unsigned size = sig->stack_sizes[i] ? sig->stack_sizes[i] : 8;
        at = (at + size - 1) & ~(size_t)(size - 1);
        if (at + size > stack_cap) break;
        uint64_t v = args[first + 8 + i];
        memcpy(stack + at, &v, size);   // little-endian: the low bytes
        at += size;
    }
    return (at + 15) & ~(size_t)15;
}

void rt64_native_call(CPU64 *c, uint64_t fn) {
#if defined(__aarch64__)
    // At a call or an import thunk, [rsp] is the return address: stack arguments follow it.
    const rt64_native_sig *sig = rt64_native_sig_of(fn);
    uint64_t gp[6] = {c->r[RDI], c->r[RSI], c->r[RDX], c->r[RCX], c->r[R8], c->r[R9]};
    uint64_t guest_stack[2 + RT64_NATIVE_MAX_STACK];
    for (int i = 0; i < 2 + RT64_NATIVE_MAX_STACK; ++i) guest_stack[i] = R64(c->r[RSP] + 8 + 8 * (uint64_t)i);
    uint64_t ints[8], x8;
    uint8_t stack[RT64_NATIVE_MAX_STACK * 8 + 16] __attribute__((aligned(16))) = {0};
    size_t bytes = rt64_native_layout(gp, guest_stack, sig, ints, stack, sizeof(stack), &x8);
    double fps[8];
    for (int i = 0; i < 8; ++i) fps[i] = c->xmm[i].g[0];
    uint64_t out[4];
    native_trampoline(ints, fps, fn, out, stack, bytes, x8);
    // A structure returned through the hidden pointer: rax hands that pointer back (System V).
    c->r[RAX] = sig && sig->hidden_return ? gp[0] : out[0];
    c->r[RDX] = out[1];
    c->xmm[0] = rt64_xq(out[2]); c->xmm[1] = rt64_xq(out[3]);
#else
    (void)fn;
    rt64_trap(c, c->rip, "native calls need an arm64 host");
#endif
}

int rt64_is_native(uint64_t address) {
    Dl_info info;
    // Lifted modules are mapped by the runtime, not by dyld: dladdr knows only native images.
    return address && dladdr((const void *)(uintptr_t)address, &info) && info.dli_fbase;
}

// Native libraries standing in for Linux ones (native_libs64.h, round 28).
#include "native_libs64.h"
#include <pthread.h>
#include <stdlib.h>

#define NATIVE_MAX 8
static struct { char soname[96]; void *handle; } natives[NATIVE_MAX];
static int native_count;
static pthread_once_t natives_once = PTHREAD_ONCE_INIT;

static void natives_init(void) {
    const char *spec = getenv("RT64_NATIVE_LIBS");
    if (!spec) return;
    char *copy = strdup(spec), *save = NULL;
    for (char *entry = strtok_r(copy, ";", &save); entry && native_count < NATIVE_MAX; entry = strtok_r(NULL, ";", &save)) {
        char *eq = strchr(entry, '=');
        if (!eq) continue;
        *eq = 0;
        void *h = dlopen(eq + 1, RTLD_NOW | RTLD_GLOBAL);
        if (!h) { fprintf(stderr, "rt64: %s stands in for %s but didn't load: %s\n", eq + 1, entry, dlerror()); continue; }
        snprintf(natives[native_count].soname, sizeof natives[0].soname, "%s", entry);
        natives[native_count++].handle = h;
    }
    free(copy);
}

int rt64_native_library(const char *name) {
    pthread_once(&natives_once, natives_init);
    const char *base = strrchr(name, '/') ? strrchr(name, '/') + 1 : name;
    for (int i = 0; i < native_count; ++i) {
        // libSDL2-2.0.so.0 also answers libSDL2-2.0.so and libSDL2.so (what dlopen callers try).
        const char *so = natives[i].soname, *dot = strstr(so, ".so");
        size_t stem = dot ? (size_t)(dot - so) : strlen(so);
        if (!strcmp(base, so) || (!strncmp(base, so, stem) && !strncmp(base + stem, ".so", 3))) return i;
        const char *dash = memchr(so, '-', stem);
        if (dash && !strncmp(base, so, (size_t)(dash - so)) && !strcmp(base + (dash - so), ".so")) return i;
    }
    return -1;
}

uint64_t rt64_native_library_symbol(int index, const char *name) {
    pthread_once(&natives_once, natives_init);
    if (index < 0 || index >= native_count) return 0;
    return (uint64_t)(uintptr_t)dlsym(natives[index].handle, name);
}

uint64_t rt64_native_symbol(const char *name) {
    pthread_once(&natives_once, natives_init);
    for (int i = 0; i < native_count; ++i) {
        void *p = dlsym(natives[i].handle, name);
        if (p) return (uint64_t)(uintptr_t)p;
    }
    return 0;
}
