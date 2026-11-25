// Locale and character-set shims of the x86-64 runtime, for glibc's view
// of them: __ctype_b_loc() and friends return glibc-format tables (class
// bits of glibc, indices -128..255), a locale_t is a glibc __locale_struct
// whose __ctype_b/__ctype_tolower/__ctype_toupper fields libstdc++ reads
// directly, the *_l functions use the "C" locale, and iconv is the host's.
#include "rt64.h"

#include <ctype.h>
#include <iconv.h>
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

// glibc's class bits (ctype.h, little-endian _ISbit).
enum { G_UPPER = 0x100, G_LOWER = 0x200, G_ALPHA = 0x400, G_DIGIT = 0x800, G_XDIGIT = 0x1000, G_SPACE = 0x2000,
       G_PRINT = 0x4000, G_GRAPH = 0x8000, G_BLANK = 0x1, G_CNTRL = 0x2, G_PUNCT = 0x4, G_ALNUM = 0x8 };

static uint16_t class_table[384];
static int32_t lower_table[384], upper_table[384];
static const uint16_t *class_pointer = class_table + 128;
static const int32_t *lower_pointer = lower_table + 128, *upper_pointer = upper_table + 128;

// struct __locale_struct: __locales[13], __ctype_b, __ctype_tolower, __ctype_toupper, __names[13].
typedef struct { void *locales[13]; const uint16_t *ctype_b; const int32_t *tolower, *toupper; const char *names[13]; } GlibcLocale;
static GlibcLocale c_locale;

__attribute__((constructor)) static void tables(void) {
    for (int i = -128; i < 256; ++i) {
        int ch = i < 0 ? i + 256 : i;      // glibc's signed-char entries mirror 128..255
        uint16_t bits = 0;
        if (ch < 128) {
            if (isupper(ch)) bits |= G_UPPER;
            if (islower(ch)) bits |= G_LOWER;
            if (isalpha(ch)) bits |= G_ALPHA;
            if (isdigit(ch)) bits |= G_DIGIT;
            if (isxdigit(ch)) bits |= G_XDIGIT;
            if (isspace(ch)) bits |= G_SPACE;
            if (isprint(ch)) bits |= G_PRINT;
            if (isgraph(ch)) bits |= G_GRAPH;
            if (ch == ' ' || ch == '\t') bits |= G_BLANK;
            if (iscntrl(ch)) bits |= G_CNTRL;
            if (ispunct(ch)) bits |= G_PUNCT;
            if (isalnum(ch)) bits |= G_ALNUM;
        }
        class_table[i + 128] = i == -1 ? 0 : bits;       // EOF
        lower_table[i + 128] = ch < 128 ? tolower(ch) : i;
        upper_table[i + 128] = ch < 128 ? toupper(ch) : i;
    }
    c_locale.ctype_b = class_pointer;
    c_locale.tolower = lower_pointer;
    c_locale.toupper = upper_pointer;
    for (int i = 0; i < 13; ++i) c_locale.names[i] = "C";
}

