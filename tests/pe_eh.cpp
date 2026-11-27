// make check: MSVC C++ exceptions in a lifted PE module (runtime/x64/msvceh64.c).
// Compiled by clang for x86_64-windows-msvc (the __CxxFrameHandler3 tables,
// catch and cleanup funclets MSVC emits), linked into pefix.dll by
// tests/pe_link.py with tests/pe_programs.s, called by tests/pe_checks.c.
// No C++ library: only the language (throw, try/catch, destructors).
#define EXPORT extern "C" __attribute__((noinline))
#define NOINLINE __attribute__((noinline))

static volatile int destroyed;          // destructors run, counted
struct Guard { int add; NOINLINE ~Guard() { destroyed += add; } };
struct Base { int v; Base(int v) : v(v) {} virtual int get() const { return v; } virtual ~Base() {} };
struct Derived : Base { int extra; Derived(int v) : Base(v), extra(1000) {} int get() const override { return v + extra; } ~Derived() override { destroyed += 100; } };
struct Other { int x; };

NOINLINE static void throw_int(int x) { if (x) throw x; }
NOINLINE static void throw_derived(int x) { if (x) throw Derived(x); }
NOINLINE static void throw_other() { throw Other{5}; }
// A frame in between with locals to destroy and a try block that doesn't take an int.
NOINLINE static int middle(int x) {
    Guard g{1};
    try { Guard h{10}; throw_int(x); } catch (Other &) { return -1; }
    return 0;
}

EXPORT int pe_eh_int(int x) {
    try { throw_int(x); } catch (int v) { return v + 1; }
    return 0;
}
EXPORT int pe_eh_unwind(int x) {
    destroyed = 0;
    try { return middle(x); } catch (int v) { return v * 100 + destroyed; }
}
EXPORT int pe_eh_base_ref(int x) {
    destroyed = 0;
    try { throw_derived(x); } catch (Base &b) { return b.get(); }
    return 0;
}
EXPORT int pe_eh_destroyed(void) { return destroyed; }
EXPORT int pe_eh_all(void) {
    try { throw_other(); } catch (int) { return 1; } catch (...) { return 2; }
    return 0;
}
NOINLINE static int rethrow_inner(int x) {
    try { throw_int(x); } catch (int) { destroyed += 7; throw; }
    return 0;
}
EXPORT int pe_eh_rethrow(int x) {
    destroyed = 0;
    try { return rethrow_inner(x); } catch (int v) { return v + destroyed; }
}
// Two throws in a row through the same frames, and a catch that continues the loop.
EXPORT int pe_eh_loop(int n) {
    int sum = 0;
    for (int i = 1; i <= n; ++i) {
        try { throw_int(i); } catch (int v) { sum += v; }
    }
    return sum;
}
