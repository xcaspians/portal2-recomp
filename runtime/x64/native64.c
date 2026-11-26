// Calls from lifted x86-64 code into native arm64 code (P2CE piece D, round
// 23): Valve's macOS libsteam_api.dylib and the steamclient.dylib it loads.
// Guest and host share one address space (rt64.h), so pointers pass as they
// are; only the calling convention differs. System V x86-64 passes integers in
// rdi, rsi, rdx, rcx, r8, r9 then on the stack, and floating point in
// xmm0-xmm7; Apple arm64 passes integers in x0-x7 and floating point in d0-d7.
// The trampoline loads all of both (the 7th and 8th integers from the guest's
// stack), calls, and hands back x0/x1 and d0/d1 as rax/rdx and xmm0/xmm1.
// Not covered: more than 8 integer or 8 floating-point arguments, and
// structures returned through a hidden pointer (rdi on x86-64, x8 on arm64).
#include "rt64.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#if defined(__aarch64__)
// (ints[8], doubles[8], function, out[4]): out = {x0, x1, d0 bits, d1 bits}.
__attribute__((naked)) static void native_trampoline(const uint64_t *ints, const double *fps, uint64_t fn, uint64_t *out) {
    __asm__ volatile(
        "stp x29, x30, [sp, #-32]!\n"
        "mov x29, sp\n"
        "str x3, [sp, #16]\n"
        "mov x16, x2\n"
        "mov x17, x0\n"
        "ldp d0, d1, [x1]\n"
        "ldp d2, d3, [x1, #16]\n"
        "ldp d4, d5, [x1, #32]\n"
        "ldp d6, d7, [x1, #48]\n"
        "ldp x0, x1, [x17]\n"
        "ldp x2, x3, [x17, #16]\n"
        "ldp x4, x5, [x17, #32]\n"
        "ldp x6, x7, [x17, #48]\n"
        "blr x16\n"
        "ldr x9, [sp, #16]\n"
        "stp x0, x1, [x9]\n"
        "stp d0, d1, [x9, #16]\n"
        "ldp x29, x30, [sp], #32\n"
        "ret\n");
}
#endif

void rt64_native_call(CPU64 *c, uint64_t fn) {
#if defined(__aarch64__)
    // At a call or an import thunk, [rsp] is the return address: stack arguments follow it.
    uint64_t ints[8] = {c->r[RDI], c->r[RSI], c->r[RDX], c->r[RCX], c->r[R8], c->r[R9],
                        R64(c->r[RSP] + 8), R64(c->r[RSP] + 16)};
    double fps[8];
    for (int i = 0; i < 8; ++i) fps[i] = c->xmm[i].g[0];
    uint64_t out[4];
    native_trampoline(ints, fps, fn, out);
    c->r[RAX] = out[0]; c->r[RDX] = out[1];
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
