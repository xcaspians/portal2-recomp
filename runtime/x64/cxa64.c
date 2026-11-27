// Process and C++ ABI shims of the x86-64 runtime: exit handlers
// (__cxa_atexit, atexit, __cxa_finalize), thread-local storage
// (__tls_get_addr), stack protector, fork handlers, process identity and
// system configuration. P2CE links libstdc++ statically into each module,
// so the C++ runtime itself (operator new, exceptions, RTTI) is guest code.
#include "rt64.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <unistd.h>

static void s_cxa_atexit(CPU64 *c) { rt64_add_exit_handler(ARG(c, 0), ARG(c, 1), ARG(c, 2), 1); RET(c, 0); }
static void s_atexit(CPU64 *c) { rt64_add_exit_handler(ARG(c, 0), 0, 0, 0); RET(c, 0); }
static void s_cxa_thread_atexit(CPU64 *c) { rt64_add_exit_handler(ARG(c, 0), ARG(c, 1), ARG(c, 2), 1); RET(c, 0); }
static void s_cxa_finalize(CPU64 *c) { if (ARG(c, 0)) rt64_run_exit_handlers(c, ARG(c, 0)); }
void rt64_print_backtrace(CPU64 *c);   // loader64.c
// RT64_VERBOSE: who asked to leave, and with what status.
static void report_exit(CPU64 *c, const char *how, int status) {
    if (!getenv("RT64_VERBOSE")) return;
    fprintf(stderr, "rt64: %s(%d) called from:\n", how, status);
    rt64_print_backtrace(c);
}
static void s_exit(CPU64 *c) {
    int status = (int)ARG(c, 0);
    report_exit(c, "exit", status);
    rt64_run_exit_handlers(c, 0);
    fflush(NULL);
    rt64_program_exit(c, status);
}
// __libc_start_main(main, argc, argv, init, fini, rtld_fini, stack_end): the
// constructors already ran (rt64_open); main(argc, argv, envp), then exit.
static void s_libc_start_main(CPU64 *c) {
    uint64_t main_function = ARG(c, 0), argc = ARG(c, 1), argv = ARG(c, 2);
    uint64_t envp = argv + 8 * (argc + 1);
    c->r[RDI] = argc; c->r[RSI] = argv; c->r[RDX] = envp;
    c->r[RSP] &= ~UINT64_C(15);
    rt64_call(c, main_function);
    c->r[RDI] = (uint32_t)c->r[RAX];
    s_exit(c);
}
static void s_quick_exit(CPU64 *c) { report_exit(c, "_exit", (int)ARG(c, 0)); fflush(NULL); _exit((int)ARG(c, 0)); }
// struct tls_index { unsigned long module, offset; }
static void s_tls_get_addr(CPU64 *c) { RET(c, rt64_tls_get_addr(c, R64(ARG(c, 0)), R64(ARG(c, 0) + 8))); }
static void s_stack_chk_fail(CPU64 *c) { rt64_trap(c, R64(c->r[RSP]), "stack smashing detected (__stack_chk_fail)"); }
static void s_cxa_pure_virtual(CPU64 *c) { rt64_trap(c, R64(c->r[RSP]), "pure virtual method called"); }
static void s_register_atfork(CPU64 *c) { RET(c, 0); }   // no fork of a lifted process
static void s_getpid(CPU64 *c) { RET(c, (int64_t)getpid()); }
static void s_getppid(CPU64 *c) { RET(c, (int64_t)getppid()); }
static void s_getuid(CPU64 *c) { RET(c, (int64_t)getuid()); }
static void s_geteuid(CPU64 *c) { RET(c, (int64_t)geteuid()); }
static void s_getgid(CPU64 *c) { RET(c, (int64_t)getgid()); }
static void s_getpagesize(CPU64 *c) { RET(c, 4096); }    // the guest's page size (Linux x86-64)
static unsigned cpu_count(void) {
    int n = 0; size_t size = sizeof n;
    return sysctlbyname("hw.logicalcpu", &n, &size, NULL, 0) || n < 1 ? 1u : (unsigned)n;
}
static void s_get_nprocs(CPU64 *c) { RET(c, (int64_t)cpu_count()); }
// sysconf: Linux _SC_* numbers.
static void s_sysconf(CPU64 *c) {
    switch ((int)ARG(c, 0)) {
    case 2: RET(c, 100); break;                                   // _SC_CLK_TCK
    case 4: RET(c, 1024); break;                                  // _SC_OPEN_MAX
    case 30: RET(c, 4096); break;                                 // _SC_PAGESIZE
    case 83: case 84: RET(c, (int64_t)cpu_count()); break;        // _SC_NPROCESSORS_CONF/ONLN
    case 85: RET(c, sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / 4096); break;   // _SC_PHYS_PAGES
    case 0: RET(c, 2097152); break;                               // _SC_ARG_MAX
    case 3: RET(c, 65536); break;                                 // _SC_NGROUPS_MAX
    default: rt64_set_errno(22); RET(c, -1); break;
    }
}

