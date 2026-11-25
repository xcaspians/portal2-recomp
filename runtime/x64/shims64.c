// Import shims of the x86-64 runtime: the registry (RT64_SHIMS tables of
// every runtime/x64/<domain>64.c), the guest errno, and the C library
// functions whose arguments and results have the same layout on Linux x86-64
// and Apple arm64 (both LP64), so a guest pointer goes to the host function
// as is. Arguments: rdi, rsi, rdx, rcx, r8, r9 (System V AMD64), doubles in
// xmm0-7; result in rax or xmm0.
#include "rt64.h"

#include <ctype.h>
#include <malloc/malloc.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

static RT64ShimTable *tables;

void rt64_add_shims(RT64ShimTable *table) {
    table->next = tables;
    tables = table;
}

rt64_shim rt64_find_shim(const char *name) {
    for (RT64ShimTable *t = tables; t; t = t->next)
        for (const RT64Shim *s = t->shims; s && s->name; ++s)
            if (!strcmp(s->name, name)) return s->fn;
    // Steam runtime builds link some file functions with --wrap (P2CE's
    // launcher: __wrap_access, __wrap_fopen, __wrap_open, __wrap___xstat64...),
    // whose wrappers fix the case of paths for Linux. macOS file systems ignore
    // case by default: the plain function answers (round 23).
    if (!strncmp(name, "__wrap_", 7) && name[7]) return rt64_find_shim(name + 7);
    return NULL;
}

void *rt64_find_data(const char *name) {
    for (RT64ShimTable *t = tables; t; t = t->next)
        for (const RT64Data *d = t->data; d && d->name; ++d)
            if (!strcmp(d->name, name)) return d->address;
    return NULL;
}

// errno: the guest reads Linux numbers through __errno_location().
static _Thread_local int guest_errno;
int *rt64_errno_slot(void) { return &guest_errno; }

int rt64_linux_errno(int e) {
    switch (e) {
    case EAGAIN: return 11; case EDEADLK: return 35; case ENAMETOOLONG: return 36; case ENOLCK: return 37;
    case ENOSYS: return 38; case ENOTEMPTY: return 39; case ELOOP: return 40; case ENOMSG: return 42;
    case EIDRM: return 43; case ENOSTR: return 60; case ENODATA: return 61; case ETIME: return 62;
    case ENOSR: return 63; case ENOLINK: return 67; case EPROTO: return 71; case EMULTIHOP: return 72;
    case EBADMSG: return 74; case EOVERFLOW: return 75; case EILSEQ: return 84; case EUSERS: return 87;
    case ENOTSOCK: return 88; case EDESTADDRREQ: return 89; case EMSGSIZE: return 90; case EPROTOTYPE: return 91;
    case ENOPROTOOPT: return 92; case EPROTONOSUPPORT: return 93; case ESOCKTNOSUPPORT: return 94;
    case EOPNOTSUPP: return 95; case EPFNOSUPPORT: return 96; case EAFNOSUPPORT: return 97;
    case EADDRINUSE: return 98; case EADDRNOTAVAIL: return 99; case ENETDOWN: return 100;
    case ENETUNREACH: return 101; case ENETRESET: return 102; case ECONNABORTED: return 103;
    case ECONNRESET: return 104; case ENOBUFS: return 105; case EISCONN: return 106; case ENOTCONN: return 107;
    case ESHUTDOWN: return 108; case ETOOMANYREFS: return 109; case ETIMEDOUT: return 110;
    case ECONNREFUSED: return 111; case EHOSTDOWN: return 112; case EHOSTUNREACH: return 113;
    case EALREADY: return 114; case EINPROGRESS: return 115; case ESTALE: return 116; case EDQUOT: return 122;
    case ECANCELED: return 125; case EOWNERDEAD: return 130; case ENOTRECOVERABLE: return 131;
    default: return e;             // 1-34 are the same on both systems
    }
}

void rt64_set_errno(int host_errno) { guest_errno = rt64_linux_errno(host_errno); }

