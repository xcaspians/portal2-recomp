// x86-64 module loader: registry of the lifted modules, loading with
// relocations against other modules' exports, shims and data imports,
// static TLS, constructors and exit handlers, and the dispatcher that maps
// a guest code address to its lifted function. See rt64.h.
#include "rt64.h"
#include "native_libs64.h"

#include <errno.h>
#include <setjmp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define RT64_MAX_MODULES 256
#define RT64_IMPORT_STRIDE 16
// Return address pushed by rt64_call: never guest code (non-canonical).
#define RT64_SENTINEL UINT64_C(0xdead5e0000000000)
// Static TLS reserved for every thread: all modules' PT_TLS blocks, plus
// room for modules opened after the thread started.
#define RT64_TLS_AREA (1u << 20)
#define RT64_TCB_SIZE 0x700

struct RT64Loaded {
    const RT64Module *module;
    uint64_t base;
    uint64_t imports;              // PROT_NONE range: one address per import
    rt64_shim *shims;              // per import: the shim when it resolved to one
    uint32_t *hash; uint32_t hash_mask;   // export name -> index + 1
    uint64_t modid;                // TLS module id (1-based)
    uint64_t tls_offset;           // block at thread pointer - tls_offset
    int initialised;
};

static RT64Loaded loaded[RT64_MAX_MODULES];
static size_t loaded_count;
static const RT64Module *registry[RT64_MAX_MODULES];
static size_t registry_count;
static pthread_mutex_t loader_lock = PTHREAD_MUTEX_INITIALIZER;
static uint64_t tls_used;                       // bytes of static TLS assigned
static uint64_t *thread_pointers;               // every thread pointer handed out
static size_t thread_count, thread_capacity;
static int verbose = -1;

static int is_verbose(void) {
    if (verbose < 0) verbose = getenv("RT64_VERBOSE") != NULL;
    return verbose;
}

void rt64_register(const RT64Module *module) {
    for (size_t i = 0; i < registry_count; ++i)
        if (registry[i] == module) return;
    if (registry_count < RT64_MAX_MODULES) registry[registry_count++] = module;
}