// qsort/bsearch with a guest comparator.
typedef struct { CPU64 *c; uint64_t compare; } Comparator;
static int compare_guest(void *context, const void *x, const void *y) {
    Comparator *k = context;
    uint64_t saved[16];
    memcpy(saved, k->c->r, sizeof saved);
    k->c->r[RDI] = (uintptr_t)x; k->c->r[RSI] = (uintptr_t)y;
    rt64_call(k->c, k->compare);
    int r = (int)k->c->r[RAX];
    memcpy(k->c->r, saved, sizeof saved);
    return r;
}
static void s_qsort(CPU64 *c) {
    Comparator k = {c, ARG(c, 3)};
    qsort_r(PTR(c, 0), (size_t)ARG(c, 1), (size_t)ARG(c, 2), &k, compare_guest);
}
static void s_bsearch(CPU64 *c) {
    Comparator k = {c, ARG(c, 4)};
    const char *base = PTR(c, 1);
    size_t lo = 0, hi = (size_t)ARG(c, 2), size = (size_t)ARG(c, 3);
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        int r = compare_guest(&k, PTR(c, 0), base + mid * size);
        if (!r) { RET(c, (uintptr_t)(base + mid * size)); return; }
        if (r < 0) hi = mid; else lo = mid + 1;
    }
    RET(c, 0);
}
// Signals: handlers are recorded but never delivered yet (the guest's
// faults are the host's); masks and alternate stacks succeed.
static void s_signal_ok(CPU64 *c) { RET(c, 0); }
static void s_sigemptyset(CPU64 *c) { memset(PTR(c, 0), 0, 128); RET(c, 0); }
static void s_sigfillset(CPU64 *c) { memset(PTR(c, 0), 0xff, 128); RET(c, 0); }
static void s_sigaddset(CPU64 *c) {
    int n = (int)ARG(c, 1) - 1;
    if (n >= 0 && n < 1024) W64(ARG(c, 0) + 8 * (uint64_t)(n / 64), R64(ARG(c, 0) + 8 * (uint64_t)(n / 64)) | (UINT64_C(1) << (n % 64)));
    RET(c, 0);
}
static void s_raise(CPU64 *c) {
    char why[64];
    snprintf(why, sizeof why, "raise(%d) called by the guest", (int)ARG(c, 0));
    rt64_trap(c, R64(c->r[RSP]), why);
}
static void s_alarm(CPU64 *c) { RET(c, 0); }

