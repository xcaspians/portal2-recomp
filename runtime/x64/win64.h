// Windows x64 side of the x86-64 runtime (pe64.c): the thread and process
// blocks a PE module reads through gs:, its static TLS, Control Flow Guard,
// and the Windows calling convention at the host boundary.
//
// Windows x64 passes the first four arguments in rcx, rdx, r8 and r9 (or
// xmm0-xmm3 for floats, by position), the rest on the stack above a 32-byte
// shadow area the caller reserves; rbx, rbp, rdi, rsi, r12-r15 and
// xmm6-xmm15 are callee-saved. Lifted-to-lifted calls never care: only the
// host boundary (shims, callbacks) does.
#pragma once
#include "rt64.h"

// Argument i of a Windows x64 call, read by a shim (at that point [rsp] is
// the return address, so stack argument i >= 4 is at rsp + 8 + 8 * i).
#define WARG(c, i) ((i) < 4 ? (c)->r[(const int[]){RCX, RDX, R8, R9}[(i) & 3]] \
                            : R64((c)->r[RSP] + 8 + 8 * (uint64_t)(i)))
#define WPTR(c, i) ((void *)(uintptr_t)WARG(c, i))
#define WFLOAT(c, i) ((c)->xmm[(i)].f[0])
#define WDOUBLE(c, i) ((c)->xmm[(i)].g[0])

// TEB offsets read by MSVC-built code (NT_TIB first).
enum {
    TEB_EXCEPTION_LIST = 0x00, TEB_STACK_BASE = 0x08, TEB_STACK_LIMIT = 0x10, TEB_SELF = 0x30,
    TEB_PROCESS_ID = 0x40, TEB_THREAD_ID = 0x48, TEB_TLS_POINTER = 0x58, TEB_PEB = 0x60,
    TEB_LAST_ERROR = 0x68, TEB_TLS_SLOTS = 0x1480, TEB_TLS_EXPANSION = 0x1780, TEB_SIZE = 0x2000,
};
enum { PEB_BEING_DEBUGGED = 0x02, PEB_IMAGE_BASE = 0x10, PEB_LDR = 0x18, PEB_PARAMETERS = 0x20,
       PEB_PROCESS_HEAP = 0x30, PEB_OS_MAJOR = 0x118, PEB_OS_MINOR = 0x11c, PEB_OS_BUILD = 0x120,
       PEB_PLATFORM = 0x124, PEB_SESSION = 0x2c0, PEB_SIZE = 0x1000 };

// The process block (one, created on first use).
uint64_t rt64_win_peb(void);
// A thread block for this CPU (gs_base): its stack range, a fresh thread id,
// and every loaded PE module's TLS block. Returns the TEB's address.
uint64_t rt64_win_thread(CPU64 *c, uint64_t stack_low, uint64_t stack_high);
uint32_t rt64_win_thread_id(const CPU64 *c);
// Host -> guest call with the Windows convention: args[0..3] in rcx, rdx,
// r8, r9, the rest on the stack above the shadow area. Returns rax; every
// other register is left as it was.
uint64_t rt64_win_call(CPU64 *c, uint64_t target, int argc, const uint64_t *args);
// Windows' GetLastError slot of the calling guest thread.
#define WIN_SET_LAST_ERROR(c, e) W32((c)->gs_base + TEB_LAST_ERROR, (uint32_t)(e))
