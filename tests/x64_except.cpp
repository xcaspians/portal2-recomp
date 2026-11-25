// C++ exceptions through lifted code (round C of docs/fork/P2CE_PLAN.md),
// for `make p2ce-modules-check`: compiled for x86_64-linux-gnu, linked by
// tests/x64_link.py against P2CE's libmimalloc.so, which carries the C++
// runtime and the unwinder (__cxa_throw, __gxx_personality_v0,
// _Unwind_RaiseException). No headers: it builds without a Linux sysroot.
struct Oops { int code; };

extern "C" int x64e_catch_here(int v) {
    try {
        if (v) throw Oops{v};
        return 0;
    } catch (const Oops &o) {
        return o.code + 1;
    }
}

__attribute__((noinline)) static int thrower(int v) {
    if (v > 0) throw Oops{v * 2};
    return -1;
}

__attribute__((noinline)) static int middle(int v) {
    return thrower(v) + 1000;      // no handler here: unwound through
}

extern "C" int x64e_catch_two_frames_up(int v) {
    try {
        return middle(v);
    } catch (Oops &o) {
        return o.code;
    }
}

extern "C" int x64e_catch_all(int v) {
    try {
        thrower(v);
    } catch (...) {
        return 7;
    }
    return 0;
}