static void s_ctype_b_loc(CPU64 *c) { RET(c, (uintptr_t)&class_pointer); }
static void s_ctype_tolower_loc(CPU64 *c) { RET(c, (uintptr_t)&lower_pointer); }
static void s_ctype_toupper_loc(CPU64 *c) { RET(c, (uintptr_t)&upper_pointer); }
static void s_ctype_get_mb_cur_max(CPU64 *c) { RET(c, 1); }
static void s_newlocale(CPU64 *c) { RET(c, (uintptr_t)&c_locale); }
static void s_uselocale(CPU64 *c) { RET(c, (uintptr_t)&c_locale); }
static void s_freelocale(CPU64 *c) { (void)c; }
static void s_setlocale(CPU64 *c) { RET(c, (uintptr_t)"C"); }
// nl_langinfo: CODESET (14) and the radix/thousands characters (0x10000, 0x10001).
static void s_nl_langinfo(CPU64 *c) {
    switch ((int)ARG(c, 0)) {
    case 14: RET(c, (uintptr_t)"ANSI_X3.4-1968"); break;
    case 0x10000: RET(c, (uintptr_t)"."); break;
    default: RET(c, (uintptr_t)""); break;
    }
}
static void s_strcoll(CPU64 *c) { RET(c, (int64_t)strcmp(PTR(c, 0), PTR(c, 1))); }
static void s_strxfrm(CPU64 *c) {
    size_t n = strlen(PTR(c, 1));
    if (n < (size_t)ARG(c, 2)) memcpy(PTR(c, 0), PTR(c, 1), n + 1);
    RET(c, n);
}
static void s_wcscoll(CPU64 *c) { RET(c, (int64_t)wcscmp(PTR(c, 0), PTR(c, 1))); }
static void s_wcsxfrm(CPU64 *c) {
    size_t n = wcslen(PTR(c, 1));
    if (n < (size_t)ARG(c, 2)) wmemcpy(PTR(c, 0), PTR(c, 1), n + 1);
    RET(c, n);
}
static void s_strtod_l(CPU64 *c) { c->xmm[0].g[0] = strtod(PTR(c, 0), PTR(c, 1)); }
static void s_strtof_l(CPU64 *c) { c->xmm[0].f[0] = strtof(PTR(c, 0), PTR(c, 1)); }
static void s_towlower_l(CPU64 *c) { RET(c, (int64_t)towlower((wint_t)ARG(c, 0))); }
static void s_towupper_l(CPU64 *c) { RET(c, (int64_t)towupper((wint_t)ARG(c, 0))); }
// wctype: glibc returns an opaque description; here the class bit itself.
static void s_wctype(CPU64 *c) {
    static const struct { const char *name; uint16_t bit; } classes[] = {
        {"upper", G_UPPER}, {"lower", G_LOWER}, {"alpha", G_ALPHA}, {"digit", G_DIGIT}, {"xdigit", G_XDIGIT},
        {"space", G_SPACE}, {"print", G_PRINT}, {"graph", G_GRAPH}, {"blank", G_BLANK}, {"cntrl", G_CNTRL},
        {"punct", G_PUNCT}, {"alnum", G_ALNUM}};
    for (size_t i = 0; i < sizeof classes / sizeof *classes; ++i)
        if (!strcmp(PTR(c, 0), classes[i].name)) { RET(c, classes[i].bit); return; }
    RET(c, 0);
}
static void s_iswctype(CPU64 *c) {
    wint_t ch = (wint_t)ARG(c, 0);
    uint64_t bit = ARG(c, 1);
    int r = 0;
    if (ch < 128) r = (class_table[ch + 128] & bit) != 0;
    else if (bit & (G_ALPHA | G_ALNUM | G_PRINT | G_GRAPH)) r = iswalpha(ch) != 0;
    else if (bit & G_SPACE) r = iswspace(ch) != 0;
    RET(c, r);
}
static void s_btowc(CPU64 *c) { int ch = (int)ARG(c, 0); RET(c, ch >= 0 && ch < 128 ? ch : -1); }   // WEOF
static void s_wctob(CPU64 *c) { uint32_t ch = (uint32_t)ARG(c, 0); RET(c, ch < 128 ? (int64_t)ch : -1); }
// mbstate_t is 8 bytes on Linux, 128 on Apple: the guest's is not used ("C" locale, single bytes).
static void s_mbrtowc(CPU64 *c) {
    const unsigned char *s = PTR(c, 1);
    if (!s) { RET(c, 0); return; }
    if (!ARG(c, 2)) { RET(c, (uint64_t)-2); return; }
    if (ARG(c, 0)) W32(ARG(c, 0), *s);
    RET(c, *s ? 1 : 0);
}
static void s_wcrtomb(CPU64 *c) {
    if (!ARG(c, 0)) { RET(c, 1); return; }
    uint32_t ch = (uint32_t)ARG(c, 1);
    if (ch > 255) { rt64_set_errno(84); RET(c, (uint64_t)-1); return; }     // EILSEQ
    W8(ARG(c, 0), ch);
    RET(c, 1);
}
static void s_mbsrtowcs(CPU64 *c) {       // (dst, &src, len, state)
    uint64_t dst = ARG(c, 0), srcp = ARG(c, 1);
    size_t len = (size_t)ARG(c, 2), n = 0;
    const unsigned char *s = (const unsigned char *)(uintptr_t)R64(srcp);
    while ((!dst || n < len) && s[n]) { if (dst) W32(dst + 4 * n, s[n]); ++n; }
    if (dst && n < len) { W32(dst + 4 * n, 0); W64(srcp, 0); }
    else if (dst) W64(srcp, (uintptr_t)(s + n));
    RET(c, n);
}
static void s_mbsnrtowcs(CPU64 *c) {      // (dst, &src, nms, len, state)
    uint64_t dst = ARG(c, 0), srcp = ARG(c, 1);
    size_t nms = (size_t)ARG(c, 2), len = (size_t)ARG(c, 3), n = 0;
    const unsigned char *s = (const unsigned char *)(uintptr_t)R64(srcp);
    while (n < nms && (!dst || n < len) && s[n]) { if (dst) W32(dst + 4 * n, s[n]); ++n; }
    if (dst) W64(srcp, n < nms && !s[n] ? 0 : (uintptr_t)(s + n));
    if (dst && n < len && n < nms && !s[n]) W32(dst + 4 * n, 0);
    RET(c, n);
}
static void s_wcsnrtombs(CPU64 *c) {      // (dst, &src, nwc, len, state)
    uint64_t dst = ARG(c, 0), srcp = ARG(c, 1);
    size_t nwc = (size_t)ARG(c, 2), len = (size_t)ARG(c, 3), n = 0;
    uint64_t s = R64(srcp);
    while (n < nwc && (!dst || n < len)) {
        uint32_t ch = (uint32_t)R32(s + 4 * n);
        if (!ch) break;
        if (ch > 255) { rt64_set_errno(84); RET(c, (uint64_t)-1); return; }
        if (dst) W8(dst + n, ch);
        ++n;
    }
    if (dst) {
        int ended = n < nwc && !R32(s + 4 * n);
        if (ended && n < len) W8(dst + n, 0);
        W64(srcp, ended ? 0 : s + 4 * n);
    }
    RET(c, n);
}
static void s_iconv_open(CPU64 *c) { iconv_t cd = iconv_open(PTR(c, 0), PTR(c, 1)); RET(c, (uintptr_t)cd); }
static void s_iconv(CPU64 *c) { RET(c, iconv((iconv_t)PTR(c, 0), PTR(c, 1), PTR(c, 2), PTR(c, 3), PTR(c, 4))); }
static void s_iconv_close(CPU64 *c) { RET(c, iconv_close((iconv_t)PTR(c, 0))); }
// gettext: no translations.
static void s_gettext(CPU64 *c) { RET(c, ARG(c, 0)); }
static void s_dgettext(CPU64 *c) { RET(c, ARG(c, 1)); }
static void s_bindtextdomain(CPU64 *c) { RET(c, ARG(c, 1) ? ARG(c, 1) : (uintptr_t)"/usr/share/locale"); }
static void s_textdomain(CPU64 *c) { RET(c, ARG(c, 0)); }

