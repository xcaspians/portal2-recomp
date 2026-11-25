// Valve's coroutines (tier1 coroutine.h, exported by libvstdlib.so), run by the
// runtime instead of the guest's own copy (game profile host_exports:
// libvstdlib.so:Coroutine_, tools/elfprep.py --host-export).
//
// SHIMS: Coroutine_Create Coroutine_Continue Coroutine_YieldToMain Coroutine_Cancel
// SHIMS: Coroutine_IsActive Coroutine_GetCurrentlyActive Coroutine_GetStackDepth
// SHIMS: Coroutine_DebugBreak Coroutine_ValidateGlobals Coroutine_ReleaseThreadMemory
//
// Why: on Linux, vstdlib switches coroutines by copying slices of the stack to
// the heap and back, then longjmp-ing into a frame that is not on the host's
// call stack. Lifted code keeps one host C frame per guest frame, so such a
// jump can't be expressed (runtime/nonlocal.c refuses it). Left 4 Dead 2's
// matchmaking.so uses them.
//
// How: each coroutine runs its guest function on its own host thread, with its
// own guest stack (rt_foreign_thread_attach), and control passes back and
// forth like a baton: Coroutine_Continue hands the guest lock to the coroutine
// and waits; Coroutine_YieldToMain (or the end of the function) hands it back.
// Only one of the two ever runs, as with Valve's single-thread version. The
// coroutine uses its caller's thread-local storage (gs base), so thread-local
// variables read as on the caller's thread.
//
// Cancelling a coroutine stopped in YieldToMain ends its host thread there,
// without running any more of its guest code (Valve's drops its saved stack).
#include "rt.h"
#include "modules.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CO_NEW, CO_RUNNING, CO_YIELDED, CO_DONE };
typedef struct Co {
    uint32_t id, func, param;
    int state, cancel;
    uint32_t caller_gs;            // the thread-local storage it runs with: its caller's
    pthread_t thread;
    dispatch_semaphore_t go, back; // Continue -> coroutine, coroutine -> Continue
    struct Co *next;
} Co;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static Co *all;
static uint32_t next_id = 1;
static __thread Co *self;          // the coroutine this host thread runs, if any

static Co *find(uint32_t id) {
    pthread_mutex_lock(&lock);
    Co *co = all;
    while (co && co->id != id) co = co->next;
    pthread_mutex_unlock(&lock);
    return co;
}

static void *coroutine_thread(void *arg) {
    Co *co = arg;
    self = co;
    CPU *c = rt_foreign_thread_attach();
    dispatch_semaphore_wait(co->go, DISPATCH_TIME_FOREVER);
    if (!co->cancel) {
        rt_guest_enter();
        uint32_t own_gs = c->gs_base;
        c->gs_base = co->caller_gs;
        uint32_t param = co->param;
        rt_invoke(c, co->func, &param, 1);
        c->gs_base = own_gs;
        rt_guest_leave();
    }
    co->state = CO_DONE;
    rt_foreign_thread_detach();
    dispatch_semaphore_signal(co->back);
    return NULL;
}

// Hands the baton to the coroutine and waits for it (guest lock released meanwhile).
static void run(CPU *c, Co *co) {
    co->caller_gs = c->gs_base;
    co->state = CO_RUNNING;
    unsigned saved = rt_guest_suspend();
    dispatch_semaphore_signal(co->go);
    dispatch_semaphore_wait(co->back, DISPATCH_TIME_FOREVER);
    rt_guest_resume(saved);
    if (co->state == CO_DONE) pthread_join(co->thread, NULL);
}

static int coroutine_shim(CPU *c, const char *name) {
    if (strncmp(name, "Coroutine_", 10)) return 0;
    const char *what = name + 10;
    if (!strcmp(what, "Create")) {   // HCoroutine Coroutine_Create(CoroutineFunc_t, void *)
        Co *co = calloc(1, sizeof(*co));
        if (!co) { c->eax = 0; return 1; }
        co->func = rt_arg32(c, 0); co->param = rt_arg32(c, 1);
        co->go = dispatch_semaphore_create(0); co->back = dispatch_semaphore_create(0);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 8u << 20);   // host frames of lifted code
        pthread_mutex_lock(&lock);
        co->id = next_id++;
        co->next = all; all = co;
        pthread_mutex_unlock(&lock);
        if (pthread_create(&co->thread, &attr, coroutine_thread, co)) rt_trap(c, c->eip, "Coroutine_Create: no thread");
        pthread_attr_destroy(&attr);
        c->eax = co->id;
    } else if (!strcmp(what, "Continue")) {   // bool Coroutine_Continue(HCoroutine, const char *name)
        Co *co = find(rt_arg32(c, 0));
        if (co && co->state != CO_DONE) run(c, co);
        c->eax = co && co->state != CO_DONE;
    } else if (!strcmp(what, "YieldToMain")) {
        Co *co = self;
        if (!co) { c->eax = 0; return 1; }   // not in a coroutine: nothing to yield to (Valve asserts)
        co->state = CO_YIELDED;
        uint32_t gs = c->gs_base;
        unsigned saved = rt_guest_suspend();
        dispatch_semaphore_signal(co->back);
        dispatch_semaphore_wait(co->go, DISPATCH_TIME_FOREVER);
        if (co->cancel) {   // dropped while stopped here: end the thread, no more guest code
            co->state = CO_DONE;
            c->gs_base = gs;
            rt_foreign_thread_detach();
            dispatch_semaphore_signal(co->back);
            pthread_exit(NULL);
        }
        rt_guest_resume(saved);
        c->gs_base = co->caller_gs;   // its (possibly new) caller's thread-local storage
        co->state = CO_RUNNING;
    } else if (!strcmp(what, "Cancel")) {
        Co *co = find(rt_arg32(c, 0));
        if (co && co->state != CO_DONE && co->state != CO_RUNNING) {
            co->cancel = 1;
            unsigned saved = rt_guest_suspend();
            dispatch_semaphore_signal(co->go);
            dispatch_semaphore_wait(co->back, DISPATCH_TIME_FOREVER);
            pthread_join(co->thread, NULL);
            rt_guest_resume(saved);
            co->state = CO_DONE;
        }
    } else if (!strcmp(what, "IsActive")) {
        Co *co = find(rt_arg32(c, 0));
        c->eax = co && co->state != CO_DONE && co->state != CO_NEW;
    } else if (!strcmp(what, "GetCurrentlyActive")) {
        c->eax = self ? self->id : 0;
    } else if (!strcmp(what, "GetStackDepth")) {
        c->eax = 0;   // no copied stack: nothing to measure
    } else if (!strcmp(what, "DebugBreak") || !strcmp(what, "ValidateGlobals") ||
               !strcmp(what, "ReleaseThreadMemory")) {
        c->eax = 0;
    } else {
        return 0;
    }
    return 1;
}
RT_SHIM_PROVIDER(coroutine_shim)
