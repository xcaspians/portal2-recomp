// Linux epoll on macOS kqueue, for the x86-64 runtime (round 28: P2CE's
// libvideo.so waits on file descriptors with it). An epoll instance is a
// kqueue; EPOLLIN and EPOLLOUT become EVFILT_READ and EVFILT_WRITE, the
// guest's 64-bit epoll_data rides in each kevent's udata, EPOLLET is EV_CLEAR
// and EPOLLONESHOT EV_ONESHOT. struct epoll_event is packed on x86-64:
// 4 bytes of events, then 8 of data (12 bytes).
#include "rt64.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

enum {
    L_EPOLLIN = 0x001, L_EPOLLPRI = 0x002, L_EPOLLOUT = 0x004, L_EPOLLERR = 0x008, L_EPOLLHUP = 0x010,
    L_EPOLLRDHUP = 0x2000, L_EPOLLONESHOT = 1u << 30, L_EPOLLET = 1u << 31,
    L_EPOLL_CTL_ADD = 1, L_EPOLL_CTL_DEL = 2, L_EPOLL_CTL_MOD = 3, L_EPOLL_CLOEXEC = 02000000,
};

#define EPOLL_EVENT_SIZE 12

static void fail(CPU64 *c, int host_errno) { rt64_set_errno(host_errno); RET(c, (uint64_t)-1); }

static void s_epoll_create1(CPU64 *c) {
    int kq = kqueue();
    if (kq < 0) { fail(c, errno); return; }
    if ((int)ARG(c, 0) & L_EPOLL_CLOEXEC) fcntl(kq, F_SETFD, FD_CLOEXEC);
    RET(c, kq);
}

static void s_epoll_create(CPU64 *c) {
    if ((int)ARG(c, 0) <= 0) { fail(c, EINVAL); return; }   // size: a hint, but must be positive
    ARG(c, 0) = 0;
    s_epoll_create1(c);
}