static const RT64Shim shims[] = {
    {"__ctype_b_loc", s_ctype_b_loc}, {"__ctype_tolower_loc", s_ctype_tolower_loc},
    {"__ctype_toupper_loc", s_ctype_toupper_loc}, {"__ctype_get_mb_cur_max", s_ctype_get_mb_cur_max},
    {"newlocale", s_newlocale}, {"__newlocale", s_newlocale}, {"duplocale", s_newlocale}, {"__duplocale", s_newlocale},
    {"uselocale", s_uselocale}, {"__uselocale", s_uselocale}, {"freelocale", s_freelocale},
    {"__freelocale", s_freelocale}, {"setlocale", s_setlocale}, {"nl_langinfo", s_nl_langinfo},
    {"__nl_langinfo_l", s_nl_langinfo}, {"nl_langinfo_l", s_nl_langinfo},
    {"strcoll", s_strcoll}, {"__strcoll_l", s_strcoll}, {"strxfrm", s_strxfrm}, {"__strxfrm_l", s_strxfrm},
    {"wcscoll", s_wcscoll}, {"__wcscoll_l", s_wcscoll}, {"wcsxfrm", s_wcsxfrm}, {"__wcsxfrm_l", s_wcsxfrm},
    {"__strtod_l", s_strtod_l}, {"strtod_l", s_strtod_l}, {"__strtof_l", s_strtof_l}, {"strtof_l", s_strtof_l},
    {"__towlower_l", s_towlower_l}, {"__towupper_l", s_towupper_l}, {"wctype", s_wctype},
    {"__wctype_l", s_wctype}, {"iswctype", s_iswctype}, {"__iswctype_l", s_iswctype}, {"btowc", s_btowc},
    {"wctob", s_wctob}, {"mbrtowc", s_mbrtowc}, {"wcrtomb", s_wcrtomb}, {"mbsrtowcs", s_mbsrtowcs},
    {"mbsnrtowcs", s_mbsnrtowcs}, {"wcsnrtombs", s_wcsnrtombs}, {"iconv_open", s_iconv_open}, {"iconv", s_iconv},
    {"iconv_close", s_iconv_close}, {"gettext", s_gettext}, {"dgettext", s_dgettext},
    {"bindtextdomain", s_bindtextdomain}, {"bind_textdomain_codeset", s_bindtextdomain},
    {"textdomain", s_textdomain},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
