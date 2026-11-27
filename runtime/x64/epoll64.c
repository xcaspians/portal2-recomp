// Linux epoll on macOS kqueue, for the x86-64 runtime (round 28: P2CE's
// libvideo.so waits on file descriptors with it). An epoll instance is a
// kqueue; EPOLLIN and EPOLLOUT become EVFILT_READ and EVFILT_WRITE, the
// guest's 64-bit epoll_data rides in each kevent's udata, EPOLLET is EV_CLEAR
// and EPOLLONESHOT EV_ONESHOT. struct epoll_event is packed on x86-64:
// 4 bytes of events, then 8 of data (12 bytes).
#include "rt64.h"

#include <errno.h>
#include <fcntl.h>
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

// eventfd and timerfd have no macOS counterpart: they answer ENOSYS, as an
// older Linux kernel would. The callers seen so far (asio's reactor, in
// libvideo.so) then fall back to a pipe and to epoll_wait's own timeout.
static void s_unsupported(CPU64 *c) { fail(c, ENOSYS); }

static const RT64Shim shims[] = {
    {"epoll_create", s_epoll_create}, {"epoll_create1", s_epoll_create1}, {"epoll_ctl", s_epoll_ctl},
    {"epoll_wait", s_epoll_wait}, {"epoll_pwait", s_epoll_pwait},
    {"eventfd", s_unsupported}, {"timerfd_create", s_unsupported}, {0, 0},
};
RT64_SHIMS(shims, NULL)
