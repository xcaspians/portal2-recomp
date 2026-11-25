// More C library shims of the x86-64 runtime, for the engine and its
// launcher: fortified (_chk) variants, semaphores (Linux sem_t holds a
// pointer to a host counter), resource limits, uuid, wide strings, a few
// maths functions. Sockets, inotify, shared memory and terminals are
// refused: a lifted program has no network and no other processes.
#include "rt64.h"

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

#define S(i) ((const char *)PTR(c, i))
#define W(i) ((const wchar_t *)PTR(c, i))
#define Z(i) ((size_t)ARG(c, i))

static void s_memcpy_chk(CPU64 *c) { RET(c, (uintptr_t)memcpy(PTR(c, 0), PTR(c, 1), Z(2))); }
static void s_memmove_chk(CPU64 *c) { RET(c, (uintptr_t)memmove(PTR(c, 0), PTR(c, 1), Z(2))); }
static void s_memset_chk(CPU64 *c) { RET(c, (uintptr_t)memset(PTR(c, 0), (int)ARG(c, 1), Z(2))); }
static void s_strcpy_chk(CPU64 *c) { RET(c, (uintptr_t)strcpy(PTR(c, 0), S(1))); }
static void s_strncpy_chk(CPU64 *c) { RET(c, (uintptr_t)strncpy(PTR(c, 0), S(1), Z(2))); }
static void s_strcat_chk(CPU64 *c) { RET(c, (uintptr_t)strcat(PTR(c, 0), S(1))); }
static void s_strncat_chk(CPU64 *c) { RET(c, (uintptr_t)strncat(PTR(c, 0), S(1), Z(2))); }
static void s_realpath_chk(CPU64 *c) {
    char buffer[PATH_MAX];
    if (!realpath(S(0), buffer)) { rt64_set_errno(errno); RET(c, 0); return; }
    if (!ARG(c, 1)) { RET(c, rt64_guest_strdup(c, buffer)); return; }
    strcpy(PTR(c, 1), buffer);
    RET(c, ARG(c, 1));
}
static void s_strtok(CPU64 *c) { RET(c, (uintptr_t)strtok(PTR(c, 0), S(1))); }
static void s_strcasestr(CPU64 *c) { RET(c, (uintptr_t)strcasestr(S(0), S(1))); }
static void s_wcscpy(CPU64 *c) { RET(c, (uintptr_t)wcscpy(PTR(c, 0), W(1))); }
static void s_wcsncmp(CPU64 *c) { RET(c, (int64_t)wcsncmp(W(0), W(1), Z(2))); }
static void s_wcscasecmp(CPU64 *c) { RET(c, (int64_t)wcscasecmp(W(0), W(1))); }
static void s_wcsncasecmp(CPU64 *c) { RET(c, (int64_t)wcsncasecmp(W(0), W(1), Z(2))); }
static void s_wcsstr(CPU64 *c) { RET(c, (uintptr_t)wcsstr(W(0), W(1))); }
static void s_wcsdup(CPU64 *c) {
    size_t n = (wcslen(W(0)) + 1) * 4;
    uint64_t copy = rt64_guest_malloc(c, n);
    if (copy) memcpy((void *)(uintptr_t)copy, W(0), n);
    RET(c, copy);
}
static void s_wcstol(CPU64 *c) { RET(c, wcstol(W(0), PTR(c, 1), (int)ARG(c, 2))); }
static void s_wcstof(CPU64 *c) { c->xmm[0].f[0] = wcstof(W(0), PTR(c, 1)); }
static void s_iswcntrl(CPU64 *c) { RET(c, iswcntrl((wint_t)ARG(c, 0)) != 0); }
static void s_lround(CPU64 *c) { RET(c, lround(c->xmm[0].g[0])); }
static void s_lroundf(CPU64 *c) { RET(c, lroundf(c->xmm[0].f[0])); }
static void s_scalbn(CPU64 *c) { c->xmm[0].g[0] = scalbn(c->xmm[0].g[0], (int)ARG(c, 0)); }
static void s_scalbnf(CPU64 *c) { c->xmm[0].f[0] = scalbnf(c->xmm[0].f[0], (int)ARG(c, 0)); }
static void s_frexpf(CPU64 *c) { c->xmm[0].f[0] = frexpf(c->xmm[0].f[0], PTR(c, 0)); }
static void s_ungetc(CPU64 *c) { RET(c, ungetc((int)ARG(c, 0), PTR(c, 1))); }
static void s_setbuf(CPU64 *c) { setbuf(PTR(c, 0), PTR(c, 1)); }
static void s_perror(CPU64 *c) { fprintf(stderr, "%s: %s\n", S(0) ? S(0) : "", strerror(*rt64_errno_slot())); }
static void s_tzset(CPU64 *c) { (void)c; tzset(); }
static void s_system(CPU64 *c) { RET(c, -1); }
static void s_zero(CPU64 *c) { RET(c, 0); }
static void s_refuse(CPU64 *c) { rt64_set_errno(EACCES); RET(c, -1); }
static void s_nosys(CPU64 *c) { rt64_set_errno(ENOSYS); RET(c, -1); }
static void s_kill(CPU64 *c) {
    if ((int)ARG(c, 1) == 0) { RET(c, (int)ARG(c, 0) == getpid() ? 0 : -1); return; }
    rt64_set_errno(EPERM); RET(c, -1);
}