static void s_epoll_ctl(CPU64 *c) {
    int kq = (int)ARG(c, 0), op = (int)ARG(c, 1), fd = (int)ARG(c, 2);
    const uint8_t *ev = PTR(c, 3);
    uint32_t events = 0;
    uint64_t data = 0;
    if (op != L_EPOLL_CTL_DEL) {
        if (!ev) { fail(c, EFAULT); return; }
        memcpy(&events, ev, 4);
        memcpy(&data, ev + 4, 8);
    }
    struct kevent ch[4];
    int n = 0;
    if (op == L_EPOLL_CTL_DEL || op == L_EPOLL_CTL_MOD) {
        // Remove both filters, whichever were registered (errors ignored).
        struct kevent del[2];
        EV_SET(&del[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        EV_SET(&del[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        for (int i = 0; i < 2; ++i) kevent(kq, &del[i], 1, NULL, 0, NULL);
        if (op == L_EPOLL_CTL_DEL) { RET(c, 0); return; }
    } else if (op != L_EPOLL_CTL_ADD) {
        fail(c, EINVAL);
        return;
    }
    uint16_t flags = EV_ADD | EV_ENABLE | (events & L_EPOLLET ? EV_CLEAR : 0) | (events & L_EPOLLONESHOT ? EV_ONESHOT : 0);
    void *udata = (void *)(uintptr_t)data;
    if (events & (L_EPOLLIN | L_EPOLLPRI | L_EPOLLRDHUP)) EV_SET(&ch[n++], fd, EVFILT_READ, flags, 0, 0, udata);
    if (events & L_EPOLLOUT) EV_SET(&ch[n++], fd, EVFILT_WRITE, flags, 0, 0, udata);
    if (n == 0) {   // only EPOLLERR/EPOLLHUP wanted (always reported on Linux): watch reads for EOF
        EV_SET(&ch[n++], fd, EVFILT_READ, flags, 0, 0, udata);
    }
    if (kevent(kq, ch, n, NULL, 0, NULL) < 0) { fail(c, errno); return; }
    RET(c, 0);
}

static void epoll_wait_ms(CPU64 *c, int kq, uint8_t *out, int max, int timeout_ms) {
    if (max <= 0 || !out) { fail(c, EINVAL); return; }
    struct kevent got[64];
    int want = max > 32 ? 64 : max * 2;
    struct timespec ts, *tp = NULL;
    if (timeout_ms >= 0) { ts.tv_sec = timeout_ms / 1000; ts.tv_nsec = (long)(timeout_ms % 1000) * 1000000L; tp = &ts; }
    int n = kevent(kq, NULL, 0, got, want, tp);
    if (n < 0) { fail(c, errno); return; }
    // One epoll_event per descriptor: its read and write readiness merged.
    int count = 0;
    for (int i = 0; i < n; ++i) {
        uint32_t events = got[i].filter == EVFILT_WRITE ? L_EPOLLOUT : L_EPOLLIN;
        if (got[i].flags & EV_EOF) events |= L_EPOLLHUP | L_EPOLLRDHUP;
        if (got[i].flags & EV_ERROR) events = L_EPOLLERR;
        uint64_t data = (uint64_t)(uintptr_t)got[i].udata;
        int j = 0;
        for (; j < count; ++j) {
            uint64_t d;
            memcpy(&d, out + j * EPOLL_EVENT_SIZE + 4, 8);
            if (d == data) {   // the layout has no fd: entries merge by their data, one per descriptor

                uint32_t e;
                memcpy(&e, out + j * EPOLL_EVENT_SIZE, 4);
                e |= events;
                memcpy(out + j * EPOLL_EVENT_SIZE, &e, 4);
                break;
            }
        }
        if (j == count && count < max) {
            memcpy(out + count * EPOLL_EVENT_SIZE, &events, 4);
            memcpy(out + count * EPOLL_EVENT_SIZE + 4, &data, 8);
            ++count;
        }
    }
    RET(c, count);
}

static void s_epoll_wait(CPU64 *c) { epoll_wait_ms(c, (int)ARG(c, 0), PTR(c, 1), (int)ARG(c, 2), (int)ARG(c, 3)); }
// The signal mask (fifth argument) is ignored: the guest's signals aren't delivered through it.
static void s_epoll_pwait(CPU64 *c) { s_epoll_wait(c); }

// eventfd: a 64-bit counter behind one descriptor (libvideo.so's completion
// port manager needs it, with no fallback). The guest gets one end of a
// socket pair; a write adds to the counter and, from zero, puts a byte in
// through the other end, so the guest's end polls readable (epoll above,
// poll, select); a read drains those bytes and returns the counter, reset
// to zero (or one, with EFD_SEMAPHORE). read/write/close in posix64.c ask
// rt64_eventfd_io first.
#define MAX_EVENTFDS 64
static struct { int fd, other, flags; uint64_t count; } eventfds[MAX_EVENTFDS];
static pthread_mutex_t eventfd_lock = PTHREAD_MUTEX_INITIALIZER;
enum { L_EFD_SEMAPHORE = 1, L_EFD_NONBLOCK = 04000, L_EFD_CLOEXEC = 02000000 };

static int eventfd_slot(int fd) {
    for (int i = 0; i < MAX_EVENTFDS; ++i)
        if (eventfds[i].fd == fd && eventfds[i].other > 0) return i;
    return -1;
}

static void s_eventfd(CPU64 *c) {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) { fail(c, errno); return; }
    int flags = (int)ARG(c, 1);
    fcntl(fds[1], F_SETFL, O_NONBLOCK);
    if (flags & L_EFD_CLOEXEC) { fcntl(fds[0], F_SETFD, FD_CLOEXEC); fcntl(fds[1], F_SETFD, FD_CLOEXEC); }
    pthread_mutex_lock(&eventfd_lock);
    int slot = -1;
    for (int i = 0; i < MAX_EVENTFDS && slot < 0; ++i) if (eventfds[i].other <= 0) slot = i;
    if (slot < 0) { pthread_mutex_unlock(&eventfd_lock); close(fds[0]); close(fds[1]); fail(c, EMFILE); return; }
    eventfds[slot].fd = fds[0];
    eventfds[slot].other = fds[1];
    eventfds[slot].flags = flags;
    eventfds[slot].count = (uint32_t)ARG(c, 0);
    if (eventfds[slot].count) { char b = 1; (void)write(fds[1], &b, 1); }
    pthread_mutex_unlock(&eventfd_lock);
    RET(c, fds[0]);
}

int rt64_eventfd_io(int fd, int op, void *buf, size_t n, int64_t *result);
int rt64_eventfd_io(int fd, int op, void *buf, size_t n, int64_t *result) {
    pthread_mutex_lock(&eventfd_lock);
    int slot = eventfd_slot(fd);
    if (slot < 0) { pthread_mutex_unlock(&eventfd_lock); return 0; }
    if (op == 2) {   // close: both ends
        close(eventfds[slot].other);
        eventfds[slot].other = 0;
        eventfds[slot].fd = -1;
        pthread_mutex_unlock(&eventfd_lock);
        *result = close(fd) < 0 ? (rt64_set_errno(errno), -1) : 0;
        return 1;
    }
    if (n < 8) { pthread_mutex_unlock(&eventfd_lock); rt64_set_errno(EINVAL); *result = -1; return 1; }
    if (op == 1) {   // write: add
        uint64_t v;
        memcpy(&v, buf, 8);
        if (v == UINT64_MAX) { pthread_mutex_unlock(&eventfd_lock); rt64_set_errno(EINVAL); *result = -1; return 1; }
        int was_zero = eventfds[slot].count == 0;
        eventfds[slot].count += v;
        if (was_zero && v) { char b = 1; (void)write(eventfds[slot].other, &b, 1); }
        pthread_mutex_unlock(&eventfd_lock);
        *result = 8;
        return 1;
    }
    // read: wait for a non-zero counter unless non-blocking.
    while (eventfds[slot].count == 0) {
        int nonblock = eventfds[slot].flags & L_EFD_NONBLOCK;
        pthread_mutex_unlock(&eventfd_lock);
        if (nonblock) { rt64_set_errno(EAGAIN); *result = -1; return 1; }
        struct pollfd p = {fd, POLLIN, 0};
        poll(&p, 1, -1);
        pthread_mutex_lock(&eventfd_lock);
        slot = eventfd_slot(fd);
        if (slot < 0) { pthread_mutex_unlock(&eventfd_lock); rt64_set_errno(EBADF); *result = -1; return 1; }
    }
    uint64_t v = eventfds[slot].flags & L_EFD_SEMAPHORE ? 1 : eventfds[slot].count;
    eventfds[slot].count -= v;
    if (eventfds[slot].count == 0) {   // drain the wake-up bytes: no longer readable
        char drain[64];
        int flags = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        while (read(fd, drain, sizeof drain) > 0) {}
        fcntl(fd, F_SETFL, flags);
    }
    pthread_mutex_unlock(&eventfd_lock);
    memcpy(buf, &v, 8);
    *result = 8;
    return 1;
}

// timerfd has no counterpart here: ENOSYS, as an older Linux kernel would
// answer. asio's reactor then times out through epoll_wait instead.
static void s_unsupported(CPU64 *c) { fail(c, ENOSYS); }

static const RT64Shim shims[] = {
    {"epoll_create", s_epoll_create}, {"epoll_create1", s_epoll_create1}, {"epoll_ctl", s_epoll_ctl},
    {"epoll_wait", s_epoll_wait}, {"epoll_pwait", s_epoll_pwait},
    {"eventfd", s_eventfd}, {"timerfd_create", s_unsupported}, {0, 0},
};
RT64_SHIMS(shims, NULL)
