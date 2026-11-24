# Fork round 2 report (24 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp),
`main` at the merge of `feat/ios-icloud-saves`. Nothing was pushed upstream
and the upstream PR (E) is still not opened. Your iPhone and iPad mini were
offline the whole round, so everything was built and checked on the Mac and
in the iOS Simulator. The device build is ready
(`build/ios/device/Portal2.app`, version `0.4.0-fork.2 (65d8395)`) and goes
on both devices when you're back.

`make check`: 278 C checks and 236 Python tests pass. The device and
simulator apps both compile with no warnings.

## What shipped (all merged into the fork's `main`)

### 1. In-game settings menu

- **Where:** a "Touch Controls" gear at the top right, **only while the
  game's pause menu is open**. It never shows in gameplay or the main menu,
  and there's no gear on the touch overlay.
  - The runtime asks the engine itself whether a level is loaded. The engine
    calls it uses were found in the symbols of your Mac copy of Portal 2,
    which uses the same interface version as the Linux build.
  - The app checks those calls while running. If they ever misbehave, the
    gear stays off rather than guessing.
- **Panel:** dark and translucent, drawn over the game. It holds:
  - button opacity (0–100 %)
  - button size (75–150 %; every button stays on screen, checked on three
    screen sizes)
  - show portal buttons
  - tap sensitivity (a 0.5–3 slider, default 1)
  - swap left and right, vibration
  - camera speed, gyro on/off and speed
  - frames per second, including Auto
  - iCloud saves, with "Last synced" under it
  - Maps & Mods
  - Reset to defaults (press twice)
  - the version at the bottom
- **Live:** every change applies at once. While the panel is open the touch
  buttons are drawn behind it, so you see opacity and size change as you
  drag. The one exception is frames per second with the Metal renderer:
  it applies on the next launch, and the panel says so under that item.
- **Kept in sync with the Settings app:** both read and write the app's
  preferences, which are the single source of truth (documented in
  `docs/ios.md`). The Settings app page got the same new sliders.
- **Controller:**
  - the controller's options button (View/Select/−) opens and closes the
    panel; its name shows under the gear;
  - d-pad or stick moves between items and changes values;
  - A flips a switch or presses a button, B closes;
  - the game gets no controller presses while the panel is open.
- **Portal 2 only.** Half-Life 2 and Portal use other engine interface
  versions, so the gear doesn't appear there yet.
- **Checked in the simulator:** the engine reported the level, the pause
  menu opened, and the panel drew correctly over the game with the buttons
  behind it.

### 2. iCloud saves

Built as approved:
- Saves and their screenshots go through the app's iCloud Drive container.
- It syncs at launch, a few seconds after a save, on leaving the app and on
  coming back.
- **A conflict keeps both copies.** The other device's copy gets a new name
  with its device name and the time, and the newer one shows first in the
  game.
- **Deleting a save never deletes it elsewhere.** A save about to be replaced
  is backed up first (kept two weeks).
- The Mac is left out.
- On by default, with the switch and "Last synced" in the menu and in the
  Settings app.

Checks cover the rules: new files, one-way changes, conflicts, deletions,
file names, and which files move.

**It needs one setup step from you** (see "Needs Jacksen"). Turning iCloud on
creates an iCloud container on your developer account, and Apple never
deletes those, so I didn't do it unattended. Until then the app runs exactly
as before and says iCloud isn't available. That was checked in the simulator.

### 3. Mods, the cheap items

- **`docs/mods.md`:** custom maps through the Files app or `launch.args`,
  numbered addon folders (`portal2_dlc3` and up, no gaps), console tweaks.
  Linked from `docs/ios.md` and the README.
- **`tools/ios_device.sh mods push <map.bsp | folder> [portal2_dlcN]`:**
  copies a map or an addon over the cable. Maps go to
  `game/portal2/maps/custom`, addons to `game/portal2_dlcN`. It refuses the
  game's own folders, odd names and map names the console can't type. Tests
  cover the path handling.
- **Maps & Mods in the menu:**
  - It lists custom maps from `maps/custom`, `maps/workshop` and addon
    folders, and starts one with a tap.
  - File names are checked, so a name can never sneak a second console
    command in.
  - **Checked in the simulator:** a map placed only in a `portal2_dlc3`
    folder was listed, started and loaded. That's good evidence that addon
    folders mount, but it's still on your device test list.

## Bugs found and their causes

1. **The iOS simulator build stopped at "gl4es modified and different from
   the patch".**
   - The build script compared the patched gl4es source with its patch file
     letter for letter. Those include short commit codes, which git writes
     7 or 8 characters long depending on the machine.
   - So building a second target (simulator after device) always failed.
   - **Fix:** the codes are ignored, and any real difference is still
     refused. Covered by a test.
