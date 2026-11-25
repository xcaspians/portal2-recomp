# Maps and mods on iPhone and iPad (Portal 2)

What this Portal 2 build can load without Steam, and how to get the files
onto the device. Everything here uses **your own files**: maps you made or
downloaded yourself, never Valve's game files copied from someone else.

The app's files are visible in the **Files** app (On My iPhone or On My
iPad > Portal 2) and in the Finder when the device is connected (the device's
Files tab). The game's folder is `game/` in there. Quit the game before
changing anything in it.

## Custom maps

Put single-player `.bsp` maps in **`game/portal2/maps/custom/`** (create the
`custom` folder the first time). Use plain file names, letters, digits, `_`
and `-` only, since the console loads a map by its name.

Then start one:

- **In the game:** pause menu > **Touch Controls** (gear, top right) >
  **Maps & Mods**, tap the map.
- **At launch:** add two lines to `Documents/launch.args` (one argument per
  line), for example `+map`, then `custom/sp_laser_maze`. Remove them
  afterwards, or every launch starts that map.

Workshop maps you have on a Mac: `tools/ios_device.sh workshop push` (below)
copies all of them. There is no Workshop on iPhone (no Steam), so the game's
own Community Test Chambers screen stays empty; Community Maps (below) replaces it.

From the Mac, over the cable:

```sh
tools/ios_device.sh mods push ~/Downloads/sp_laser_maze.bsp      # one map
tools/ios_device.sh mods push ~/Downloads/my_map_pack            # a folder that holds only .bsp maps
```

## Community Maps (Steam Workshop, in the game)

Pause menu > **Touch Controls** (gear) > Maps & Mods > **Community Maps**
browses Portal 2's Steam Workshop chambers and downloads them, without Steam
on the device (`runtime/ios/workshop_ios.m`, `runtime/ios/workshop.c`).

- **Browse** (Popular: most subscribed, Newest, Trending: this week, Search),
  single player by default or co-op: this needs **your own free Steam Web API
  key**. Get it at steamcommunity.com/dev/apikey while signed in to Steam (any
  domain name works, for example `localhost`), copy it, and paste it in the
  key field. It is stored in this device's Keychain and only sent to Steam;
  "API key saved · Change" removes it. Never share it or put it in a file
  you send to someone: Valve's terms make a key personal.
- **Without a key:** paste a Workshop link
  (`https://steamcommunity.com/sharedfiles/filedetails/?id=...`) or the ID
  alone in "Paste a Workshop link or ID". A collection opens as the list of
  its maps.
- **Details** (tap a map): larger preview, description, size, update date,
  and **Download** (with progress and Cancel), then **Play**, **Update** when
  the Workshop has a newer version, and **Delete** (press twice).
- **Co-op maps** are labelled and can't be started alone: host them on a Mac
  (LAN co-op) and join from the device.
- **Downloaded** lists the maps on the device, with an update badge.

Where they go: `Documents/portal2-userdata/portal2/maps/workshop/<id>/<name>.bsp`
(the user folder, which the engine reads before the game's), with
`p2workshop.txt` next to the map (the Workshop's update date for the
badge). They also show up in Maps & Mods. A download is written as
`<name>.bsp.part` and only becomes a map once complete and checked; leftovers
of a canceled or failed download are removed. The folder is kept out of
iCloud backups.

Keyless details and downloads use `ISteamRemoteStorage/GetPublishedFileDetails`
and `GetCollectionDetails`; browsing uses `IPublishedFileService/QueryFiles`;
authors' names come from `ISteamUser/GetPlayerSummaries` (with the key).
Findings and rules: [fork/ROUND3_REPORT.md](fork/ROUND3_REPORT.md).

`P2_COMMUNITY_TEST=<id>` (simulator checks) opens Community Maps on that
chamber at the first pause, downloads it and starts it; `show:<id>` only
opens its details.

### The Mac's Workshop maps, over the cable

The Mac game downloads your subscribed chambers by itself (through Steam) into
`~/Library/Application Support/Portal 2 Natif/userdata/portal2/maps/workshop`.
To put them on the device:

```sh
tools/ios_device.sh workshop push --dry-run   # what would be copied, and the total size
tools/ios_device.sh workshop push             # copy (P2_DEVICE=<UDID> for the iPad)
```

- They land where Community Maps keeps its downloads
  (`Documents/portal2-userdata/portal2/maps/workshop/<Workshop ID>/`), with its
  note (title, update date), so they show as **Downloaded** with working
  update badges. The Mac names its folders by Steam content handle: the tool
  takes each map's Workshop ID from the `thumb<ID>.jpg` next to it and fetches
  its details from the same keyless API as Community Maps
  (`tools/workshop_push.py`, checked by `tests/test_workshop_push.py`).
- A map already on the device with the same size is skipped (its note is
  added if missing); one with another size (another version, downloaded on
  the device) is left alone. **Nothing on the device is ever deleted**, and
  the Mac folder is left as is.
- Before copying, the tool launches the app for a second with
  `P2_DEVICE_REPORT=1`: it writes `Documents/device-report.txt` (model,
  memory, free space) and quits without starting the game, since devicectl
  only knows the total capacity. If the maps plus 1 GB to spare don't fit, it
  stops with a plain message. `tools/ios_device.sh space` prints that report.

## Addon folders

Portal 2 mounts numbered folders next to its own: `portal2_dlc1` and
`portal2_dlc2` are the game's, and the engine looks for the next numbers
itself. A folder **`game/portal2_dlc3/`** with the same layout as the game
(`materials/`, `models/`, `sound/`, `particles/`, `maps/`, `cfg/`...) is
expected to load on top of the base game. A second addon is `portal2_dlc4`,
and so on: **numbers with no gaps**, since the engine may stop at the first
missing one.

**Not yet confirmed on a device** (it is on the test list): whether a
`portal2_dlc3` folder mounts on iOS as it does on PC.

```sh
tools/ios_device.sh mods push ~/Downloads/portal2_dlc3                   # the folder's own name
tools/ios_device.sh mods push ~/Downloads/"Cool Gun Skin" portal2_dlc3   # any folder, given a number
```

`mods push` refuses `portal2_dlc1` and `portal2_dlc2` (the game's own),
names that are not `portal2_dlcN`, and map names the console can't type
(`tools/mods_dest.py`, checked by `tests/test_mods_dest.py`).

What an addon can change: textures, models, sounds and particles (for
example the portal gun's look, menu backgrounds, loading screens), and
config files. An addon that replaces a file the game updates later may need
updating too. A broken model can crash the game: remove the folder to go
back.

## Console tweaks

Add them to `Documents/launch.args` as two lines each (`+name`, then the
value), or put them in an addon's `cfg/autoexec.cfg`. Variables found in the
game's code, not all tried on iOS yet:

| Variable | What it does |
|---|---|
| `cl_crosshairscale` | crosshair size |
| `cl_crosshaircolor` | crosshair colour |
| `cl_fov` | field of view |
| `cl_viewmodelfov` | field of view of the portal gun |
| `cl_colorblind` | colour-blind assistance |

## Not possible yet

- **Sourcemods with `-game`** (a mod folder instead of `portal2`): the app
  passes `-game portal2` itself, and a `-game` in `launch.args` most likely
  loses.
- **Portal Stories: Mel** on iOS: it has its own engine build (Mac only so
  far, upstream PR #52).
- **Browsing the Workshop without a key**: only reading Valve's web page,
  which can break without warning, so it isn't offered.
- **Co-op robot skins and gestures**: they come from Steam's item inventory,
  which iOS doesn't have.
