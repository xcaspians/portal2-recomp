// MSVC C++ exceptions for lifted PE32+ modules (x64, __CxxFrameHandler3).
//
// _CxxThrowException(object, ThrowInfo) ends here (rt64_msvc_throw, called
// by the Windows bridge's shim). Like Windows, it walks the guest stack with
// the modules' .pdata/.xdata unwind information (a virtual unwind of the
// guest registers: nothing on the host stack is read), and for every frame
// whose language handler data is an FH3 FuncInfo (__CxxFrameHandler3 or
// __GSHandlerCheck_EH) maps the frame's pc to its EH state, then looks for
// a try block covering that state with a catch that takes the thrown type.
//
// Found: the catch object is built in the catching frame (copied, or by
// reference), the frames in between are unwound (their cleanup funclets run,
// destroying locals), the catching frame is unwound to its try block, and
// the catch funclet runs with rdx = the frame's establisher frame, all on the
// still-deep guest stack, as Windows does. The funclet returns the
// continuation address; the exception object is destroyed; the guest
// registers become the catching frame's and rip the continuation. The lifted
// host frames in between return (`goto resume`) until the lifted function
// that has the continuation as a resume point (tools/peprep64.py lists them
// as landing pads) continues there.
//
// `throw;` (a null ThrowInfo) rethrows the exception whose catch funclet is
// running on this thread. An exception leaving a catch funclet continues the
// search in the frame that holds that catch, outside its try block.
// Not handled: SEH filters (__C_specific_handler frames are passed through),
// FH4 tables, exceptions leaving destructors, catch(...) of non-C++ faults.
#include "rt64.h"
#include "win64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint64_t r[16]; XMM64 x[16]; uint64_t rip; } Ctx;

// The module's exception directory (from its mapped headers) and the RUNTIME_FUNCTION covering rva.
static uint64_t function_entry(uint64_t base, uint64_t rva) {
    uint64_t nt = base + R32(base + 0x3c);
    if (R32(nt) != 0x4550) return 0;                      // "PE\0\0"
    uint64_t opt = nt + 24;
    if (R16(opt) != 0x20b) return 0;                      // PE32+
    uint64_t dir = base + R32(opt + 112 + 3 * 8);
    uint64_t count = R32(opt + 112 + 3 * 8 + 4) / 12;
    uint64_t lo = 0, hi = count;
    while (lo < hi) {
        uint64_t mid = (lo + hi) / 2, e = dir + 12 * mid;
        if (rva < R32(e)) hi = mid;
        else if (rva >= R32(e + 4)) lo = mid + 1;
        else return e;
    }
    return 0;
}

static unsigned op_slots(uint8_t op, uint8_t info, int version) {
    switch (op) {
    case 1: return info ? 3 : 2;                              // UWOP_ALLOC_LARGE
    case 4: case 8: return 2;                                // UWOP_SAVE_NONVOL, UWOP_SAVE_XMM128
    case 5: case 9: return 3;                                // the _FAR forms
    case 6: return version >= 2 ? 2 : 2;                      // UWOP_EPILOG (v2) / old SAVE_XMM
    case 7: return 3;                                        // spare / old SAVE_XMM_FAR
    default: return 1;
    }
}

