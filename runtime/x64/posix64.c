// POSIX file, directory and memory shims of the x86-64 runtime, with the
// Linux x86-64 layouts and constants the guest uses: open() flags, struct
// stat (144 bytes, through glibc's __xstat family), struct dirent64,
// struct statvfs, mmap() flags, struct utsname, struct sysinfo, and the
// raw syscall() numbers the engine calls (gettid, getrandom).
#include "rt64.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <utime.h>

#define S(i) ((const char *)PTR(c, i))
#define I(i) ((int)ARG(c, i))
#define CHECK(c, r) do { int64_t r_ = (int64_t)(r); if (r_ < 0) rt64_set_errno(errno); RET(c, r_); } while (0)

static int host_open_flags(int f) {
    int h = f & 3;      // O_RDONLY, O_WRONLY, O_RDWR
    if (f & 0x40) h |= O_CREAT;
    if (f & 0x80) h |= O_EXCL;
    if (f & 0x100) h |= O_NOCTTY;
    if (f & 0x200) h |= O_TRUNC;
    if (f & 0x400) h |= O_APPEND;
    if (f & 0x800) h |= O_NONBLOCK;
    if (f & 0x10000) h |= O_DIRECTORY;
    if (f & 0x20000) h |= O_NOFOLLOW;
    if (f & 0x80000) h |= O_CLOEXEC;
    return h;
}


// /proc emulation: the few Linux files the engine and its libraries read.
// /proc/self/exe is the program's Linux path (RT64_EXE, set by src/main64.c).
static int content_fd(const char *text) {
    char name[] = "/tmp/rt64-procXXXXXX";
    int fd = mkstemp(name);
    if (fd < 0) return -1;
    unlink(name);
    if (write(fd, text, strlen(text)) < 0) { close(fd); return -1; }
    lseek(fd, 0, SEEK_SET);
    return fd;
}

int rt64_proc_open(const char *path) {
    char text[16384];
    if (strncmp(path, "/proc/", 6)) return -2;
    if (!strcmp(path, "/proc/cpuinfo")) {
        unsigned n = 0; size_t length = sizeof n;
        if (sysctlbyname("hw.logicalcpu", &n, &length, NULL, 0) || !n) n = 1;
        size_t used = 0;
        for (unsigned i = 0; i < n && used < sizeof text - 512; ++i)
            used += (size_t)snprintf(text + used, sizeof text - used,
                "processor\t: %u\nvendor_id\t: GenuineIntel\nmodel name\t: rt64 x86-64 on Apple silicon\n"
                "cpu MHz\t\t: 3000.000\ncpu cores\t: %u\nflags\t\t: fpu tsc cx8 cmov mmx fxsr sse sse2 ht syscall "
                "lm pni ssse3 cx16 sse4_1 popcnt lahf_lm abm bmi1 bmi2\n\n", i, n);
        return content_fd(text);
    }
    if (!strcmp(path, "/proc/meminfo")) {
        uint64_t memory = 0; size_t length = sizeof memory;
        sysctlbyname("hw.memsize", &memory, &length, NULL, 0);
        snprintf(text, sizeof text, "MemTotal:       %llu kB\nMemFree:        %llu kB\nMemAvailable:   %llu kB\n"
                 "SwapTotal:      0 kB\nSwapFree:       0 kB\n", (unsigned long long)(memory / 1024),
                 (unsigned long long)(memory / 2048), (unsigned long long)(memory / 2048));
        return content_fd(text);
    }
    if (!strcmp(path, "/proc/self/maps")) {
        size_t used = 0;
        text[0] = 0;
        for (size_t i = 0; i < rt64_loaded_count() && used < sizeof text - 256; ++i) {
            uint64_t base;
            const RT64Module *m = rt64_loaded_module(i, &base);
            used += (size_t)snprintf(text + used, sizeof text - used, "%llx-%llx r-xp 00000000 00:00 0 %s\n",
                                     (unsigned long long)base, (unsigned long long)(base + m->size), m->name);
        }
        return content_fd(text);
    }
    if (!strcmp(path, "/proc/self/status")) {
        snprintf(text, sizeof text, "Name:\tp2ce\nState:\tR (running)\nPid:\t%d\nTracerPid:\t0\nThreads:\t1\n", getpid());
        return content_fd(text);
    }
    if (!strcmp(path, "/proc/self/cmdline") || !strcmp(path, "/proc/self/environ") || !strcmp(path, "/proc/stat")) {
        return content_fd("");
    }
    return -2;    // other /proc paths: the host's open() fails with ENOENT
}

