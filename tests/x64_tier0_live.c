// Live check of the x86-64 lifter on P2CE's own libtier0.so (depot 440005):
// `P2_P2CE_TESTS=1 make p2ce-tier0-check` (tests/test_p2ce_live.py). The
// module is prepared and lifted from the user's depot into build/ at check
// time; nothing of it is in the repository. Constructors are not run yet
// (round C of docs/fork/P2CE_PLAN.md), so only functions that need no
// initialised state are called.
#include "x64/rt64.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern const RT64Module p2ce_tier0;
static CPU64 cpu;
static uint64_t stack_[65536];
static int failures, checks;

#define EXPECT(cond) do { checks++; if (!(cond)) { failures++; \
    fprintf(stderr, "p2ce-tier0: %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

static uint64_t call(const char *name, uint64_t a0, uint64_t a1, uint64_t a2) {
    uint64_t at = rt64_export(&p2ce_tier0, name);
    if (!at) { fprintf(stderr, "p2ce-tier0: no export %s\n", name); exit(1); }
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 65536));
    cpu.r[RDI] = a0; cpu.r[RSI] = a1; cpu.r[RDX] = a2;
    rt64_call(&cpu, at);
    printf("  %-36s -> %#llx\n", name, (unsigned long long)cpu.r[RAX]);
    return cpu.r[RAX];
}
#define PTR(p) ((uint64_t)(uintptr_t)(p))

int main(void) {
    if (rt64_load(&p2ce_tier0, NULL)) return 1;
    printf("p2ce-tier0: libtier0.so mapped at %#llx (guest addresses are host addresses)\n", (unsigned long long)*p2ce_tier0.base);

    EXPECT((call("Plat_IsInBenchmarkMode", 0, 0, 0) & 0xff) == 0);
    call("Plat_SetBenchmarkMode", 1, 0, 0);
    EXPECT((call("Plat_IsInBenchmarkMode", 0, 0, 0) & 0xff) == 1);
    call("Plat_SetBenchmarkMode", 0, 0, 0);
    EXPECT((call("Plat_IsInBenchmarkMode", 0, 0, 0) & 0xff) == 0);

    char dest[8];
    call("V_tier0_strncpy", PTR(dest), PTR("Portal 2 CE"), sizeof dest);
    EXPECT(!strcmp(dest, "Portal "));                       // truncated and terminated
    EXPECT((int32_t)call("V_tier0_stricmp", PTR("Aperture"), PTR("aPERTURE"), 0) == 0);
    EXPECT((int32_t)call("V_tier0_stricmp", PTR("chell"), PTR("wheatley"), 0) < 0);

    int64_t value = 41;
    EXPECT(call("ThreadInterlockedCompareExchange64", PTR(&value), 42, 41) == 41 && value == 42);
    EXPECT(call("ThreadInterlockedCompareExchange64", PTR(&value), 7, 41) == 42 && value == 42);
    void *slot = (void *)0x1234;
    EXPECT(call("ThreadInterlockedExchangePointer", PTR(&slot), 0x5678, 0) == 0x1234 && slot == (void *)0x5678);

    char zeroed[16];
    memset(zeroed, 0x55, sizeof zeroed);
    call("ZeroMemory", PTR(zeroed), 12, 0);                  // tail call to the memset import
    EXPECT(zeroed[0] == 0 && zeroed[11] == 0 && zeroed[12] == 0x55);

    call("Plat_FloatTime", 0, 0, 0);                          // clock_gettime import + SSE2
    double first = cpu.xmm[0].g[0];
    struct timespec pause = {0, 20 * 1000 * 1000};
    nanosleep(&pause, NULL);
    call("Plat_FloatTime", 0, 0, 0);
    double second = cpu.xmm[0].g[0];
    printf("  Plat_FloatTime: %.6f s, then %.6f s\n", first, second);
    EXPECT(first >= 0 && second - first >= 0.015 && second - first < 1.0);

    // No channel is registered before the constructors run: not found.
    EXPECT((int32_t)call("LoggingSystem_FindChannel", PTR("Console"), 0, 0) == -1);

    if (failures) { fprintf(stderr, "p2ce-tier0: %d of %d checks failed\n", failures, checks); return 1; }
    printf("p2ce-tier0: PASS, %d checks on functions lifted from P2CE's libtier0.so\n", checks);
    return 0;
}
