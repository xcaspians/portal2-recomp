# The Elder Scrolls V: Skyrim Special Edition: plan (proposal)

Written 26 November 2026 (fork round 29). **Research and plan only: nothing is
built for Skyrim yet, and no repository has been created.** Each piece below
has a size and a milestone Caspian can check.

**Size:** "S" fits in part of a round, "M" is about one round, "L" is one to
two rounds, "XL" is two or more. **Total: about 13 to 17 rounds to the main
menu on the Mac, then 3 to 5 more for the iPad** (was 14 to 19 before the
round 0 inventory: see "After round 0" below). The lifter, the dylib linking
and the renderer carry over; the whole operating-system side is new.

**What carries over from P2CE:**
- the x86-64 lifter (`tools/lift64.py`, `simd64.py`, `x87_64.py`) and its
  compile cache;
- the dylib-per-module linking (`tools/link64.py`) and `librt64.dylib`;
- the native-library bridge (`native_libs64.h`) and the arm64 call trampoline;
- the copy-on-write game folder (`tools/run64.py`);
- Valve's `libsteam_api.dylib` bridge;
- DXVK 1.10 native on MoltenVK (`tools/build_dxvk_macos.sh`, proven by
  `make p2ce-dxvk-clear`);
- round 29's lesson: lifter bugs give wrong results, not traps, so every one
  gets a check in `make check`.

## The big difference: Windows binaries

P2CE ships Linux ELF files. Skyrim SE only ships Windows ones: `SkyrimSE.exe`
(PE32+ x86-64), `steam_api64.dll` and `bink2w64.dll`. Portal: Revolution hit
the same question (`Prompts/backlog.md`). Building a 64-bit PE front end once
serves both games.

**What a PE32+ front end takes:**
- **The file format:**
  - sections, base relocations, imports by name and by ordinal, exports;
  - the TLS directory, and the `.pdata`/`.xdata` unwind tables.
  - `tools/peprep.py` already reads PE32 headers, relocations and exports for
    the Miles plugins; it gets the 64-bit fields.
- **The Windows x64 calling convention, only at the bridge:**
  - Lifted-to-lifted calls don't care: they are guest code calling guest code.
  - Where guest code calls the host (a shim, DXVK, Steam), arguments come in
    `rcx, rdx, r8, r9`, then the stack after a 32-byte shadow space. Floats
    take the same positions in `xmm0`–`xmm3`.
  - `xmm6`–`xmm15` are callee-saved.
  - Callbacks (window procedures, thread starts, audio callbacks) go the other
    way.
  - Varargs (`printf` family) are a small special case.
- **LLP64 structures:**
  - On Linux x86-64 the C structures matched Apple arm64, so most shims passed
    pointers through. On Windows `long` is 32 bits, `wchar_t` is 16 bits
    (UTF-16), and many structures differ.
  - So more shims copy or convert. That's the main reason the bridge is bigger.
- **Thread-local storage and the TEB:**
  - Windows code reads its thread block through `gs:`. Static TLS is
    `gs:[0x58]` plus `_tls_index`. The lifter maps `fs:` for Linux today and
    gets a `gs:` twin.
- **Exceptions (SEH):**
  - x64 Windows exceptions are table-driven (`.pdata`), like Linux's
    `.eh_frame`, so the runtime's resume points and landing pads carry over.
  - New: `RtlVirtualUnwind`, `RaiseException`, `__C_specific_handler` and the
    MSVC C++ handlers (`__CxxFrameHandler3`/`4`).

**The Windows system bridge, by size:**