// A game folder mirrored with symlinks to the read-only depot: writing to a
// file replaces its symlink with a private copy first (RT64_COPY_ON_WRITE).
static void copy_on_write(const char *path) {
    struct stat st;
    if (!getenv("RT64_COPY_ON_WRITE") || lstat(path, &st) || !S_ISLNK(st.st_mode)) return;
    char target[PATH_MAX];
    if (!realpath(path, target)) { unlink(path); return; }
    char temporary[PATH_MAX];
    snprintf(temporary, sizeof temporary, "%s.rt64-copy", path);
    FILE *in = fopen(target, "rb"), *out = fopen(temporary, "wb");
    if (in && out) {
        char buffer[65536];
        size_t n;
        while ((n = fread(buffer, 1, sizeof buffer, in))) fwrite(buffer, 1, n, out);
    }
    if (in) fclose(in);
    if (out) { fclose(out); rename(temporary, path); }
}
void rt64_prepare_write(const char *path) { copy_on_write(path); }

static void s_open(CPU64 *c) {
    int proc = rt64_proc_open(S(0));
    if (proc != -2) { if (proc < 0) rt64_set_errno(ENOENT); RET(c, proc); return; }
    if (I(1) & 3) copy_on_write(S(0));
    CHECK(c, open(S(0), host_open_flags(I(1)), (mode_t)ARG(c, 2)));
}
static void s_openat(CPU64 *c) {
    int dir = I(0) == -100 ? AT_FDCWD : I(0);
    CHECK(c, openat(dir, S(1), host_open_flags(I(2)), (mode_t)ARG(c, 3)));
}
// An eventfd (epoll64.c) answers read, write and close itself: 1 when fd is
// one and the operation (0 read, 1 write, 2 close) was done, its result in
// *result. Declared here, not in rt64.h, whose hash keys every lifted
// module's compile cache.
int rt64_eventfd_io(int fd, int op, void *buf, size_t n, int64_t *result);
static void s_close(CPU64 *c) { int64_t r; if (rt64_eventfd_io(I(0), 2, NULL, 0, &r)) { RET(c, r); return; } CHECK(c, close(I(0))); }
static void s_read(CPU64 *c) {
    int64_t r;
    if (rt64_eventfd_io(I(0), 0, PTR(c, 1), (size_t)ARG(c, 2), &r)) { RET(c, r); return; }
    CHECK(c, read(I(0), PTR(c, 1), (size_t)ARG(c, 2)));
}
static void s_write(CPU64 *c) {
    int64_t r;
    if (rt64_eventfd_io(I(0), 1, PTR(c, 1), (size_t)ARG(c, 2), &r)) { RET(c, r); return; }
    CHECK(c, write(I(0), PTR(c, 1), (size_t)ARG(c, 2)));
}
static void s_pread(CPU64 *c) { CHECK(c, pread(I(0), PTR(c, 1), (size_t)ARG(c, 2), (off_t)ARG(c, 3))); }
static void s_pwrite(CPU64 *c) { CHECK(c, pwrite(I(0), PTR(c, 1), (size_t)ARG(c, 2), (off_t)ARG(c, 3))); }
static void s_writev(CPU64 *c) { CHECK(c, writev(I(0), PTR(c, 1), I(2))); }   // struct iovec: same layout
static void s_lseek(CPU64 *c) { CHECK(c, lseek(I(0), (off_t)ARG(c, 1), I(2))); }
static void s_dup(CPU64 *c) { CHECK(c, dup(I(0))); }
static void s_dup2(CPU64 *c) { CHECK(c, dup2(I(0), I(1))); }
static void s_pipe(CPU64 *c) { CHECK(c, pipe(PTR(c, 0))); }
static void s_isatty(CPU64 *c) { RET(c, isatty(I(0))); }
static void s_access(CPU64 *c) { CHECK(c, access(S(0), I(1))); }
static void s_unlink(CPU64 *c) { CHECK(c, unlink(S(0))); }
static void s_rmdir(CPU64 *c) { CHECK(c, rmdir(S(0))); }
static void s_mkdir(CPU64 *c) { CHECK(c, mkdir(S(0), (mode_t)ARG(c, 1))); }
static void s_chdir(CPU64 *c) { CHECK(c, chdir(S(0))); }
static void s_chmod(CPU64 *c) { CHECK(c, chmod(S(0), (mode_t)ARG(c, 1))); }
static void s_fchmod(CPU64 *c) { CHECK(c, fchmod(I(0), (mode_t)ARG(c, 1))); }
static void s_symlink(CPU64 *c) { CHECK(c, symlink(S(0), S(1))); }
static void s_link(CPU64 *c) { CHECK(c, link(S(0), S(1))); }
static void s_truncate(CPU64 *c) { CHECK(c, truncate(S(0), (off_t)ARG(c, 1))); }
static void s_ftruncate(CPU64 *c) { CHECK(c, ftruncate(I(0), (off_t)ARG(c, 1))); }
static void s_fsync(CPU64 *c) { CHECK(c, fsync(I(0))); }
static void s_readlink(CPU64 *c) {
    if (!strcmp(S(0), "/proc/self/exe") && getenv("RT64_EXE")) {
        const char *exe = getenv("RT64_EXE");
        size_t n = strlen(exe) < (size_t)ARG(c, 2) ? strlen(exe) : (size_t)ARG(c, 2);
        memcpy(PTR(c, 1), exe, n);
        RET(c, (int64_t)n);
        return;
    }
    CHECK(c, readlink(S(0), PTR(c, 1), (size_t)ARG(c, 2)));
}
static void s_getcwd(CPU64 *c) {
    char buffer[PATH_MAX];
    if (!getcwd(buffer, sizeof buffer)) { rt64_set_errno(errno); RET(c, 0); return; }
    if (!ARG(c, 0)) { RET(c, rt64_guest_strdup(c, buffer)); return; }
    if (strlen(buffer) + 1 > (size_t)ARG(c, 1)) { rt64_set_errno(ERANGE); RET(c, 0); return; }
    strcpy(PTR(c, 0), buffer);
    RET(c, ARG(c, 0));
}
static void s_realpath(CPU64 *c) {
    char buffer[PATH_MAX];
    if (!realpath(S(0), buffer)) { rt64_set_errno(errno); RET(c, 0); return; }
    if (!ARG(c, 1)) { RET(c, rt64_guest_strdup(c, buffer)); return; }
    strcpy(PTR(c, 1), buffer);
    RET(c, ARG(c, 1));
}
static void s_fcntl(CPU64 *c) {
    switch (I(1)) {
    case 1: CHECK(c, fcntl(I(0), F_GETFD)); break;
    case 2: CHECK(c, fcntl(I(0), F_SETFD, I(2))); break;
    case 3: { int f = fcntl(I(0), F_GETFL); if (f < 0) { CHECK(c, f); break; }
              int l = f & 3; if (f & O_APPEND) l |= 0x400; if (f & O_NONBLOCK) l |= 0x800; RET(c, l); break; }
    case 4: CHECK(c, fcntl(I(0), F_SETFL, host_open_flags(I(2)) & (O_APPEND | O_NONBLOCK))); break;
    case 1030: CHECK(c, fcntl(I(0), F_DUPFD_CLOEXEC, I(2))); break;
    case 0: CHECK(c, fcntl(I(0), F_DUPFD, I(2))); break;
    default: rt64_set_errno(EINVAL); RET(c, -1); break;
    }
}
static void s_flock(CPU64 *c) { CHECK(c, flock(I(0), I(1))); }   // LOCK_* values are the same
static void s_ioctl(CPU64 *c) { rt64_set_errno(ENOTTY); RET(c, -1); }
static void s_poll(CPU64 *c) { CHECK(c, poll(PTR(c, 0), (nfds_t)ARG(c, 1), I(2))); }   // struct pollfd: same

