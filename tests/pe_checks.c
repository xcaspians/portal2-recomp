// make check: the PE test program (tests/pe_programs.s, linked by
// tests/pe_link.py into pefix.dll, prepared by tools/peprep64.py and lifted
// by tools/lift64.py) loaded by the runtime and called with the Windows x64
// convention: arguments on the stack, the TEB through gs:, static TLS, MSVC
// jump tables, imports by name and by ordinal, Control Flow Guard, base
// relocations, and a function known only from .pdata.
#include "x64/win64.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

extern const RT64Module pe_fixture;

static int failures, passes;
#define CHECK(cond, ...) do { if (cond) passes++; else { failures++; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
                              fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

// The host DLL the fixture imports from (pehost.dll), as Windows-convention shims.
static void host_twice(CPU64 *c) { RET(c, 2 * WARG(c, 0)); }
static void host_sum5(CPU64 *c) { RET(c, WARG(c, 0) + 10 * WARG(c, 1) + 100 * WARG(c, 2) + 1000 * WARG(c, 3) + 10000 * WARG(c, 4)); }
static void host_seven(CPU64 *c) { RET(c, 7); }
// MSVC's _JUMP_BUFFER: Frame, Rbx, Rsp, Rbp, Rsi, Rdi, R12-R15, Rip (Windows' own layout).
static const int jump_saved[] = {RBX, -1, RBP, RSI, RDI, R12, R13, R14, R15};
static void host_setjmp(CPU64 *c) {
    uint64_t buf = WARG(c, 0);
    W64(buf, WARG(c, 1));
    for (int i = 0; i < 9; ++i) if (jump_saved[i] >= 0) W64(buf + 8 + 8 * (uint64_t)i, c->r[jump_saved[i]]);
    W64(buf + 16, c->r[RSP] + 8);           // the stack after the return
    W64(buf + 80, R64(c->r[RSP]));          // the return address
    RET(c, 0);
}
static void host_longjmp(CPU64 *c) {
    uint64_t buf = WARG(c, 0);
    int value = (int)WARG(c, 1);
    for (int i = 0; i < 9; ++i) if (jump_saved[i] >= 0) c->r[jump_saved[i]] = R64(buf + 8 + 8 * (uint64_t)i);
    c->r[RSP] = R64(buf + 16);
    c->rip = R64(buf + 80);
    RET(c, (uint32_t)(value ? value : 1));
    c->transferred = 1;
}
// vcruntime's two C++ exception entry points (tests/pe_eh.cpp): throw goes to the
// runtime's MSVC unwinder; the frame handler is only ever named in tables.
static void host_throw(CPU64 *c) { rt64_msvc_throw(c, WARG(c, 0), WARG(c, 1)); }
static void host_frame_handler(CPU64 *c) { rt64_trap(c, R64(c->r[RSP]), "__CxxFrameHandler3 called"); }
static const RT64Shim shims[] = {
    {"pehost.dll!_CxxThrowException", host_throw}, {"pehost.dll!__CxxFrameHandler3", host_frame_handler},
    {"pehost.dll!twice", host_twice}, {"pehost.dll!sum5", host_sum5}, {"pehost.dll!#7", host_seven},
    {"pehost.dll!_setjmp", host_setjmp}, {"pehost.dll!longjmp", host_longjmp}, {NULL, NULL}};
static const RT64Data data[] = {{NULL, NULL}};
RT64_SHIMS(shims, data)

// Native arm64 functions the PE program calls through pointers.
static double native_mix(int64_t a, double b, int32_t c, float d) { return (double)a + b * 10 + c * 100 + d * 1000; }
static int64_t native_sum10(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e, int64_t f, int64_t g, int64_t h,
                            int64_t i, int64_t j) {
    return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h + 9 * i + 10 * j;
}
typedef struct { int64_t a, b, c; } Three;

// Several threads calling a native function with a signature while another thread keeps
// changing the signature table: each thread's cached lookup (native64.c win_lookup) must
// give the right signature every time, and hit once seen.
static _Atomic int sig_threads_done;
typedef struct { uint64_t fn; int wrong; uint64_t hits; } SigWorker;
static void *sig_worker(void *arg) {
    SigWorker *w = arg;
    static __thread CPU64 cpu;
    uint64_t before = rt64_win_native_cache_hits(&cpu);
    for (int i = 0; i < 20000; ++i) {
        cpu.r[RCX] = 1; cpu.xmm[1].g[0] = 2.0; cpu.r[R8] = 3; cpu.xmm[3].q[0] = 0; cpu.xmm[3].f[0] = 4.0f;
        rt64_native_call_win(&cpu, w->fn);
        if (cpu.xmm[0].g[0] != 4321.0) w->wrong++;
    }
    w->hits = rt64_win_native_cache_hits(&cpu) - before;
    atomic_fetch_add(&sig_threads_done, 1);
    return NULL;
}
static Three native_make3(int64_t k) { return (Three){k, 2 * k, 3 * k}; }

