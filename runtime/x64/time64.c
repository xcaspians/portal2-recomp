// Time shims of the x86-64 runtime. struct timespec and struct tm have the
// same layout on Linux x86-64 and Apple arm64; struct timeval does not
// (Apple's tv_usec is an int), nor do the clock ids.
#include "rt64.h"

#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

static int host_clock(int linux_id, clockid_t *id) {
    switch (linux_id) {
    case 0: case 5: case 8: *id = CLOCK_REALTIME; return 0;       // REALTIME, _COARSE, _ALARM
    case 1: case 6: case 7: case 9: *id = CLOCK_MONOTONIC; return 0;   // MONOTONIC, _COARSE, BOOTTIME
    case 4: *id = CLOCK_MONOTONIC_RAW; return 0;
    case 2: *id = CLOCK_PROCESS_CPUTIME_ID; return 0;
    case 3: *id = CLOCK_THREAD_CPUTIME_ID; return 0;
    default: return -1;
    }
}

static void s_clock_gettime(CPU64 *c) {
    clockid_t id;
    if (host_clock((int)ARG(c, 0), &id)) { rt64_set_errno(EINVAL); RET(c, -1); return; }
    RET(c, clock_gettime(id, PTR(c, 1)));
}
static void s_clock_getres(CPU64 *c) {
    clockid_t id;
    if (host_clock((int)ARG(c, 0), &id)) { rt64_set_errno(EINVAL); RET(c, -1); return; }
    RET(c, clock_getres(id, PTR(c, 1)));
}
static void put_timeval(uint64_t at, struct timeval tv) { W64(at, (uint64_t)tv.tv_sec); W64(at + 8, (uint64_t)(int64_t)tv.tv_usec); }
static void s_gettimeofday(CPU64 *c) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (ARG(c, 0)) put_timeval(ARG(c, 0), tv);
    if (ARG(c, 1)) { W32(ARG(c, 1), 0); W32(ARG(c, 1) + 4, 0); }
    RET(c, 0);
}
static void s_time(CPU64 *c) { time_t t = time(NULL); if (ARG(c, 0)) W64(ARG(c, 0), (uint64_t)t); RET(c, t); }
static void s_nanosleep(CPU64 *c) { int r = nanosleep(PTR(c, 0), PTR(c, 1)); if (r) rt64_set_errno(errno); RET(c, r); }
static void s_usleep(CPU64 *c) { RET(c, usleep((useconds_t)ARG(c, 0))); }
static void s_sleep(CPU64 *c) { RET(c, sleep((unsigned)ARG(c, 0))); }
static void s_clock(CPU64 *c) { RET(c, (int64_t)((double)clock() * 1000000.0 / CLOCKS_PER_SEC)); }   // Linux CLOCKS_PER_SEC
static void s_localtime_r(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)localtime_r(&t, PTR(c, 1))); }
static void s_gmtime_r(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)gmtime_r(&t, PTR(c, 1))); }
static void s_localtime(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)localtime(&t)); }
static void s_gmtime(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)gmtime(&t)); }
static void s_mktime(CPU64 *c) { RET(c, mktime(PTR(c, 0))); }
static void s_timegm(CPU64 *c) { RET(c, timegm(PTR(c, 0))); }
static void s_asctime_r(CPU64 *c) { RET(c, (uintptr_t)asctime_r(PTR(c, 0), PTR(c, 1))); }
static void s_asctime(CPU64 *c) { RET(c, (uintptr_t)asctime(PTR(c, 0))); }
static void s_ctime_r(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)ctime_r(&t, PTR(c, 1))); }
static void s_ctime(CPU64 *c) { time_t t = (time_t)R64(ARG(c, 0)); RET(c, (uintptr_t)ctime(&t)); }
static void s_strftime(CPU64 *c) { RET(c, strftime(PTR(c, 0), (size_t)ARG(c, 1), PTR(c, 2), PTR(c, 3))); }
static void s_difftime(CPU64 *c) { c->xmm[0].g[0] = difftime((time_t)ARG(c, 0), (time_t)ARG(c, 1)); }
// struct rusage: two timevals then 14 longs (Linux).
static void s_getrusage(CPU64 *c) {
    struct rusage u;
    int who = (int)ARG(c, 0) == -1 ? RUSAGE_CHILDREN : RUSAGE_SELF;     // RUSAGE_THREAD (1) as SELF
    int r = getrusage(who, &u);
    uint64_t at = ARG(c, 1);
    memset((void *)(uintptr_t)at, 0, 144);
    if (!r) {
        put_timeval(at, u.ru_utime); put_timeval(at + 16, u.ru_stime);
        long fields[14] = {u.ru_maxrss / 1024, u.ru_ixrss, u.ru_idrss, u.ru_isrss, u.ru_minflt, u.ru_majflt,
                           u.ru_nswap, u.ru_inblock, u.ru_oublock, u.ru_msgsnd, u.ru_msgrcv, u.ru_nsignals,
                           u.ru_nvcsw, u.ru_nivcsw};
        memcpy((void *)(uintptr_t)(at + 32), fields, sizeof fields);
    }
    RET(c, r);
}
// struct tms: four clock_t (long), in Linux ticks of 100 Hz.
static void s_times(CPU64 *c) {
    struct tms t;
    clock_t now = times(&t);
    double scale = 100.0 / (double)sysconf(_SC_CLK_TCK);
    if (ARG(c, 0)) {
        W64(ARG(c, 0), (uint64_t)(t.tms_utime * scale)); W64(ARG(c, 0) + 8, (uint64_t)(t.tms_stime * scale));
        W64(ARG(c, 0) + 16, (uint64_t)(t.tms_cutime * scale)); W64(ARG(c, 0) + 24, (uint64_t)(t.tms_cstime * scale));
    }
    RET(c, (int64_t)(now * scale));
}

static long guest_timezone;
static int guest_daylight;
__attribute__((constructor)) static void zone(void) {
    tzset();
    guest_timezone = timezone;
    guest_daylight = daylight;
}

static const RT64Shim shims[] = {
    {"clock_gettime", s_clock_gettime}, {"clock_getres", s_clock_getres}, {"gettimeofday", s_gettimeofday},
    {"time", s_time}, {"nanosleep", s_nanosleep}, {"usleep", s_usleep}, {"sleep", s_sleep}, {"clock", s_clock},
    {"localtime_r", s_localtime_r}, {"gmtime_r", s_gmtime_r}, {"localtime", s_localtime}, {"gmtime", s_gmtime},
    {"mktime", s_mktime}, {"timegm", s_timegm}, {"asctime_r", s_asctime_r}, {"asctime", s_asctime},
    {"ctime_r", s_ctime_r}, {"ctime", s_ctime}, {"strftime", s_strftime}, {"__strftime_l", s_strftime},
    {"strftime_l", s_strftime}, {"difftime", s_difftime}, {"getrusage", s_getrusage}, {"times", s_times},
    {NULL, NULL},
};
static const RT64Data data[] = {{"timezone", &guest_timezone}, {"__timezone", &guest_timezone},
                                {"daylight", &guest_daylight}, {"__daylight", &guest_daylight}, {NULL, NULL}};
RT64_SHIMS(shims, data)
