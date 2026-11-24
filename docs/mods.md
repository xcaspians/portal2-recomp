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

Workshop maps you have on a Mac: copy the `.bsp` from the Mac's Portal 2
folder (`portal2/maps/workshop/<number>/`) into `game/portal2/maps/custom/`.
There is no Workshop on iPhone (no Steam), so the game's Community Test
Chambers screen stays empty.

From the Mac, over the cable:

```sh
tools/ios_device.sh mods push ~/Downloads/sp_laser_maze.bsp      # one map
tools/ios_device.sh mods push ~/Downloads/my_map_pack            # a folder that holds only .bsp maps
```

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
- **Co-op robot skins and gestures**: they come from Steam's item inventory,
  which iOS doesn't have.