#define S(i) ((const char *)PTR(c, i))
#define I(i) ((int)ARG(c, i))
#define Z(i) ((size_t)ARG(c, i))
#define W(i) ((const wchar_t *)PTR(c, i))

static void s_errno_location(CPU64 *c) { RET(c, (uintptr_t)rt64_errno_slot()); }
static void s_memset(CPU64 *c) { RET(c, (uintptr_t)memset(PTR(c, 0), I(1), Z(2))); }
static void s_memcpy(CPU64 *c) { RET(c, (uintptr_t)memcpy(PTR(c, 0), PTR(c, 1), Z(2))); }
static void s_memmove(CPU64 *c) { RET(c, (uintptr_t)memmove(PTR(c, 0), PTR(c, 1), Z(2))); }
static void s_memcmp(CPU64 *c) { RET(c, (int64_t)memcmp(PTR(c, 0), PTR(c, 1), Z(2))); }
static void s_memchr(CPU64 *c) { RET(c, (uintptr_t)memchr(PTR(c, 0), I(1), Z(2))); }
static void s_strlen(CPU64 *c) { RET(c, strlen(S(0))); }
static void s_strnlen(CPU64 *c) { RET(c, strnlen(S(0), Z(1))); }
static void s_strcmp(CPU64 *c) { RET(c, (int64_t)strcmp(S(0), S(1))); }
static void s_strncmp(CPU64 *c) { RET(c, (int64_t)strncmp(S(0), S(1), Z(2))); }
static void s_strcasecmp(CPU64 *c) { RET(c, (int64_t)strcasecmp(S(0), S(1))); }
static void s_strncasecmp(CPU64 *c) { RET(c, (int64_t)strncasecmp(S(0), S(1), Z(2))); }
static void s_strchr(CPU64 *c) { RET(c, (uintptr_t)strchr(S(0), I(1))); }
static void s_strrchr(CPU64 *c) { RET(c, (uintptr_t)strrchr(S(0), I(1))); }
static void s_strstr(CPU64 *c) { RET(c, (uintptr_t)strstr(S(0), S(1))); }
static void s_strcpy(CPU64 *c) { RET(c, (uintptr_t)strcpy(PTR(c, 0), S(1))); }
static void s_strncpy(CPU64 *c) { RET(c, (uintptr_t)strncpy(PTR(c, 0), S(1), Z(2))); }
static void s_strcat(CPU64 *c) { RET(c, (uintptr_t)strcat(PTR(c, 0), S(1))); }
static void s_strncat(CPU64 *c) { RET(c, (uintptr_t)strncat(PTR(c, 0), S(1), Z(2))); }
static void s_strdup(CPU64 *c) { RET(c, rt64_guest_strdup(c, S(0))); }
static void s_strspn(CPU64 *c) { RET(c, strspn(S(0), S(1))); }
static void s_strcspn(CPU64 *c) { RET(c, strcspn(S(0), S(1))); }
static void s_strpbrk(CPU64 *c) { RET(c, (uintptr_t)strpbrk(S(0), S(1))); }
static void s_strtok_r(CPU64 *c) { RET(c, (uintptr_t)strtok_r(PTR(c, 0), S(1), PTR(c, 2))); }
static const char *linux_strerror(int e) {
    static const struct { int linux_number, host; } map[] = {{11, EAGAIN}, {38, ENOSYS}, {39, ENOTEMPTY},
        {40, ELOOP}, {75, EOVERFLOW}, {95, EOPNOTSUPP}, {110, ETIMEDOUT}, {111, ECONNREFUSED}, {115, EINPROGRESS}};
    for (size_t i = 0; i < sizeof map / sizeof *map; ++i) if (map[i].linux_number == e) return strerror(map[i].host);
    return strerror(e);
}
static void s_strerror(CPU64 *c) { RET(c, (uintptr_t)linux_strerror(I(0))); }
static void s_strsignal(CPU64 *c) {
    static _Thread_local char text[32];
    snprintf(text, sizeof text, "Signal %d", I(0));
    RET(c, (uintptr_t)text);
}
// GNU strerror_r returns the message (P2CE is built with _GNU_SOURCE).
static void s_strerror_r(CPU64 *c) { RET(c, (uintptr_t)linux_strerror(I(0))); }
static void s_tolower(CPU64 *c) { RET(c, (int64_t)tolower(I(0))); }
static void s_toupper(CPU64 *c) { RET(c, (int64_t)toupper(I(0))); }
static void s_towlower(CPU64 *c) { RET(c, (int64_t)towlower((wint_t)I(0))); }
static void s_towupper(CPU64 *c) { RET(c, (int64_t)towupper((wint_t)I(0))); }
#define CTYPE(name) static void s_##name(CPU64 *c) { RET(c, (int64_t)(name(I(0)) != 0)); }
CTYPE(isalnum) CTYPE(isalpha) CTYPE(isdigit) CTYPE(isspace) CTYPE(isupper) CTYPE(islower) CTYPE(isprint)
CTYPE(iscntrl) CTYPE(ispunct) CTYPE(isxdigit) CTYPE(isgraph)
#define WCTYPE(name) static void s_##name(CPU64 *c) { RET(c, (int64_t)(name((wint_t)I(0)) != 0)); }
WCTYPE(iswalnum) WCTYPE(iswalpha) WCTYPE(iswspace) WCTYPE(iswdigit) WCTYPE(iswupper) WCTYPE(iswlower)
WCTYPE(iswprint) WCTYPE(iswpunct)
static void s_wcslen(CPU64 *c) { RET(c, wcslen(W(0))); }
static void s_wcscmp(CPU64 *c) { RET(c, (int64_t)wcscmp(W(0), W(1))); }
static void s_wcschr(CPU64 *c) { RET(c, (uintptr_t)wcschr(W(0), (wchar_t)I(1))); }
static void s_wcsncpy(CPU64 *c) { RET(c, (uintptr_t)wcsncpy(PTR(c, 0), W(1), Z(2))); }
static void s_wcsncat(CPU64 *c) { RET(c, (uintptr_t)wcsncat(PTR(c, 0), W(1), Z(2))); }
static void s_wmemcpy(CPU64 *c) { RET(c, (uintptr_t)wmemcpy(PTR(c, 0), W(1), Z(2))); }
static void s_wmemmove(CPU64 *c) { RET(c, (uintptr_t)wmemmove(PTR(c, 0), W(1), Z(2))); }
static void s_wmemset(CPU64 *c) { RET(c, (uintptr_t)wmemset(PTR(c, 0), (wchar_t)I(1), Z(2))); }
static void s_wmemcmp(CPU64 *c) { RET(c, (int64_t)wmemcmp(W(0), W(1), Z(2))); }
static void s_wmemchr(CPU64 *c) { RET(c, (uintptr_t)wmemchr(W(0), (wchar_t)I(1), Z(2))); }
static void s_mbstowcs(CPU64 *c) { RET(c, mbstowcs(PTR(c, 0), S(1), Z(2))); }
static void s_wcstombs(CPU64 *c) { RET(c, wcstombs(PTR(c, 0), W(1), Z(2))); }
static void s_malloc(CPU64 *c) { RET(c, (uintptr_t)malloc(Z(0))); }
static void s_calloc(CPU64 *c) { RET(c, (uintptr_t)calloc(Z(0), Z(1))); }
static void s_realloc(CPU64 *c) { RET(c, (uintptr_t)realloc(PTR(c, 0), Z(1))); }
static void s_free(CPU64 *c) { free(PTR(c, 0)); }
static void s_aligned_alloc(CPU64 *c) {
    void *p = NULL;
    size_t alignment = Z(0) < sizeof(void *) ? sizeof(void *) : Z(0);
    RET(c, posix_memalign(&p, alignment, Z(1)) ? 0 : (uintptr_t)p);
}
static void s_posix_memalign(CPU64 *c) {
    void *p = NULL;
    int r = posix_memalign(&p, Z(1), Z(2));
    if (!r) *(void **)PTR(c, 0) = p;
    RET(c, r ? rt64_linux_errno(r) : 0);
}
static void s_malloc_usable_size(CPU64 *c) { RET(c, PTR(c, 0) ? malloc_size(PTR(c, 0)) : 0); }
static void s_strtol(CPU64 *c) { errno = 0; RET(c, strtol(S(0), PTR(c, 1), I(2))); if (errno) rt64_set_errno(errno); }
static void s_strtoul(CPU64 *c) { errno = 0; RET(c, strtoul(S(0), PTR(c, 1), I(2))); if (errno) rt64_set_errno(errno); }
static void s_strtoll(CPU64 *c) { errno = 0; RET(c, strtoll(S(0), PTR(c, 1), I(2))); if (errno) rt64_set_errno(errno); }
static void s_strtoull(CPU64 *c) { errno = 0; RET(c, strtoull(S(0), PTR(c, 1), I(2))); if (errno) rt64_set_errno(errno); }
static void s_strtod(CPU64 *c) { c->xmm[0].g[0] = strtod(S(0), PTR(c, 1)); }
static void s_strtof(CPU64 *c) { c->xmm[0].f[0] = strtof(S(0), PTR(c, 1)); }
static void s_atoi(CPU64 *c) { RET(c, (int64_t)atoi(S(0))); }
static void s_atol(CPU64 *c) { RET(c, atol(S(0))); }
static void s_atof(CPU64 *c) { c->xmm[0].g[0] = atof(S(0)); }
static void s_abs(CPU64 *c) { RET(c, (int64_t)abs(I(0))); }
static void s_labs(CPU64 *c) { RET(c, labs((long)ARG(c, 0))); }
static void s_rand(CPU64 *c) { RET(c, (int64_t)rand()); }
static void s_srand(CPU64 *c) { srand((unsigned)I(0)); }
static void s_getenv(CPU64 *c) { RET(c, (uintptr_t)getenv(S(0))); }
static void s_setenv(CPU64 *c) { RET(c, (int64_t)setenv(S(0), S(1), I(2))); }
static void s_unsetenv(CPU64 *c) { RET(c, (int64_t)unsetenv(S(0))); }
static void s_abort(CPU64 *c) { rt64_trap(c, R64(c->r[RSP]), "abort() called by the guest"); }
static void s_assert_fail(CPU64 *c) {
    char why[512];
    snprintf(why, sizeof why, "assertion failed: %s (%s:%d, %s)", S(0), S(1), I(2), S(3) ? S(3) : "?");
    rt64_trap(c, R64(c->r[RSP]), why);
}
// Math: doubles in xmm0/xmm1, floats in the low lane.
#define MATH1(name) static void s_##name(CPU64 *c) { c->xmm[0].g[0] = name(c->xmm[0].g[0]); }
#define MATH1F(name) static void s_##name(CPU64 *c) { c->xmm[0].f[0] = name(c->xmm[0].f[0]); }
#define MATH2(name) static void s_##name(CPU64 *c) { c->xmm[0].g[0] = name(c->xmm[0].g[0], c->xmm[1].g[0]); }
#define MATH2F(name) static void s_##name(CPU64 *c) { c->xmm[0].f[0] = name(c->xmm[0].f[0], c->xmm[1].f[0]); }
MATH1(sin) MATH1(cos) MATH1(tan) MATH1(asin) MATH1(acos) MATH1(atan) MATH1(exp) MATH1(exp2) MATH1(log)
MATH1(log2) MATH1(log10) MATH1(sqrt) MATH1(floor) MATH1(ceil) MATH1(round) MATH1(trunc) MATH1(cbrt)
MATH1(sinh) MATH1(cosh) MATH1(tanh) MATH1(log1p) MATH1(expm1) MATH1(rint) MATH1(nearbyint)
MATH1F(sinf) MATH1F(cosf) MATH1F(tanf) MATH1F(asinf) MATH1F(acosf) MATH1F(atanf) MATH1F(expf) MATH1F(exp2f)
MATH1F(logf) MATH1F(log2f) MATH1F(log10f) MATH1F(sqrtf) MATH1F(floorf) MATH1F(ceilf) MATH1F(roundf)
MATH1F(truncf) MATH1F(rintf)
MATH2(pow) MATH2(atan2) MATH2(fmod) MATH2(hypot) MATH2(fmin) MATH2(fmax) MATH2(remainder) MATH2(copysign)
MATH2F(powf) MATH2F(atan2f) MATH2F(fmodf) MATH2F(hypotf) MATH2F(fminf) MATH2F(fmaxf) MATH2F(copysignf)
static void s_ldexp(CPU64 *c) { c->xmm[0].g[0] = ldexp(c->xmm[0].g[0], I(0)); }
static void s_ldexpf(CPU64 *c) { c->xmm[0].f[0] = ldexpf(c->xmm[0].f[0], I(0)); }
static void s_frexp(CPU64 *c) { c->xmm[0].g[0] = frexp(c->xmm[0].g[0], PTR(c, 0)); }
static void s_modf(CPU64 *c) { c->xmm[0].g[0] = modf(c->xmm[0].g[0], PTR(c, 0)); }
static void s_lrint(CPU64 *c) { RET(c, lrint(c->xmm[0].g[0])); }
static void s_lrintf(CPU64 *c) { RET(c, lrintf(c->xmm[0].f[0])); }
static void s_sincos(CPU64 *c) { double v = c->xmm[0].g[0]; *(double *)PTR(c, 0) = sin(v); *(double *)PTR(c, 1) = cos(v); }
static void s_sincosf(CPU64 *c) { float v = c->xmm[0].f[0]; *(float *)PTR(c, 0) = sinf(v); *(float *)PTR(c, 1) = cosf(v); }
// fegetround/fesetround: Linux FE_* values are x87 control-word bits.
static void s_fegetround(CPU64 *c) { RET(c, (int64_t)((c->mxcsr >> 3) & 0xc00)); }
static void s_fesetround(CPU64 *c) {
    c->mxcsr = (c->mxcsr & ~0x6000u) | ((uint32_t)(I(0) & 0xc00) << 3);
    c->fcw = (uint16_t)((c->fcw & ~0xc00u) | (I(0) & 0xc00));
    RET(c, 0);
}

