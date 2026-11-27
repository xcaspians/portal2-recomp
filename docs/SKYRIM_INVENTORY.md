# Skyrim Special Edition: round 0 inventory

Written 26 November 2026 (Skyrim round 0, fork round 30). **Research only.**
Everything below was read in place from Caspian's own copy: app 489830,
downloaded with the Steam console's `download_depot` into Steam's
`steamapps/content/app_489830/`. Nothing was copied, extracted, lifted,
built or written there, and no saves were touched.

**What was read:**
- **`SkyrimSE.exe`** (depot 489833):
  - FileVersion **1.7.104.0** ("TESV: Skyrim", original name `TESV.exe`);
  - PE timestamp 24 August 2026;
  - 38 MB.
- **`Data/Skyrim - Shaders.bsa`** (depot 489832): 68 MB, BSA version 105, not
  compressed. It was read in memory for counts only.
- The other files were only listed by name (`bink2w64.dll`, `steam_api64.dll`,
  the INI presets, `SkyrimSELauncher.exe`).

The scripts were throwaway, in the session's scratch folder. Their method is
described below so the numbers can be redone.

## 1. DRM: GO (with one rule question)

| Check | Result |
|---|---|
| `.bind` section | **present** (`va 0x38ef000`, 229 KB, entropy 7.96: encrypted) |
| Entry point | **inside `.bind`** (RVA `0x38ef310`) |
| `.text` encrypted? | **No.** The entropy of all 380 64 KB chunks is 4.65–6.57 (encrypted data reads ~8.0); MSVC prologues are everywhere (47,008 `mov [rsp+…], rbx`, 85,452 `sub rsp, imm8`); objdump decodes it as ordinary code. |

**So `SkyrimSE.exe` is wrapped in SteamStub, but with code encryption off.**
- Only the stub at the entry point is encrypted. The game's code itself sits in
  plain form in `.text`.
- The lifter can read it as it is on disk: **no decryption, unpacking or
  modified file is needed.**
- The game's real start-up code (MSVC's `mainCRTStartup`) is in plain `.text`:
  - 5 candidates match its shape (`0x1406e5bd0`, `0x1407ddf80`, `0x140934ca0`,
    `0x1415a8324`, `0x1415a8514`);
  - picking the one that reaches `__scrt_common_main_seh` and `WinMain` is
    ordinary static analysis in round 1.