| Area | What Skyrim needs from it | Host side | Size |
|---|---|---|---|
| kernel32 / ntdll | files (Windows paths, case-insensitive, `\` separators), threads, events, critical sections, waits, `Interlocked*`, heap, `VirtualAlloc`, TLS, time, modules (`GetModuleHandle`, `GetProcAddress`), error codes | POSIX, the existing copy-on-write mirror | XL (the biggest single piece) |
| C runtime | `ucrtbase`/`vcruntime140`/`msvcp140` if imported (shims, like the Linux libc ones), or nothing if Skyrim links it statically | the Mac's libc | M–L |
| user32 / gdi32 | the window, the message loop, cursor, display modes, `MessageBox` | SDL2 (already native) | L |
| advapi32 / shell32 / ole32 | registry reads (install path), `SHGetKnownFolderPath` ("My Games" saves, sent to our own folder), `CoInitialize`, `CoCreateInstance` | small emulations | M |
| DirectInput 8, XInput | keyboard, mouse, controller | SDL2 | M |
| XAudio2 2.7, X3DAudio | all game sound (COM objects with callbacks) | FAudio (zlib licence, the XAudio2 API reimplemented natively) | L |
| Direct3D 11 / DXGI | rendering (COM) | DXVK 1.10 native | L (thunks only) |
| Steam API | `steam_api64.dll` | Valve's `libsteam_api.dylib` | M |

**COM is the other new piece.** D3D11, DXGI and XAudio2 are COM: the game
calls methods through vtables, with the Windows convention. DXVK native and
FAudio are arm64 libraries with Apple's convention, so every method the game
can call needs a thunk. These should be generated from the headers (`d3d11.h`,
`dxgi.h`, FAudio's), not written by hand. That's several hundred methods but
one generator.

**Comparison with Portal: Revolution.** Revolution has the same PE32+ front end
question, but a much smaller system surface (a Source engine, mostly SDL-like).
Skyrim needs the full Windows desktop set above. Doing the front end and the
kernel32 core once serves both. Revolution's route (b), P2CE's Linux engine
with Revolution's Windows game DLLs, has no Skyrim equivalent: Skyrim has no
Linux build.

## Renderer: DXVK 1.10 native on MoltenVK

- **Direct route.** Yes. Skyrim SE is a D3D11 game, so its calls can go
  straight to the native DXVK 1.10 build through the COM thunks above, the same
  build `make p2ce-dxvk-clear` already proves on this Mac.
- **The window.** DXVK native's SDL window system takes an `SDL_Window *` where
  Windows passes an `HWND`. The user32 shim makes every `HWND` Skyrim creates an
  SDL window, so the swap chain finds it.
- **Evidence.** Skyrim SE runs on Apple silicon in CrossOver, with Wine plus
  DXVK on MoltenVK. A recent CodeWeavers forum report says 1.7.104 crashes with
  D3DMetal and works with DXVK. That's the same renderer stack, so no renderer
  dealbreaker is expected.
- **Shaders** come precompiled (DXBC in `Skyrim - Shaders.bsa`), which is what
  DXVK reads. No run-time HLSL compiler is needed unless the inventory finds
  `D3DCompile` imported.

**MoltenVK gaps against Skyrim** (from `make p2ce-moltenvk-features`):

| Gap | Skyrim | Effect |
|---|---|---|
| Geometry shaders | **to check** in round 0 (scan the shader archive's DXBC program types) | effects that use them go missing (made optional by the macOS DXVK patches) |
| Transform feedback / stream output | unlikely | missing effects only |
| Tessellation | DXVK-macOS handles it; Skyrim SE's vanilla renderer isn't known to rely on it, **to check** | — |
| Depth clip disable | likely (shadow maps commonly turn depth clipping off) | DXVK falls back to depth clamping, which Metal has; only edge cases differ |
| Primitive restart off, D24S8 | automatic workarounds | none visible |
| Compute shaders | supported by MoltenVK | — |

None of these looks like a wall. The worst case is a missing effect.

## Other blockers

- **Steam DRM (SteamStub): checked in round 0, GO.** The executable is wrapped,
  but its code isn't encrypted (`SKYRIM_INVENTORY.md`); what follows was the
  concern before the check.
- *(Before round 0)* **Steam DRM (SteamStub): the one possible dealbreaker, to check first.**
  - Many Steam executables are wrapped by SteamStub. The real code is encrypted
    on disk and decrypted at start-up by a `.bind` section.
  - A static recompiler can't lift encrypted code. Decrypting it ahead of time
    is DRM removal, which this project's rules put out of scope, like faking
    ownership.
  - I couldn't confirm from public sources whether the current Steam
    `SkyrimSE.exe` is wrapped. Round 0 checks it in seconds (a `.bind` section
    and an entry point inside it).
  - **If it is wrapped, it needs Caspian's decision** before any other round.
    The honest options are:
    - (a) stop;
    - (b) a DRM-free copy Caspian owns (the GOG edition has no SteamStub, but it's a
      separate purchase and a different build);
    - (c) running the stub's own decryption inside the runtime with the real
      Steam client and lifting what it produces. That still produces unpacked
      code on disk, so it is (b)'s legal question in a different form.
  - **If it's not wrapped, there's no DRM wall.** `steam_api64.dll` goes to
    Valve's `libsteam_api.dylib` (flat API plus interface thunks), exactly as
    P2CE does, with no ownership faking.
- **Bink 2 videos** (`bink2w64.dll`): a Windows DLL of plain code, lifted as a
  second PE module (its SIMD paths pick SSE through CPUID). The intro can also
  be skipped through the INI, so this isn't on the critical path.
- **Papyrus** (the scripting VM) is a bytecode interpreter inside
  `SkyrimSE.exe`. It writes no machine code, so it's just more lifted code, not
  a V8-sized wall.
- **Scaleform GFx** (the menus) interprets ActionScript 2 with no JIT, so it's
  also just lifted code.
- **Havok** is statically linked and SIMD-heavy. The lifter's SSE coverage is
  complete; **AVX** traps today (behind CPUID). If the inventory finds AVX
  without a CPUID-guarded fallback, that's an L-sized lifter round.
- **Size of the code:** `SkyrimSE.exe` is about 37 MB. That's well under
  P2CE's `engine.so`, so the lift, compile and link fit the existing tools.
- **The Creation Club / Bethesda.net store** (1.6+) wants the network. It's
  refused like every other network handle, and the game is expected to carry on
  offline.

**No V8-sized wall is visible beyond SteamStub.** The size is in breadth (the
Windows bridge, COM), not in one impossible piece.

## The pieces, in order

| Round | What | Size | Milestone Caspian can check |
|---|---|---|---|
| 0 | **Inventory** from Caspian's own install (needs the depots on the Mac, read-only): SteamStub check, the full import table by DLL, TLS and `.pdata` counts, instruction coverage with the lift64 decoder (AVX count), the shader archive's program types (geometry/hull/domain/compute). | S | A one-page `SKYRIM_INVENTORY.md` with those numbers, and a go/no-go on SteamStub. |
| 1 | **PE32+ front end:** `peprep64` (format, relocations, imports, exports, TLS, `.pdata`), Win64 module type in lift64, the `gs:` thread block. A small Windows test program built from assembly (`clang -target x86_64-windows`) in `make check`. | L | `make check` lifts and runs the PE test program; `SkyrimSE.exe` lifts with a coverage report. |
| 2 | **Windows-convention bridge:** shims declared with Windows prototypes, the trampoline both ways, varargs, UTF-16 strings; a COM thunk generator. | L | `make check` calls shims and a COM object both ways from the PE test program. |
| 3 | **kernel32 core and the C runtime:** files, threads, sync, heap, virtual memory, TLS, time, modules (137 KERNEL32 imports); the UCRT (190) and the MSVC C++ library (MSVCP140, 90). | XL | `SkyrimSE.exe` starts on the Mac, reads its INI files and writes its first log line. |
| 4 | **Exceptions:** unwinding over `.pdata`, `RaiseException`, the MSVC C++ handlers. | L | A C++ exception thrown and caught two lifted frames up in the test program; Skyrim gets past start-up. |
| 5 | **Steam:** `steam_api64.dll` to `libsteam_api.dylib`. | M | Steam shows Skyrim SE as running. |
| 6 | **Window, input, sound:** user32 on SDL, DirectInput and XInput on SDL, XAudio2 and X3DAudio on FAudio. | L | An empty Skyrim window opens and reads the keyboard; a sound plays. |
| 7 | **Direct3D 11:** COM thunks to DXVK 1.10 native, DXGI swap chain on the SDL window. | L | The window clears to a colour, then the Bethesda logo or loading screen draws. |
| 8 | **Bink** (or the intro skipped cleanly). | M | The intro video plays, or is skipped with no error. |
| 9 | **To the main menu:** archive loading, Scaleform, Papyrus start-up, the remaining import gaps one at a time. | L | **Skyrim SE reaches its main menu on the Mac** with mouse and keyboard. |
| 10 | **Into the game:** a new game, the Helgen cart ride, saving and loading (saves in the project's own folder, never the real ones). | L | Caspian plays out of Helgen on the Mac. |
| 11 | **iOS build and memory:** the runtime in an iOS app, MoltenVK on iOS, the data on the device. | L | Skyrim reaches its main menu on the iPad Pro. |
| 12 | **iOS memory pressure:** texture and cell budgets, streaming. | M | Riverwood and Whiterun without a memory kill. |
| 13 | **Touch and controller:** a touch layout like Portal 2's iOS app, controller support from round 6. | M | Caspian plays on the iPad Pro with touch and with a controller. |

### Sizes behind the total

- **Mac to the main menu:** rounds 0 to 9, about 13 to 17 rounds. Round 3 alone
  can take 3 or more.
- **iOS:** rounds 11 to 13, 3 to 5 more.

## iPad memory budget

- **What Skyrim SE uses on a PC, vanilla (community figures):** about 2 to 3
  GB of system memory plus 2 to 4 GB of video memory.
- **On the iPad, memory is shared,** so the total is about 4 to 7 GB, plus the
  lifted code (estimated a few hundred MB).
- **iPad Pro 11-inch M5:**
  - iOS reports 11.5 GB;
  - the app's limit, with Sauce's increased-memory-limit entitlement, measured
    about 5.1 GB for Mel.
  - So Skyrim fits only with lower texture settings (the PC "low"/"medium"
    texture sizes) and a smaller cell-loading radius (`uGridsToLoad` stays 5;
    the distant-object settings come down).
- **iPad mini (12 GB):** a similar limit to the Pro's. Measure it on the first
  iOS build, install-only as usual.
- **The game data:** about 12 to 15 GB on the device. Plan for on-demand
  archives later if space is tight.
- **Free Apple ID:** the increased-memory-limit entitlement isn't confirmed for
  free accounts (`FREE_APPLE_ID.md`). Without it, Skyrim won't fit, so the iPad
  route assumes the paid account.

## Where it lives (proposal: nothing created)

**Recommendation: a new repository for Skyrim, with the x86-64 tools shared,
not copied.**

- **Why a new repo:**
  - This repo is "portal2-recomp", its README, installer and CI are about
    Source games, and its rules mention Valve.
  - Skyrim has a different publisher, a different engine, different data
    folders and saves, and its own app.
  - A separate repo keeps both histories readable and `make check` fast.
- **How to share without copy-pasting, in two steps:**
  1. **Now (no change to this repo):** the Skyrim repo adds this repo as a git
     submodule, pinned to a commit, and builds `tools/lift64.py`,
     `runtime/x64/` and `tools/link64.py` from it. Lifter fixes land here (with
     their check) and Skyrim bumps the pin.
  2. **Once the PE front end and the Windows bridge settle:** move the x86-64
     toolkit (lift64 and friends, `runtime/x64`, the PE front end, the Windows
     bridge) into its own small repo with its own `make check`. Both projects
     then submodule it, and Portal: Revolution uses the same Windows bridge.
     That touches only the P2CE tools here, never how Portal 2, Portal, HL2, Mel
     or L4D2 are built.
- **The alternative:** Skyrim inside this repo (`games/skyrim/`). It's simpler
  for the first rounds, but it mixes two products, and the installer, CI and
  README would all have to grow a second identity. I don't recommend it.

**Round 1 layout of the new repo:**

```
CLAUDE.md          slim: name rules (Caspian only), no game files or Bethesda code
                   committed, saves never touched, Mac first, make check rules
ARCHITECTURE.md    the pipeline: PE front end → lift64 → per-module dylibs →
                   Windows bridge (kernel32/user32/COM) → SDL, FAudio, DXVK/MoltenVK
PROGRESS.md        one table, one row per round, milestone reached or not
Prompts/           one file per round, like this repo's
docs/              SKYRIM_PLAN.md (this file, moved), SKYRIM_INVENTORY.md
third_party/       the toolkit submodule (step 1: this repo at a pinned commit)
tools/             peprep64.py and the Skyrim-specific build scripts
runtime/win64/     the Windows bridge, one file per DLL family, tests beside it
tests/             the PE test program (assembly), checks for every shim
Makefile           make check (no game files), make skyrim-inventory, make skyrim-launch
```

## Rules for every Skyrim round

- **Game files:** read from Caspian's own Steam install. Never copy them into
  the repo, never write to the Steam folder, and never touch the Skyrim saves.
  The copy-on-write mirror holds any file the game writes.
- **No Bethesda code or data committed,** and no DRM removal.
- **Mac first.** Nothing about Portal 2, Portal, HL2, Mel, L4D2 or P2CE changes
  because of Skyrim, except shared lifter fixes, each with its check.
- **Lifter bugs** each get a check in `make check`, in the shared toolkit.

## After round 0 (26 November 2026)

`SKYRIM_INVENTORY.md` has the numbers. **The estimate drops from 14–19 to
13–17 rounds** to the main menu on the Mac.

**Lower:**
- **Coverage:** lift64 already handles effectively all of Skyrim's 5.4 million
  real instructions, and AVX is 6 instructions behind a CPUID check. So round 1
  needs no lifter work beyond the PE front end, and the "AVX without fallback"
  L round is gone.
- **Renderer:** no geometry, hull or domain shaders at all (16,044 shader model
  5.0 programs: pixel, vertex, 19 compute). None of the MoltenVK feature gaps
  applies.
- **A small import table:** 606 functions from 32 DLLs, no delay-loaded imports,
  no TLS callbacks.
- **No JIT, no anti-tamper, no self-modifying code** outside SteamStub's own
  stub.

**Higher (smaller):**
- **The Microsoft C++ library is a DLL** (MSVCP140, 90 functions, Microsoft's
  C++ ABI). That's about one extra round inside round 3: shims, or Wine's
  `msvcp140` (LGPL) built as a PE and lifted.
- **Control Flow Guard** (the loader fills `__guard_dispatch_icall_fptr`) and the
  **d3dx9 maths** (9 functions): S each.
- **The real entry point** has to be found statically (5 candidates), because
  the stub isn't run: S, in round 1.

**Round 1 needs Caspian's two decisions first:**
- the SteamStub rule question in `SKYRIM_INVENTORY.md` (start at the game's own
  entry and check ownership through Valve's library);
- where the code lives (the new-repo proposal above).
