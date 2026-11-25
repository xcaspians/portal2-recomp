// stdio shims of the x86-64 runtime. A guest FILE * is a host FILE *
// (the guest never looks inside it), stdin/stdout/stderr are host objects
// holding the host streams. The printf family formats from the guest's
// variable arguments: in registers and on the stack for printf(fmt, ...),
// through an x86-64 va_list (gp_offset, fp_offset, overflow_arg_area,
// reg_save_area) for vprintf(fmt, ap).
#include "rt64.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static FILE *guest_stdin, *guest_stdout, *guest_stderr;
__attribute__((constructor)) static void streams(void) {
    guest_stdin = stdin; guest_stdout = stdout; guest_stderr = stderr;
}

// Guest variable arguments ----------------------------------------------------

typedef struct {
    CPU64 *c;
    int gp, fp;                 // registers used so far (direct calls)
    uint64_t stack;             // next stack argument (direct) / overflow area (va_list)
    uint64_t va;                // guest va_list address, or 0
} GuestArgs;

static const int gp_registers[6] = {RDI, RSI, RDX, RCX, R8, R9};

static GuestArgs direct_args(CPU64 *c, int fixed) {
    // At the shim, rsp points at the return address; stack arguments follow.
    GuestArgs a = {c, fixed, 0, c->r[RSP] + 8, 0};
    return a;
}

static GuestArgs list_args(CPU64 *c, uint64_t va) {
    GuestArgs a = {c, 0, 0, 0, va};
    return a;
}

static uint64_t next_int(GuestArgs *a) {
    if (a->va) {
        uint32_t gp_offset = (uint32_t)R32(a->va);
        if (gp_offset < 48) { W32(a->va, gp_offset + 8); return R64(R64(a->va + 16) + gp_offset); }
        uint64_t area = R64(a->va + 8);
        W64(a->va + 8, area + 8);
        return R64(area);
    }
    if (a->gp < 6) return a->c->r[gp_registers[a->gp++]];
    uint64_t v = R64(a->stack);
    a->stack += 8;
    return v;
}

static double next_double(GuestArgs *a) {
    if (a->va) {
        uint32_t fp_offset = (uint32_t)R32(a->va + 4);
        if (fp_offset < 176) { W32(a->va + 4, fp_offset + 16); return rt64_rd(R64(a->va + 16) + fp_offset); }
        uint64_t area = R64(a->va + 8);
        W64(a->va + 8, area + 8);
        return rt64_rd(area);
    }
    if (a->fp < 8) return a->c->xmm[a->fp++].g[0];
    double v = rt64_rd(a->stack);
    a->stack += 8;
    return v;
}

static double next_long_double(GuestArgs *a) {   // 16 bytes on the stack/overflow area, 16-aligned
    uint64_t *slot = a->va ? &(uint64_t){R64(a->va + 8)} : &a->stack;
    uint64_t at = (*slot + 15) & ~UINT64_C(15);
    double v = rt64_ld80(at);
    if (a->va) W64(a->va + 8, at + 16); else a->stack = at + 16;
    return v;
}

// Formatting -------------------------------------------------------------------

typedef struct { char *buffer; size_t size, length; FILE *stream; } Output;

static void emit(Output *o, const char *text, size_t n) {
    if (o->stream) { fwrite(text, 1, n, o->stream); o->length += n; return; }
    if (o->length < o->size) {
        size_t room = o->size - o->length;
        memcpy(o->buffer + o->length, text, n < room ? n : room);
    }
    o->length += n;
}