// setjmp/longjmp. The guest's jmp_buf (200 bytes on Linux) holds rbx, rbp,
// r12-r15, the stack pointer after the call and the return address. longjmp
// restores them and returns from the setjmp call again: the lifted functions
// between the two leave their host frames until the one that called setjmp
// finds its resume point (tools/lift64.py, `resume:`).
static void s_setjmp(CPU64 *c) {
    uint64_t buf = ARG(c, 0);
    static const int saved[6] = {RBX, RBP, R12, R13, R14, R15};
    for (int i = 0; i < 6; ++i) W64(buf + 8 * (uint64_t)i, c->r[saved[i]]);
    W64(buf + 48, c->r[RSP] + 8);
    W64(buf + 56, R64(c->r[RSP]));
    W32(buf + 64, 0);
    RET(c, 0);
}
static void s_longjmp(CPU64 *c) {
    uint64_t buf = ARG(c, 0);
    int value = (int)ARG(c, 1);
    static const int saved[6] = {RBX, RBP, R12, R13, R14, R15};
    for (int i = 0; i < 6; ++i) c->r[saved[i]] = R64(buf + 8 * (uint64_t)i);
    c->r[RSP] = R64(buf + 48);
    c->rip = R64(buf + 56);
    c->r[RAX] = (uint64_t)(uint32_t)(value ? value : 1);
    c->transferred = 1;
}

// No child processes from a lifted program (zenity message boxes, crash
// handlers): fork, exec and spawn fail with EAGAIN/ENOSYS.
static void s_fork(CPU64 *c) { rt64_set_errno(EAGAIN); RET(c, -1); }
static void s_exec(CPU64 *c) { rt64_set_errno(ENOSYS); RET(c, -1); }
static void s_spawn(CPU64 *c) { RET(c, 11); }                    // returns the error (EAGAIN)
static void s_waitpid(CPU64 *c) { rt64_set_errno(ECHILD); RET(c, -1); }

static const char *program_name = "p2ce";
static const char *program_full = "p2ce";

static const RT64Shim shims[] = {
    {"__cxa_atexit", s_cxa_atexit}, {"atexit", s_atexit}, {"__cxa_thread_atexit_impl", s_cxa_thread_atexit},
    {"__cxa_finalize", s_cxa_finalize}, {"exit", s_exit}, {"__libc_start_main", s_libc_start_main}, {"_exit", s_quick_exit}, {"_Exit", s_quick_exit},
    {"quick_exit", s_quick_exit}, {"__tls_get_addr", s_tls_get_addr}, {"__stack_chk_fail", s_stack_chk_fail},
    {"__cxa_pure_virtual", s_cxa_pure_virtual}, {"__register_atfork", s_register_atfork},
    {"pthread_atfork", s_register_atfork}, {"getpid", s_getpid}, {"getppid", s_getppid}, {"getuid", s_getuid},
    {"geteuid", s_geteuid}, {"getgid", s_getgid}, {"getpagesize", s_getpagesize}, {"get_nprocs", s_get_nprocs},
    {"get_nprocs_conf", s_get_nprocs}, {"sysconf", s_sysconf}, {"__sysconf", s_sysconf},
    {"qsort", s_qsort}, {"bsearch", s_bsearch}, {"signal", s_signal_ok}, {"sigaction", s_signal_ok},
    {"sigprocmask", s_signal_ok}, {"pthread_sigmask", s_signal_ok}, {"sigaltstack", s_signal_ok},
    {"sigemptyset", s_sigemptyset}, {"sigfillset", s_sigfillset}, {"sigaddset", s_sigaddset},
    {"raise", s_raise}, {"alarm", s_alarm}, {"fork", s_fork}, {"vfork", s_fork}, {"execv", s_exec},
    {"execve", s_exec}, {"execvp", s_exec}, {"posix_spawn", s_spawn}, {"posix_spawnp", s_spawn},
    {"waitpid", s_waitpid}, {"wait", s_waitpid},
    {"_setjmp", s_setjmp}, {"setjmp", s_setjmp}, {"__sigsetjmp", s_setjmp}, {"sigsetjmp", s_setjmp},
    {"longjmp", s_longjmp}, {"_longjmp", s_longjmp}, {"siglongjmp", s_longjmp}, {"__longjmp_chk", s_longjmp},
    {NULL, NULL},
};
extern char **environ;
static const RT64Data data[] = {
    {"__progname", &program_name}, {"program_invocation_short_name", &program_name},
    {"__progname_full", &program_full}, {"program_invocation_name", &program_full},
    {"environ", &environ}, {"__environ", &environ},
    {NULL, NULL},
};
RT64_SHIMS(shims, data)