// Semaphores: the guest's sem_t (32 bytes) points to this.
typedef struct { pthread_mutex_t lock; pthread_cond_t changed; int64_t count; } Semaphore;
static Semaphore *semaphore(CPU64 *c) { return *(Semaphore **)PTR(c, 0); }
static void s_sem_init(CPU64 *c) {
    Semaphore *s = calloc(1, sizeof *s);
    pthread_mutex_init(&s->lock, NULL); pthread_cond_init(&s->changed, NULL);
    s->count = (int64_t)(uint32_t)ARG(c, 2);
    *(Semaphore **)PTR(c, 0) = s;
    RET(c, 0);
}
static void s_sem_destroy(CPU64 *c) {
    Semaphore *s = semaphore(c);
    if (s) { pthread_mutex_destroy(&s->lock); pthread_cond_destroy(&s->changed); free(s); }
    RET(c, 0);
}
static void s_sem_post(CPU64 *c) {
    Semaphore *s = semaphore(c);
    pthread_mutex_lock(&s->lock); s->count++; pthread_cond_signal(&s->changed); pthread_mutex_unlock(&s->lock);
    RET(c, 0);
}
static void s_sem_wait(CPU64 *c) {
    Semaphore *s = semaphore(c);
    pthread_mutex_lock(&s->lock);
    while (s->count <= 0) pthread_cond_wait(&s->changed, &s->lock);
    s->count--;
    pthread_mutex_unlock(&s->lock);
    RET(c, 0);
}
static void s_sem_trywait(CPU64 *c) {
    Semaphore *s = semaphore(c);
    pthread_mutex_lock(&s->lock);
    int ok = s->count > 0;
    if (ok) s->count--;
    pthread_mutex_unlock(&s->lock);
    if (!ok) rt64_set_errno(EAGAIN);
    RET(c, ok ? 0 : -1);
}
static void s_sem_timedwait(CPU64 *c) {      // absolute CLOCK_REALTIME deadline
    Semaphore *s = semaphore(c);
    pthread_mutex_lock(&s->lock);
    int r = 0;
    while (s->count <= 0 && r == 0) r = pthread_cond_timedwait(&s->changed, &s->lock, PTR(c, 1));
    int ok = s->count > 0;
    if (ok) s->count--;
    pthread_mutex_unlock(&s->lock);
    if (!ok) rt64_set_errno(ETIMEDOUT);
    RET(c, ok ? 0 : -1);
}
static void s_sem_getvalue(CPU64 *c) { W32(ARG(c, 1), (uint32_t)semaphore(c)->count); RET(c, 0); }