// Linux printf conversions, formatted one at a time by the host's snprintf.
static int format(Output *o, const char *f, GuestArgs *a) {
    char spec[64], piece[512];
    while (*f) {
        if (*f != '%') {
            const char *start = f;
            while (*f && *f != '%') ++f;
            emit(o, start, (size_t)(f - start));
            continue;
        }
        if (f[1] == '%') { emit(o, "%", 1); f += 2; continue; }
        const char *start = f++;
        size_t n = 1;
        spec[0] = '%';
        int star_width = 0, star_precision = 0, width = 0, precision = 0;
        while (*f && strchr("-+ #0'", *f) && n < 20) spec[n++] = *f++;
        if (*f == '*') { star_width = 1; width = (int)next_int(a); spec[n++] = '*'; ++f; }
        while (*f >= '0' && *f <= '9' && n < 40) spec[n++] = *f++;
        if (*f == '.') {
            spec[n++] = *f++;
            if (*f == '*') { star_precision = 1; precision = (int)next_int(a); spec[n++] = '*'; ++f; }
            while (*f >= '0' && *f <= '9' && n < 50) spec[n++] = *f++;
        }
        char length[3] = {0};
        if (*f == 'h' || *f == 'l' || *f == 'L' || *f == 'z' || *f == 'j' || *f == 't' || *f == 'q') {
            length[0] = *f++;
            if ((length[0] == 'h' || length[0] == 'l') && *f == length[0]) length[1] = *f++;
        }
        char conv = *f ? *f++ : 0;
        int len;
        #define PRINT(...) (star_width && star_precision ? snprintf(piece, sizeof piece, spec, width, precision, __VA_ARGS__) : \
                            star_width ? snprintf(piece, sizeof piece, spec, width, __VA_ARGS__) : \
                            star_precision ? snprintf(piece, sizeof piece, spec, precision, __VA_ARGS__) : \
                            snprintf(piece, sizeof piece, spec, __VA_ARGS__))
        switch (conv) {
        case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': case 'c': {
            uint64_t v = next_int(a);
            int64_t value;
            if (conv == 'c') { spec[n++] = 'c'; spec[n] = 0; len = PRINT((int)v); break; }
            spec[n++] = 'l'; spec[n++] = 'l'; spec[n++] = conv; spec[n] = 0;
            if (length[0] == 'l' || length[0] == 'z' || length[0] == 'j' || length[0] == 't' || length[0] == 'q')
                value = (int64_t)v;
            else if (length[0] == 'h' && length[1] == 'h')
                value = (conv == 'd' || conv == 'i') ? (int8_t)v : (int64_t)(uint8_t)v;
            else if (length[0] == 'h')
                value = (conv == 'd' || conv == 'i') ? (int16_t)v : (int64_t)(uint16_t)v;
            else
                value = (conv == 'd' || conv == 'i') ? (int32_t)v : (int64_t)(uint32_t)v;
            len = PRINT((long long)value);
            break;
        }
        case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A': {
            double v = length[0] == 'L' ? next_long_double(a) : next_double(a);
            spec[n++] = conv; spec[n] = 0;
            len = PRINT(v);
            break;
        }
        case 's': {
            uint64_t p = next_int(a);
            if (length[0] == 'l') { spec[n++] = 'l'; spec[n++] = 's'; spec[n] = 0;
                                    len = PRINT(p ? (const wchar_t *)(uintptr_t)p : L"(null)"); }
            else { spec[n++] = 's'; spec[n] = 0; len = PRINT(p ? (const char *)(uintptr_t)p : "(null)"); }
            // A long string goes out directly (piece is a fixed buffer).
            if (len >= (int)sizeof piece && length[0] != 'l' && !star_width && !star_precision && n == 2) {
                emit(o, (const char *)(uintptr_t)p, strlen((const char *)(uintptr_t)p));
                continue;
            }
            break;
        }
        case 'p': {
            uint64_t p = next_int(a);
            spec[n++] = 'p'; spec[n] = 0;
            len = PRINT((void *)(uintptr_t)p);
            break;
        }
        case 'n': {
            uint64_t p = next_int(a);
            if (length[0] == 'l' || length[0] == 'z') W64(p, o->length); else W32(p, (uint32_t)o->length);
            continue;
        }
        default:
            emit(o, start, (size_t)(f - start));
            continue;
        }
        if (len > 0) emit(o, piece, (size_t)len < sizeof piece ? (size_t)len : sizeof piece - 1);
    }
    return (int)o->length;
}

static int to_buffer(char *buffer, size_t size, const char *f, GuestArgs *a) {
    Output o = {buffer, size, 0, NULL};
    int n = format(&o, f, a);
    if (size) buffer[o.length < size ? o.length : size - 1] = 0;
    return n;
}

