// First import shims of the x86-64 runtime: the libc functions whose
// arguments and results have the same layout on Linux x86-64 and Apple arm64
// (both LP64), so a guest pointer is passed to the host function as is.
// Arguments: rdi, rsi, rdx, rcx, r8, r9 (System V AMD64); result in rax.
#include "rt64.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define P(reg) ((void *)(uintptr_t)c->r[reg])
#define S(reg) ((const char *)(uintptr_t)c->r[reg])
#define RET(value) (c->r[RAX] = (uint64_t)(value))

static void s_memset(CPU64 *c) { RET((uintptr_t)memset(P(RDI), (int)c->r[RSI], (size_t)c->r[RDX])); }
static void s_memcpy(CPU64 *c) { RET((uintptr_t)memcpy(P(RDI), P(RSI), (size_t)c->r[RDX])); }
static void s_memmove(CPU64 *c) { RET((uintptr_t)memmove(P(RDI), P(RSI), (size_t)c->r[RDX])); }
static void s_memcmp(CPU64 *c) { RET((int64_t)memcmp(P(RDI), P(RSI), (size_t)c->r[RDX])); }
static void s_strlen(CPU64 *c) { RET(strlen(S(RDI))); }
static void s_strcmp(CPU64 *c) { RET((int64_t)strcmp(S(RDI), S(RSI))); }
static void s_strncmp(CPU64 *c) { RET((int64_t)strncmp(S(RDI), S(RSI), (size_t)c->r[RDX])); }
static void s_strcasecmp(CPU64 *c) { RET((int64_t)strcasecmp(S(RDI), S(RSI))); }
static void s_strchr(CPU64 *c) { RET((uintptr_t)strchr(S(RDI), (int)c->r[RSI])); }
static void s_strrchr(CPU64 *c) { RET((uintptr_t)strrchr(S(RDI), (int)c->r[RSI])); }
static void s_strstr(CPU64 *c) { RET((uintptr_t)strstr(S(RDI), S(RSI))); }
static void s_tolower(CPU64 *c) { RET((int64_t)tolower((int)c->r[RDI])); }
static void s_toupper(CPU64 *c) { RET((int64_t)toupper((int)c->r[RDI])); }
static void s_malloc(CPU64 *c) { RET((uintptr_t)malloc((size_t)c->r[RDI])); }
static void s_calloc(CPU64 *c) { RET((uintptr_t)calloc((size_t)c->r[RDI], (size_t)c->r[RSI])); }
static void s_realloc(CPU64 *c) { RET((uintptr_t)realloc(P(RDI), (size_t)c->r[RSI])); }
static void s_free(CPU64 *c) { free(P(RDI)); }

// Linux clock ids differ from Apple's; struct timespec is {long, long} on both.
static void s_clock_gettime(CPU64 *c) {
    clockid_t id;
    switch ((int)c->r[RDI]) {
    case 0: id = CLOCK_REALTIME; break;
    case 1: case 6: id = CLOCK_MONOTONIC; break;          // MONOTONIC, MONOTONIC_COARSE
    case 4: id = CLOCK_MONOTONIC_RAW; break;
    case 2: id = CLOCK_PROCESS_CPUTIME_ID; break;
    case 3: id = CLOCK_THREAD_CPUTIME_ID; break;
    case 5: id = CLOCK_REALTIME; break;                   // REALTIME_COARSE
    case 7: id = CLOCK_MONOTONIC; break;                  // BOOTTIME
    default: RET(-1); return;
    }
    RET((int64_t)clock_gettime(id, P(RSI)));
}

static const struct { const char *name; rt64_shim fn; } shims[] = {
    {"memset", s_memset}, {"memcpy", s_memcpy}, {"memmove", s_memmove}, {"memcmp", s_memcmp},
    {"strlen", s_strlen}, {"strcmp", s_strcmp}, {"strncmp", s_strncmp}, {"strcasecmp", s_strcasecmp},
    {"strchr", s_strchr}, {"strrchr", s_strrchr}, {"strstr", s_strstr},
    {"tolower", s_tolower}, {"toupper", s_toupper},
    {"malloc", s_malloc}, {"calloc", s_calloc}, {"realloc", s_realloc}, {"free", s_free},
    {"clock_gettime", s_clock_gettime},
};

rt64_shim rt64_find_shim(const char *name) {
    for (size_t i = 0; i < sizeof shims / sizeof *shims; ++i)
        if (!strcmp(shims[i].name, name)) return shims[i].fn;
    return NULL;
}
