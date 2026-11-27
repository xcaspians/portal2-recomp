// Native libraries standing in for Linux ones (round 28: P2CE's SDL2, then
// DXVK). RT64_NATIVE_LIBS lists them, separated by ';':
//   libSDL2-2.0.so.0=/path/libSDL2.dylib;libdxvk_d3d11.so.0=/path/libdxvk_d3d11.dylib
// A lifted module's import that nothing lifted defines and no shim answers
// resolves to the native function (loader64.c); dlopen of one of these names
// gives a native handle whose dlsym answers native addresses (dl64.c). Calls
// to them go through the arm64 trampoline (native64.c), so their arguments
// must be laid out alike on Linux x86-64 and Apple arm64 (LP64, the same
// structure layouts): true of SDL's and Direct3D's. Variadic functions aren't:
// they need shims, which are always found first.
// Internal to the runtime: not in rt64.h, whose hash keys every lifted
// module's compile cache.
#pragma once
#include <stdint.h>

int rt64_native_library(const char *name);                        // index, or -1 when not one of them
uint64_t rt64_native_library_symbol(int index, const char *name);  // 0 if undefined
uint64_t rt64_native_symbol(const char *name);                     // any of them, 0 if none