static int to_stream(FILE *stream, const char *f, GuestArgs *a) {
    Output o = {NULL, 0, 0, stream};
    return format(&o, f, a);
}

#define STR(i) ((const char *)PTR(c, i))
#define FILEP(i) ((FILE *)PTR(c, i))

static void s_printf(CPU64 *c) { GuestArgs a = direct_args(c, 1); RET(c, to_stream(stdout, STR(0), &a)); }
static void s_vprintf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 1)); RET(c, to_stream(stdout, STR(0), &a)); }
static void s_fprintf(CPU64 *c) { GuestArgs a = direct_args(c, 2); RET(c, to_stream(FILEP(0), STR(1), &a)); }
static void s_vfprintf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 2)); RET(c, to_stream(FILEP(0), STR(1), &a)); }
static void s_sprintf(CPU64 *c) { GuestArgs a = direct_args(c, 2); RET(c, to_buffer(PTR(c, 0), SIZE_MAX / 2, STR(1), &a)); }
static void s_vsprintf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 2)); RET(c, to_buffer(PTR(c, 0), SIZE_MAX / 2, STR(1), &a)); }
static void s_snprintf(CPU64 *c) { GuestArgs a = direct_args(c, 3); RET(c, to_buffer(PTR(c, 0), (size_t)ARG(c, 1), STR(2), &a)); }
static void s_vsnprintf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 3)); RET(c, to_buffer(PTR(c, 0), (size_t)ARG(c, 1), STR(2), &a)); }
static void s_snprintf_chk(CPU64 *c) { GuestArgs a = direct_args(c, 5); RET(c, to_buffer(PTR(c, 0), (size_t)ARG(c, 1), STR(4), &a)); }
static void s_vsnprintf_chk(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 5)); RET(c, to_buffer(PTR(c, 0), (size_t)ARG(c, 1), STR(4), &a)); }
static void s_puts(CPU64 *c) { RET(c, puts(STR(0))); }
static void s_putchar(CPU64 *c) { RET(c, putchar((int)ARG(c, 0))); }
static void s_fputs(CPU64 *c) { RET(c, fputs(STR(0), FILEP(1))); }
static void s_fputc(CPU64 *c) { RET(c, fputc((int)ARG(c, 0), FILEP(1))); }
static void s_fwrite(CPU64 *c) { RET(c, fwrite(PTR(c, 0), (size_t)ARG(c, 1), (size_t)ARG(c, 2), FILEP(3))); }
static void s_fread(CPU64 *c) { RET(c, fread(PTR(c, 0), (size_t)ARG(c, 1), (size_t)ARG(c, 2), FILEP(3))); }
static void s_fflush(CPU64 *c) { RET(c, fflush(FILEP(0))); }
static void s_fopen(CPU64 *c) { FILE *f = fopen(STR(0), STR(1)); if (!f) rt64_set_errno(errno); RET(c, (uintptr_t)f); }
static void s_fdopen(CPU64 *c) { RET(c, (uintptr_t)fdopen((int)ARG(c, 0), STR(1))); }
static void s_freopen(CPU64 *c) { RET(c, (uintptr_t)freopen(STR(0), STR(1), FILEP(2))); }
static void s_fclose(CPU64 *c) { RET(c, fclose(FILEP(0))); }
static void s_fgets(CPU64 *c) { RET(c, (uintptr_t)fgets(PTR(c, 0), (int)ARG(c, 1), FILEP(2))); }
static void s_fgetc(CPU64 *c) { RET(c, fgetc(FILEP(0))); }
static void s_fseek(CPU64 *c) { RET(c, fseeko(FILEP(0), (off_t)ARG(c, 1), (int)ARG(c, 2))); }
static void s_ftell(CPU64 *c) { RET(c, ftello(FILEP(0))); }
static void s_rewind(CPU64 *c) { rewind(FILEP(0)); }
static void s_feof(CPU64 *c) { RET(c, feof(FILEP(0))); }
static void s_ferror(CPU64 *c) { RET(c, ferror(FILEP(0))); }
static void s_fileno(CPU64 *c) { RET(c, fileno(FILEP(0))); }
static void s_setvbuf(CPU64 *c) { RET(c, setvbuf(FILEP(0), PTR(c, 1), (int)ARG(c, 2) == 2 ? _IONBF : (int)ARG(c, 2) == 1 ? _IOLBF : _IOFBF, (size_t)ARG(c, 3))); }
static void s_remove(CPU64 *c) { int r = remove(STR(0)); if (r) rt64_set_errno(errno); RET(c, r); }
static void s_rename(CPU64 *c) { int r = rename(STR(0), STR(1)); if (r) rt64_set_errno(errno); RET(c, r); }