static uint64_t fn(const char *name) {
    char full[96];
    snprintf(full, sizeof full, "pefix.dll!%s", name);
    uint64_t address = rt64_export(&pe_fixture, full);
    if (!address) { fprintf(stderr, "pe-check: no export %s\n", full); exit(1); }
    return address;
}

static uint64_t call(CPU64 *c, const char *name, int argc, const uint64_t *args) {
    return rt64_win_call(c, fn(name), argc, args);
}

// rt64_win_each_teb's callback: counts the TEBs, and how often it sees the one asked for.
static void pe_count_teb(uint64_t t, void *arg) {
    struct { size_t *seen, *found; uint64_t teb; } *walk = arg;
    ++*walk->seen;
    if (t == walk->teb) ++*walk->found;
}

int main(void) {
    if (rt64_load(&pe_fixture, NULL)) { fprintf(stderr, "pe-check: cannot load pefix.dll\n"); return 1; }
    uint64_t base = 0;
    rt64_module_at(fn("pe_add5"), &base);
    size_t stack_size = 1 << 20;
    uint8_t *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    static CPU64 cpu;
    CPU64 *c = &cpu;
    rt64_cpu_init(c, (uint64_t)(uintptr_t)(stack + stack_size));
    uint64_t teb = rt64_win_thread(c, (uint64_t)(uintptr_t)stack, (uint64_t)(uintptr_t)(stack + stack_size));

    CHECK(R64(base + 0x3c) && R16(base) == 0x5a4d, "headers mapped from the file");
    uint32_t pe_header = (uint32_t)R32(base + 0x3c);
    CHECK(R64(base + pe_header + 24 + 24) == base, "ImageBase field holds the load address");
    CHECK(call(c, "pe_add5", 5, (const uint64_t[]){1, 20, 300, 4000, 50000}) == 54321, "five Windows arguments");
    CHECK(call(c, "pe_teb_self", 0, NULL) == teb, "gs:[0x30] is the TEB");
    {
        size_t seen = 0, found = 0;
        struct { size_t *seen, *found; uint64_t teb; } walk = {&seen, &found, teb};
        rt64_win_each_teb(pe_count_teb, &walk);
        CHECK(seen >= 1 && found == 1, "rt64_win_each_teb visits every TEB, this one once");
    }
    CHECK(R64(teb + TEB_STACK_BASE) == (uint64_t)(uintptr_t)(stack + stack_size), "TEB stack base");
    CHECK(call(c, "pe_tls_bump", 0, NULL) == 42, "static TLS from the template");
    CHECK(call(c, "pe_tls_bump", 0, NULL) == 43, "static TLS keeps its value");
    static const uint32_t cases[] = {100, 111, 122, 133, 0xffffffff, 0xffffffff};
    for (uint64_t i = 0; i < 6; ++i)
        CHECK((uint32_t)call(c, "pe_switch", 1, (const uint64_t[]){i}) == cases[i], "MSVC jump table, case %llu",
              (unsigned long long)i);
    for (uint64_t i = 0; i < 7; ++i)
        CHECK((uint32_t)call(c, "pe_switch2", 1, (const uint64_t[]){i}) == (i > 5 ? 0 : i & 1 ? 1 : 2),
              "two-level MSVC jump table, case %llu", (unsigned long long)i);
    for (uint64_t i = 0; i < 5; ++i)
        CHECK((uint32_t)call(c, "pe_switch3", 1, (const uint64_t[]){i}) == (i > 3 ? 39u : i < 2 ? 30u : 31u),
              "two-level MSVC jump table, base added from a second register, case %llu", (unsigned long long)i);
    CHECK(call(c, "pe_bnd", 1, (const uint64_t[]){0}) == 0 && call(c, "pe_bnd", 1, (const uint64_t[]){5}) == 1,
          "BND-prefixed branches and returns");
    CHECK(call(c, "pe_setjmp", 1, (const uint64_t[]){5}) == 5, "longjmp back to the call after an MSVC _setjmp");
    rt64_win_native_set_sig((uintptr_t)native_mix, "pdif", -1);
    call(c, "pe_native_mix", 1, (const uint64_t[]){(uintptr_t)native_mix});
    CHECK(c->xmm[0].g[0] == 4321.0, "native call with integer and float slots: %g", c->xmm[0].g[0]);
    {   // The signature lookup is cached per thread: the same call again hits, with the same answer.
        uint64_t hits = rt64_win_native_cache_hits(c);
        call(c, "pe_native_mix", 1, (const uint64_t[]){(uintptr_t)native_mix});
        CHECK(c->xmm[0].g[0] == 4321.0 && rt64_win_native_cache_hits(c) > hits, "native signature from the thread's cache");
        // A native target reached through rt64_dispatch is remembered in the dispatch cache too:
        // the second call hits it (no module walk) and gives the same answer.
        uint64_t d_hits, d_misses, d_hits2, d_misses2;
        rt64_dispatch_stats(c, &d_hits, &d_misses);
        call(c, "pe_native_mix", 1, (const uint64_t[]){(uintptr_t)native_mix});
        rt64_dispatch_stats(c, &d_hits2, &d_misses2);
        CHECK(c->xmm[0].g[0] == 4321.0 && d_hits2 > d_hits && d_misses2 == d_misses,
              "native target from the dispatch cache (hits %llu -> %llu, misses %llu -> %llu)", (unsigned long long)d_hits,
              (unsigned long long)d_hits2, (unsigned long long)d_misses, (unsigned long long)d_misses2);
        SigWorker workers[4];
        pthread_t threads[4];
        for (int i = 0; i < 4; ++i) { workers[i] = (SigWorker){(uintptr_t)native_mix, 0, 0}; pthread_create(&threads[i], NULL, sig_worker, &workers[i]); }
        for (int k = 0; atomic_load(&sig_threads_done) < 4; ++k)    // the tables change meanwhile
            rt64_win_native_set_sig((uintptr_t)native_make3, k & 1 ? "ppp" : "pp", 0);
        int wrong = 0, hit_all = 1;
        for (int i = 0; i < 4; ++i) { pthread_join(threads[i], NULL); wrong += workers[i].wrong; hit_all &= workers[i].hits > 0; }
        CHECK(!wrong && hit_all, "native signature lookups from four threads while the table changes (%d wrong)", wrong);
    }
    CHECK(call(c, "pe_native_sum10", 1, (const uint64_t[]){(uintptr_t)native_sum10}) == 385, "native call, six stack slots");
    rt64_win_native_set_sig((uintptr_t)native_make3, "pp", 0);
    Three three = {0, 0, 0};
    uint64_t got = call(c, "pe_native_make3", 2, (const uint64_t[]){(uintptr_t)native_make3, (uintptr_t)&three});
    CHECK(got == (uintptr_t)&three && three.a == 7 && three.b == 14 && three.c == 21, "structure through the hidden pointer");
    rt64_win_native_set_override((uintptr_t)native_sum10, host_sum5);
    CHECK(call(c, "pe_native_sum10", 1, (const uint64_t[]){(uintptr_t)native_sum10}) == 54321,
          "a host override stands in for a native function");
    CHECK(call(c, "pe_lea_call", 0, NULL) == 77, "a function only a lea reaches, its start swallowed by the sweep");
    CHECK(call(c, "pe_import_twice", 1, (const uint64_t[]){21}) == 42, "import by name");
    CHECK(call(c, "pe_import_sum5", 0, NULL) == 54321, "import with a stack argument");
    CHECK(call(c, "pe_import_ordinal", 0, NULL) == 7, "import by ordinal");
    CHECK(call(c, "pe_guard_call", 1, (const uint64_t[]){0}) == 21, "Control Flow Guard dispatch (relocated pointer)");
    CHECK(call(c, "pe_guard_call", 1, (const uint64_t[]){1}) == 1007, "Control Flow Guard dispatch, second target");
    CHECK(call(c, "pe_hidden_call", 0, NULL) == 4242, "function known only from .pdata");
    CHECK(call(c, "pe_eh_int", 1, (const uint64_t[]){41}) == 42, "C++ throw caught by value in the caller");
    CHECK(call(c, "pe_eh_int", 1, (const uint64_t[]){0}) == 0, "no throw: the try block's normal way out");
    CHECK(call(c, "pe_eh_unwind", 1, (const uint64_t[]){3}) == 311,
          "C++ throw through a frame whose catch doesn't match: its locals destroyed, caught above");
    CHECK(call(c, "pe_eh_base_ref", 1, (const uint64_t[]){5}) == 1005, "derived class caught by base reference (virtual call)");
    { uint64_t d = call(c, "pe_eh_destroyed", 0, NULL); CHECK(d == 100, "the exception object destroyed once after the catch (%llu)", (unsigned long long)d); }
    CHECK(call(c, "pe_eh_all", 0, NULL) == 2, "catch (...) after a catch that doesn't match");
    CHECK(call(c, "pe_eh_rethrow", 1, (const uint64_t[]){4}) == 11, "throw; inside a catch, caught by the caller");
    CHECK(call(c, "pe_eh_loop", 1, (const uint64_t[]){10}) == 55, "ten throws caught in a loop, which goes on");
    CHECK(c->r[RSP] == (uint64_t)(uintptr_t)(stack + stack_size), "stack balanced");
    if (failures) { fprintf(stderr, "pe-check: %d of %d checks failed\n", failures, failures + passes); return 1; }
    printf("pe-check: PASS, %d checks (Windows x64 calls, TEB, static TLS, MSVC jump tables, imports by name and "
           "ordinal, Control Flow Guard, base relocations, .pdata entries, MSVC C++ exceptions)\n", passes);
    return 0;
}