// struct stat (Linux x86-64)
static void put_stat(uint64_t at, const struct stat *st) {
    memset((void *)(uintptr_t)at, 0, 144);
    W64(at, (uint64_t)st->st_dev); W64(at + 8, st->st_ino); W64(at + 16, st->st_nlink);
    W32(at + 24, st->st_mode); W32(at + 28, st->st_uid); W32(at + 32, st->st_gid);
    W64(at + 40, (uint64_t)st->st_rdev); W64(at + 48, (uint64_t)st->st_size);
    W64(at + 56, (uint64_t)st->st_blksize); W64(at + 64, (uint64_t)st->st_blocks);
    W64(at + 72, (uint64_t)st->st_atimespec.tv_sec); W64(at + 80, (uint64_t)st->st_atimespec.tv_nsec);
    W64(at + 88, (uint64_t)st->st_mtimespec.tv_sec); W64(at + 96, (uint64_t)st->st_mtimespec.tv_nsec);
    W64(at + 104, (uint64_t)st->st_ctimespec.tv_sec); W64(at + 112, (uint64_t)st->st_ctimespec.tv_nsec);
}
static void stat_result(CPU64 *c, int r, const struct stat *st, uint64_t out) {
    if (r) { rt64_set_errno(errno); RET(c, -1); return; }
    put_stat(out, st);
    RET(c, 0);
}
static void s_stat(CPU64 *c) { struct stat st; stat_result(c, stat(S(0), &st), &st, ARG(c, 1)); }
static void s_lstat(CPU64 *c) { struct stat st; stat_result(c, lstat(S(0), &st), &st, ARG(c, 1)); }
static void s_fstat(CPU64 *c) { struct stat st; stat_result(c, fstat(I(0), &st), &st, ARG(c, 1)); }
static void s_xstat(CPU64 *c) { struct stat st; stat_result(c, stat(S(1), &st), &st, ARG(c, 2)); }
static void s_lxstat(CPU64 *c) { struct stat st; stat_result(c, lstat(S(1), &st), &st, ARG(c, 2)); }
static void s_fxstat(CPU64 *c) { struct stat st; stat_result(c, fstat(I(1), &st), &st, ARG(c, 2)); }
static void s_fstatat(CPU64 *c) {
    struct stat st;
    int dir = I(0) == -100 ? AT_FDCWD : I(0);
    int flags = (I(3) & 0x100) ? AT_SYMLINK_NOFOLLOW : 0;
    stat_result(c, fstatat(dir, S(1), &st, flags), &st, ARG(c, 2));
}
// struct statvfs (Linux x86-64): 11 unsigned longs, then spare ints.
static void s_statvfs(CPU64 *c) {
    struct statvfs v;
    if (statvfs(S(0), &v)) { rt64_set_errno(errno); RET(c, -1); return; }
    uint64_t f[11] = {v.f_bsize, v.f_frsize, v.f_blocks, v.f_bfree, v.f_bavail, v.f_files, v.f_ffree,
                      v.f_favail, v.f_fsid, v.f_flag, v.f_namemax};
    memset(PTR(c, 1), 0, 112);
    memcpy(PTR(c, 1), f, sizeof f);
    RET(c, 0);
}
static void s_utime(CPU64 *c) { CHECK(c, utime(S(0), PTR(c, 1))); }       // struct utimbuf: two time_t
static void s_utimes(CPU64 *c) {
    if (!ARG(c, 1)) { CHECK(c, utimes(S(0), NULL)); return; }
    struct timeval tv[2] = {{(time_t)R64(ARG(c, 1)), (suseconds_t)R64(ARG(c, 1) + 8)},
                            {(time_t)R64(ARG(c, 1) + 16), (suseconds_t)R64(ARG(c, 1) + 24)}};
    CHECK(c, utimes(S(0), tv));
}

