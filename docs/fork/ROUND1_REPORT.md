# Fork round 1 report (24 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp).
`origin` still points at upstream (pushes to it are blocked locally), the
fork is the `fork` remote. Nothing was pushed upstream.

## What shipped (merged into the fork's `main`)

| Item | Branch | What it does |
|---|---|---|
| Overlay fix | `fix/ios-touch-overlay-rehide` | The touch controls hide again as soon as a controller button or stick is used, and come back only on a touch. New choice "Off with a controller" in Settings > On-screen controls. Check: `ios_touch_overlay_hides_on_controller_input`. |
| B2: 120 Hz "Auto" | `feat/ios-fps-auto` | New Frames per second choice "Auto (screen's max)": 120 on a ProMotion screen, 60 otherwise, still lowered by auto power saving. Check: `ios_fps_auto_follows_the_screen`. **60 stays the default** for now: the rule was to make Auto the default only if both test devices hold above ~90 fps in a busy chamber, and they haven't been measured yet (see B1). |

`make check` passes (266 C checks, 217 Python tests). Both changes were
compiled into the iOS app (`tools/build_ios.py --release`).

## Part A: in-game Touch Controls page

**A1 (investigated, waiting for your OK).** A real new tab in Portal 2's own
Options menu **can't be done cleanly**:

- The options screens are C++ classes inside the game's `client.so`. Their
  layout files (`.res`, inside Valve's VPK archives) can only arrange
  controls that the C++ already knows. A new button can't open a new screen:
  its command would be ignored.
- The only workable trick: rewrite the Keyboard/Mouse screen's layout file on
  the device and relabel it "Touch Controls", with sliders bound to new
  console variables the runtime registers. Toggles would have to be 0–1
  sliders, a Reset button isn't possible, and with the touch overlay acting
  as a gamepad the game may show its controller menu, where that entry
  doesn't exist. It also depends on Valve's control names in data Steam can
  update. About 3–5 days, fragile.
- A brand new game panel (registering a VGUI class) would take weeks and
  break easily.

**Recommendation:** the separate menu you already chose as the fallback: a
panel drawn by the runtime over the game (dark, translucent), opened from a
gear next to the overlay's pause button and from the pause menu, working by
touch and with a controller. It doesn't depend on Valve's files, and it can
have real switches, a Reset button and the version number. About 1–2 days
for the panel, plus 1–2 days so every setting applies live (today they are
read once at launch). **A2 is not built yet: say OK and it's next.**

## Part B: 120 Hz

- **B1, not measured.** Your iPhone and iPad mini (A20 Pro) were
  "unavailable" (locked or not connected) during this round, so there are no
  numbers yet. The iPad Pro was reachable but isn't one of the named test
  devices.
- **B2, shipped** (above), with 60 kept as the default until B1 is done.

## Part C: iCloud save sync (proposal, waiting for your OK)

- **Where:** an iCloud Drive app container (`iCloud.<app id>`), not the
  key-value store. The key-value store holds 1 MB in total, and Portal 2
  saves are several MB each.
- **What:** the same files the cable sync already moves (the `.sav` files and
  their screenshots in `portal2/SAVE/<id>/`), never settings or video config.
- **When:** at launch, before the game reads its saves (fetch newer ones);
  a few seconds after the game writes a save (upload, so a burst of
  autosaves becomes one upload); when the app goes to the background.
- **Conflicts:** the same rules as `tools/save_sync.py`, which already never
  silently overwrites. If both devices changed the same save since the last
  sync, both are kept: the other device's copy gets a new name (for example
  `quick (iPad mini 24 Nov 14-05).sav`), so both appear in the game's Load
  list. The game sorts by date, so the newer one shows first. Deletions are
  never synced.
- **Mac:** left out. The Mac app isn't provisioned for iCloud, and it goes
  through real Steam, which may already sync its saves with Steam Cloud. The
  cable sync (`tools/ios_device.sh saves`) stays the Mac ↔ iPhone path.
- **Setup:** iCloud capability on your paid team's app ID, through the same
  signing project, with Xcode creating the profile.
- **Settings:** on by default, with an "iCloud saves" switch and a "last
  synced" time on the in-game page (A2). Until A2 exists, the switch would sit
  in the Settings app.