static const RT64Shim shims[] = {
    {"__errno_location", s_errno_location},
    {"memset", s_memset}, {"memcpy", s_memcpy}, {"memmove", s_memmove}, {"memcmp", s_memcmp}, {"bcmp", s_memcmp},
    {"memchr", s_memchr}, {"strlen", s_strlen}, {"strnlen", s_strnlen}, {"strcmp", s_strcmp},
    {"strncmp", s_strncmp}, {"strcasecmp", s_strcasecmp}, {"strncasecmp", s_strncasecmp},
    {"strchr", s_strchr}, {"strrchr", s_strrchr}, {"strstr", s_strstr}, {"strcpy", s_strcpy},
    {"strncpy", s_strncpy}, {"strcat", s_strcat}, {"strncat", s_strncat}, {"strdup", s_strdup},
    {"strspn", s_strspn}, {"strcspn", s_strcspn}, {"strpbrk", s_strpbrk}, {"strtok_r", s_strtok_r},
    {"strerror", s_strerror}, {"strerror_r", s_strerror_r}, {"strsignal", s_strsignal}, {"tolower", s_tolower}, {"toupper", s_toupper}, {"towlower", s_towlower},
    {"towupper", s_towupper}, {"isalnum", s_isalnum}, {"isalpha", s_isalpha}, {"isdigit", s_isdigit},
    {"isspace", s_isspace}, {"isupper", s_isupper}, {"islower", s_islower}, {"isprint", s_isprint},
    {"iscntrl", s_iscntrl}, {"ispunct", s_ispunct}, {"isxdigit", s_isxdigit}, {"isgraph", s_isgraph},
    {"iswalnum", s_iswalnum}, {"iswalpha", s_iswalpha}, {"iswspace", s_iswspace}, {"iswdigit", s_iswdigit},
    {"iswupper", s_iswupper}, {"iswlower", s_iswlower}, {"iswprint", s_iswprint}, {"iswpunct", s_iswpunct},
    {"wcslen", s_wcslen}, {"wcscmp", s_wcscmp}, {"wcschr", s_wcschr}, {"wcsncpy", s_wcsncpy},
    {"wcsncat", s_wcsncat}, {"wmemcpy", s_wmemcpy}, {"wmemmove", s_wmemmove}, {"wmemset", s_wmemset},
    {"wmemcmp", s_wmemcmp}, {"wmemchr", s_wmemchr}, {"mbstowcs", s_mbstowcs}, {"wcstombs", s_wcstombs},
    {"malloc", s_malloc}, {"calloc", s_calloc}, {"realloc", s_realloc}, {"free", s_free},
    {"aligned_alloc", s_aligned_alloc}, {"memalign", s_aligned_alloc}, {"posix_memalign", s_posix_memalign},
    {"malloc_usable_size", s_malloc_usable_size},
    // mimalloc (P2CE preloads it): the host allocator stands in for it.
    {"mi_malloc", s_malloc}, {"mi_calloc", s_calloc}, {"mi_realloc", s_realloc}, {"mi_free", s_free},
    {"mi_malloc_aligned", s_aligned_alloc}, {"mi_usable_size", s_malloc_usable_size}, {"mi_strdup", s_strdup},
    {"strtol", s_strtol}, {"strtoul", s_strtoul}, {"strtoll", s_strtoll}, {"strtoull", s_strtoull},
    {"__isoc23_strtol", s_strtol}, {"strtod", s_strtod}, {"strtof", s_strtof}, {"atoi", s_atoi},
    {"atol", s_atol}, {"atof", s_atof}, {"abs", s_abs}, {"labs", s_labs}, {"rand", s_rand}, {"srand", s_srand},
    {"getenv", s_getenv}, {"secure_getenv", s_getenv}, {"setenv", s_setenv}, {"unsetenv", s_unsetenv},
    {"abort", s_abort}, {"__assert_fail", s_assert_fail},
    {"sin", s_sin}, {"cos", s_cos}, {"tan", s_tan}, {"asin", s_asin}, {"acos", s_acos}, {"atan", s_atan},
    {"exp", s_exp}, {"exp2", s_exp2}, {"log", s_log}, {"log2", s_log2}, {"log10", s_log10}, {"sqrt", s_sqrt},
    {"floor", s_floor}, {"ceil", s_ceil}, {"round", s_round}, {"trunc", s_trunc}, {"cbrt", s_cbrt},
    {"sinh", s_sinh}, {"cosh", s_cosh}, {"tanh", s_tanh}, {"log1p", s_log1p}, {"expm1", s_expm1},
    {"rint", s_rint}, {"nearbyint", s_nearbyint},
    {"sinf", s_sinf}, {"cosf", s_cosf}, {"tanf", s_tanf}, {"asinf", s_asinf}, {"acosf", s_acosf},
    {"atanf", s_atanf}, {"expf", s_expf}, {"exp2f", s_exp2f}, {"logf", s_logf}, {"log2f", s_log2f},
    {"log10f", s_log10f}, {"sqrtf", s_sqrtf}, {"floorf", s_floorf}, {"ceilf", s_ceilf}, {"roundf", s_roundf},
    {"truncf", s_truncf}, {"rintf", s_rintf},
    {"pow", s_pow}, {"atan2", s_atan2}, {"fmod", s_fmod}, {"hypot", s_hypot}, {"fmin", s_fmin},
    {"fmax", s_fmax}, {"remainder", s_remainder}, {"copysign", s_copysign},
    {"powf", s_powf}, {"atan2f", s_atan2f}, {"fmodf", s_fmodf}, {"hypotf", s_hypotf}, {"fminf", s_fminf},
    {"fmaxf", s_fmaxf}, {"copysignf", s_copysignf},
    {"ldexp", s_ldexp}, {"ldexpf", s_ldexpf}, {"frexp", s_frexp}, {"modf", s_modf}, {"lrint", s_lrint},
    {"lrintf", s_lrintf}, {"sincos", s_sincos}, {"sincosf", s_sincosf},
    {"fegetround", s_fegetround}, {"fesetround", s_fesetround},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