2. **Following the manual iOS build steps in `docs/ios.md` put the libraries
   in the wrong folder.**
   - With a relative `P2_IOS_DEPS=deps/ios-deps`, the script moved into that
     folder first, then used the same relative path again. The libraries
     ended up in `deps/ios-deps/deps/ios-deps/…`, and the build then couldn't
     find `SDL.h`.
   - `setup.sh` passes a full path, so the one-command install was never
     affected.
   - **Fix:** the path is made absolute first. Covered by a test.

## What turned out to need you

- **The iCloud setup step.** It's permanent on your Apple account, so I left
  it for you.
- **The tap sensitivity default is still 1.** Calibrating it needs your
  recording of light taps.

## Needs Jacksen

1. **Plug in and unlock** your iPhone and iPad mini. I then install this
   round's build on both:
   `P2_DEVICE=<UDID> tools/ios_device.sh install`. Your game data stays.
2. **Controller test** of the overlay fix from round 1 (checklist below).
3. **Back-tap recording with light taps:**
   - turn on Settings > Portal 2 > Record taps;
   - play a few minutes, tapping lightly on both sides;
   - tell me, and I pull the file and set the default for the new 0.5–3
     slider.
4. **120 Hz measurement** on both devices, quiet and busy chambers.
   - I run it with `tools/ios_bench.py` once they're connected.
   - That decides whether "Auto" becomes the default.
5. **Addon folder test:** does a `portal2_dlc3` folder mount on the device?
   It did in the simulator. Try
   `tools/ios_device.sh mods push <map.bsp or folder> portal2_dlc3`, then
   Maps & Mods.
6. **iCloud setup, once** (the exact steps are in `docs/ios.md`, "iCloud
   saves"):
   1. add `"ios_icloud": true` to `config/local.json`;
   2. with the iPhone connected and unlocked, run
      `./setup.sh --ios --game portal2`;
   3. if Xcode reports a capability error: open
      `ios/signing/P2Signing.xcodeproj`, go to Portal2 target > Signing &
      Capabilities, check iCloud > iCloud Documents with the container
      `iCloud.local.portal2recomp.vn8jwve7jx` ticked, press ⌘B, and run
      step 2 again;
   4. install on the iPad mini too.
7. **OK on E:** opening the upstream PR for the overlay fix, after the
   controller test. The PR text is in the round 1 report.

## Hand-test checklist (iPhone and iPad mini)

**Menu**
- [ ] In a chamber, open the pause menu: the "Touch Controls" gear is at the
  top right. Resume: it's gone. Main menu: no gear.
- [ ] Open it. Drag Button opacity and Button size: the buttons behind the
  panel change while you drag. Resume: the overlay has the new look.
- [ ] Change tap sensitivity, swap left/right, vibration, camera speed and
  gyro, then resume. Each takes effect without relaunching.
- [ ] Frames per second: with Metal, the panel says "Applies on next
  launch"; after a relaunch the counter follows it.
- [ ] Quit the app completely and reopen it: every value is kept. The
  Settings app page shows the same values.
- [ ] Change something in the Settings app, come back to the game: it
  applied.
- [ ] Controller: pause, press View/Select to open, then use the d-pad to
  move, left/right to change, A on switches and buttons, B to close. The
  game's pause menu doesn't react behind it.
- [ ] Reset to defaults (press twice): everything goes back to default.
- [ ] The version line at the bottom reads `0.4.0-fork.2 (…)`.

**iCloud** (after the setup step)
- [ ] Save on the iPhone, wait ten seconds, then open the game on the iPad
  mini: the save is in Load.
- [ ] The menu shows "Last synced …" on both devices.
- [ ] Forced conflict: in airplane mode on both, save "quick" on each, turn
  airplane mode off, then open each game. Both copies are in Load, one with
  the other device's name and the time, the newer first.
- [ ] Delete a save on one device: it stays on the other.

**Maps & Mods**
- [ ] `tools/ios_device.sh mods push some_map.bsp`, then pause > gear > Maps
  & Mods: it's listed, and a tap loads it.
- [ ] With nothing installed, the list says where to put maps.
- [ ] An addon folder pushed as `portal2_dlc3` is listed under "Addon
  folders", and its maps start.

**Round 1 leftovers**
- [ ] The controller overlay test (touch shows the controls, any button hides
  them; "Off with a controller").
- [ ] 120 Hz: set Frames per second to Auto, relaunch, and check the counter
  in a quiet and a busy chamber.
