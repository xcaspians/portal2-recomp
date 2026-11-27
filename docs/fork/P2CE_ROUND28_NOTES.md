# P2CE, round 28: libraries, the Mac's SDL, DXVK on MoltenVK

Caspian approved four things this round: the library split, renderer route (a2), option 1 (DXVK 1.10 native
on MoltenVK), and round E before round F. P2CE only: nothing else's build, link or rendering changed.

## The library split

One executable holding every lifted module stopped linking once the launcher's whole list was in: arm64
branches reach ±128 MB, and past about 2 GB of code `ld`'s branch islands run out.

**What I chose: one dylib per module** (`tools/link64.py`), not text split into groups.
- **Why it's sturdier:**
  - A lifted module refers only to the runtime (`rt64_dispatch` and friends) and libc. Calls between modules
    go through the runtime's module table by guest address, never by symbol, so each module links on its
    own and nothing has to decide where to cut.
  - The largest image is one module (engine.so, 661 MB of code), which already linked inside a bigger
    executable.
  - Changing one module relinks only that one.
- **The runtime is `librt64.dylib`,** so its state (the module table, threads, TLS) exists once.
- **`make check` runs the x86-64 checks linked this way too** (`build/x64-dylib-check`, about 1 s, no game
  files).

## Round E: the Mac's SDL2

- **Native libraries for Linux names.** `RT64_NATIVE_LIBS` maps a Linux soname to a macOS dylib
  (`runtime/x64/native_libs64.h`).
  - An import that nothing lifted defines and no shim answers resolves to the native function.
  - The guest's `dlopen` of that name gives a native handle; its `dlsym` answers native addresses.
  - Shims always come first, so variadic functions keep theirs.
  - Calls go through the existing arm64 trampoline.
- **P2CE's SDL is the Mac's SDL2** (2.32, the host-library bundle's) instead of its lifted Linux SDL 2.26.
  Windows, the keyboard and the mouse are Cocoa's.

## Round F: DXVK 1.10 native on MoltenVK

- **`tools/build_dxvk_macos.sh`** fetches and builds, at pinned versions:
  - glslang 16.6.0;
  - MoltenVK v1.4.2;
  - DXVK 1.10.x from Gcenx/DXVK-macOS (`8f1e28de`: geometry shaders, tessellation, cull distance and
    transform feedback made optional), with dxvk-native's 1.x non-Windows build carried over
    (`tools/dxvk/dxvk-1.10-macos-native.patch`, 88 files).
- **What the patch does:**
  - the SDL2 window system and the native DirectX/Windows headers (from dxvk-native `c8dc91fa`);
  - a macOS platform layer (executable path, thread names, `yield` for `_mm_pause`, 2.0's non-x86
    `util_bit.h`);
  - the Vulkan library linked directly;
  - Win32-only sharing (shared textures and fences) off.
- **Why 1.10 and not newer.** DXVK made null descriptors mandatory in July 2022 and finished its native build
  in October, so no upstream version has both a native build and no null-descriptor requirement. The macOS
  fork sits exactly at `f95f5418`, the commit before the requirement.
- **`make p2ce-dxvk-clear` passes.**
  - It opens an SDL window, creates a D3D11 device at feature level 11_0 and a swap chain, clears to a
    colour, presents, and reads back exactly that colour (26 140 217 255).
  - That proves DXVK → MoltenVK → Metal on this Mac without P2CE.

### MoltenVK gaps

`make p2ce-moltenvk-features` lists what MoltenVK v1.4.2 lacks. The small ones, to fill as we go:
- **Geometry shaders:** optional with the fork's patch; effects that use them go missing.
- **Cull distance:** optional with the fork's patch.
- **Transform feedback:** optional; stream-output effects go missing.
- **Depth clip enable:** not available, so depth clipping always stays on; only states that turn it off draw
  wrong.
- **Disabling primitive restart:** Metal can't, so MoltenVK warns and keeps it on.
- **D24S8 depth buffers:** DXVK uses D32S8 instead (automatic).
- **robustness2 `nullDescriptor` / `robustBufferAccess2`:** not needed by DXVK 1.10; they're what DXVK 2.x and
  3.x would require.

### DXMT, worth revisiting later (not switched to)

- DXMT has a native build (`libdxmt-native.dylib`, the D3D11/DXGI entry points) that goes straight to Metal,
  with no Vulkan step. It also translates DXBC shaders straight to Metal's own format, where MoltenVK goes
  through SPIR-V and Metal source.
- **For iOS:** Metal is the only GPU API there. MoltenVK works on iOS and is App Store safe. DXMT's native
  build is macOS-focused; whether its shader converter and Metal 3 requirements build for iOS is the question
  to check. If it does, it removes a whole layer and its feature gaps (no Vulkan feature list to satisfy).
- I found no stable release of its native build, so revisit once it has one.

## Runtime and lifter fixes found on the way (all tested in `make check`)

- **Lifter:** an MMX shift by an immediate came out as `c->xmm[None]` (libavcodec).
- **Lifter:** a jump table whose base sits in a register loaded far above the jump (shaderapidx11.so) wasn't
  recognised; the jump trapped.
- **epoll on kqueue; a real eventfd;** timerfd says ENOSYS (libvideo.so's asio reactor and completion ports).
- **`faccessat`;** `fcntl` record locks with Linux's `struct flock` (P2CE's single-instance lock waited
  forever).
- **Linux's CA bundle paths open the Mac's `/etc/ssl/cert.pem`.** `RT64_TRACE_PATHS` and `RT64_CA_BUNDLE`
  help debug it.
- **`build64` skips relifting unchanged modules** (key: the module's bytes, the lifter's sources, the
  options).
- **Debian's x86-64 `libstdc++`/`libgcc_s`** (`tools/fetch_debian_amd64.sh`, SHA-256 from Debian's index),
  for libvideo.so.