// Directories: a guest DIR * is this wrapper; readdir returns a Linux dirent64.
typedef struct { DIR *dir; uint8_t entry[280]; } GuestDir;
static void s_opendir(CPU64 *c) {
    DIR *d = opendir(S(0));
    if (!d) { rt64_set_errno(errno); RET(c, 0); return; }
    GuestDir *g = calloc(1, sizeof *g);
    g->dir = d;
    RET(c, (uintptr_t)g);
}
static void s_fdopendir(CPU64 *c) {
    DIR *d = fdopendir(I(0));
    if (!d) { rt64_set_errno(errno); RET(c, 0); return; }
    GuestDir *g = calloc(1, sizeof *g);
    g->dir = d;
    RET(c, (uintptr_t)g);
}
static void fill_dirent(uint8_t *out, const struct dirent *e) {
    memset(out, 0, 280);
    uint64_t ino = e->d_ino, off = 0;
    uint16_t reclen = (uint16_t)((19 + e->d_namlen + 1 + 7) & ~7);
    memcpy(out, &ino, 8); memcpy(out + 8, &off, 8); memcpy(out + 16, &reclen, 2);
    out[18] = e->d_type;                // DT_* values are the same
    memcpy(out + 19, e->d_name, e->d_namlen < 255 ? e->d_namlen : 255);
}
static void s_readdir(CPU64 *c) {
    GuestDir *g = PTR(c, 0);
    errno = 0;
    struct dirent *e = readdir(g->dir);
    if (!e) { if (errno) rt64_set_errno(errno); RET(c, 0); return; }
    fill_dirent(g->entry, e);
    RET(c, (uintptr_t)g->entry);
}
static void s_closedir(CPU64 *c) { GuestDir *g = PTR(c, 0); int r = closedir(g->dir); free(g); RET(c, r); }
static void s_dirfd(CPU64 *c) { RET(c, dirfd(((GuestDir *)PTR(c, 0))->dir)); }
static void s_rewinddir(CPU64 *c) { rewinddir(((GuestDir *)PTR(c, 0))->dir); }
// scandir(path, &list, filter, compare): entries and list from the guest's malloc.
static void s_scandir(CPU64 *c) {
    uint64_t list_out = ARG(c, 1), filter = ARG(c, 2), compare = ARG(c, 3);
    DIR *d = opendir(S(0));
    if (!d) { rt64_set_errno(errno); RET(c, -1); return; }
    size_t count = 0, capacity = 16;
    uint64_t *entries = malloc(capacity * 8);
    struct dirent *e;
    while ((e = readdir(d))) {
        uint64_t entry = rt64_guest_malloc(c, 280);
        fill_dirent((uint8_t *)(uintptr_t)entry, e);
        if (filter) {
            uint64_t saved[16]; memcpy(saved, c->r, sizeof saved);
            c->r[RDI] = entry; rt64_call(c, filter);
            int keep = (int)c->r[RAX];
            memcpy(c->r, saved, sizeof saved);
            if (!keep) continue;          // leaked to the guest heap: tiny, and rare
        }
        if (count == capacity) entries = realloc(entries, (capacity *= 2) * 8);
        entries[count++] = entry;
    }
    closedir(d);
    if (compare) {       // insertion sort with the guest comparator (directories are small)
        uint64_t saved[16]; memcpy(saved, c->r, sizeof saved);
        uint64_t pair = rt64_guest_malloc(c, 16);
        for (size_t i = 1; i < count; ++i)
            for (size_t j = i; j > 0; --j) {
                W64(pair, entries[j - 1]); W64(pair + 8, entries[j]);
                c->r[RDI] = pair; c->r[RSI] = pair + 8; rt64_call(c, compare);
                if ((int)c->r[RAX] <= 0) break;
                uint64_t t = entries[j]; entries[j] = entries[j - 1]; entries[j - 1] = t;
            }
        memcpy(c->r, saved, sizeof saved);
    }
    uint64_t list = rt64_guest_malloc(c, (count ? count : 1) * 8);
    memcpy((void *)(uintptr_t)list, entries, count * 8);
    free(entries);
    W64(list_out, list);
    RET(c, (int64_t)count);
}
static void s_alphasort(CPU64 *c) {
    RET(c, (int64_t)strcoll((const char *)(uintptr_t)(R64(ARG(c, 0)) + 19), (const char *)(uintptr_t)(R64(ARG(c, 1)) + 19)));
}