// Resource limits (Linux RLIMIT_* numbers, struct rlimit of two unsigned longs).
static void s_getrlimit(CPU64 *c) {
    int resource;
    switch ((int)ARG(c, 0)) {
    case 3: resource = RLIMIT_STACK; break;
    case 4: resource = RLIMIT_CORE; break;
    case 7: resource = RLIMIT_NOFILE; break;
    case 9: resource = RLIMIT_AS; break;
    default: W64(ARG(c, 1), UINT64_MAX); W64(ARG(c, 1) + 8, UINT64_MAX); RET(c, 0); return;
    }
    struct rlimit l;
    getrlimit(resource, &l);
    W64(ARG(c, 1), l.rlim_cur == RLIM_INFINITY ? UINT64_MAX : l.rlim_cur);
    W64(ARG(c, 1) + 8, l.rlim_max == RLIM_INFINITY ? UINT64_MAX : l.rlim_max);
    RET(c, 0);
}
// uuid_t is 16 bytes on both sides.
static void s_uuid_generate(CPU64 *c) {
    uint8_t *u = PTR(c, 0);
    arc4random_buf(u, 16);
    u[6] = (uint8_t)((u[6] & 0x0f) | 0x40); u[8] = (uint8_t)((u[8] & 0x3f) | 0x80);
}
static void s_uuid_clear(CPU64 *c) { memset(PTR(c, 0), 0, 16); }
static void s_uuid_unparse(CPU64 *c) {
    const uint8_t *u = PTR(c, 0);
    snprintf(PTR(c, 1), 37, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             u[0], u[1], u[2], u[3], u[4], u[5], u[6], u[7], u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
}
static void s_uuid_parse(CPU64 *c) {
    unsigned v[16];
    int n = sscanf(S(0), "%2x%2x%2x%2x-%2x%2x-%2x%2x-%2x%2x-%2x%2x%2x%2x%2x%2x", &v[0], &v[1], &v[2], &v[3], &v[4],
                   &v[5], &v[6], &v[7], &v[8], &v[9], &v[10], &v[11], &v[12], &v[13], &v[14], &v[15]);
    if (n != 16) { RET(c, -1); return; }
    for (int i = 0; i < 16; ++i) ((uint8_t *)PTR(c, 1))[i] = (uint8_t)v[i];
    RET(c, 0);
}
// struct statfs (Linux x86-64, 120 bytes): type, bsize, blocks, bfree, bavail, files, ffree, fsid, namelen, frsize, flags.
static void s_statfs(CPU64 *c) {
    struct statfs s;
    if (statfs(S(0), &s)) { rt64_set_errno(errno); RET(c, -1); return; }
    uint64_t at = ARG(c, 1);
    memset((void *)(uintptr_t)at, 0, 120);
    W64(at, 0xef53); W64(at + 8, s.f_bsize); W64(at + 16, s.f_blocks); W64(at + 24, s.f_bfree);
    W64(at + 32, s.f_bavail); W64(at + 40, s.f_files); W64(at + 48, s.f_ffree); W64(at + 64, 255);
    W64(at + 72, s.f_bsize);
    RET(c, 0);
}
static void s_mkostemp(CPU64 *c) { int fd = mkstemp(PTR(c, 0)); if (fd < 0) rt64_set_errno(errno); RET(c, fd); }
static void s_pipe2(CPU64 *c) { int r = pipe(PTR(c, 0)); if (r) rt64_set_errno(errno); RET(c, r); }

static const RT64Shim shims[] = {
    {"__memcpy_chk", s_memcpy_chk}, {"__memmove_chk", s_memmove_chk}, {"__memset_chk", s_memset_chk},
    {"__strcpy_chk", s_strcpy_chk}, {"__strncpy_chk", s_strncpy_chk}, {"__strcat_chk", s_strcat_chk},
    {"__strncat_chk", s_strncat_chk}, {"__realpath_chk", s_realpath_chk}, {"strtok", s_strtok},
    {"strcasestr", s_strcasestr}, {"wcscpy", s_wcscpy}, {"wcsncmp", s_wcsncmp}, {"wcscasecmp", s_wcscasecmp},
    {"wcsncasecmp", s_wcsncasecmp}, {"wcsstr", s_wcsstr}, {"wcsdup", s_wcsdup}, {"wcstol", s_wcstol},
    {"wcstof", s_wcstof}, {"iswcntrl", s_iswcntrl}, {"lround", s_lround}, {"lroundf", s_lroundf},
    {"scalbn", s_scalbn}, {"scalbnf", s_scalbnf}, {"frexpf", s_frexpf}, {"ungetc", s_ungetc}, {"setbuf", s_setbuf},
    {"perror", s_perror}, {"tzset", s_tzset}, {"system", s_system}, {"kill", s_kill},
    {"sem_init", s_sem_init}, {"sem_destroy", s_sem_destroy}, {"sem_post", s_sem_post}, {"sem_wait", s_sem_wait},
    {"sem_trywait", s_sem_trywait}, {"sem_timedwait", s_sem_timedwait}, {"sem_getvalue", s_sem_getvalue},
    {"getrlimit", s_getrlimit}, {"getrlimit64", s_getrlimit}, {"setrlimit", s_zero}, {"setrlimit64", s_zero},
    {"uuid_generate", s_uuid_generate}, {"uuid_generate_random", s_uuid_generate}, {"uuid_clear", s_uuid_clear},
    {"uuid_unparse", s_uuid_unparse}, {"uuid_unparse_lower", s_uuid_unparse}, {"uuid_parse", s_uuid_parse},
    {"statfs", s_statfs}, {"statfs64", s_statfs}, {"mkostemp", s_mkostemp}, {"pipe2", s_pipe2},
    {"setpriority", s_zero}, {"sched_setscheduler", s_zero}, {"sched_getscheduler", s_zero},
    {"sched_getparam", s_zero}, {"prctl", s_zero}, {"pthread_setcanceltype", s_zero}, {"setsid", s_zero},
    {"socket", s_refuse}, {"socketpair", s_refuse}, {"connect", s_refuse}, {"bind", s_refuse},
    {"listen", s_refuse}, {"accept", s_refuse}, {"send", s_refuse}, {"sendto", s_refuse}, {"sendmsg", s_refuse},
    {"recv", s_refuse}, {"recvfrom", s_refuse}, {"recvmsg", s_refuse}, {"setsockopt", s_refuse},
    {"getsockopt", s_refuse}, {"getsockname", s_refuse}, {"getaddrinfo", s_refuse}, {"freeaddrinfo", s_zero},
    {"inet_pton", s_zero}, {"select", s_nosys}, {"inotify_init", s_nosys}, {"inotify_init1", s_nosys},
    {"inotify_add_watch", s_nosys}, {"inotify_rm_watch", s_nosys}, {"shmget", s_nosys}, {"shmat", s_nosys},
    {"shmdt", s_nosys}, {"shmctl", s_nosys}, {"tcgetattr", s_nosys}, {"tcsetattr", s_nosys},
    {"ptrace", s_nosys}, {"process_vm_readv", s_nosys}, {"mount", s_nosys}, {"clone", s_nosys},
    {"sigtimedwait", s_nosys}, {"backtrace", s_zero}, {"dlmopen", s_zero},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