**The rule question (Caspian's call before round 1).** *Decided 26 November
2026: approved with the recommended approach (`SKYRIM_PLAN.md`, Decision 1).*
- Starting at the game's own entry point means SteamStub's launch check never
  runs. The stub's code is encrypted, so it can't be lifted statically.
- Ownership would still be checked for real, with no faking:
  - the game itself calls `SteamAPI_Init` through Valve's `libsteam_api.dylib`,
    which fails unless the Steam client is running and the account owns app
    489830;
  - the runtime can also ask Steam `BIsSubscribedApp(489830)` before starting,
    and refuse to run otherwise.
- **Recommended:** do both, so the port only ever runs for a Steam account that
  owns Skyrim. Nothing is written to disk.
- **If that isn't acceptable**, the alternatives are:
  - a DRM-free copy (GOG's edition): a separate purchase, a different
    build, and about one extra round to re-inventory;
  - an unwrap step (Steamless-style), which writes a decrypted executable: out
    of scope under this project's rules, and not recommended.

## 2. PE32+ basics

| | |
|---|---|
| Machine / format | x86-64 (`0x8664`), PE32+ (`0x20b`), GUI subsystem |
| Image base / size | `0x140000000` / 57.2 MB (`0x3929000`) |
| DLL characteristics | `0x8160`: ASLR with high-entropy addresses, NX, terminal-server aware; base relocations present (342 KB) |
| Entry point | RVA `0x38ef310`, in `.bind` (SteamStub) |
| TLS | 10,764 bytes of static TLS, **no TLS callbacks** |
| `.pdata` | **122,258 functions** (RUNTIME_FUNCTION entries) |
| Load config | **Control Flow Guard on** (guard flags `0x100`) |
| Signed | yes (Authenticode, 10.6 KB) |

| Section | Virtual size | Notes |
|---|---|---|
| `.text` | 23.8 MB | the game's code, plain |
| `.rdata` | 8.7 MB | |
| `.data` | 22.1 MB (1.2 MB on disk) | |
| `.pdata` | 1.4 MB | unwind table |
| `.text` (second) | 6.4 KB | read-write data despite the name (flags `0xc0000040`), entropy 2.6 |
| `_RDATA` | 1.8 KB | |
| `.rsrc` | 587 KB | icons, version |
| `.reloc` | 342 KB | |
| `.bind` | 229 KB | SteamStub, encrypted |

## 3. Imports: 32 DLLs, 606 functions, no delay-loaded imports

| DLL | Functions | Kind | Host side |
|---|---|---|---|
| KERNEL32 | 137 | Windows | POSIX shims |
| USER32 | 44 | Windows | SDL2 |
| GDI32 | 1 | Windows (`GetStockObject`) | stub |
| ADVAPI32 | 6 | Windows (registry, user name) | small emulation |
| SHELL32 | 2 | Windows (`SHGetFolderPathA`, `ShellExecuteA`) | the project's own save folder |
| ole32 | 8 | Windows COM (`CoCreateInstance`: XAudio2) | COM thunks |
| WINMM | 4 | Windows timers | trivial |
| WS2_32 | 32 | Windows sockets | POSIX sockets, or refused |
| WINHTTP | 14 | Windows HTTP (Bethesda.net / Creation Club) | **refused** (offline) |
| CRYPT32 | 6 | certificates (same) | refused |
| SETUPAPI | 4 | device enumeration | stub (no devices) |
| HID | 11 | raw HID controllers | stub, or SDL's HID later |
| **MSVCP140** | **90** | **Microsoft C++ standard library** (iostreams, locale) | **new: MSVC-ABI C++ shims** |
| **VCRUNTIME140** | **23** | **MSVC runtime** (`_CxxThrowException`, `__C_specific_handler`, `longjmp`) | **exceptions round** |
| api-ms-win-crt-* (11 sets) | 190 | Universal C runtime | the Mac's libc |
| d3d11 | 1 | `D3D11CreateDeviceAndSwapChain` | DXVK 1.10 native |
| dxgi | 1 | `CreateDXGIFactory` | DXVK 1.10 native |
| **d3dx9_42** | **9** | **old D3DX9 math** (matrix and vector helpers) | **reimplement (small, pure maths)** |
| X3DAudio1_7 | 2 | 3D audio maths | FAudio's F3DAudio |
| XINPUT1_3 | 3 (by ordinal) | controller | SDL2 |
| DINPUT8 | 1 | keyboard and mouse | SDL2 |
| **steam_api64** | **10** | **Steam** (flat API and `SteamInternal_CreateInterface`) | Valve's `libsteam_api.dylib` |
| **bink2w64** | **9** | **Bink 2 video** (a game-folder DLL) | lift as a second PE module, or skip |

**Not standard Windows or DirectX (flagged):**
- Steam (`steam_api64`) and Bink (`bink2w64`), as expected.
- `d3dx9_42`: an old DirectX SDK helper, used only for maths.
- The Microsoft C/C++ runtime as DLLs (MSVCP140, VCRUNTIME140, UCRT), not built
  in.

**Not in the import table:**
- **Havok** is linked statically (826 `hkp` strings in the executable).
- So are **Scaleform** (GFx) and **Papyrus**.

**Loaded at run time by name** (`LoadLibrary`/`GetProcAddress`):
- `xaudio2_7.dll` (through COM, as expected);
- `dbghelp.dll` (crash dumps);
- `imm32.dll` (text input);
- `secur32.dll`.

## 4. lift64 coverage

**Method.**
- Every instruction objdump decodes in `.text` was given to
  `lift64.translate()`, the same test `make p2ce-coverage` uses (lift64, simd64,
  x87_64). Nothing was generated or kept.
- Only instructions inside the 122,258 `.pdata` function ranges count. That
  leaves out MSVC's in-code jump tables, padding and data between functions:
  403,379 decoded "instructions" fall outside and were skipped.

| | Count | Share |
|---|---|---|
| Instructions inside functions | **5,387,558** | |
| Refused by lift64 | 11,591 | 0.215 % |
| of which objdump `<unknown>` (bytes that aren't instructions: data inside functions) | 11,424 | |
| of which other forms | 167 | 0.003 % |
| **AVX/AVX2** | **6** (4 AVX/FMA/F16C, 2 AVX2) | all behind a CPUID/`xgetbv` check: they trap only if the CPU claims AVX, which the runtime's CPUID doesn't |

**The 167 other refusals are also data read as code:**
- `rep add`, `repne push`, `repne cwde`;
- `enter 0x49, -0x67`;
- moves to and from `ss`/`gs`;
- `retfq`, `getsec`, `verw`;
- `divps xmm24` (an AVX-512 register);
- `ficom [rip + 0x15d91d00]` (outside the image).

**So lift64 already handles effectively all of Skyrim's real instructions.**
That's the same picture as P2CE's engine (100 %).

## 5. Graphics

- **Direct3D 11:**
  - through `D3D11CreateDeviceAndSwapChain` and `CreateDXGIFactory`;
  - no D3D9 device, since `d3dx9_42` is only its maths;
  - no D3D12;
  - no run-time shader compiler (`d3dcompiler` isn't imported or named).

**`Skyrim - Shaders.bsa`: 16,044 compiled DXBC programs, all shader model 5.0.**

| Program type | Count |
|---|---|
| Pixel | 12,635 |
| Vertex | 3,390 |
| Compute | 19 |
| **Geometry / hull / domain** | **0** |

**Inside `SkyrimSE.exe`:** 57 more DXBC programs, shader model 4.0/5.0: 45
pixel and 12 vertex (the menus' and tools' shaders).

**Against the MoltenVK gaps** (`make p2ce-moltenvk-features`):
- **No geometry shaders, no tessellation, no stream output: none of the
  feature gaps applies.**
- Compute shaders are supported by MoltenVK.
- What's left is render state: depth clip disable (DXVK clamps), primitive
  restart and D24S8, all with automatic workarounds.

## 6. Walls

| Possible wall | Found? |
|---|---|
| Anti-tamper (Denuvo, VMProtect, Themida, Arxan) | **none**: SteamStub (above) is the only protection |
| JIT or code written at run time | **none**: no `VirtualProtect` or `FlushInstructionCache` anywhere in the executable; `VirtualAlloc` only for memory |
| Self-modifying code | **none outside `.bind`**: `.text` is plain and isn't re-protected |
| Scripting VMs | Papyrus and Scaleform's ActionScript are interpreters in the executable: just more lifted code |
| Debugger checks | `IsDebuggerPresent` only (imported; answer "no") |
| Control Flow Guard | on: indirect calls go through `__guard_dispatch_icall_fptr`, a pointer the loader fills. The runtime fills it with a plain dispatcher. Small. |
| **The MSVC C++ library as a DLL** | **new medium-sized piece:** 90 MSVCP140 functions (iostreams, locale, threads) with Microsoft's C++ ABI. Mac's libc++ can't stand in directly. They're either shimmed one by one, or Wine's MSVC-compatible `msvcp140` (LGPL) is built as a PE and lifted like any module. |

**Nothing V8-sized.**