## Sentry's crash dump consent

- With the Mac's SDL, libtier0's "Consent for Crash Dump Upload" box appeared on every run.
- `runtime/x64/sdl64.c` answers it No itself (the button that reads no, else Escape's), every run, without
  showing it. No file in the copy-on-write game folder can bring it back. Other message boxes still reach SDL.
- Nothing could be sent anyway: the runtime's libcurl refuses every handle, and Sentry's `crashpad_handler` is
  a Linux program that can't run here. `make p2ce-launch` also passes `-nominidumps -nobreakpad
  -nocrashdialog`.

## Modules now in the launch

The launcher's whole list, which all lift with no traps:
- richpresence, toolframework, vguimatsurface, workshop, panorama, panoramauiclient, video_services, devui;
- FreeType and fontconfig;
- libvideo and its ffmpeg libraries;
- Debian's libstdc++.

## Where it stops: two walls

### The windowed path (the renderer's): V8, plan round G

`make p2ce-launch-dx11` lifts `shaderapidx11.so` and gives it DXVK and the Mac's SDL. P2CE's SDL manager
starts SDL on Cocoa. Then Panorama, an application system the launcher loads unconditionally, initialises and
makes its first V8 call before the renderer creates the window:

```
steam64: SteamInternal_SteamAPI_Init ok (app 440000)
SDL video target is 'cocoa'
rt64: trap at 0x70d98762f7: call to unimplemented import _ZN2v814HeapStatisticsC1Ev
rt64:   panorama.so+0x3a22f7
```

- **The blocker is V8**, P2CE's JavaScript engine: libv8.so, 76 MB, plus ICU, abseil and Chrome's zlib. That's
  plan round G, the riskiest step:
  - V8 normally writes new machine code while it runs, which a recompiler can't follow;
  - its built-in functions are machine code stored inside libv8.so;
  - the plan is `--jitless` with those built-ins lifted.
- Panorama also wants Cairo, which links X11 and xcb (not on a Mac).
- **No launch option skips Panorama.** The engine prints "Panorama is not running!" in places, but the
  launcher always loads it.

### The dedicated path (towards the console banner): OpenSSL in libvideo.so

With `-dedicated` there's no Panorama, and the engine runs much further:
- Steam's `InitSafe`;
- libvideo's start-up with its own C++ runtime, epoll, eventfd, the single-instance lock, `faccessat`.

Then:

```
steam64: SteamAPI_InitSafe ok (app 440000)
Timed out waiting for game mapping!
opensslconnection.cpp (1519) : Assertion Failed: unable to load trusted SSL root certificates
```

- **The certificate file opens fine.** The trace shows `fopen` succeeding, then `fgets` reading the first
  certificate line by line.
- **OpenSSL stops on the last, short base64 line** without ever asking for `-----END CERTIFICATE-----`, with
  the Mac's bundle and with a three-certificate test bundle alike.
- **So OpenSSL's PEM reader, lifted inside libvideo.so, decides wrongly.** That's most likely a lifter
  semantics error in that code (no trap is reported, so it's a wrong result, not a missing instruction).
- **Finding it means stepping through OpenSSL's PEM code instruction by instruction.**

## Round 29: the OpenSSL bug, and the next wall on the dedicated path

**The bug was the lifter's, as suspected: `repe cmpsb` ran as `repne cmpsb`.**
- objdump prints the F3 prefix on `cmps`/`scas` as plain `rep`, which means
  REPE (repeat while equal). The x86-64 lifter treated a bare `rep` on those
  two like REPNE and stopped at the first *equal* byte.
- OpenSSL 1.0.2j's `PEM_read_bio`, inside libvideo.so, checks each line
  against `"-----END "` with `repe cmpsb`. Any base64 line with one byte in the
  same place counted as the END line. The first certificate's last line,
  `jLMi6Et8V…`, has an `E` at index 5, so the certificate ended there and every
  load failed.
- **Fixed** in `tools/lift64.py` (PR #27). Checked in `make check` by
  `test_lift64` and `x64_repe_cmps`, which fails without the fix.
- The i386 lifter already had it right.

**Dedicated now runs past libvideo** (no more SSL assertion), and through
Steam's game-server init. Game-server init fails, but that isn't fatal: the
engine says "Unable to load Steam support library … LAN mode only" (that text
doesn't reach the terminal; see below) and carries on. Then it stops:

```
steam64: SteamAPI_InitSafe ok (app 440000)
Timed out waiting for game mapping!
steam64: SteamInternal_SteamAPI_Init ok (app 440000)
steam64: SteamAPI_RestartAppIfNecessary(440000): no (never relaunched through Steam)
rt64: _exit(1) called from:
rt64:   [rsp+0] libtier0.so+0x86c7f      (Plat_ExitProcess)
rt64:   [rsp+0x20] libtier0.so+0x847a6   (LoggingSystem_Log)
rt64:   [rsp+0x80] engine.so+0x9f1f8b    (the engine's Error())
```

- **The message is "Could not load library matchmaking".**
  - `p2ce/bin/linux64/matchmaking.so` isn't lifted: it's not in
    `P2CE_SYSTEMS`.
  - The next step is to add it (and whatever it loads after it), the same way
    the round 28 modules were added.
- **Engine messages go missing on the dedicated path.** `Msg` lines reach the
  terminal, but `Warning` and `Error` text doesn't: the `Error()` text above
  was read from the stack. Finding where the dedicated console sends them is
  worth doing alongside.
- **New:** with `RT64_VERBOSE=1`, `exit` and `_exit` print their status and a
  rough guest backtrace.
- **Why the logs used to say `RestartAppIfNecessary` here:** that was the last
  line before the stop. objdump's PLT labels in engine.so sit 6 bytes into each
  entry, after `mov r11d, index`, so `call <X@plt+0xa>` is really the *next*
  import. Keep that in mind when reading engine.so disassembly.
