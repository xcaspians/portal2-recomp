// pthread shims of the x86-64 runtime. Linux's pthread objects are smaller
// than Apple's (mutex 40 bytes against 64, cond 48, rwlock 56), so the
// guest object holds a pointer to a host object in its first 8 bytes,
// created on first use: Linux's static initialisers are all zeros, and the
// mutex kind is read from offset 16 (PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP).
// pthread_once_t (an int) and pthread_key_t (an unsigned int) are used as is.
#include "rt64.h"

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>

enum { LINUX_MUTEX_NORMAL, LINUX_MUTEX_RECURSIVE, LINUX_MUTEX_ERRORCHECK };

static pthread_mutex_t create_lock = PTHREAD_MUTEX_INITIALIZER;

static void *host_object(uint64_t guest, void *(*create)(uint64_t guest)) {
    void *existing = __atomic_load_n((void **)(uintptr_t)guest, __ATOMIC_ACQUIRE);
    if (existing) return existing;
    pthread_mutex_lock(&create_lock);
    existing = *(void **)(uintptr_t)guest;
    if (!existing) {
        existing = create(guest);
        __atomic_store_n((void **)(uintptr_t)guest, existing, __ATOMIC_RELEASE);
    }
    pthread_mutex_unlock(&create_lock);
    return existing;
}

