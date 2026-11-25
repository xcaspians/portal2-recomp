# Portal 2: Community Edition and Portal: Revolution: research (round 10)

Written 24 November 2026. Research and a first attempt only: how far it
got, what's in the way, and a size for the job.

## Inputs

| What | Depot | On this Mac? |
|---|---|---|
| P2CE shared content | 440002 | yes (1.3 GB) |
| P2CE Linux build | 440005 (`linux`) | yes (986 MB) |
| P2CE shader binaries | 440007 | yes (186 MB) |
| P2CE `windows,linux` depot (468 MB, not in tonight's list) | 440003 | no: probably needed to *run* it, not to study it |
| Revolution content | 601361 | yes (12 GB): **no binaries in it** |
| Revolution binaries | 601363 (263 MB, no OS tag) | no |

Depot facts come from Steam's own app info cache on this Mac
(`appcache/appinfo.vdf`): Revolution has **no Linux-tagged depot**; its only
other depot, 601363, carries no OS tag, so it should hold the binaries for
every platform. Command for Caspian: `download_depot 601360 601363` (and
`download_depot 440000 440003` before trying to run P2CE).

## P2CE's Linux build, sized up

| | P2CE (Strata Source) | What the pipeline does today |
|---|---|---|
| Architecture | **x86-64 ELF64** everywhere (`bin/linux64`, `p2ce/bin/linux64`) | i386 only: `tools/elfprep.py` stops at its first check ("ELF32 attendu") |
| ABI | System V AMD64: 16 registers, arguments in registers, RIP-relative addressing, 64-bit pointers | i386 cdecl: stack arguments, 32-bit guest addresses at a host offset |
| Renderer | **Direct3D 11** through DXVK (`shaderapidx11.so`, `libdxvk_d3d11.so.0`, `libdxvk_dxgi.so.0`) → Vulkan | Direct3D 9 (`shaderapivk`) → the runtime's bridge → mtld3d → Metal; no D3D11 path |
| UI | **Panorama** with JavaScript on **V8** (`panorama.so`, `libv8.so`, Pango/Cairo text) | VGUI |
| Libraries | SDL2, Steamworks (`libsteam_api.so`), mimalloc (preloaded), ffmpeg (`libavcodec.so.57`...), Jolt physics (`vphysics_jolt.so`), Steam Audio (`libphonon.so`), DXC (`libdxcompiler.so`), Discord SDK, Qt (tools), FreeType/Fontconfig | SDL2, FreeType, Fontconfig, Miles, OpenAL bridged; others lifted |
| Content | VPK (version 2) like Portal 2's | VPK read by the game itself (fine) |

## What carries over, what doesn't

- **Carries over:** the overall method (lift ahead of time, run on a
  runtime that bridges system calls and host libraries), the game-profile,
  prep and iOS app tooling, the touch overlay, menus and device tools, Steam
  through Valve's dylib on the Mac and the stub on iOS.
- **Must be rebuilt:**
  1. **The lifter and runtime for x86-64.** Decoding REX prefixes and 64-bit
     operands, 16 registers, RIP-relative memory, the AMD64 calling
     convention, a 64-bit guest address space, and every import shim
     retyped for LP64 structures. One upside: x86-64 Linux and arm64
     Apple are both LP64 little-endian, so most C structures have the same
     layout on both sides, and guest memory could be host memory, which
     makes bridging host libraries simpler than with i386.
  2. **Direct3D 11.** Either lift DXVK too and bridge its Vulkan calls to
     MoltenVK (Vulkan on Metal, macOS and iOS), or write a D3D11 → Metal
     path. The current mtld3d bridge is Direct3D 9 only.
  3. **V8.** Its JIT writes x86-64 machine code at run time, which an
     ahead-of-time recompiler can't translate, so V8 must run in its
     interpreter-only mode (`--jitless`, set when Panorama starts it), and
     its built-in functions (machine code embedded in `libv8.so`) must be
     lifted like any other code. This is the riskiest part.
- **Strata-specific:** Panorama, Jolt, Steam Audio and DXC are ordinary code
  once the above works, but each adds imports and indirect-call roots.

## First attempt

`tools/elfprep.py --out build/p2ce-probe bin/linux64/libtier0.so@0x10000000`
stops at once: `AssertionError: ELF32 attendu` (`elfprep.py`, line 29). That is
the expected first wall: every stage after it (gameprep, lift, runtime)
assumes 32-bit x86. Nothing else was attempted, since nothing further can
run without the x86-64 lifter.

## Portal: Revolution

Its content (601361) is a standalone game folder (`revolution/gameinfo.txt`,
its own search paths, no P2CE mount) and holds no binaries: its engine is in
601363, not downloaded. With them, the comparison is a `file`/`strings` pass
like L4D2's: if it is P2CE's Strata build, the P2CE work carries over with a
game profile; if it is a modified engine (likely, per its store page), add
its own interface versions and roots, and possibly its own renderer changes.

## Size of the job

About **10 to 15 rounds** before P2CE reaches its menu on the Mac:
x86-64 lifter and runtime (4–5), Direct3D 11 through DXVK and MoltenVK (2–3),
V8 jitless with its built-ins (2–3, uncertain), Panorama and the rest up to
the menu (2–3). iOS after that: memory (64-bit, V8), MoltenVK on iOS, touch
layout (2–3). Much bigger than Mel, as expected; worth doing only as its
own project.

## Update, 25 November 2026: depots 440003 and 601363

Both depots are now on the Mac. The `file`/`strings` comparison the round-10
note asked for:

| | P2CE 440003 | P2CE 440005 | Revolution 601363 |
|---|---|---|---|
| Platform | **Windows**, x86-64 (`bin/win64`, PE32+) | Linux, x86-64 (`bin/linux64`, ELF64) | **Windows only**, x86-64 (`bin/win64`, PE32+): **no Linux build at all** |
| Engine build | `Exe build: 07:12:08 Apr 23 2026` | `Exe build: 07:08:27 Apr 23 2026` | `Exe build: 01:56:13 Jan  5 2025` |
| Source paths in `engine.dll`/`.so` | `/builds/portal-2-community-edition/release/` | same | `E:\Licensee\PortalRevolution\revo\` (a licensed Strata branch), plus the same `D:\src\chaos\p2ce\` paths as P2CE |
| Interfaces | `VEngineClient015`, `VEngineServer023`, `VFileSystem018`, `VClient018` | same | same versions |
| Files | 162 in `bin/win64` | 114 `.so` | 113 in `bin/win64`; 90 shared with P2CE |
| Physics | `vphysics.dll` and `vphysics_jolt.dll` | `vphysics_jolt.so` | `vphysics.dll` only |
| Own files | | | `revolution.exe`, `soundsystem.dll`, `openvr_api.dll`, MySQL connectors, `mdllib.dll`, `unitlib.dll`; `revolution/bin/win64/client.dll`, `server.dll` |

Conclusions:

- **440003 is not needed to run P2CE on this project:** it is P2CE's Windows
  build. The Linux build (440005) is complete on its own: its `p2ce.sh` starts
  `bin/linux64/p2ce -game p2ce` with `libmimalloc.so` preloaded. The P2CE
  plan stays on 440005.
- **Revolution is a modified Strata engine from January 2025** (P2CE is from
  April 2026). Its interface versions are the same, but **it ships Windows
  binaries only**. Its game code (`revolution/bin/win64/client.dll`,
  `server.dll`) exists only as Windows DLLs, so running it here would need a
  64-bit **PE** front end for the lifter (Windows file format and calling
  convention, which differ from Linux's: arguments in `rcx`, `rdx`, `r8`,
  `r9`, a 32-byte shadow area on the stack, `xmm6`-`xmm15` preserved) and a
  Windows system bridge, or P2CE's Linux engine with Revolution's Windows
  game DLLs, which mixes a 2025 game with a 2026 engine. Either way it is a
  separate project after P2CE, not a game profile on top of it.
