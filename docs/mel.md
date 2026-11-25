# Portal Stories: Mel (experimental)

The same pipeline also recompiles **Portal Stories: Mel** (Steam app 317400, a
Portal 2 community mod shipped with its own 2021 Linux build of the Portal 2
engine, mod `portal_stories`). It is experimental and **macOS only** for now:
the Mac app starts, plays the Bink menu video, loads chambers across chapters
1–4 on Metal or OpenGL, and saves and loads games cleanly, but a full
playthrough has not been completed yet and additional indirect call roots may
still be discovered.

**You must own Portal 2 and have Portal Stories: Mel on Steam** (the mod is
free on Steam for Portal 2 owners). As for Portal 2, everything is generated on
your machine from your own copy and nothing from Valve or Prism Studios is in
this repository. Keep the generated app and code for yourself.

## Install

You need what Portal 2 needs (see the [README](../README.md#what-you-need)),
Mel's game data and its Linux binaries (two separate downloads):

1. Install *Portal Stories: Mel* with Steam, or open the Steam console
   (`open steam://open/console`) and type `download_depot 317400 317401` (about
   8.6 GB) to download its game data (which also contains `portal_stories/bin/*.so`).
   Wait for "Depot download complete".
2. Open the Steam console and type `download_depot 317400 317404` (about 90 MB)
   for the Linux engine binaries. Wait for "Depot download complete".
3. Tick Portal Stories Mel in the Natif Installer, or run
   `./setup.sh --game mel` (`./setup.sh` alone builds every game found) in a
   checkout ([README](../README.md#install-on-mac)). Without paths, it finds
   the game in `~/Library/Application Support/Steam/steamapps/common/Portal Stories Mel`
   or `depot_317401`, and the Linux binaries in `depot_317404`; otherwise pass
   them:
   `./setup.sh --game mel "/path/to/Portal Stories Mel" "/path/to/depot_317404"`.

What differs from the Portal 2 install: `setup.sh` downloads Valve's
`libsteam_api.dylib` from Steamworks.NET 20.1.0 (same as Portal 2), plus two
Debian bullseye i386 packages (`libstdc++6` and `libgcc-s1`, SHA-256 verified)
for `libstdc++.so.6` and `libgcc_s.so.1`, builds in `build/mel` (about 15
minutes the first time; other games' builds are left alone), and creates
`~/Applications/Portal Stories Mel Natif.app` (converting `portal2.ico` to
`.icns` with `sips`). Open Steam with your account, then open the app.

- Saves and settings: `~/Library/Application Support/Portal Stories Mel Natif/userdata`.
- Log: `~/Library/Logs/PortalStoriesMelNatif.log`.
- Renderer: **Metal** when `libd3d9metal` is there, as for Portal 2. To force
  OpenGL: `open --env P2_RENDERER=opengl -a "Portal Stories Mel Natif"`.
- `./uninstall.sh` offers to remove the app and, if you confirm, its saves,
  with separate questions for the other games.

## What differs from Portal 2

- **Game profile** (`tools/games.py`): program `portal2_linux`, engine
  libraries under `bin/` (`lib_dirs`), mod libraries under `portal_stories/bin/`
  relative to the game data folder (`game_lib_dirs`), i386 system libraries
  `libstdc++.so.6` and `libgcc_s.so.1` (`system_libs`, path in
  `system_lib_dir`), roots in `config/mel-roots.json`, build under `build/mel`,
  binary `build/mel/mel-native`. Pick it with `--game mel` (or `P2_GAME=mel`).
- **Non-PIE executable**: Mel's `portal2_linux` is an `ET_EXEC` binary linked
  at `0x08048000` (Portal 2's is PIE), so `tools/gameprep.py` keeps its linked
  base at `0`.
- **Dynamic `libstdc++` and `libgcc_s`**: unlike current Portal 2 (which links
  `libstdc++` statically), Mel's modules dynamically depend on `libstdc++.so.6`
  (including `STB_GNU_UNIQUE` template statics such as COW `std::string` empty
  storage) and `libgcc_s.so.1`.
- **Steamworks SDK 1.40**: Mel asks for older Steam interfaces (`SteamClient017`,
  `SteamUser019`, `SteamFriends015`, `SteamUtils009`,
  `STEAMUSERSTATS_INTERFACE_VERSION011`, `STEAMREMOTESTORAGE_INTERFACE_VERSION014`,
  `STEAMUGC_INTERFACE_VERSION010`, `STEAMHTTP_INTERFACE_VERSION002`,
  `SteamController005`, `SteamGameServer012`, `SteamNetworking005`). No arm64
  dylib exists for SDK 1.40, so `tools/gen_steam.py` extracts vtable slots from
  the i386 flat functions of Mel's own `libsteam_api.so`, cross-checks their
  stack word counts against Steamworks.NET 20.1.0 signatures, and calls each
  slot directly on the host interface object returned by `steamclient.dylib`
  ([steam.md](steam.md)).
- **12-byte D3D9 vertex elements**: the 2021 Linux DXVK-native headers compiled
  into Mel's `shaderapivk.so` define `WORD` as 32 bits, making
  `D3DVERTEXELEMENT9` 12 bytes (`Stream` and `Offset` as `uint32_t`).
  `runtime/d3d9.c` detects the 12-byte sentinel `{0xff, 0, D3DDECLTYPE_UNUSED}`
  and converts elements to and from the standard 8-byte layout.
- **Bink code sections**: `engine.so` and `valve_avi.so` embed Bink in custom
  executable sections (`BINK*`) containing inline jump tables that throw
  `objdump`'s linear sweep out of step. `tools/elfprep.py` collects `data_ptrs`
  across all executable code sections, `LazyInsns._resync` in `tools/lift.py`
  re-decodes missed function entries on demand, and `config/mel-roots.json`
  lists the remaining Bink code-pointer entries.
- **Additional runtime shims**: FreeType `FT_Bitmap_New`/`Convert`/`Done`
  (`runtime/fonts.c`), fortified `__recv_chk`, `__wcscpy_chk`, `__wcsncat_chk`,
  and `_IO_getc`/`_IO_putc`.

## Manual setup

The `config/local.json` layout and the `gameprep` → `build_game` →
`make_app`/`run_game` sequence are in [usage.md](usage.md#manual-build): add
`--game mel` to each command. Mel's keys (`game_root`, `linux_root`,
`system_lib_dir`, `steam_api_dylib`, `steam_appid` 317400, `depot_id` 317404)
go under `games.mel`.

## What was tested

Tested on Apple silicon (macOS, 20 November 2026) with `--release`:

- Main menu (Bink background video, VGUI fonts, localized UI, Steam logon,
  Miles audio) on Metal.
- Maps loaded with `+map` and run without crashing or trapping:
  `sp_a1_tramride`, `sp_a2_underbounce`, `sp_a3_paint_fling`, `sp_a4_overgrown`
  on Metal; `sp_a2_underbounce` on OpenGL (`P2_RENDERER=opengl`).
- Saving and reloading a game (`save` / `load`) in `sp_a2_underbounce` on
  Metal, verified visually with in-game screenshots of the main menu, the
  chamber, and the reloaded save.
- Built and verified `~/Applications/Portal Stories Mel Natif.app` (with icon
  converted from `portal2.ico`).

To jump straight to a chamber from Terminal with Steam running:

```sh
~/Applications/Portal\ Stories\ Mel\ Natif.app/Contents/MacOS/launcher -- -novid +map sp_a2_underbounce
```

## Known limits

- **iOS: phase 1** ([ios.md](ios.md#portal-stories-mel-phase-1),
  [fork/MEL_IOS_PLAN.md](fork/MEL_IOS_PLAN.md)): the app builds for the
  simulator and the device; not played on a device yet.
- **Playthrough coverage**: a full playthrough, later chapters, Steam
  achievements, and extended gameplay performance have not been tested yet.
- **Indirect call roots**: a crash ending with `target not recompiled` means an
  uncompiled indirect call target; please report the end of
  `~/Library/Logs/PortalStoriesMelNatif.log` (or run
  `tools/discover_game.py --game mel`).
