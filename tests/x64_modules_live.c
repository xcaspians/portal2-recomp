// Round C milestone of docs/fork/P2CE_PLAN.md (`make p2ce-modules-check`):
// P2CE's libtier0.so, libvstdlib.so and filesystem_stdio.so, lifted whole by
// tools/build64.py from the user's depot, are opened like dlopen does (Linux
// symbol order, relocations against each other, static TLS, constructors),
// then CommandLine()->CheckParm() and the file system's CreateInterface run.
#include "x64/rt64.h"

#include <stdio.h>
#include <stdlib.h>

extern const RT64Module m_mimalloc, m_tier0, m_vstdlib, m_filesystem_stdio, m_x64except;
extern char **environ;
static CPU64 cpu;
static int failures, checks;

#define EXPECT(cond) do { checks++; if (!(cond)) { failures++; \
    fprintf(stderr, "p2ce-modules: %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)
#define ADDR(p) ((uint64_t)(uintptr_t)(p))

static uint64_t call(uint64_t target, uint64_t a0, uint64_t a1, uint64_t a2) {
    cpu.r[RDI] = a0; cpu.r[RSI] = a1; cpu.r[RDX] = a2;
    rt64_call(&cpu, target);
    return cpu.r[RAX];
}

// ICommandLine (Source SDK order, checked against the vtable of P2CE's
// CCommandLine): CreateCmdLine(const char *), CreateCmdLine(argc, argv),
// GetCmdLine(), CheckParm(name, &value), ...
enum { CREATE_CMD_LINE = 0, GET_CMD_LINE = 2, CHECK_PARM = 3 };
static uint64_t method(uint64_t object, int index) { return R64(R64(object) + 8 * (uint64_t)index); }

int main(int argc, char **argv) {
    static uint64_t stack_[1 << 17];
    rt64_set_main_arguments(argc, argv, environ);
    rt64_cpu_init(&cpu, (uint64_t)(uintptr_t)(stack_ + (1 << 17)));
    rt64_thread_tls(&cpu);
    // Registered by hand: this also links the static libraries' tables in.
    rt64_register(&m_mimalloc); rt64_register(&m_tier0); rt64_register(&m_vstdlib); rt64_register(&m_filesystem_stdio);
    rt64_register(&m_x64except);

    EXPECT(rt64_open(&cpu, "filesystem_stdio.so") != NULL);        // pulls its DT_NEEDED modules first
    EXPECT(rt64_loaded_count() == 4);                               // libmimalloc, libtier0, libvstdlib
    for (size_t i = 0; i < rt64_loaded_count(); ++i) {
        uint64_t base;
        const RT64Module *m = rt64_loaded_module(i, &base);
        printf("  %-22s at %#llx\n", m->name, (unsigned long long)base);
    }

    uint64_t command_line = call(rt64_export(&m_tier0, "CommandLine"), 0, 0, 0);
    EXPECT(command_line != 0);
    call(method(command_line, CREATE_CMD_LINE), command_line, ADDR("p2ce -game p2ce -novid +map sp_a1_intro1"), 0);
    const char *line = (const char *)(uintptr_t)call(method(command_line, GET_CMD_LINE), command_line, 0, 0);
    printf("  CommandLine()->GetCmdLine() = \"%s\"\n", line ? line : "(null)");
    uint64_t value = 0;
    const char *game = (const char *)(uintptr_t)call(method(command_line, CHECK_PARM), command_line, ADDR("-game"), ADDR(&value));
    printf("  CheckParm(\"-game\") = \"%s\", value \"%s\"\n", game ? game : "(null)",
           value ? (const char *)(uintptr_t)value : "(null)");
    EXPECT(game && !strcmp(game, "-game") && value && !strcmp((const char *)(uintptr_t)value, "p2ce"));
    EXPECT(call(method(command_line, CHECK_PARM), command_line, ADDR("-novid"), 0) != 0);
    EXPECT(call(method(command_line, CHECK_PARM), command_line, ADDR("-windowed"), 0) == 0);

    // filesystem_stdio's factory: the engine asks it for its file system.
    uint64_t factory = rt64_export(&m_filesystem_stdio, "CreateInterface");
    EXPECT(factory != 0);
    int return_code = -1;
    uint64_t filesystem = call(factory, ADDR("VFileSystem018"), ADDR(&return_code), 0);
    printf("  CreateInterface(\"VFileSystem018\") = %#llx, return code %d\n", (unsigned long long)filesystem, return_code);
    EXPECT(filesystem != 0 && return_code == 0);
    EXPECT(call(factory, ADDR("NoSuchInterface001"), ADDR(&return_code), 0) == 0 && return_code == 1);

    // C++ exceptions: tests/x64_except.cpp, thrown and caught by P2CE's own
    // C++ runtime and unwinder (in libmimalloc.so), through lifted frames.
    EXPECT(rt64_open(&cpu, "libx64except.so") != NULL);
    uint64_t here = rt64_export(&m_x64except, "x64e_catch_here");
    uint64_t up = rt64_export(&m_x64except, "x64e_catch_two_frames_up");
    uint64_t all = rt64_export(&m_x64except, "x64e_catch_all");
    EXPECT(call(here, 0, 0, 0) == 0);
    EXPECT(call(here, 41, 0, 0) == 42);                    // throw and catch in one function
    EXPECT(call(up, 21, 0, 0) == 42);                      // unwound through two lifted frames
    EXPECT(call(up, -1, 0, 0) == 999);                     // no throw
    EXPECT(call(all, 3, 0, 0) == 7);                       // catch (...)
    printf("  C++ exceptions: thrown, unwound and caught in lifted code\n");

    if (failures) { fprintf(stderr, "p2ce-modules: %d of %d checks failed\n", failures, checks); return 1; }
    printf("p2ce-modules: PASS, %d checks (4 P2CE modules lifted whole, constructors, CheckParm, C++ exceptions)\n", checks);
    return 0;
}