static void *create_mutex(uint64_t guest) {
    pthread_mutex_t *m = malloc(sizeof *m);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    int kind = (int)R32(guest + 16);
    pthread_mutexattr_settype(&attr, kind == LINUX_MUTEX_RECURSIVE ? PTHREAD_MUTEX_RECURSIVE :
                              kind == LINUX_MUTEX_ERRORCHECK ? PTHREAD_MUTEX_ERRORCHECK : PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
    return m;
}
static void *create_cond(uint64_t guest) { (void)guest; pthread_cond_t *v = malloc(sizeof *v); pthread_cond_init(v, NULL); return v; }
static void *create_rwlock(uint64_t guest) { (void)guest; pthread_rwlock_t *v = malloc(sizeof *v); pthread_rwlock_init(v, NULL); return v; }

#define MUTEX(i) ((pthread_mutex_t *)host_object(ARG(c, i), create_mutex))
#define COND(i) ((pthread_cond_t *)host_object(ARG(c, i), create_cond))
#define RWLOCK(i) ((pthread_rwlock_t *)host_object(ARG(c, i), create_rwlock))
#define RESULT(c, r) RET(c, (r) ? rt64_linux_errno(r) : 0)

static void s_mutex_init(CPU64 *c) {
    uint64_t guest = ARG(c, 0);
    int kind = ARG(c, 1) ? (int)R32(ARG(c, 1)) : 0;          // pthread_mutexattr_t: the kind
    memset((void *)(uintptr_t)guest, 0, 40);
    W32(guest + 16, (uint32_t)kind);
    host_object(guest, create_mutex);
    RET(c, 0);
}
static void s_mutex_destroy(CPU64 *c) {
    void *m = *(void **)PTR(c, 0);
    if (m) { pthread_mutex_destroy(m); free(m); *(void **)PTR(c, 0) = NULL; }
    RET(c, 0);
}
static void s_mutex_lock(CPU64 *c) { RESULT(c, pthread_mutex_lock(MUTEX(0))); }
static void s_mutex_trylock(CPU64 *c) { RESULT(c, pthread_mutex_trylock(MUTEX(0))); }
static void s_mutex_unlock(CPU64 *c) { RESULT(c, pthread_mutex_unlock(MUTEX(0))); }
static void s_mutexattr_init(CPU64 *c) { W32(ARG(c, 0), 0); RET(c, 0); }
static void s_mutexattr_settype(CPU64 *c) { W32(ARG(c, 0), (uint32_t)ARG(c, 1)); RET(c, 0); }
static void s_noop_zero(CPU64 *c) { RET(c, 0); }
static void s_cond_init(CPU64 *c) { memset(PTR(c, 0), 0, 48); host_object(ARG(c, 0), create_cond); RET(c, 0); }
static void s_cond_destroy(CPU64 *c) {
    void *v = *(void **)PTR(c, 0);
    if (v) { pthread_cond_destroy(v); free(v); *(void **)PTR(c, 0) = NULL; }
    RET(c, 0);
}
static void s_cond_wait(CPU64 *c) { RESULT(c, pthread_cond_wait(COND(0), MUTEX(1))); }
static void s_cond_timedwait(CPU64 *c) {
    // Linux pthread_cond_timedwait uses CLOCK_REALTIME by default, like Apple.
    RESULT(c, pthread_cond_timedwait(COND(0), MUTEX(1), PTR(c, 2)));
}
static void s_cond_signal(CPU64 *c) { RESULT(c, pthread_cond_signal(COND(0))); }
static void s_cond_broadcast(CPU64 *c) { RESULT(c, pthread_cond_broadcast(COND(0))); }
static void s_rwlock_init(CPU64 *c) { memset(PTR(c, 0), 0, 56); host_object(ARG(c, 0), create_rwlock); RET(c, 0); }
static void s_rwlock_destroy(CPU64 *c) {
    void *v = *(void **)PTR(c, 0);
    if (v) { pthread_rwlock_destroy(v); free(v); *(void **)PTR(c, 0) = NULL; }
    RET(c, 0);
}
static void s_rwlock_rdlock(CPU64 *c) { RESULT(c, pthread_rwlock_rdlock(RWLOCK(0))); }
static void s_rwlock_wrlock(CPU64 *c) { RESULT(c, pthread_rwlock_wrlock(RWLOCK(0))); }
static void s_rwlock_unlock(CPU64 *c) { RESULT(c, pthread_rwlock_unlock(RWLOCK(0))); }

// pthread_once: 0 = not run, 1 = running, 2 = done (glibc uses other bits;
// only the "done" state is ever read by inlined code, through this call).
static pthread_mutex_t once_lock = PTHREAD_MUTEX_INITIALIZER;
static void s_once(CPU64 *c) {
    uint64_t control = ARG(c, 0), routine = ARG(c, 1);
    if (__atomic_load_n((int *)(uintptr_t)control, __ATOMIC_ACQUIRE) == 2) { RET(c, 0); return; }
    pthread_mutex_lock(&once_lock);
    if (*(int *)(uintptr_t)control == 0) {
        *(int *)(uintptr_t)control = 1;
        rt64_call(c, routine);
        __atomic_store_n((int *)(uintptr_t)control, 2, __ATOMIC_RELEASE);
    }
    pthread_mutex_unlock(&once_lock);
    RET(c, 0);
}

// Keys: host keys; a guest destructor is not run yet (threads come with
// round D of the plan).
static void s_key_create(CPU64 *c) {
    pthread_key_t key;
    int r = pthread_key_create(&key, NULL);
    if (!r) W32(ARG(c, 0), (uint32_t)key);
    RESULT(c, r);
}
static void s_key_delete(CPU64 *c) { RESULT(c, pthread_key_delete((pthread_key_t)ARG(c, 0))); }
static void s_getspecific(CPU64 *c) { RET(c, (uintptr_t)pthread_getspecific((pthread_key_t)ARG(c, 0))); }
static void s_setspecific(CPU64 *c) { RESULT(c, pthread_setspecific((pthread_key_t)ARG(c, 0), PTR(c, 1))); }
static void s_self(CPU64 *c) { RET(c, (uintptr_t)pthread_self()); }
static void s_equal(CPU64 *c) { RET(c, ARG(c, 0) == ARG(c, 1)); }
static void s_sched_yield(CPU64 *c) { RET(c, sched_yield()); }

static const RT64Shim shims[] = {
    {"pthread_mutex_init", s_mutex_init}, {"pthread_mutex_destroy", s_mutex_destroy},
    {"pthread_mutex_lock", s_mutex_lock}, {"pthread_mutex_trylock", s_mutex_trylock},
    {"pthread_mutex_unlock", s_mutex_unlock}, {"pthread_mutexattr_init", s_mutexattr_init},
    {"pthread_mutexattr_settype", s_mutexattr_settype}, {"pthread_mutexattr_destroy", s_noop_zero},
    {"pthread_cond_init", s_cond_init}, {"pthread_cond_destroy", s_cond_destroy},
    {"pthread_cond_wait", s_cond_wait}, {"pthread_cond_timedwait", s_cond_timedwait},
    {"pthread_cond_signal", s_cond_signal}, {"pthread_cond_broadcast", s_cond_broadcast},
    {"pthread_rwlock_init", s_rwlock_init}, {"pthread_rwlock_destroy", s_rwlock_destroy},
    {"pthread_rwlock_rdlock", s_rwlock_rdlock}, {"pthread_rwlock_wrlock", s_rwlock_wrlock},
    {"pthread_rwlock_unlock", s_rwlock_unlock},
    {"pthread_once", s_once}, {"__pthread_once", s_once},
    {"pthread_key_create", s_key_create}, {"__pthread_key_create", s_key_create},
    {"pthread_key_delete", s_key_delete}, {"pthread_getspecific", s_getspecific},
    {"pthread_setspecific", s_setspecific}, {"pthread_self", s_self}, {"pthread_equal", s_equal},
    {"sched_yield", s_sched_yield},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