// Memory mappings (Linux MAP_* and MADV_* values).
static void s_mmap(CPU64 *c) {
    int f = I(3), flags = 0;
    if (f & 1) flags |= MAP_SHARED;
    if (f & 2) flags |= MAP_PRIVATE;
    if (f & 0x10) flags |= MAP_FIXED;
    if (f & 0x20) flags |= MAP_ANON;
    if (f & 0x4000) flags |= MAP_NORESERVE;
    void *p = mmap(PTR(c, 0), (size_t)ARG(c, 1), I(2) & 7, flags, (f & 0x20) ? -1 : I(4), (off_t)ARG(c, 5));
    if (p == MAP_FAILED) { rt64_set_errno(errno); RET(c, -1); return; }
    RET(c, (uintptr_t)p);
}
static void s_munmap(CPU64 *c) { CHECK(c, munmap(PTR(c, 0), (size_t)ARG(c, 1))); }
static void s_mprotect(CPU64 *c) { CHECK(c, mprotect(PTR(c, 0), (size_t)ARG(c, 1), I(2) & 7)); }
static void s_madvise(CPU64 *c) {
    if (I(2) == 4) {      // MADV_DONTNEED: Linux gives back zero pages; remap anonymous memory there
        void *p = mmap(PTR(c, 0), (size_t)ARG(c, 1), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
        RET(c, p == MAP_FAILED ? -1 : 0);
        return;
    }
    RET(c, 0);            // other advice is only a hint
}

// struct utsname (Linux): six fields of 65 bytes.
static void s_uname(CPU64 *c) {
    char *u = PTR(c, 0);
    memset(u, 0, 6 * 65);
    strcpy(u, "Linux");
    gethostname(u + 65, 64);
    strcpy(u + 130, "5.15.0-rt64");
    strcpy(u + 195, "#1 SMP");
    strcpy(u + 260, "x86_64");
    strcpy(u + 325, "(none)");
    RET(c, 0);
}
// struct sysinfo (Linux x86-64).
static void s_sysinfo(CPU64 *c) {
    uint64_t at = ARG(c, 0), memory = 0;
    size_t size = sizeof memory;
    sysctlbyname("hw.memsize", &memory, &size, NULL, 0);
    struct timeval boot; size = sizeof boot;
    sysctlbyname("kern.boottime", &boot, &size, NULL, 0);
    memset((void *)(uintptr_t)at, 0, 112);
    W64(at, (uint64_t)(time(NULL) - boot.tv_sec));
    W64(at + 32, memory); W64(at + 40, memory / 2);        // totalram, freeram (an estimate)
    W16(at + 80, 1);                                       // procs
    W32(at + 104, 1);                                      // mem_unit
    RET(c, 0);
}
// syscall(number, ...) with Linux x86-64 numbers.
static void s_syscall(CPU64 *c) {
    switch ((int)ARG(c, 0)) {
    case 186: { uint64_t tid; pthread_threadid_np(NULL, &tid); RET(c, (int64_t)(tid & 0x7fffffff)); break; }   // gettid
    case 318: arc4random_buf(PTR(c, 1), (size_t)ARG(c, 2)); RET(c, ARG(c, 2)); break;                     // getrandom
    case 39: RET(c, getpid()); break;
    default: rt64_set_errno(ENOSYS); RET(c, -1); break;
    }
}
// The SYSCALL instruction (inlined system calls): Linux returns -errno in rax.
void rt64_syscall(CPU64 *c) {
    int64_t r;
    uint64_t a0 = c->r[RDI], a1 = c->r[RSI], a2 = c->r[RDX];
    switch ((int)c->r[RAX]) {
    case 0: r = read((int)a0, (void *)(uintptr_t)a1, (size_t)a2); break;
    case 1: r = write((int)a0, (const void *)(uintptr_t)a1, (size_t)a2); break;
    case 3: r = close((int)a0); break;
    case 39: r = getpid(); break;
    case 186: { uint64_t tid; pthread_threadid_np(NULL, &tid); r = (int64_t)(tid & 0x7fffffff); break; }
    case 318: arc4random_buf((void *)(uintptr_t)a0, (size_t)a1); r = (int64_t)a1; break;
    case 60: case 231: fflush(NULL); _exit((int)a0);
    default: c->r[RAX] = (uint64_t)-38; return;          // -ENOSYS
    }
    c->r[RAX] = r < 0 ? (uint64_t)-(int64_t)rt64_linux_errno(errno) : (uint64_t)r;
}
static void s_getentropy(CPU64 *c) { arc4random_buf(PTR(c, 0), (size_t)ARG(c, 1)); RET(c, 0); }
static void s_gethostname(CPU64 *c) { CHECK(c, gethostname(PTR(c, 0), (size_t)ARG(c, 1))); }
static void s_getdtablesize(CPU64 *c) { RET(c, getdtablesize()); }
static void s_pathconf(CPU64 *c) { RET(c, I(1) == 4 ? 4096 : I(1) == 3 ? 255 : -1); }   // _PC_PATH_MAX, _PC_NAME_MAX
static void s_dirname(CPU64 *c) { RET(c, (uintptr_t)dirname(PTR(c, 0))); }
static void s_basename(CPU64 *c) { const char *s = S(0), *slash = strrchr(s, '/'); RET(c, (uintptr_t)(slash ? slash + 1 : s)); }
static void s_tmpnam_r(CPU64 *c) {
    char *p = PTR(c, 0);
    if (!p) { RET(c, 0); return; }
    snprintf(p, 20, "/tmp/rt64%06x", arc4random() & 0xffffff);
    RET(c, (uintptr_t)p);
}

static const RT64Shim shims[] = {
    {"open", s_open}, {"open64", s_open}, {"__open_2", s_open}, {"openat", s_openat}, {"close", s_close},
    {"read", s_read}, {"write", s_write}, {"pread", s_pread}, {"pread64", s_pread}, {"pwrite", s_pwrite},
    {"pwrite64", s_pwrite}, {"writev", s_writev}, {"lseek", s_lseek}, {"lseek64", s_lseek}, {"dup", s_dup},
    {"dup2", s_dup2}, {"pipe", s_pipe}, {"isatty", s_isatty}, {"access", s_access}, {"unlink", s_unlink},
    {"rmdir", s_rmdir}, {"mkdir", s_mkdir}, {"chdir", s_chdir}, {"chmod", s_chmod}, {"fchmod", s_fchmod},
    {"symlink", s_symlink}, {"link", s_link}, {"truncate", s_truncate}, {"ftruncate", s_ftruncate},
    {"ftruncate64", s_ftruncate}, {"fsync", s_fsync}, {"readlink", s_readlink}, {"getcwd", s_getcwd},
    {"realpath", s_realpath}, {"fcntl", s_fcntl}, {"fcntl64", s_fcntl}, {"flock", s_flock}, {"ioctl", s_ioctl},
    {"poll", s_poll}, {"stat", s_stat}, {"stat64", s_stat}, {"lstat", s_lstat}, {"lstat64", s_lstat},
    {"fstat", s_fstat}, {"fstat64", s_fstat}, {"__xstat", s_xstat}, {"__xstat64", s_xstat},
    {"__lxstat", s_lxstat}, {"__lxstat64", s_lxstat}, {"__fxstat", s_fxstat}, {"__fxstat64", s_fxstat},
    {"fstatat", s_fstatat}, {"fstatat64", s_fstatat}, {"statvfs", s_statvfs}, {"statvfs64", s_statvfs},
    {"utime", s_utime}, {"utimes", s_utimes},
    {"opendir", s_opendir}, {"fdopendir", s_fdopendir}, {"readdir", s_readdir}, {"readdir64", s_readdir},
    {"closedir", s_closedir}, {"dirfd", s_dirfd}, {"rewinddir", s_rewinddir}, {"scandir", s_scandir},
    {"scandir64", s_scandir}, {"alphasort", s_alphasort}, {"alphasort64", s_alphasort},
    {"mmap", s_mmap}, {"mmap64", s_mmap}, {"munmap", s_munmap}, {"mprotect", s_mprotect}, {"madvise", s_madvise},
    {"uname", s_uname}, {"sysinfo", s_sysinfo}, {"syscall", s_syscall}, {"getentropy", s_getentropy},
    {"gethostname", s_gethostname}, {"getdtablesize", s_getdtablesize}, {"pathconf", s_pathconf},
    {"dirname", s_dirname}, {"basename", s_basename}, {"__xpg_basename", s_basename}, {"tmpnam_r", s_tmpnam_r},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
