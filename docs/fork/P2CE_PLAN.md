# Portal 2: Community Edition: roadmap

Written 24 November 2026 (fork round 13). Builds on
[P2CE_RESEARCH.md](P2CE_RESEARCH.md). Future rounds are specced from this
page; each round below has a milestone Caspian can check himself and a size.

**Size:** "S" fits in part of a round, "M" is about one round, "L" is one to
two rounds, "XL" is two or more. The total is about 13 to 17 rounds to the
main menu on the Mac, then 3 to 4 more for iOS. Rounds A to D are the
foundation; nothing visible happens on screen before round G.

**Ground rules for every round:** work on the `feat/x86-64` branch (or a
branch from it), never change how Portal 2, Portal, Half-Life 2, Mel or L4D2
are built (they use the i386 tools, which stay as they are), and keep
`make check` fast and free of game files. A test that needs the P2CE depot
is gated behind `P2_P2CE_TESTS=1`.

## The pieces, in order

| Round | What | Size | Milestone Caspian can check |
|---|---|---|---|
| A (13, done) | **64-bit foundation.** ELF64 reader (`tools/elfprep64.py`), a first 64-bit lifter (`tools/lift64.py`), a small 64-bit runtime (`runtime/x64/`), a test harness in `make check`. | L | `make check` passes; `P2_P2CE_TESTS=1 make p2ce-tier0-check` lifts P2CE's `libtier0.so` and calls a few of its functions. |
| B (16, done) | **Lifter coverage and speed.** The rest of the integer set, the SSE/SSE2 set the engine uses (copied from the i386 lifter's `float_lift.py` ideas, widened to 16 registers), x87 leftovers, jump tables in every form clang emits, unused-flag removal (the i386 lifter's `dead_flags`), splitting the output in files and compiling in parallel like `tools/build_game.py`. | L | `make p2ce-coverage` reports 99 %+ of the instructions of every P2CE `.so` handled; the whole of `libtier0.so` and `libvstdlib.so` compiles. |
| | *Done 25 November 2026:* SSE3/SSSE3/SSE4.1, x87, MMX, BMI added (`tools/simd64.py`, `tools/x87_64.py`); AVX and later trap behind CPUID; flag hand-over dropped at calls; split, cached builds (`tools/build64.py`). `make p2ce-coverage`: 147 x86-64 files (114 `.so`), 81.4 million instructions, 2,716 not handled (99.997 %), every file above 99.7 % (engine, client, server: 100 %); 1.87 million of the handled ones are traps on purpose (AVX/AVX-512 1.83 M, AES/SHA, data decoded as code). `make p2ce-build`: `libtier0.so` and `libvstdlib.so` compile whole (15 s). | | **Reached.** |
| C (17, done) | **Many modules and the dynamic loader.** Several `.so` files at once with Linux's symbol search order (`lookup_scopes` in `elfprep.py`), `dlopen`/`dlsym` of lifted modules, constructors and destructors, thread-local storage (`__tls_get_addr`, `fs:`), C++ exceptions and `longjmp` (the i386 runtime's resume points). | L | A test loads `libtier0.so` + `libvstdlib.so` + `filesystem_stdio.so`, runs their constructors, and `CommandLine()->CheckParm` works. |
| | *Done:* `runtime/x64/loader64.c` (load order, relocations against other modules, static TLS, constructors, exit handlers), `dl64.c`, setjmp/longjmp and C++ exceptions through resume points and landing pads; the C library shims those modules need. `make p2ce-modules-check`: 15 checks, including `CheckParm("-game")` → `p2ce` and an exception caught two lifted frames up. `libmimalloc.so` had to join: it carries P2CE's C++ runtime. | | **Reached.** |
| D (18, partly) | **System bridge for 64-bit.** The import shims (libc, pthread, files, time, sockets, `dlopen`) retyped for 64-bit structures. Most are simpler than for i386 because Linux x86-64 and Apple arm64 lay out C structures the same way and guest pointers are host pointers. Threads, signals, `mmap`, `/proc` emulation. | XL | The dedicated server program (`srcds`-style launcher of P2CE) starts, loads `engine.so`, and prints its console banner in the Terminal. |
| | *Done:* threads, program start (kernel stack, `_start`, `__libc_start_main`), `/proc`, SYSCALL, files/time/locale/stdio shims, a copy-on-write mirror of the game folder (`tools/run64.py`). `make p2ce-launch` runs P2CE's own `p2ce` → `launcher.so` → `filesystem_stdio.so`, which stops at `Failed to initialize filesystem Steam API context` (no Steam client for Linux) before `engine.so` loads. **Not reached** in round 18; **round 23**: the Steam bridge to Valve's `libsteam_api.dylib` works (Steam starts for app 440000, the file system mounts); the launcher now stops at the renderer, `shaderapidx11.so` (P2CE_ROUND23_NOTES.md). There is no separate dedicated-server program in the P2CE depot, so the milestone is the launcher's engine start. | | Not yet. |
| | *Round 27:* `inputsystem.so` and the engine's other systems lift and compile (material system, data cache, studio render, both physics libraries, engine audio, sound emitter, script system, VScript, localize, scene file cache, VGUI: 2.98 million instructions, 1 trap, in `vphysics_jolt.so`); `make p2ce-launch` now lifts them all and runs with `-shaderapi shaderapiempty`. The run itself stops **inside Valve's own `SteamInternal_SteamAPI_Init`**, waiting on the Steam client (it gets no answer; the same with round 23's runtime, so not a code change), before the launcher reaches the systems. Native calls now cover hidden-pointer returns and more than eight arguments. Renderer: [P2CE_RENDERER_DECISION.md](P2CE_RENDERER_DECISION.md), waiting for Caspian. | | Not yet (blocked on the Steam client). |
| E | **SDL2, input, audio, Steam.** The existing bridges (SDL video/input, OpenAL or Steam Audio's output, Valve's `libsteam_api.dylib`) wired to the 64-bit runtime. | M | P2CE's launcher opens an empty SDL window on the Mac and reads the keyboard; Steam sees the game as running. |
| F | **Direct3D 11 through DXVK and MoltenVK.** Lift DXVK (`libdxvk_d3d11.so.0`, `libdxvk_dxgi.so.0`) like any other module and bridge its Vulkan calls to MoltenVK (Vulkan on Metal). Shaders: DXVK turns D3D11 bytecode into SPIR-V, MoltenVK turns SPIR-V into Metal. Fallback if DXVK fights back: a direct D3D11-to-Metal bridge (much bigger). | XL | The window clears to a colour, then draws the loading screen. |
| G | **V8 without its compiler (jitless).** V8 normally writes new x86-64 machine code while it runs, which an ahead-of-time recompiler can't follow. Start it with `--jitless` (interpreter only) and lift its built-in functions, which are machine code stored inside `libv8.so`. Riskiest step. | XL (uncertain) | A test runs `1 + 1` and a small JavaScript function through the lifted V8 and gets the right answers. |
| H | **Panorama to the main menu.** `panorama.so`, its text stack (Pango, Cairo, HarfBuzz), Jolt physics, Steam Audio, DXC, ffmpeg for videos; one indirect-call or import gap at a time, like Mel and L4D2. | L | **P2CE reaches its main menu on the Mac**, with mouse and keyboard. |
| I | **Into a map.** Map loading, physics, portals, sound, saves; play the first chambers. | L | Caspian plays the first test chambers on the Mac. |
| J | **iOS: memory and build.** The 64-bit runtime in the iOS app (arm64 iOS is also LP64, so the memory model carries over), MoltenVK on iOS, V8 jitless (iOS forbids writing code at run time anyway), app size and memory budget. | L | P2CE starts on the iPad mini and reaches its main menu. |
| K | **iOS: touch and polish.** The touch overlay, menus and device tools that Portal 2's iOS app already has, mapped to Panorama's menus. | M | Caspian plays a map on the iPad mini with touch controls. |

Portal: Revolution follows P2CE once its binaries (depot 601363) are on the
Mac: if it is P2CE's Strata build, it is a new game profile; if it is a
modified engine, its own interface versions and indirect-call roots, about one
extra round.

## Order and why

- **A, B, C before anything else:** every other piece is lifted code, so the
  lifter has to be complete and correct first. Bugs found later in the lifter
  are expensive, because they look like game bugs.
- **D before E:** the game can't open a window before it can read files and
  start threads.
- **F before G:** the renderer is needed to see anything; V8 is only needed
  once Panorama draws the menus.
- **iOS last:** everything in A to I carries over, and iOS adds its own
  limits (memory, no code written at run time, MoltenVK on iOS).

## Risks

- **V8's built-in functions** are hand-written machine code with their own
  calling rules; they may need special handling in the lifter.
- **DXVK** uses many threads and a large part of Vulkan; MoltenVK does not
  support all of it (geometry shaders, for example, are emulated or missing).
- **Size:** P2CE's Linux build is about 1 GB, much larger than Portal 2's.
  Generated C and compile time will grow; round B's splitting and caching
  matter.
- **iOS memory:** a 64-bit engine plus V8 plus Metal on a 4 GB iPad is tight.

## Round 28 (26 Nov 2026): past Steam, stopped by the link size

With the Steam client healthy again (its background service had stopped at 4:26 that morning), `make
p2ce-launch` gets past `SteamInternal_SteamAPI_Init ok (app 440000)`. The launcher then loads its systems one
by one:

- **video_services:** lifted and added to `P2CE_SYSTEMS`. The next stop is `richpresence`: "Unable to load
  interface RichPresenceInterface001".
- **The rest of the launcher's list** all lift with no traps: richpresence, toolframework, vguimatsurface,
  workshop, panorama (1.3 M instructions) and panoramauiclient.
- **But one executable holding them no longer links.** `ld: B/BL out of range (max is +/-128MB)` at a branch
  island. With panorama the code reaches about 1 GB of displacement; even without it, 237 MB.
- **Next step (needs Caspian's OK):** link each lifted module as its own dylib (as the real engine has its
  own .so files) and resolve calls between them through the existing module table, or split the text into
  separately linked groups. Then continue to the console banner.


**Later in round 28** ([P2CE_ROUND28_NOTES.md](P2CE_ROUND28_NOTES.md)):
- **The split is done**: one dylib per lifted module.
- **Round E has started**: the Mac's SDL2 stands in for P2CE's, and SDL runs on Cocoa.
- **Round F's renderer builds**: DXVK 1.10 native on MoltenVK clears a window and reads the colour back.
- **Not reached yet: P2CE's own window.**
  - The windowed path stops at Panorama's first V8 call (round G).
  - The dedicated path stops at OpenSSL in libvideo.so failing to read the CA certificates, most likely a
    lifter error.
