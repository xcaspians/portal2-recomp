# P2CE, round 23: piece D's Steam connection

Caspian OK'd it after round 18: the Steam part of round E moved ahead, so
P2CE's launcher gets past "Failed to initialize filesystem Steam API context".

## What was built

- **Native calls from lifted code** (`runtime/x64/native64.c`). P2CE is
  x86-64 and Valve's Mac Steam library is arm64 in the same address space,
  so pointers pass as they are; only the calling convention differs. A small
  arm64 trampoline moves the System V registers (and two stack arguments)
  into arm64's, calls, and returns both the integer and floating-point
  results. The dispatcher sends any call into a dyld-loaded image there
  (that's how the Steam interfaces' C++ methods, which live in Valve's
  `steamclient.dylib`, are reached).
- **The Steam bridge** (`runtime/x64/steam64.c`). Linux's `libsteam_api.so`
  isn't lifted any more; its functions, imported or looked up with `dlsym`,
  call Valve's macOS `libsteam_api.dylib` (Steamworks.NET 2024.8.0, the same
  one Half-Life 2 and Portal use). Three are done by the runtime itself:
  - `SteamInternal_ContextInit` fills the game's Steam context by calling
    the game's own function (Valve's library can't call lifted code);
  - `SteamAPI_RestartAppIfNecessary` always answers "no": Steam is never
    asked to relaunch anything;
  - callbacks and call results use Steam's *manual dispatch*: the runtime
    pulls them and runs the game's `CCallbackBase::Run` on the game's thread.
  No emulation: every answer comes from the Steam client on the Mac, logged
  in by Caspian. No key or password is read, stored or logged anywhere.
- **Two runtime/lifter fixes found on the way** (each with a check):
  `--wrap`ped file functions (`__wrap_access`, `__wrap_fopen`...) answer as
  the plain ones; and the lifter recognises jump tables whose `add` and `jmp`
  are separated by an unrelated instruction (a case of the launcher's
  switch ended in a trap).

## How far it gets

`make p2ce-launch` (Steam running on the Mac):

```
steam64: SteamInternal_SteamAPI_Init ok (app 440000)
...
AppFramework : Unable to load module shaderapidx11!
Couldn't load interface VENGINE_LAUNCHER_API_VERSION004!
```

Steam starts for P2CE's app, the file system mounts, and the launcher moves
on to loading the engine's systems. It stops at the renderer,
`shaderapidx11.so`, which isn't lifted: on Linux it's Direct3D 11 through
DXVK (to Vulkan), so it needs a bridge of its own (the plan's rendering
round), not just lifting.

## What's next

1. Lift `shaderapidx11.so` and decide its bridge: Direct3D 11 to Metal (a
   new layer; the D3D9 one doesn't cover it) or Vulkan through MoltenVK
   under DXVK. This is the big decision for P2CE.
2. With `-textmode`, try a null shader API (`shaderapiempty.so`, if the
   depot has it) to reach the engine's console banner without a renderer.
3. Steam's interfaces through C++ vtables are reached but barely exercised:
   structures returned through a hidden pointer aren't handled yet (rare
   in Steam's API), and more than eight arguments aren't either.
