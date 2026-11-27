// make check: the PE test program (tests/pe_programs.s, linked by
// tests/pe_link.py into pefix.dll, prepared by tools/peprep64.py and lifted
// by tools/lift64.py) loaded by the runtime and called with the Windows x64
// convention: arguments on the stack, the TEB through gs:, static TLS, MSVC
// jump tables, imports by name and by ordinal, Control Flow Guard, base
// relocations, and a function known only from .pdata.
#include "x64/win64.h"

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
static const RT64Shim shims[] = {
    {"pehost.dll!twice", host_twice}, {"pehost.dll!sum5", host_sum5}, {"pehost.dll!#7", host_seven},
    {"pehost.dll!_setjmp", host_setjmp}, {"pehost.dll!longjmp", host_longjmp}, {NULL, NULL}};
static const RT64Data data[] = {{NULL, NULL}};
RT64_SHIMS(shims, data)

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
    CHECK(call(c, "pe_import_twice", 1, (const uint64_t[]){21}) == 42, "import by name");
    CHECK(call(c, "pe_import_sum5", 0, NULL) == 54321, "import with a stack argument");
    CHECK(call(c, "pe_import_ordinal", 0, NULL) == 7, "import by ordinal");
    CHECK(call(c, "pe_guard_call", 1, (const uint64_t[]){0}) == 21, "Control Flow Guard dispatch (relocated pointer)");
    CHECK(call(c, "pe_guard_call", 1, (const uint64_t[]){1}) == 1007, "Control Flow Guard dispatch, second target");
    CHECK(call(c, "pe_hidden_call", 0, NULL) == 4242, "function known only from .pdata");
    CHECK(c->r[RSP] == (uint64_t)(uintptr_t)(stack + stack_size), "stack balanced");
    if (failures) { fprintf(stderr, "pe-check: %d of %d checks failed\n", failures, failures + passes); return 1; }
    printf("pe-check: PASS, %d checks (Windows x64 calls, TEB, static TLS, MSVC jump tables, imports by name and "
           "ordinal, Control Flow Guard, base relocations, .pdata entries)\n", passes);
    return 0;
}