// scanf family: each conversion goes to the host's sscanf/fscanf with %n
// appended, so the input advances exactly as glibc's would.
static int scan(const char *input, FILE *stream, const char *f, GuestArgs *a) {
    int assigned = 0;
    size_t used = 0;
    char spec[128];
    while (*f) {
        if (isspace((unsigned char)*f)) {
            if (stream) { int ch; while ((ch = fgetc(stream)) != EOF && isspace(ch)) {} if (ch != EOF) ungetc(ch, stream); }
            else while (isspace((unsigned char)input[used])) ++used;
            ++f;
            continue;
        }
        if (*f != '%' || f[1] == '%') {
            char want = *f == '%' ? '%' : *f;
            f += *f == '%' ? 2 : 1;
            int ch = stream ? fgetc(stream) : (unsigned char)input[used];
            if (ch != want) { if (stream && ch != EOF) ungetc(ch, stream); break; }
            if (!stream) ++used;
            continue;
        }
        const char *start = f++;
        int suppress = *f == '*';
        if (suppress) ++f;
        while (*f >= '0' && *f <= '9') ++f;
        char length[3] = {0};
        if (*f == 'h' || *f == 'l' || *f == 'L' || *f == 'z' || *f == 'j' || *f == 't' || *f == 'q') {
            length[0] = *f++;
            if ((length[0] == 'h' || length[0] == 'l') && *f == length[0]) length[1] = *f++;
        }
        if (*f == '[') { ++f; if (*f == '^') ++f; if (*f == ']') ++f; while (*f && *f != ']') ++f; }
        char conv = *f ? *f++ : 0;
        size_t n = (size_t)(f - start);
        if (n + 4 > sizeof spec) break;
        memcpy(spec, start, n);
        strcpy(spec + n, "%n");
        int consumed = 0, r;
        if (suppress) {
            r = stream ? fscanf(stream, spec, &consumed) : sscanf(input + used, spec, &consumed);
            if (r == EOF) break;
            used += (size_t)consumed;
            continue;
        }
        uint64_t target = next_int(a);
        if (conv == 'n') {
            if (length[0] == 'l') W64(target, used); else W32(target, (uint32_t)used);
            continue;
        }
        // Linux long double is 16 bytes of x87 format: read into a double, then store as 80-bit.
        if ((conv == 'f' || conv == 'e' || conv == 'g' || conv == 'E' || conv == 'G' || conv == 'a') && length[0] == 'L') {
            double v;
            spec[n - 2] = 'l'; spec[n - 1] = conv; spec[n] = 0; strcat(spec, "%n");
            r = stream ? fscanf(stream, spec, &v, &consumed) : sscanf(input + used, spec, &v, &consumed);
            if (r == 1) rt64_st80(target, v);
        } else {
            r = stream ? fscanf(stream, spec, (void *)(uintptr_t)target, &consumed)
                       : sscanf(input + used, spec, (void *)(uintptr_t)target, &consumed);
        }
        if (r == EOF) return assigned ? assigned : EOF;
        if (r < 1) break;
        assigned++;
        used += (size_t)consumed;
    }
    return assigned;
}

