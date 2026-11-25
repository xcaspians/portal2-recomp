// Valve's coroutines run by the runtime (runtime/coroutine.c): a guest
// function that yields twice then returns; Continue answers "still running"
// twice then "done", the steps happen in order, never two at once, and the
// coroutine sees its caller's thread-local storage.
#include "checks.h"
#include "modules.h"
#include <pthread.h>

#define STACK 0x2001e000u

static int steps, overlap, running;
static uint32_t seen_gs;
static pthread_t caller;
static uint32_t call0(CPU *c, const char *name, unsigned n, const uint32_t *args) {
    return rt_invoke(c, rt_dynamic_import(name), args, n);
}
static int co_func(CPU *c, const char *name) {
    if (strcmp(name, "test_coroutine_body")) return 0;
    overlap |= running++ != 0;
    assert(!pthread_equal(pthread_self(), caller));    // its own host thread
    seen_gs = c->gs_base;
    assert(R32(c->esp + 4) == 0x1234);                 // its parameter
    for (int i = 0; i < 2; ++i) {
        ++steps; --running;
        call0(c, "Coroutine_YieldToMain", 0, NULL);
        overlap |= running++ != 0;
    }
    ++steps; --running;
    c->eax = 0;
    return 1;
}
RT_SHIM_PROVIDER(co_func)

RT_CHECK(coroutines_continue_and_yield_in_turn) {
    caller = pthread_self();
    CPU c; rt_cpu_reset(&c); c.esp = STACK; c.gs_base = 0x7777;
    rt_guest_enter();
    uint32_t args[2] = {rt_dynamic_import("test_coroutine_body"), 0x1234};
    uint32_t h = call0(&c, "Coroutine_Create", 2, args);
    assert(h);
    assert(call0(&c, "Coroutine_IsActive", 1, &h) == 0 && steps == 0);   // created, not started
    assert(call0(&c, "Coroutine_Continue", 1, &h) == 1 && steps == 1);   // yielded
    assert(seen_gs == 0x7777);                                           // the caller's TLS
    assert(call0(&c, "Coroutine_IsActive", 1, &h) == 1);
    assert(call0(&c, "Coroutine_GetCurrentlyActive", 0, NULL) == 0);     // the caller isn't one
    assert(call0(&c, "Coroutine_Continue", 1, &h) == 1 && steps == 2);
    assert(call0(&c, "Coroutine_Continue", 1, &h) == 0 && steps == 3);   // returned: done
    assert(call0(&c, "Coroutine_Continue", 1, &h) == 0 && steps == 3);   // stays done
    assert(!overlap);
    // Cancelled while stopped in YieldToMain: no more of its code runs.
    steps = 0;
    uint32_t h2 = call0(&c, "Coroutine_Create", 2, args);
    assert(call0(&c, "Coroutine_Continue", 1, &h2) == 1 && steps == 1);
    call0(&c, "Coroutine_Cancel", 1, &h2);
    assert(steps == 1 && call0(&c, "Coroutine_IsActive", 1, &h2) == 0);
    rt_guest_leave();
}