static const char *basename_of(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

const RT64Module *rt64_registered(const char *name) {
    const char *base = basename_of(name);
    for (size_t i = 0; i < registry_count; ++i)
        if (!strcmp(registry[i]->name, base)) return registry[i];
    return NULL;
}

static RT64Loaded *find_loaded(const RT64Module *module) {
    for (size_t i = 0; i < loaded_count; ++i)
        if (loaded[i].module == module) return &loaded[i];
    return NULL;
}

size_t rt64_loaded_count(void) { return loaded_count; }
const RT64Module *rt64_loaded_module(size_t index, uint64_t *base) {
    if (index >= loaded_count) return NULL;
    if (base) *base = loaded[index].base;
    return loaded[index].module;
}

const RT64Module *rt64_module_at(uint64_t address, uint64_t *base) {
    for (size_t i = 0; i < loaded_count; ++i)
        if (address >= loaded[i].base && address < loaded[i].base + loaded[i].module->size) {
            if (base) *base = loaded[i].base;
            return loaded[i].module;
        }
    return NULL;
}

// The guest return addresses on the stack, for a rough backtrace (traps, and
// exit/_exit under RT64_VERBOSE). Declared where it's used, not in rt64.h.
void rt64_print_backtrace(CPU64 *c);
void rt64_print_backtrace(CPU64 *c) {
    uint64_t base;
    for (int i = 0, shown = 0; i < 512 && shown < 12; ++i) {
        uint64_t word = R64(c->r[RSP] + 8 * (uint64_t)i);
        const RT64Module *owner = rt64_module_at(word, &base);
        if (owner) { fprintf(stderr, "rt64:   [rsp+%#x] %s+%#llx\n", 8 * i, owner->name,
                             (unsigned long long)(word - base)); shown++; }
    }
}

void rt64_trap(CPU64 *c, uint64_t pc, const char *why) {
    fprintf(stderr, "rt64: trap at %#llx: %s (rsp %#llx)\n", (unsigned long long)pc, why,
            c ? (unsigned long long)c->r[RSP] : 0ull);
    uint64_t base;
    const RT64Module *m = rt64_module_at(pc, &base);
    if (m) fprintf(stderr, "rt64:   %s+%#llx\n", m->name, (unsigned long long)(pc - base));
    if (c) rt64_print_backtrace(c);
    abort();
}

void rt64_bad_return(CPU64 *c, uint64_t expected) {
    char why[96];
    snprintf(why, sizeof why, "returned to %#llx instead of %#llx (longjmp/exceptions are not supported yet)",
             (unsigned long long)c->rip, (unsigned long long)expected);
    rt64_trap(c, expected, why);
}

static uint32_t hash_name(const char *s) {
    uint32_t h = 2166136261u;
    while (*s) h = (h ^ (uint8_t)*s++) * 16777619u;
    return h;
}

static void build_hash(RT64Loaded *l) {
    uint32_t size = 16;
    while (size < 2 * l->module->export_count) size <<= 1;
    l->hash = calloc(size, sizeof *l->hash);
    l->hash_mask = size - 1;
    for (size_t i = 0; i < l->module->export_count; ++i) {
        uint32_t h = hash_name(l->module->exports[i].name) & l->hash_mask;
        while (l->hash[h]) h = (h + 1) & l->hash_mask;
        l->hash[h] = (uint32_t)i + 1;
    }
}

static const RT64Export *find_export(const RT64Loaded *l, const char *name) {
    if (!l->hash) return NULL;
    for (uint32_t h = hash_name(name) & l->hash_mask; l->hash[h]; h = (h + 1) & l->hash_mask) {
        const RT64Export *e = &l->module->exports[l->hash[h] - 1];
        if (!strcmp(e->name, name)) return e;
    }
    return NULL;
}

uint64_t rt64_export(const RT64Module *module, const char *name) {
    const RT64Loaded *l = find_loaded(module);
    const RT64Export *e = l ? find_export(l, name) : NULL;
    return e ? l->base + e->offset : 0;
}

static const RT64Loaded *defining_module(const char *name, const RT64Export **export) {
    for (size_t i = 0; i < loaded_count; ++i) {
        const RT64Export *e = find_export(&loaded[i], name);
        if (e) { *export = e; return &loaded[i]; }
    }
    return NULL;
}

uint64_t rt64_lookup_global(const char *name) {
    const RT64Export *e;
    const RT64Loaded *l = defining_module(name, &e);
    return l ? l->base + e->offset : 0;
}

uint64_t rt64_import_address(const RT64Module *module, size_t index) {
    const RT64Loaded *l = find_loaded(module);
    return l && index < module->import_count ? l->imports + RT64_IMPORT_STRIDE * index : 0;
}

static int ignorable_weak(const char *name) {
    return !strncmp(name, "_ITM_", 5) || !strncmp(name, "_ZGTt", 5) || !strcmp(name, "__gmon_start__");
}

// Address an import resolves to: another module's export, else a host
// object for data, else the import's own thunk address (a shim, or a trap
// naming it when called). Weak undefined symbols resolve to 0.
static uint64_t resolve_import(RT64Loaded *l, size_t index) {
    const RT64Import *import = &l->module->imports[index];
    const RT64Export *e;
    const RT64Loaded *owner = defining_module(import->name, &e);
    if (owner) return owner->base + e->offset;
    if (import->flags & RT64_IMPORT_OBJECT) {
        void *data = rt64_find_data(import->name);
        if (!data) {
            if (import->flags & RT64_IMPORT_WEAK) return 0;
            data = calloc(1, 256);          // unknown data object: zeroed, reported
            if (is_verbose()) fprintf(stderr, "rt64: %s: data import %s has no host object (zeroed)\n",
                                      l->module->name, import->name);
        }
        return (uint64_t)(uintptr_t)data;
    }
    l->shims[index] = rt64_find_shim(import->name);
    if (!l->shims[index]) {   // a native library standing in for a Linux one (native_libs64.h)
        uint64_t native = rt64_native_symbol(import->name);
        if (native) return native;
    }
    if (!l->shims[index] && (import->flags & RT64_IMPORT_WEAK) && ignorable_weak(import->name)) return 0;
    if (!l->shims[index] && (import->flags & RT64_IMPORT_WEAK)) return 0;
    return l->imports + RT64_IMPORT_STRIDE * index;
}

static void copy_tls_template(const RT64Loaded *l, uint64_t thread_pointer) {
    const RT64Module *m = l->module;
    uint8_t *block = (uint8_t *)(uintptr_t)(thread_pointer - l->tls_offset);
    memcpy(block, (const void *)(uintptr_t)(l->base + m->tls_vaddr), m->tls_filesz);
    memset(block + m->tls_filesz, 0, m->tls_memsz - m->tls_filesz);
}

static uint64_t align_up(uint64_t value, uint64_t alignment) {
    return alignment > 1 ? (value + alignment - 1) & ~(alignment - 1) : value;
}

static int load_locked(const RT64Module *module, const char *image_path) {
    if (find_loaded(module)) return 0;
    if (loaded_count == RT64_MAX_MODULES) return -1;
    if (!image_path) image_path = module->image_path;
    FILE *f = fopen(image_path, "rb");
    if (!f) { fprintf(stderr, "rt64: %s: %s\n", image_path, strerror(errno)); return -1; }
    uint8_t *base = mmap(NULL, module->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    size_t got = base == MAP_FAILED ? 0 : fread(base, 1, module->size, f);
    fclose(f);
    if (base == MAP_FAILED || got != module->size) {
        fprintf(stderr, "rt64: %s: short image (%zu of %llu bytes)\n", image_path, got, (unsigned long long)module->size);
        return -1;
    }
    size_t import_span = RT64_IMPORT_STRIDE * (module->import_count ? module->import_count : 1);
    void *imports = mmap(NULL, import_span, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (imports == MAP_FAILED) return -1;
    RT64Loaded *l = &loaded[loaded_count];
    memset(l, 0, sizeof *l);
    l->module = module; l->base = (uint64_t)(uintptr_t)base; l->imports = (uint64_t)(uintptr_t)imports;
    l->shims = calloc(module->import_count + 1, sizeof *l->shims);
    l->modid = loaded_count + 1;
    build_hash(l);
    if (module->tls_memsz) {
        uint64_t end = align_up(tls_used + module->tls_memsz, module->tls_align ? module->tls_align : 1);
        if (end > RT64_TLS_AREA) { fprintf(stderr, "rt64: %s: static TLS area full\n", module->name); return -1; }
        tls_used = end;
        l->tls_offset = end;
    }
    *module->base = l->base;
    loaded_count++;     // visible to symbol lookups from here on (a module may import its own exports)
    uint64_t *resolved = malloc((module->import_count + 1) * sizeof *resolved);
    for (size_t i = 0; i < module->import_count; ++i) resolved[i] = resolve_import(l, i);
    for (size_t i = 0; i < module->reloc_count; ++i) {
        const RT64Reloc *r = &module->relocs[i];
        uint64_t value;
        switch (r->kind) {
        case RT64_REL_BASE: value = l->base + (uint64_t)r->addend; break;
        case RT64_REL_IMPORT: value = resolved[r->index] ? resolved[r->index] + (uint64_t)r->addend : 0; break;
        case RT64_REL_MODID: value = l->modid; break;
        case RT64_REL_TPOFF: value = (uint64_t)r->addend - l->tls_offset; break;
        case RT64_REL_IMPORT_TLS: {
            const RT64Export *e;
            const RT64Loaded *owner = defining_module(module->imports[r->index].name, &e);
            if (!owner) { fprintf(stderr, "rt64: %s: TLS symbol %s not found\n", module->name,
                                  module->imports[r->index].name); value = 0; break; }
            value = r->addend == 0 ? owner->modid : r->addend == 2 ? e->offset : e->offset - owner->tls_offset;
            break;
        }
        default: value = (uint64_t)r->addend; break;
        }
        memcpy(base + r->offset, &value, 8);
    }
    free(resolved);
    // Threads that already exist get this module's TLS block too.
    if (module->tls_memsz)
        for (size_t i = 0; i < thread_count; ++i) copy_tls_template(l, thread_pointers[i]);
    if (is_verbose()) fprintf(stderr, "rt64: loaded %s at %#llx\n", module->name, (unsigned long long)l->base);
    return 0;
}

int rt64_load(const RT64Module *module, const char *image_path) {
    pthread_mutex_lock(&loader_lock);
    int result = load_locked(module, image_path);
    pthread_mutex_unlock(&loader_lock);
    return result;
}

static int main_argc;
static uint64_t main_argv, main_envp;
void rt64_set_main_arguments(int argc, char **argv, char **envp) {
    main_argc = argc; main_argv = (uint64_t)(uintptr_t)argv; main_envp = (uint64_t)(uintptr_t)envp;
}

static void call_with_arguments(CPU64 *c, uint64_t target) {
    uint64_t saved[3] = {c->r[RDI], c->r[RSI], c->r[RDX]};
    c->r[RDI] = (uint64_t)main_argc; c->r[RSI] = main_argv; c->r[RDX] = main_envp;
    rt64_call(c, target);
    c->r[RDI] = saved[0]; c->r[RSI] = saved[1]; c->r[RDX] = saved[2];
}

void rt64_run_constructors(CPU64 *c, const RT64Module *module) {
    RT64Loaded *l = find_loaded(module);
    if (!l || l->initialised) return;
    l->initialised = 1;
    if (is_verbose()) fprintf(stderr, "rt64: constructors of %s\n", module->name);
    if (module->init) call_with_arguments(c, l->base + module->init);
    for (size_t i = 0; i < module->init_count; ++i)
        if (module->init_array[i] && module->init_array[i] != UINT64_MAX)
            call_with_arguments(c, l->base + module->init_array[i]);
}

RT64Loaded *rt64_open(CPU64 *c, const char *name) {
    const RT64Module *module = rt64_registered(name);
    if (!module) return NULL;
    RT64Loaded *l = find_loaded(module);
    if (l && l->initialised) return l;
    // DT_NEEDED first (lifted ones; system libraries are shims).
    for (size_t i = 0; i < module->needed_count; ++i)
        if (rt64_registered(module->needed[i]) && !rt64_open(c, module->needed[i])) return NULL;
    if (rt64_load(module, NULL)) return NULL;
    rt64_run_constructors(c, module);
    return find_loaded(module);
}

// TLS ---------------------------------------------------------------------

void rt64_thread_tls(CPU64 *c) {
    uint8_t *area = mmap(NULL, RT64_TLS_AREA + RT64_TCB_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    uint64_t tp = (uint64_t)(uintptr_t)(area + RT64_TLS_AREA);
    W64(tp, tp);                                  // fs:0, the TCB's self pointer
    W64(tp + 0x10, tp);                           // tcbhead_t.self
    W64(tp + 0x28, UINT64_C(0x2f8a3c1d00000000) ^ (uint64_t)arc4random());  // stack guard
    pthread_mutex_lock(&loader_lock);
    for (size_t i = 0; i < loaded_count; ++i)
        if (loaded[i].module->tls_memsz) copy_tls_template(&loaded[i], tp);
    if (thread_count == thread_capacity) {
        thread_capacity = thread_capacity ? 2 * thread_capacity : 16;
        thread_pointers = realloc(thread_pointers, thread_capacity * sizeof *thread_pointers);
    }
    thread_pointers[thread_count++] = tp;
    pthread_mutex_unlock(&loader_lock);
    c->fs_base = tp;
}

uint64_t rt64_tls_get_addr(CPU64 *c, uint64_t module_id, uint64_t offset) {
    if (!c->fs_base) rt64_thread_tls(c);
    if (!module_id || module_id > loaded_count) rt64_trap(c, 0, "__tls_get_addr: unknown module id");
    return c->fs_base - loaded[module_id - 1].tls_offset + offset;
}

// Memory the guest will free: from its own malloc (libmimalloc.so in P2CE)
// when a lifted module exports one, else the host's.
uint64_t rt64_guest_malloc(CPU64 *c, size_t size) {
    static uint64_t guest_malloc;
    if (!guest_malloc) guest_malloc = rt64_lookup_global("malloc");
    if (!guest_malloc || !c) return (uint64_t)(uintptr_t)malloc(size);
    uint64_t saved[16];
    memcpy(saved, c->r, sizeof saved);
    c->r[RDI] = size;
    rt64_call(c, guest_malloc);
    uint64_t result = c->r[RAX];
    memcpy(c->r, saved, sizeof saved);
    return result;
}

uint64_t rt64_guest_strdup(CPU64 *c, const char *text) {
    size_t n = strlen(text) + 1;
    uint64_t copy = rt64_guest_malloc(c, n);
    if (copy) memcpy((void *)(uintptr_t)copy, text, n);
    return copy;
}

// Exit handlers --------------------------------------------------------------

typedef struct { uint64_t function, argument, dso; int with_argument; } ExitHandler;
static ExitHandler *exit_handlers;
static size_t exit_count, exit_capacity;

void rt64_add_exit_handler(uint64_t function, uint64_t argument, uint64_t dso, int with_argument) {
    pthread_mutex_lock(&loader_lock);
    if (exit_count == exit_capacity) {
        exit_capacity = exit_capacity ? 2 * exit_capacity : 64;
        exit_handlers = realloc(exit_handlers, exit_capacity * sizeof *exit_handlers);
    }
    exit_handlers[exit_count++] = (ExitHandler){function, argument, dso, with_argument};
    pthread_mutex_unlock(&loader_lock);
}

void rt64_run_exit_handlers(CPU64 *c, uint64_t dso) {
    for (size_t i = exit_count; i-- > 0;) {
        ExitHandler h = exit_handlers[i];
        if (!h.function || (dso && h.dso != dso)) continue;
        exit_handlers[i].function = 0;
        uint64_t saved = c->r[RDI];
        c->r[RDI] = h.argument;
        rt64_call(c, h.function);
        c->r[RDI] = saved;
    }
}

// CPU and dispatch -----------------------------------------------------------

void rt64_cpu_init(CPU64 *c, uint64_t stack_top) {
    memset(c, 0, sizeof *c);
    c->r[RSP] = stack_top & ~UINT64_C(15);
    c->mxcsr = 0x1f80;
    c->fcw = 0x37f;
}

static gfn64 lookup(const RT64Loaded *l, uint64_t offset) {
    size_t lo = 0, hi = l->module->function_count;
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        uint64_t at = l->module->functions[mid].offset;
        if (at == offset) return l->module->functions[mid].fn;
        if (at < offset) lo = mid + 1; else hi = mid;
    }
    return NULL;
}

// Shims reached through an address the guest computed (dlsym of a system
// library function): one address per name, in a reserved range.
#define RT64_DYNAMIC_MAX 4096
static uint64_t dynamic_base;
static const char *dynamic_names[RT64_DYNAMIC_MAX];
static rt64_shim dynamic_shims[RT64_DYNAMIC_MAX];
static size_t dynamic_count;

uint64_t rt64_dynamic_import(const char *name) {
    rt64_shim shim = rt64_find_shim(name);
    if (!shim) return 0;
    pthread_mutex_lock(&loader_lock);
    if (!dynamic_base)
        dynamic_base = (uint64_t)(uintptr_t)mmap(NULL, RT64_IMPORT_STRIDE * RT64_DYNAMIC_MAX, PROT_NONE,
                                                 MAP_PRIVATE | MAP_ANON, -1, 0);
    size_t i = 0;
    while (i < dynamic_count && strcmp(dynamic_names[i], name)) ++i;
    if (i == dynamic_count && dynamic_count < RT64_DYNAMIC_MAX) {
        dynamic_names[i] = strdup(name); dynamic_shims[i] = shim; dynamic_count++;
    }
    pthread_mutex_unlock(&loader_lock);
    return i < RT64_DYNAMIC_MAX ? dynamic_base + RT64_IMPORT_STRIDE * i : 0;
}

void rt64_dispatch(CPU64 *c, uint64_t target) {
    if (dynamic_base && target >= dynamic_base && target < dynamic_base + RT64_IMPORT_STRIDE * dynamic_count) {
        dynamic_shims[(target - dynamic_base) / RT64_IMPORT_STRIDE](c);
        if (c->transferred) { c->transferred = 0; return; }
        c->rip = POP64(c);
        return;
    }
    for (size_t i = 0; i < loaded_count; ++i) {
        const RT64Loaded *l = &loaded[i];
        if (target >= l->imports && target < l->imports + RT64_IMPORT_STRIDE * l->module->import_count) {
            size_t index = (target - l->imports) / RT64_IMPORT_STRIDE;
            if (!l->shims[index]) {
                char why[200];
                snprintf(why, sizeof why, "call to unimplemented import %s", l->module->imports[index].name);
                rt64_trap(c, R64(c->r[RSP]), why);
            }
            l->shims[index](c);
            if (c->transferred) { c->transferred = 0; return; }   // longjmp: rip and rsp already set
            c->rip = POP64(c);        // the shim returns like a guest RET
            return;
        }
        if (target >= l->base && target < l->base + l->module->size) {
            gfn64 fn = lookup(l, target - l->base);
            if (!fn) {
                // Not a function: a jump into the middle of one, as the C++
                // unwinder does to reach a landing pad (and longjmp-like code).
                // The lifted callers leave their host frames until the function
                // that has this resume point (tools/lift64.py) continues there.
                c->rip = target;
                return;
            }
            fn(c);
            return;
        }
    }
    // Native code (P2CE piece D: the methods of Valve's Steam interfaces, in
    // steamclient.dylib): called through the arm64 trampoline (native64.c).
    if (rt64_is_native(target)) {
        rt64_native_call(c, target);
        c->rip = POP64(c);
        return;
    }
    rt64_trap(c, target, "indirect transfer outside every lifted module");
}

void rt64_call(CPU64 *c, uint64_t target) {
    PUSH64(c, RT64_SENTINEL);
    rt64_dispatch(c, target);
    if (c->rip != RT64_SENTINEL) {
        uint64_t base;
        const RT64Module *m = rt64_module_at(c->rip, &base);
        char why[200];
        snprintf(why, sizeof why, "control reached %s+%#llx, which no lifted function on the way back "
                 "had as a function or resume point", m ? m->name : "?", (unsigned long long)(c->rip - (m ? base : 0)));
        rt64_trap(c, c->rip, why);
    }
}

// Programs -------------------------------------------------------------------

static CPU64 main_cpu;
static jmp_buf program_exit;
static int program_running;
CPU64 *rt64_main_cpu(void) { return &main_cpu; }

// exit() from the guest's main thread ends rt64_run_program (cxa64.c).
void rt64_program_exit(CPU64 *c, int status) {
    if (program_running && c == &main_cpu) longjmp(program_exit, (status & 0xff) + 1);
    fflush(NULL);
    exit(status);
}

int rt64_run_program(const RT64Module *program, int argc, char **argv, char **envp) {
    size_t stack_size = (size_t)64 << 20;
    uint8_t *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (stack == MAP_FAILED) return 127;
    rt64_cpu_init(&main_cpu, (uint64_t)(uintptr_t)(stack + stack_size));
    rt64_thread_tls(&main_cpu);
    rt64_set_main_arguments(argc, argv, envp);
    rt64_register(program);
    if (!rt64_open(&main_cpu, program->name)) { fprintf(stderr, "rt64: cannot open %s\n", program->name); return 127; }
    uint64_t base = *program->base;
    // The kernel's initial stack: argc, argv[], NULL, envp[], NULL, auxv.
    CPU64 *c = &main_cpu;
    int envc = 0;
    while (envp && envp[envc]) envc++;
    c->r[RSP] -= 64;
    uint64_t random_bytes = c->r[RSP];
    arc4random_buf((void *)(uintptr_t)random_bytes, 16);
    uint64_t auxv[][2] = {{3, base + R64(base + 0x20)}, {4, 56}, {5, (uint16_t)R16(base + 0x38)}, {6, 4096},
                          {9, base + program->entry}, {25, random_bytes}, {0, 0}};
    size_t words = 1 + (size_t)argc + 1 + (size_t)envc + 1 + 2 * (sizeof auxv / sizeof *auxv);
    c->r[RSP] = (c->r[RSP] - 8 * words) & ~UINT64_C(15);
    uint64_t at = c->r[RSP];
    W64(at, (uint64_t)argc); at += 8;
    for (int i = 0; i < argc; ++i, at += 8) W64(at, (uintptr_t)argv[i]);
    W64(at, 0); at += 8;
    for (int i = 0; i < envc; ++i, at += 8) W64(at, (uintptr_t)envp[i]);
    W64(at, 0); at += 8;
    for (size_t i = 0; i < sizeof auxv / sizeof *auxv; ++i, at += 16) { W64(at, auxv[i][0]); W64(at + 8, auxv[i][1]); }
    c->r[RDX] = 0;                           // no rtld_fini
    int status = setjmp(program_exit);
    if (status) { program_running = 0; return status - 1; }
    program_running = 1;
    rt64_dispatch(c, base + program->entry);  // _start does not return: __libc_start_main exits
    program_running = 0;
    rt64_trap(c, base + program->entry, "_start returned");
}