static void s_sscanf(CPU64 *c) { GuestArgs a = direct_args(c, 2); RET(c, scan(STR(0), NULL, STR(1), &a)); }
static void s_vsscanf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 2)); RET(c, scan(STR(0), NULL, STR(1), &a)); }
static void s_fscanf(CPU64 *c) { GuestArgs a = direct_args(c, 2); RET(c, scan(NULL, FILEP(0), STR(1), &a)); }
static void s_vfscanf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 2)); RET(c, scan(NULL, FILEP(0), STR(1), &a)); }
// swprintf/vswprintf: format as bytes, then widen (the engine's format strings are ASCII).
static int wide_format(uint64_t out, size_t size, uint64_t format_w, GuestArgs *a) {
    char narrow_format[1024], text[4096];
    size_t i = 0;
    for (; i + 1 < sizeof narrow_format && R32(format_w + 4 * i); ++i) {
        uint32_t ch = (uint32_t)R32(format_w + 4 * i);
        narrow_format[i] = ch < 128 ? (char)ch : '?';
    }
    narrow_format[i] = 0;
    int n = to_buffer(text, sizeof text, narrow_format, a);
    if (!size) return n;
    size_t k = 0;
    for (; k + 1 < size && text[k] && k < sizeof text - 1; ++k) W32(out + 4 * k, (uint8_t)text[k]);
    W32(out + 4 * k, 0);
    return (size_t)n >= size ? -1 : n;
}
static void s_swprintf(CPU64 *c) { GuestArgs a = direct_args(c, 3); RET(c, wide_format(ARG(c, 0), (size_t)ARG(c, 1), ARG(c, 2), &a)); }
static void s_vswprintf(CPU64 *c) { GuestArgs a = list_args(c, ARG(c, 3)); RET(c, wide_format(ARG(c, 0), (size_t)ARG(c, 1), ARG(c, 2), &a)); }

static const RT64Shim shims[] = {
    {"printf", s_printf}, {"vprintf", s_vprintf}, {"fprintf", s_fprintf}, {"vfprintf", s_vfprintf},
    {"sprintf", s_sprintf}, {"vsprintf", s_vsprintf}, {"snprintf", s_snprintf}, {"vsnprintf", s_vsnprintf},
    {"__snprintf_chk", s_snprintf_chk}, {"__vsnprintf_chk", s_vsnprintf_chk}, {"__printf_chk", s_printf},
    {"puts", s_puts}, {"putchar", s_putchar}, {"fputs", s_fputs}, {"fputc", s_fputc}, {"putc", s_fputc},
    {"fwrite", s_fwrite}, {"fwrite_unlocked", s_fwrite}, {"fread", s_fread}, {"fflush", s_fflush},
    {"fopen", s_fopen}, {"fopen64", s_fopen}, {"fdopen", s_fdopen}, {"freopen", s_freopen},
    {"freopen64", s_freopen}, {"fclose", s_fclose}, {"fgets", s_fgets}, {"fgetc", s_fgetc}, {"getc", s_fgetc},
    {"fseek", s_fseek}, {"fseeko", s_fseek}, {"fseeko64", s_fseek}, {"ftell", s_ftell}, {"ftello", s_ftell},
    {"ftello64", s_ftell}, {"rewind", s_rewind}, {"feof", s_feof}, {"ferror", s_ferror}, {"fileno", s_fileno},
    {"fileno_unlocked", s_fileno}, {"setvbuf", s_setvbuf}, {"remove", s_remove}, {"rename", s_rename},
    {"sscanf", s_sscanf}, {"__isoc99_sscanf", s_sscanf}, {"vsscanf", s_vsscanf}, {"__isoc99_vsscanf", s_vsscanf},
    {"fscanf", s_fscanf}, {"__isoc99_fscanf", s_fscanf}, {"vfscanf", s_vfscanf}, {"__isoc99_vfscanf", s_vfscanf},
    {"swprintf", s_swprintf}, {"vswprintf", s_vswprintf},
    {NULL, NULL},
};
static const RT64Data data[] = {
    {"stdin", &guest_stdin}, {"stdout", &guest_stdout}, {"stderr", &guest_stderr},
    {"_IO_2_1_stdin_", &guest_stdin}, {NULL, NULL},
};
RT64_SHIMS(shims, data)