// One frame of RtlVirtualUnwind. On return, ctx is the caller's; *establisher the frame's
// establisher frame; *handler_data the language handler's data (0 if none). 0: no more frames.
static int virtual_unwind(Ctx *ctx, uint64_t *establisher, uint64_t *handler_data, uint64_t *func_base,
                          uint64_t *func_begin) {
    uint64_t base;
    const RT64Module *m = rt64_module_at(ctx->rip - 1, &base);
    *handler_data = 0;
    *func_base = base;
    *func_begin = 0;
    uint64_t entry = m && m->section_count ? function_entry(base, ctx->rip - 1 - base) : 0;
    if (!entry) {                                         // a leaf function: rip at [rsp]
        *establisher = ctx->r[RSP];
        ctx->rip = R64(ctx->r[RSP]);
        ctx->r[RSP] += 8;
        return m != NULL;
    }
    *func_begin = base + R32(entry);
    uint64_t offset = ctx->rip - base - R32(entry);
    uint64_t info = base + R32(entry + 8);
    int primary = 1;
    for (;;) {
        uint8_t version = R8(info) & 7, flags = R8(info) >> 3, prolog = R8(info + 1), count = R8(info + 2);
        uint8_t frame_reg = R8(info + 3) & 15, frame_off = R8(info + 3) >> 4;
        uint64_t frame = ctx->r[RSP];
        if (frame_reg) {
            int set = offset >= prolog;
            for (unsigned i = 0; !set && i < count; i += op_slots(R8(info + 4 + 2 * i + 1) & 15, R8(info + 4 + 2 * i + 1) >> 4, version))
                if ((R8(info + 4 + 2 * i + 1) & 15) == 3 && offset >= R8(info + 4 + 2 * i)) set = 1;
            if (set) frame = ctx->r[frame_reg] - 16 * (uint64_t)frame_off;
        }
        if (primary) *establisher = frame;
        for (unsigned i = 0; i < count;) {
            uint64_t code = info + 4 + 2 * (uint64_t)i;
            uint8_t at = R8(code), op = R8(code + 1) & 15, oi = R8(code + 1) >> 4;
            unsigned slots = op_slots(op, oi, version);
            i += slots;
            if (offset < at) continue;                    // not executed yet (inside the prolog)
            switch (op) {
            case 0: ctx->r[oi] = R64(ctx->r[RSP]); ctx->r[RSP] += 8; break;
            case 1: ctx->r[RSP] += oi ? R32(code + 2) : 8 * (uint64_t)R16(code + 2); break;
            case 2: ctx->r[RSP] += 8 * (uint64_t)oi + 8; break;
            case 3: ctx->r[RSP] = ctx->r[frame_reg] - 16 * (uint64_t)frame_off; break;
            case 4: ctx->r[oi] = R64(frame + 8 * (uint64_t)R16(code + 2)); break;
            case 5: ctx->r[oi] = R64(frame + R32(code + 2)); break;
            case 8: ctx->x[oi] = RX(frame + 16 * (uint64_t)R16(code + 2)); break;
            case 9: ctx->x[oi] = RX(frame + R32(code + 2)); break;
            case 10: {                                    // UWOP_PUSH_MACHFRAME
                uint64_t sp = ctx->r[RSP] + (oi ? 8 : 0);
                ctx->rip = R64(sp);
                ctx->r[RSP] = R64(sp + 24);
                return 1;
            }
            default: break;
            }
        }
        uint64_t tail = info + 4 + 2 * (uint64_t)((count + 1) & ~1);
        if (flags & 4) {                                  // UNW_FLAG_CHAININFO: the chained function's codes too
            info = base + R32(tail + 8);
            offset = ~(uint64_t)0 >> 1;
            primary = 0;
            continue;
        }
        if (primary && (flags & 3)) *handler_data = tail + 4;
        break;
    }
    ctx->rip = R64(ctx->r[RSP]);
    ctx->r[RSP] += 8;
    return 1;
}

// FH3's FuncInfo, if this handler data is one (FH3 or GS + FH3: the FuncInfo RVA comes first).
static uint64_t func_info(uint64_t base, uint64_t handler_data) {
    if (!handler_data) return 0;
    uint64_t fi = base + R32(handler_data);
    uint32_t magic = (uint32_t)R32(fi) & 0x1fffffff;      // magicNumber:29, bbtFlags:3
    return magic >= 0x19930520 && magic <= 0x19930522 ? fi : 0;
}

// The EH state at pc (the IP-to-state map: the last entry at or before pc - 1).
static int32_t state_of(uint64_t base, uint64_t fi, uint64_t pc) {
    uint32_t n = (uint32_t)R32(fi + 20);                 // nIPMapEntries, dispIPtoStateMap
    uint64_t map = base + R32(fi + 24), rva = pc - 1 - base;
    int32_t state = -1;
    for (uint32_t i = 0; i < n && R32(map + 8 * (uint64_t)i) <= rva; ++i) state = (int32_t)R32(map + 8 * (uint64_t)i + 4);
    return state;
}

