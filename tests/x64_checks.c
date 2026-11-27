// x86-64 lifter checks (build/x64-check, run by `make check`): the programs
// of tests/x64_programs.s, linked by tests/x64_link.py, prepared by
// tools/elfprep64.py and lifted by tools/lift64.py, run through the 64-bit
// runtime (runtime/x64). Expected values come from the architecture manuals.
#include "x64/rt64.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern const RT64Module x64_fixture, x64_second;
static CPU64 cpu;
static uint64_t stack_[16384];
static int failures, checks;

#define EXPECT(cond) do { checks++; if (!(cond)) { failures++; \
    fprintf(stderr, "x64-check: %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

static uint64_t sym(const char *name) {
    uint64_t at = rt64_export(&x64_fixture, name);
    if (!at) at = rt64_export(&x64_second, name);
    if (!at) { fprintf(stderr, "x64-check: no export %s\n", name); exit(1); }
    return at;
}

// Calls a guest function with up to 8 integer arguments (System V AMD64).
static uint64_t call(const char *name, int count, const uint64_t *args) {
    static const int regs[] = {RDI, RSI, RDX, RCX, R8, R9};
    uint64_t thread_pointer = cpu.fs_base;
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.fs_base = thread_pointer;
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

// Native host functions called the way lifted code calls them (P2CE piece D, round 23).
static uint64_t native_mix(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g,
                           uint64_t h, double x, double y) {
    return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h + (uint64_t)(x * 10) + (uint64_t)(y * 100);
}
static double native_half(double x) { return x / 2; }
// Round 27: more than eight integer arguments, and a structure returned through a hidden pointer.
static uint64_t native_twelve(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g,
                              uint64_t h, uint64_t i, uint64_t j, uint64_t k, uint64_t l) {
    return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h + 9 * i + 10 * j + 11 * k + 12 * l;
}
// Apple arm64 packs stack arguments by size: two 4-byte ones share a slot.
static uint64_t native_mixed(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g,
                             uint64_t h, uint32_t i, uint32_t j, uint64_t k) {
    return a + b + c + d + e + f + g + h + 1000 * (uint64_t)i + 100000 * (uint64_t)j + 10000000 * k;
}
typedef struct { uint64_t v[5]; } native_big;   // 40 bytes: memory class on both ABIs
static native_big native_make_big(uint64_t x, uint64_t y) {
    native_big b = {{x, y, x + y, x * y, 77}};
    return b;
}
typedef struct { uint64_t (*fn[3])(void); } native_vtable;

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
    // Round 23: a pthread_once inside another's routine runs (it deadlocked: P2CE's engine.so).
    EXPECT(CALL0("x64_once_nested") == 7 && CALL0("x64_once_nested") == 7);   // once each
    // Round 23: an unoptimised switch (P2CE's engine.so).
    EXPECT(CALL("x64_switch_o0", 0) == 30 && CALL("x64_switch_o0", 2) == 32 && (uint32_t)CALL("x64_switch_o0", 9) == 0xfffffffdu);
    // Round 23: instructions between the add and the jmp (P2CE's launcher.so).
    EXPECT(CALL("x64_switch_gap", 0) == 20 && CALL("x64_switch_gap", 1) == 21 && CALL("x64_switch_gap", 2) == 22);
    EXPECT((uint32_t)CALL("x64_switch_gap", 3) == 0xfffffffeu);
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

    CALL0("x64_simd");
    for (int i = 1; i < 16; ++i) EXPECT(out8(i) == i - 1);
    EXPECT(out8(0) == 0);                                                   // PSHUFB: bit 7 clears the byte
    for (int i = 0; i < 16; ++i) EXPECT(out8(16 + i) == 4 + i);            // PALIGNR 4
    EXPECT(out(32) == 0x0000000d0000000cu && out(40) == 0x0000000f0000000eu);   // PMOVZXBD
    EXPECT(out(48) == 0xfffe7fff0007fffbu && out(56) == 0xfffe7fff0007fffbu);   // PACKSSDW saturates
    EXPECT(out(64) == 0x00000003fffffffdu && out(72) == 0xffffffff0000c350u);   // PSRAD 1
    EXPECT(out(80) == 0x00000000fffffffbu && out(88) == 0xfffffffe00000000u);   // PMINSD
    EXPECT(out(96) == 0xffffffffu && out(104) == 0);                       // CMPLTPS
    float lanes[4]; memcpy(lanes, (void *)(uintptr_t)(sym("x64_out") + 112), 16);
    EXPECT(lanes[0] == 1.5f && lanes[1] == -3.0f && lanes[2] == 3.0f && lanes[3] == 0);   // INSERTPS 0x58
    memcpy(lanes, (void *)(uintptr_t)(sym("x64_out") + 128), 16);
    EXPECT(lanes[0] == 2.0f && lanes[1] == -2.5f && lanes[2] == 3.0f && lanes[3] == 4.0f); // BLENDPS 5
    double rounded[2]; memcpy(rounded, (void *)(uintptr_t)(sym("x64_out") + 144), 16);
    EXPECT(rounded[0] == 2.0 && rounded[1] == 3.0);                         // ROUNDSD 0 and 2
    EXPECT(out8(160) == 0 && out8(161) == 1 && out8(162) == 0);             // PTEST ZF, CF
    EXPECT(out(176) == 0x0000001900000005u && out(184) == 0x000000710000003du);   // PMADDWD
    EXPECT(out(192) == 0x0006000600050005u && out(200) == 0x0008000800070007u);   // PUNPCKHWD

    CALL0("x64_x87");
    double fifteen; uint64_t raw = out(0); memcpy(&fifteen, &raw, 8);
    EXPECT(fifteen == 15.0);
    EXPECT((uint32_t)out(8) == 8);                                          // FISTP rounds to nearest even
    EXPECT(out(16) == 0xa000000000000000u && (out(24) & 0xffff) == 0x4000); // 2.5 as an 80-bit value
    EXPECT(out8(32) == 1 && (out(34) & 0xffff) == 0x37f);                   // FCOMIP, FNSTCW
    EXPECT(out(40) == 0x000c000a00080006u);                                 // PADDW mm
    EXPECT(out(48) == 0x080706050c0a0806u);                                 // PACKUSWB mm
    EXPECT(out(56) == 0x0005000600070008u);                                 // PSHUFW mm 0x1b

    CALL0("x64_bmi");
    EXPECT(out(0) == 0xf && out(8) == 0x0f && out(16) == 0xe0 && out(24) == 0xf000);
    EXPECT(out(32) == 4 && out(40) == 15 && out(48) == 0 && out(56) == 28);

    memset((void *)(uintptr_t)sym("x64_out"), 0xee, 64);
    CALL0("x64_misc");
    EXPECT(out(0) == 1 && out8(8) == 1 && out8(9) == 0 && out8(10) == 21);   // PUSHFQ, POPFQ, XLATB
    EXPECT(out(16) == (uint64_t)getpid());                                   // SYSCALL 39
    EXPECT(out8(32) == 0 && out8(33) == 0xee);                               // MASKMOVDQU: byte 0 only
    EXPECT(out(48) == 0x0004000300020001u);                                  // MOVNTQ

    EXPECT(CALL0("x64_after_data") == 0x5a5a);                                // decoded again after data

    // Round C: a second module opened by name (DT_NEEDED, constructors, TLS, setjmp/longjmp).
    rt64_thread_tls(&cpu);
    rt64_register(&x64_fixture);
    rt64_register(&x64_second);
    EXPECT(rt64_open(&cpu, "libx64second.so") != NULL);
    EXPECT(rt64_loaded_count() == 2);
    EXPECT(CALL0("x64b_add_via_first") == 36);           // import resolved to the first module's export
    EXPECT(CALL0("x64b_ctor_value") == 1234);            // its .init_array ran
    EXPECT(CALL0("x64b_tls_gd") == 11 && CALL0("x64b_tls_gd") == 12);   // __tls_get_addr, .tdata copied
    EXPECT(CALL0("x64b_tls_ie") == 77);                  // TPOFF64 from fs
    EXPECT(CALL0("x64b_setjmp") == 42);                  // longjmp across a lifted frame, rbx restored
    EXPECT(CALL0("x64b_threads") == 1000 * (1 + 2 + 3 + 4));   // pthread_create/join, own CPU and stack

    // Round 23: lifted code calling native arm64 code (Valve's Steam interfaces):
    // System V registers and stack arguments to arm64, doubles both ways.
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.r[RDI] = 1; cpu.r[RSI] = 2; cpu.r[RDX] = 3; cpu.r[RCX] = 4; cpu.r[R8] = 5; cpu.r[R9] = 6;
    cpu.xmm[0].g[0] = 1.5; cpu.xmm[1].g[0] = 0.25;
    PUSH64(&cpu, 8); PUSH64(&cpu, 7);                    // 8th and 7th integer arguments
    PUSH64(&cpu, 0x1234);                                // return address slot (as at a call)
    rt64_native_call(&cpu, (uint64_t)(uintptr_t)native_mix);
    EXPECT(cpu.r[RAX] == 1 + 4 + 9 + 16 + 25 + 36 + 49 + 64 + 15 + 25);
    cpu.xmm[0].g[0] = 7.0;
    rt64_native_call(&cpu, (uint64_t)(uintptr_t)native_half);
    EXPECT(cpu.xmm[0].g[0] == 3.5);
    EXPECT(rt64_is_native((uint64_t)(uintptr_t)native_half) && !rt64_is_native(sym("x64_sum8")));
    // Through the dispatcher, as an indirect call from lifted code would arrive.
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.xmm[0].g[0] = 9.0;
    rt64_call(&cpu, (uint64_t)(uintptr_t)native_half);
    EXPECT(cpu.xmm[0].g[0] == 4.5);

    // Round 27: twelve integer arguments, no signature registered: 8-byte stack values.
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.r[RDI] = 1; cpu.r[RSI] = 2; cpu.r[RDX] = 3; cpu.r[RCX] = 4; cpu.r[R8] = 5; cpu.r[R9] = 6;
    for (int a = 12; a >= 7; --a) PUSH64(&cpu, (uint64_t)a);
    PUSH64(&cpu, 0x1234);
    rt64_native_call(&cpu, (uint64_t)(uintptr_t)native_twelve);
    EXPECT(cpu.r[RAX] == 1 + 4 + 9 + 16 + 25 + 36 + 49 + 64 + 81 + 100 + 121 + 144);
    // Mixed sizes past the eighth: registered, packed as Apple's ABI wants.
    static const rt64_native_sig mixed = {0, 3, {4, 4, 8}};
    EXPECT(rt64_native_set_sig((uint64_t)(uintptr_t)native_mixed, &mixed));
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.r[RDI] = 1; cpu.r[RSI] = 1; cpu.r[RDX] = 1; cpu.r[RCX] = 1; cpu.r[R8] = 1; cpu.r[R9] = 1;
    PUSH64(&cpu, 3); PUSH64(&cpu, 0xffffffff00000005ull); PUSH64(&cpu, 4); PUSH64(&cpu, 1); PUSH64(&cpu, 1);   // i: high bits ignored
    PUSH64(&cpu, 0x1234);
    rt64_native_call(&cpu, (uint64_t)(uintptr_t)native_mixed);
    EXPECT(cpu.r[RAX] == 8 + 1000 * 4 + 100000 * 5 + 10000000ull * 3);
    // A 40-byte structure through the hidden pointer: rdi on x86-64 becomes x8, rax returns it.
    static const rt64_native_sig big = {1, 0, {0}};
    EXPECT(rt64_native_set_sig((uint64_t)(uintptr_t)native_make_big, &big) &&
           rt64_native_sig_of((uint64_t)(uintptr_t)native_make_big)->hidden_return);
    native_big result = {{0}};
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.r[RDI] = (uint64_t)(uintptr_t)&result; cpu.r[RSI] = 6; cpu.r[RDX] = 7;
    PUSH64(&cpu, 0x1234);
    rt64_native_call(&cpu, (uint64_t)(uintptr_t)native_make_big);
    EXPECT(result.v[0] == 6 && result.v[1] == 7 && result.v[2] == 13 && result.v[3] == 42 && result.v[4] == 77);
    EXPECT(cpu.r[RAX] == (uint64_t)(uintptr_t)&result);
    // A C++ interface's method by its vtable slot (steamclient.dylib has no method names).
    native_vtable vt = {{0, 0, (uint64_t (*)(void))(uintptr_t)native_twelve}};
    native_vtable *object = &vt;
    static const rt64_native_sig four = {0, 4, {8, 8, 8, 8}};
    EXPECT(rt64_native_set_vtable_sig((uint64_t)(uintptr_t)&object, 2, &four) &&
           rt64_native_sig_of((uint64_t)(uintptr_t)native_twelve)->stack_args == 4);

    // --wrap'd file functions of Steam runtime builds resolve to the plain shims.
    EXPECT(rt64_find_shim("__wrap_access") == rt64_find_shim("access") && rt64_find_shim("access"));
    EXPECT(rt64_find_shim("__wrap_fopen") == rt64_find_shim("fopen") && !rt64_find_shim("__wrap_"));
    // P2CE's Steam bridge answers by name (steam64.c), RestartAppIfNecessary never relaunches.
    EXPECT(rt64_find_shim("SteamInternal_SteamAPI_Init") && rt64_find_shim("SteamAPI_RestartAppIfNecessary"));
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
    cpu.r[RDI] = 440000;
    rt64_find_shim("SteamAPI_RestartAppIfNecessary")(&cpu);
    EXPECT(cpu.r[RAX] == 0);

    // epoll on kqueue (epoll64.c, round 28): a pipe's read end, Linux's packed 12-byte epoll_event.
    {
        int fds[2];
        EXPECT(pipe(fds) == 0);
        rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + 16384));
        cpu.r[RDI] = 1;
        rt64_find_shim("epoll_create")(&cpu);
        int ep = (int)cpu.r[RAX];
        EXPECT(ep >= 0);
        uint8_t ev[12] = {0}, out[2 * 12] = {0};
        uint32_t in = 0x001;
        uint64_t data = 0x1122334455667788ull;
        memcpy(ev, &in, 4);
        memcpy(ev + 4, &data, 8);
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = 1; cpu.r[RDX] = (uint64_t)fds[0]; cpu.r[RCX] = (uint64_t)(uintptr_t)ev;
        rt64_find_shim("epoll_ctl")(&cpu);
        EXPECT(cpu.r[RAX] == 0);
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = (uint64_t)(uintptr_t)out; cpu.r[RDX] = 2; cpu.r[RCX] = 0;
        rt64_find_shim("epoll_wait")(&cpu);
        EXPECT(cpu.r[RAX] == 0);   // nothing to read yet, no wait
        EXPECT(write(fds[1], "x", 1) == 1);
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = (uint64_t)(uintptr_t)out; cpu.r[RDX] = 2; cpu.r[RCX] = 1000;
        rt64_find_shim("epoll_wait")(&cpu);
        uint32_t got_events;
        uint64_t got_data;
        memcpy(&got_events, out, 4);
        memcpy(&got_data, out + 4, 8);
        EXPECT(cpu.r[RAX] == 1 && (got_events & 0x001) && got_data == data);
        close(fds[1]);   // the writer gone: EPOLLHUP
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = (uint64_t)(uintptr_t)out; cpu.r[RDX] = 2; cpu.r[RCX] = 1000;
        rt64_find_shim("epoll_wait")(&cpu);
        memcpy(&got_events, out, 4);
        EXPECT(cpu.r[RAX] == 1 && (got_events & 0x010));
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = 2; cpu.r[RDX] = (uint64_t)fds[0]; cpu.r[RCX] = 0;
        rt64_find_shim("epoll_ctl")(&cpu);
        EXPECT(cpu.r[RAX] == 0);
        cpu.r[RDI] = (uint64_t)ep; cpu.r[RSI] = (uint64_t)(uintptr_t)out; cpu.r[RDX] = 2; cpu.r[RCX] = 0;
        rt64_find_shim("epoll_wait")(&cpu);
        EXPECT(cpu.r[RAX] == 0);   // removed: nothing reported
        close(fds[0]);
        close(ep);
        // eventfd/timerfd: ENOSYS (Linux 38), so asio falls back to a pipe and epoll_wait's timeout.
        cpu.r[RDI] = 0; cpu.r[RSI] = 0;
        rt64_find_shim("eventfd")(&cpu);
        EXPECT((int64_t)cpu.r[RAX] == -1 && *rt64_errno_slot() == 38);
        rt64_find_shim("timerfd_create")(&cpu);
        EXPECT((int64_t)cpu.r[RAX] == -1 && *rt64_errno_slot() == 38);
    }

    if (failures) { fprintf(stderr, "x64-check: %d of %d checks failed\n", failures, checks); return 1; }
    printf("x64-check: PASS, %d checks (System V calls, 16 registers, RIP-relative data, "
           "flags, jump tables, imports, atomics, SSE2 to SSE4.1, x87, MMX, BMI, two modules, TLS, setjmp, threads)\n", checks);
    return 0;
}