- **Effort:** about 2–3 days, plus testing on both devices.

## Part D: mods and cosmetics (research only)

- **Custom maps: work today, no code.** Put a `.bsp` in a `maps` folder with
  the Files app, and load it with `+map <name>` in `Documents/launch.args`.
  Files app access is already on (`UIFileSharingEnabled`). There's no
  Workshop on iOS (no Steam), so the community chambers menu stays empty.
- **Addon folders: very likely work.** The engine itself looks for numbered
  `portal2_dlcN` folders, so a `Documents/game/portal2_dlc3/` with
  replacement content should mount over the base game. Not tested on a
  device yet; keep the numbers without gaps.
- **Content-only sourcemods:** need a small change, because the app forces
  `-game portal2` at build time and a `-game` in `Documents/launch.args`
  probably loses.
- **Portal Stories: Mel on iOS:** large (1–2+ weeks): its own game profile,
  its own C++ runtime libraries packed into the app, its own 8.6 GB of data
  (it can't share Portal 2's), its own app ID. Better after Mel is stable on
  Mac (PR #52 upstream isn't merged and has no full playthrough yet). The
  `portal2-recomp-mel` worktree was checked: it is healthy (`git worktree
  prune` found nothing stale), so it was left in place rather than deleted
  and re-added, which would have thrown away its 1.5 GB Mel build.
- **Cosmetics:** co-op robot skins and gestures come from Steam's item
  inventory, which iOS doesn't have, and faking it is out of scope. What's
  realistic: replacement models, textures and particles for the portal gun,
  HUD, loading screens and menu backgrounds in an addon folder (your own
  files only, never Valve's redistributed), plus crosshair, FOV and
  colorblind options through console variables.

Ranked by effort:

| Option | Effort | Kind |
|---|---|---|
| Document custom maps, addon folders and console tweaks | ~2 h | docs |
| Test that `portal2_dlc3` mounts on the device | ~1 h | test |
| `tools/ios_device.sh mods push <folder>` | 2–3 h | code |
| Let `Documents/launch.args` choose `-game` | 0.5–1 day | code |
| In-app list of maps and mods to start | 2–4 days | code (fits the A2 panel) |
| Mel on iOS | 1–2+ weeks, after Mac is stable | code |
| Steam inventory skins | not realistic | — |

## Part E: pull request to upstream (waiting for your OK)

`fix/ios-touch-overlay-rehide` is pushed to the fork. The PR text against
`kanapitsas/portal2-recomp` (now named `source-recomp`) is in the session
summary. It is not opened. It says the hand test with a controller is still
pending, so ideally you do that first.

## Bugs found and their causes

- **Touch controls didn't hide again** (fixed earlier, now on the fork's
  `main`): the game only hid them when a controller connected or
  disconnected, and never watched for controller buttons or sticks being
  used.
- No new bugs this round.

## Needs Jacksen

1. **OK on A1:** build the separate in-game menu (A2)?
2. **OK on C1:** build iCloud save sync as proposed?
3. **OK on D:** which mod items, if any, to build?
4. **OK on E:** open the upstream PR (after your controller hand test)?
5. **Devices:** connect and unlock your iPhone and iPad mini, so I can
   install this round's build and measure 120 Hz (B1).
6. **Back-tap recording** (for A2's sensitivity slider): turn on Settings >
   Portal 2 > Record taps, play a few minutes doing light taps on both
   sides, then tell me. I pull the file over the cable.

## Hand-test checklist (iPhone and iPad mini, once this round's build is installed)

- [ ] Controller connected: the touch controls are hidden. Touch the screen:
  they appear. Press a button or move a stick: they hide.
- [ ] Settings > Portal 2 > On-screen controls > "Off with a controller",
  relaunch: with a controller connected, touches show and do nothing; unplug
  it and they come back.
- [ ] Settings > Portal 2 > Frames per second > "Auto (screen's max)",
  relaunch: the FPS counter (top left) reaches about 120 on these ProMotion
  screens. In Low Power Mode it drops to 30.

The in-game menu checks (sliders live, settings kept after relaunch, light
back taps at the default, controller in the menu) come with A2.