// Host -> guest call of a funclet or helper: rcx, rdx, r8; the stack below the current one.
// 0 if it returned; 1 if an exception left it (rip and rsp already set elsewhere).
static int call_guest(CPU64 *c, uint64_t target, uint64_t rcx, uint64_t rdx, uint64_t r8, uint64_t *rax) {
    uint64_t sp = c->r[RSP];
    c->r[RCX] = rcx; c->r[RDX] = rdx; c->r[R8] = r8;
    c->r[RSP] = ((sp - 256) & ~(uint64_t)15) - 32;
    PUSH64(c, RT64_SENTINEL);
    rt64_dispatch(c, target);
    if (c->rip != RT64_SENTINEL) {
        // An exception left it and was caught above: the inner throw set rip and rsp, and its
        // dispatch consumed `transferred`; the shim we're in must not return like a RET either.
        c->transferred = 1;
        return 1;
    }
    if (rax) *rax = c->r[RAX];
    c->r[RSP] = sp;
    return 0;
}

static int unwind_to_state(CPU64 *c, uint64_t base, uint64_t fi, uint64_t establisher, int32_t state, int32_t target) {
    uint64_t map = base + R32(fi + 8);
    int32_t max = (int32_t)R32(fi + 4);
    while (state > target && state < max) {
        uint64_t e = map + 8 * (uint64_t)state;
        uint32_t action = (uint32_t)R32(e + 4);
        state = (int32_t)R32(e);
        if (action && call_guest(c, base + action, 0, establisher, 0x103, NULL)) return 1;
    }
    return 0;
}

static int same_type(uint64_t a, uint64_t b) {
    return a == b || !strcmp((const char *)(uintptr_t)(a + 16), (const char *)(uintptr_t)(b + 16));
}

// The catchable type of the thrown object a handler takes, 0 if none (catch(...): the first one).
static uint64_t match(uint64_t catch_base, uint64_t handler, uint64_t throw_base, uint64_t throw_info) {
    uint32_t adjectives = (uint32_t)R32(handler);
    uint32_t type = (uint32_t)R32(handler + 4);
    uint64_t array = throw_base + R32(throw_info + 12);
    uint32_t n = (uint32_t)R32(array);
    if (!type || (adjectives & 0x40)) return n ? throw_base + R32(array + 4) : 1;
    uint64_t wanted = catch_base + type;
    if (!R8(wanted + 16)) return n ? throw_base + R32(array + 4) : 1;      // an empty name: catch(...)
    for (uint32_t i = 0; i < n; ++i) {
        uint64_t ct = throw_base + R32(array + 4 + 4 * (uint64_t)i);
        uint32_t properties = (uint32_t)R32(ct);
        if ((properties & 2) && !(adjectives & 8)) continue;               // CT_ByReferenceOnly
        if (same_type(throw_base + R32(ct + 4), wanted)) return ct;
    }
    return 0;
}

static uint64_t adjust(uint64_t object, uint64_t ct) {
    int32_t mdisp = (int32_t)R32(ct + 8), pdisp = (int32_t)R32(ct + 12), vdisp = (int32_t)R32(ct + 16);
    uint64_t at = object + (uint64_t)(int64_t)mdisp;
    if (pdisp >= 0)
        at += (uint64_t)(int64_t)((int32_t)R32(R64(object + (uint64_t)(int64_t)pdisp) + (uint64_t)(int64_t)vdisp) + pdisp);
    return at;
}

// A catch running on this thread: what `throw;` rethrows, and where to go on if
// an exception leaves it.
typedef struct {
    uint64_t object, throw_info, sentinel_slot;
    Ctx frame;                       // the catching frame (its caller's view: rip in the function)
    uint64_t establisher, base, fi;
    int32_t outside;                 // the state outside the try block
} Active;
static __thread Active active[16];
static __thread int active_count;

static void destroy(CPU64 *c, uint64_t object, uint64_t throw_info) {
    uint64_t base;
    if (!object || !throw_info || !rt64_module_at(throw_info, &base)) return;
    uint32_t dtor = (uint32_t)R32(throw_info + 4);
    if (getenv("RT64_TRACE_EH")) fprintf(stderr, "msvceh: destroy %#llx with +%#x\n", (unsigned long long)object, dtor);
    if (dtor) call_guest(c, base + dtor, object, 0, 0, NULL);
}

