// x86-64 lifter checks (build/x64-check, run by `make check`): the programs
// of tests/x64_programs.s, linked by tests/x64_link.py, prepared by
// tools/elfprep64.py and lifted by tools/lift64.py, run through the 64-bit
// runtime (runtime/x64). Expected values come from the architecture manuals.
#include "x64/rt64.h"

#include <stdio.h>
#include <stdlib.h>

extern const RT64Module x64_fixture;
static CPU64 cpu;
static uint64_t stack_[16384];
static int failures, checks;

#define EXPECT(cond) do { checks++; if (!(cond)) { failures++; \
    fprintf(stderr, "x64-check: %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

static uint64_t sym(const char *name) {
    uint64_t at = rt64_export(&x64_fixture, name);
    if (!at) { fprintf(stderr, "x64-check: no export %s\n", name); exit(1); }
    return at;
}

// Calls a guest function with up to 8 integer arguments (System V AMD64).
static uint64_t call(const char *name, int count, const uint64_t *args) {
    static const int regs[] = {RDI, RSI, RDX, RCX, R8, R9};
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    for (int i = 0; i < count && i < 6; ++i) cpu.r[regs[i]] = args[i];
    if (count > 6 && (count - 6) % 2) PUSH64(&cpu, 0);    // keep rsp 16-aligned at the call
    for (int i = count - 1; i >= 6; --i) PUSH64(&cpu, args[i]);
    uint64_t rsp = cpu.r[RSP];
    rt64_call(&cpu, sym(name));
    EXPECT(cpu.r[RSP] == rsp);                             // the callee balanced the stack
    return cpu.r[RAX];
}
#define CALL(name, ...) call(name, sizeof((uint64_t[]){__VA_ARGS__}) / 8, (uint64_t[]){__VA_ARGS__})
#define CALL0(name) call(name, 0, NULL)

static uint64_t out(int offset) { return R64(sym("x64_out") + offset); }
static uint8_t out8(int offset) { return (uint8_t)R8(sym("x64_out") + offset); }

int main(void) {
    if (rt64_load(&x64_fixture, NULL)) return 1;
    EXPECT(CALL("x64_sum8", 1, 2, 3, 4, 5, 6, 700, 8000) == 8721);

    CALL0("x64_widths");
    EXPECT(out(0) == 5);
    EXPECT(out(8) == 0xffffffffffffff12u);
    EXPECT(out(16) == 0xffffffffffff1234u);
    EXPECT(out(24) == 0xffffffffffffff00u);
    EXPECT(out(32) == 0x55667788u);
    EXPECT(out(40) == 0x7f00u);
    EXPECT(out(48) == (uint64_t)-128);
    EXPECT(out(56) == (uint64_t)-2);
    EXPECT(out(64) == 0xfffeu);

    EXPECT(CALL("x64_counter", 5) == 5);
    EXPECT(CALL("x64_counter", 0x100000000ull) == 0x100000005ull);

    EXPECT(CALL("x64_fact", 20) == 2432902008176640000ull);

    CALL0("x64_flags");
    EXPECT(out8(0) == 1 && out8(1) == 1 && out8(2) == 1 && out8(3) == 1);
    EXPECT(out8(4) == 1 && out8(5) == 1);
    EXPECT(out(8) == 0xffffffffu);
    EXPECT(out(16) == 7);
    EXPECT(out(24) == 1 && out(32) == 0);

    CALL0("x64_alu");
    EXPECT(out(0) == 0x8000000000000000u);
    EXPECT(out(8) == (uint64_t)-2);
    EXPECT(out(16) == 0x18);
    EXPECT(out(24) == 0x2345678900000000u && out(32) == 1);
    EXPECT(out(40) == (uint64_t)-14 && out(48) == (uint64_t)-2);
    EXPECT(out(56) == 0x1010);
    EXPECT(out(64) == 0x0807060504030201u);
    EXPECT(out(72) == (uint64_t)-42);
    EXPECT(out8(80) == 0 && out(88) == 0x48);

    for (uint64_t i = 0; i < 4; ++i) EXPECT(CALL("x64_switch", i) == 10 + i);
    EXPECT((uint32_t)CALL("x64_switch", 4) == 0xffffffffu);
    EXPECT(CALL("x64_callptr", 0, 21) == 42);
    EXPECT(CALL("x64_callptr", 1, 12) == 144);
    EXPECT(CALL("x64_tail", 6) == 49);

    char buffer[64] = {0};
    EXPECT(CALL("x64_imports", (uint64_t)(uintptr_t)buffer) == 7);
    EXPECT(!strcmp(buffer, "AAAAAAA") && !strcmp(buffer + 16, "AAAAAAA"));

    EXPECT(CALL0("x64_strings") == 30);
    EXPECT(out(0) == 0x0101010101010101u && out(24) == 0x0101010101010101u);
    EXPECT((out(40) & 0xffffff) == 0x010101 && out8(43) == 0xff);   // 3 bytes copied, the 4th (from x64_alu) kept
    EXPECT(out8(48) == 1 && out8(49) == 0);
    EXPECT(R64(sym("x64_count")) == 12);

    EXPECT(CALL("x64_sse", 7) == 10);                       // (7 * 3 + 0.5) / 2 = 10.75
    double value; uint64_t bits = out(0); memcpy(&value, &bits, 8);
    EXPECT(value == 10.75 && out8(8) == 1 && cpu.xmm[0].g[0] == 10.75);

    if (failures) { fprintf(stderr, "x64-check: %d of %d checks failed\n", failures, checks); return 1; }
    printf("x64-check: PASS, %d checks (System V calls, 16 registers, RIP-relative data, "
           "flags, jump tables, imports, atomics, SSE2)\n", checks);
    return 0;
}
