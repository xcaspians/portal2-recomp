// PE32+ modules on the x86-64 runtime (tools/peprep64.py): the TEB and PEB
// read through gs:, static TLS through the TEB's ThreadLocalStoragePointer,
// Control Flow Guard's two pointers, and host -> guest calls with the
// Windows convention. See win64.h. Nothing here is Windows code: these are
// the few structures MSVC-built code reads directly.
#include "win64.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define PE_TLS_MAX 64

typedef struct { const RT64Module *module; uint64_t base; } PeTls;
static PeTls pe_tls[PE_TLS_MAX];
static size_t pe_tls_count;
static uint64_t *tebs;
static size_t teb_count, teb_capacity;
static uint64_t peb;
static uint32_t next_thread_id = 0x1000;
static pthread_mutex_t pe_lock = PTHREAD_MUTEX_INITIALIZER;

static uint64_t zeroed_pages(size_t size) {
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (p == MAP_FAILED) { perror("rt64: pe64"); abort(); }
    return (uint64_t)(uintptr_t)p;
}

uint64_t rt64_win_peb(void) {
    pthread_mutex_lock(&pe_lock);
    if (!peb) {
        peb = zeroed_pages(PEB_SIZE);
        W32(peb + PEB_OS_MAJOR, 10);          // Windows 10 22H2
        W32(peb + PEB_OS_MINOR, 0);
        W16(peb + PEB_OS_BUILD, 19045);
        W32(peb + PEB_PLATFORM, 2);           // VER_PLATFORM_WIN32_NT
        W32(peb + PEB_SESSION, 1);
    }
    pthread_mutex_unlock(&pe_lock);
    return peb;
}

// A copy of a module's TLS template (zero fill after it).
static uint64_t tls_block(const PeTls *t) {
    const RT64Module *m = t->module;
    size_t size = m->tls_memsz ? m->tls_memsz : 16;
    size_t alignment = m->tls_align > 16 ? m->tls_align : 16;
    void *block = NULL;
    if (posix_memalign(&block, alignment, (size + alignment - 1) & ~(alignment - 1))) abort();
    memset(block, 0, size);
    if (m->tls_filesz) memcpy(block, (const void *)(uintptr_t)(t->base + m->tls_vaddr), m->tls_filesz);
    return (uint64_t)(uintptr_t)block;
}

// Called by the loader once a PE module is mapped and relocated: its TLS
// index goes into its index slot, and every existing thread gets its block.
void rt64_pe_loaded(const RT64Module *module, uint64_t base) {
    if (!module->pe_tls_index) return;
    pthread_mutex_lock(&pe_lock);
    if (pe_tls_count == PE_TLS_MAX) { fprintf(stderr, "rt64: %s: too many PE TLS modules\n", module->name); abort(); }
    size_t index = pe_tls_count++;
    pe_tls[index] = (PeTls){module, base};
    W32(base + module->pe_tls_index, (uint32_t)index);
    for (size_t i = 0; i < teb_count; ++i)
        W64(R64(tebs[i] + TEB_TLS_POINTER) + 8 * index, tls_block(&pe_tls[index]));
    pthread_mutex_unlock(&pe_lock);
}

uint64_t rt64_win_thread(CPU64 *c, uint64_t stack_low, uint64_t stack_high) {
    uint64_t teb = zeroed_pages(TEB_SIZE);
    uint64_t tls = zeroed_pages(8 * PE_TLS_MAX);
    W64(teb + TEB_EXCEPTION_LIST, ~UINT64_C(0));
    W64(teb + TEB_STACK_BASE, stack_high);
    W64(teb + TEB_STACK_LIMIT, stack_low);
    W64(teb + TEB_SELF, teb);
    W64(teb + TEB_PROCESS_ID, (uint64_t)getpid());
    W64(teb + TEB_TLS_POINTER, tls);
    W64(teb + TEB_PEB, rt64_win_peb());
    pthread_mutex_lock(&pe_lock);
    W64(teb + TEB_THREAD_ID, next_thread_id);
    next_thread_id += 4;
    for (size_t i = 0; i < pe_tls_count; ++i) W64(tls + 8 * i, tls_block(&pe_tls[i]));
    if (teb_count == teb_capacity) {
        teb_capacity = teb_capacity ? 2 * teb_capacity : 16;
        tebs = realloc(tebs, teb_capacity * sizeof *tebs);
    }
    tebs[teb_count++] = teb;
    pthread_mutex_unlock(&pe_lock);
    c->gs_base = teb;
    c->abi = RT64_ABI_WIN64;        // native calls from this thread take the Windows convention
    return teb;
}

uint32_t rt64_win_thread_id(const CPU64 *c) {
    return c->gs_base ? (uint32_t)R64(c->gs_base + TEB_THREAD_ID) : 0;
}

uint64_t rt64_win_call(CPU64 *c, uint64_t target, int argc, const uint64_t *args) {
    uint64_t saved[16];
    memcpy(saved, c->r, sizeof saved);
    int slots = argc > 4 ? argc : 4;
    c->r[RSP] = (c->r[RSP] - 8 * (uint64_t)slots - 64) & ~UINT64_C(15);
    for (int i = 0; i < argc; ++i) {
        if (i < 4) c->r[(const int[]){RCX, RDX, R8, R9}[i]] = args[i];
        else W64(c->r[RSP] + 8 * (uint64_t)i, args[i]);
    }
    rt64_call(c, target);
    uint64_t result = c->r[RAX];
    memcpy(c->r, saved, sizeof saved);
    return result;
}

// Control Flow Guard (the load configuration's two pointers): every target
// is valid, and the dispatcher jumps to rax with the caller's frame, so the
// target returns straight to the caller.
static void guard_check_icall(CPU64 *c) { (void)c; }
static void guard_dispatch_icall(CPU64 *c) {
    rt64_dispatch(c, c->r[RAX]);
    c->transferred = 1;             // the target's RET already returned to the caller
}

static const RT64Shim shims[] = {
    {"rt64!guard_check_icall", guard_check_icall},
    {"rt64!guard_dispatch_icall", guard_dispatch_icall},
    {NULL, NULL},
};
static const RT64Data data[] = {{NULL, NULL}};
RT64_SHIMS(shims, data)
