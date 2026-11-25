# Left 4 Dead 2: Mac-first research (round 9)

Written 24 November 2026. **Local and offline play only:** a modified build
never connects to Valve's online servers (VAC). Nothing was built tonight,
because the Linux binaries aren't on this Mac (below).

## Inputs

| What | Depot | On this Mac? |
|---|---|---|
| Game content (Mac install, `steamapps/common/Left 4 Dead 2`, 15 GB) | 551 (+ 552, 555–559 language and DLC content) | **yes** |
| Mac binaries (i386 `.dylib`) | 553 (`macos`) | yes, part of the install |
| **Linux binaries** | **554** (`linux`, 539 MB) | **no** |

The depot numbers come from **Steam's own app info cache on this Mac**
(`appcache/appinfo.vdf`, app 550, each depot's `oslist`), not a guess: 553 is
`macos`, 554 is `linux`. Command for Jacksen (Steam console):

    download_depot 550 554

## Sizing it up (from the Mac edition's i386 binaries)

| | Left 4 Dead 2 | Portal 2 (the pipeline's base) |
|---|---|---|
| Engine branch | `left4dead2` (2009), rebuilt by the 2020 "The Last Stand" update | forked from L4D2's in 2010–2011 |
| Architecture | **i386** (Mac edition; the Linux build has been 32-bit since its 2013 port) | i386 |
| IVEngineClient | `VEngineClient013` | `VEngineClient015` |
| Filesystem | `VFileSystem018` | `VFileSystem017` |
| ICvar, VGUI panel and surface, material system | `VEngineCvar007`, `VGUI_Panel009`, `VGUI_Surface031`, `VMaterialSystem080` | the same |
| Steamworks (engine) | `SteamClient020`, `SteamUser021`, `SteamFriends017`, `SteamUtils010` (a recent SDK, like Portal 2's, not Mel's 1.40) | recent SDK |
| Renderer | Windows: DXVK (`dxvk_d3d9.dll`, `shaderapivk`); Mac: togl (OpenGL) | `shaderapivk` + the runtime's Direct3D 9 → Metal bridge |
| Scripting | VScript (Squirrel, `vscript.dylib`) | VScript (Squirrel) |
| Sound | Miles (`vaudio_miles`, `bin/linux32/*.asi` plugins already in the install) | Miles, same plugins (`runtime/pe.c` translates them) |
| AI director | game code in `server` (`director_*` convars): ordinary lifted code, nothing special for the pipeline | — |

What carries over as is: the lifter, the i386 ABI, Miles' PE plugins, VScript,
the VGUI and cvar layers, the Steam bridge (a recent SDK) and, very likely, the
`shaderapivk` → Metal path (to confirm on depot 554: if its Linux build
renders through togl instead, the OpenGL path works too). What is new: the
interface versions above (`VEngineClient013`), L4D2's own module set and
indirect-call roots, its system libraries (unknown until depot 554 is here),
and four campaigns' worth of content (7.3 GB base + 5.8 GB of DLC).

## Offline only

A profile for L4D2 will start it with **`-insecure`** (the local server
doesn't use VAC), **`+sv_lan 1`** and **`-nomaster`**, and with the Steam
matchmaking lobby's "online" paths out of reach: single-player campaigns
with bots are a local listen server, which L4D2 supports offline ("Play
offline" / `map <campaign map>` with bots). On iOS there is no Steam at all
(the stub). Never `connect` to an internet server with a recompiled build.

## Estimate

Once depot 554 is downloaded:

1. **Mac, menu:** a game profile (`tools/games.py`, `config/l4d2-roots.json`),
   gameprep, the first lift and build: **half a day to a day**, most of it
   new imports and the roots found as the menu starts (Mel took about the same).
2. **Mac, a campaign map with bots, offline:** **1–2 days** (AI and the
   director run a lot of code no Portal map touches: more indirect-call
   roots; infected and particle effects through the renderer).
3. **iOS, after that:** **3–5 days**: its touch layout (shoot, shove, use,
   reload, item slots, crouch), the iOS game profile, and memory (four
   survivors, hordes; the 4 GB guard and the memory log will matter).

Merges only if nothing else in the fork changes: tonight this branch is this
note only.