static const char *type_name(uint64_t throw_info) {
    uint64_t base;
    if (!throw_info || !rt64_module_at(throw_info, &base)) return "?";
    uint64_t array = base + R32(throw_info + 12);
    if (!R32(array)) return "?";
    return (const char *)(uintptr_t)(base + R32(base + R32(array + 4) + 4) + 16);
}

static int same_frame(const Ctx *a, const Ctx *b) { return a->rip == b->rip && a->r[RSP] == b->r[RSP]; }

void rt64_msvc_throw(CPU64 *c, uint64_t object, uint64_t throw_info) {
    if (!throw_info) {                                    // throw;
        if (!active_count) rt64_trap(c, R64(c->r[RSP]), "C++ rethrow with no exception being handled (std::terminate)");
        object = active[active_count - 1].object;
        throw_info = active[active_count - 1].throw_info;
    }
    uint64_t throw_base;
    if (!rt64_module_at(throw_info, &throw_base)) rt64_trap(c, R64(c->r[RSP]), "C++ throw: ThrowInfo outside every module");
    // The thrower's context: the shim was called, [rsp] is the return address.
    Ctx start;
    memcpy(start.r, c->r, sizeof start.r);
    memcpy(start.x, c->xmm, sizeof start.x);
    start.rip = R64(c->r[RSP]);
    start.r[RSP] += 8;

    // Phase 1: the search. An exception leaving a catch funclet goes on in its frame.
    Ctx ctx = start, found_ctx;
    uint64_t found_handler = 0, found_ct = 0, found_base = 0, found_fi = 0, found_establisher = 0;
    uint64_t found_try = 0;
    int frames = 0, found_frame = -1, resumed_from = -1;
    int32_t found_state = -1;
    int depth = active_count;
    static int trace = -1;
    if (trace < 0) trace = getenv("RT64_TRACE_EH") != NULL;
    for (; frames < 100000 && !found_handler; ++frames) {
        Ctx here = ctx;
        uint64_t establisher, data, base, begin;
        int32_t forced = -2;
        if (here.rip == RT64_SENTINEL) {
            // Leaving one of our catch funclets: continue in the frame that holds the catch.
            if (depth && active[depth - 1].sentinel_slot == here.r[RSP] - 8) {
                Active *a = &active[--depth];
                resumed_from = depth;
                here = a->frame;
                forced = a->outside;
            } else {
                break;                                    // a host frame: nothing above is guest code we can reach
            }
        }
        ctx = here;
        if (!virtual_unwind(&ctx, &establisher, &data, &base, &begin)) break;
        uint64_t fi = func_info(base, data);
        if (trace) fprintf(stderr, "msvceh: frame %d: pc +%#llx, function +%#llx, establisher %#llx, FuncInfo %#llx\n", frames,
                           (unsigned long long)(here.rip - base), (unsigned long long)(begin ? begin - base : 0),
                           (unsigned long long)establisher, (unsigned long long)(fi ? fi - base : 0));
        if (forced != -2) {                               // the catching frame again: its saved establisher
            fi = active[depth].fi; base = active[depth].base; establisher = active[depth].establisher;
        }
        if (!fi) continue;
        int32_t state = forced != -2 ? forced : state_of(base, fi, here.rip);
        if (trace) fprintf(stderr, "msvceh:   state %d, %u try blocks\n", state, (unsigned)R32(fi + 12));
        uint32_t tries = (uint32_t)R32(fi + 12);
        uint64_t map = base + R32(fi + 16);
        for (uint32_t t = 0; t < tries && !found_handler; ++t) {
            uint64_t tb = map + 20 * (uint64_t)t;
            if (state < (int32_t)R32(tb) || state > (int32_t)R32(tb + 4)) continue;
            uint32_t handlers = (uint32_t)R32(tb + 12);
            uint64_t harray = base + R32(tb + 16);
            for (uint32_t h = 0; h < handlers; ++h) {
                uint64_t handler = harray + 20 * (uint64_t)h;
                uint64_t ct = match(base, handler, throw_base, throw_info);
                if (!ct) continue;
                found_handler = handler; found_ct = ct; found_base = base; found_fi = fi; found_try = tb;
                found_establisher = establisher; found_ctx = here; found_frame = frames; found_state = state;
                break;
            }
        }
    }
    if (!found_handler) {
        char why[300];
        const char *what = "";
        if (object && strstr(type_name(throw_info), "exception")) what = (const char *)(uintptr_t)R64(object + 8);
        snprintf(why, sizeof why, "C++ exception %s%s%s: no catch takes it (std::terminate)", type_name(throw_info),
                 what && *what ? " what: " : "", what ? what : "");
        rt64_trap(c, start.rip, why);
    }
    (void)found_frame;
    (void)resumed_from;

    // The catch object, in the catching frame.
    uint32_t adjectives = (uint32_t)R32(found_handler);
    int32_t disp = (int32_t)R32(found_handler + 8);
    if (disp && R32(found_handler + 4) && found_ct > 1) {
        uint64_t dest = found_establisher + (uint64_t)(int64_t)disp;
        uint64_t src = adjust(object, found_ct);
        uint32_t properties = (uint32_t)R32(found_ct), size = (uint32_t)R32(found_ct + 20), copy = (uint32_t)R32(found_ct + 24);
        if (adjectives & 8) W64(dest, src);
        else if ((properties & 1) || !copy) memcpy((void *)(uintptr_t)dest, (const void *)(uintptr_t)src, size);
        else if (call_guest(c, throw_base + copy, dest, src, (properties & 4) ? 1 : 0, NULL)) return;
    }

    // Phase 2: unwind the frames below the catching one (their cleanups), then it to its try block.
    ctx = start;
    depth = active_count;
    for (;;) {
        Ctx here = ctx;
        uint64_t establisher, data, base, begin;
        int32_t forced = -2;
        if (same_frame(&here, &found_ctx)) break;
        if (here.rip == RT64_SENTINEL) {
            if (!depth || active[depth - 1].sentinel_slot != here.r[RSP] - 8) break;
            Active *a = &active[--depth];
            // The abandoned catch: its exception dies with it unless it's the one going on.
            if (a->object && a->object != object) destroy(c, a->object, a->throw_info);
            here = a->frame;
            forced = a->outside;
            active_count = depth;
            if (same_frame(&here, &found_ctx)) break;
            ctx = here;
            if (!virtual_unwind(&ctx, &establisher, &data, &base, &begin)) break;
            if (unwind_to_state(c, a->base, a->fi, a->establisher, forced, -1)) return;
            continue;
        }
        ctx = here;
        if (!virtual_unwind(&ctx, &establisher, &data, &base, &begin)) break;
        uint64_t fi = func_info(base, data);
        if (fi && unwind_to_state(c, base, fi, establisher, state_of(base, fi, here.rip), -1)) return;
    }
    if (unwind_to_state(c, found_base, found_fi, found_establisher, found_state, (int32_t)R32(found_try))) return;

    // The catch funclet, with this exception as the one being handled.
    if (active_count == 16) rt64_trap(c, start.rip, "C++ catch blocks nested too deep");
    Active *a = &active[active_count++];
    a->object = object;
    a->throw_info = throw_info;
    a->frame = found_ctx;
    a->establisher = found_establisher;
    a->base = found_base;
    a->fi = found_fi;
    int32_t low = (int32_t)R32(found_try);
    a->outside = low >= 0 && low < (int32_t)R32(found_fi + 4) ? (int32_t)R32(found_base + R32(found_fi + 8) + 8 * (uint64_t)low) : -1;
    uint64_t sp = c->r[RSP];
    a->sentinel_slot = (((sp - 256) & ~(uint64_t)15) - 32) - 8;
    uint64_t continuation = 0;
    int left = call_guest(c, found_base + R32(found_handler + 12), 0, found_establisher, 0x100, &continuation);
    if (left) return;                                     // an exception left the catch: already handled above
    active_count--;
    destroy(c, object, throw_info);

    // Continue in the catching frame.
    static const int kept[] = {RBX, RBP, RSI, RDI, R12, R13, R14, R15, RSP};
    for (unsigned i = 0; i < sizeof kept / sizeof *kept; ++i) c->r[kept[i]] = found_ctx.r[kept[i]];
    for (int i = 6; i < 16; ++i) c->xmm[i] = found_ctx.x[i];
    c->rip = continuation;
    c->transferred = 1;
}
