# Fork rounds 6 to 10 report (night of 24–25 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp).
Everything below is merged into the fork's `main` and pushed. Nothing went
upstream, and `Prompts/` was never committed.

`make check` on `main`: **308 C checks and 264 Python tests pass** (297
and 256 at the start of the night).

**Devices at the end of the queue:**

- **iPad mini:** reachable over USB and unlocked, so it got the install
  step: saves backed up first, the new Portal app, and Mel's data. See
  "On your iPad mini" below.
- **iPhone:** reachable over Wi-Fi but locked all night, so its steps wait
  for you.
- **iPad Pro:** never touched.

## In short

| Round | Status |
|---|---|
| 6: one Portal app | **Shipped.** Portal 2 and Mel in one app with a picker and Manage games; the installer's Devices section. Installed on the iPad mini with Mel's data. |
| 7: controls and Community | **Shipped.** Back-tap calibration with a wrong-side guard, a Back taps switch, the layout editor, the gear on the main menu, the game's own Community button. |
| 8: Half-Life 2 | **Code shipped, not built.** HL2's Linux binaries aren't on this Mac (depot 224). |
| 9: Left 4 Dead 2 | **Research only.** Its Linux depot (554) isn't on this Mac. |
| 10: P2CE / Revolution | **Research and a first attempt.** The pipeline stops at its first 64-bit check, as expected. |

---

## Round 6: one Portal app, a game manager, Download from the Mac

### What shipped

- **One app, two games.** Portal 2 and Mel now live in the Portal 2 app,
  with its identifier, its profile and its **increased-memory entitlement,
  which Mel now gets too**. Half-Life 2 and Portal stay separate apps.
- **How one binary holds both** (the choice the spec asked me to explain):
  - Each game's recompiled code names its functions after their original
    addresses, and both games load from the same addresses, so the names
    collide.
  - Mel's code is therefore linked into a single block that keeps all its
    ~135,000 functions private and exposes only six entry points.
  - The shared runtime (built once) switches to the chosen game's entry
    points at launch.
  - The Mac build and Portal 2-only builds are untouched; that switch only
    exists in this app.
- **App size:** one 697 MB executable on the device (686 MB in the
  simulator), against 315 MB for Portal 2 alone. Only one game runs per
  launch, so memory use is that game's.
- **Game picker** at launch, as each game's cover (made from its own icon
  on your Mac), title and size, or "Not installed".
  - **Remember my choice** skips the picker next time.
  - **Switch game** at the bottom of the settings menu brings it back.
    Press it twice: the app closes, and the picker shows when you reopen it.
- **Manage games**, inside the picker. For each game it shows the data size,
  the saves size and when you last played it.
  - **Remove game data** deletes the game files only, never saves or
    settings. It asks first, saying how much space it frees.
  - **Get it from your Mac** says how in one line.
- **Each game keeps its own things.**
  - Portal 2's data, saves, settings and `launch.args` stay exactly where
    they were.
  - Mel gets `Documents/mel` for its data and `launch.args`,
    `Documents/mel-userdata` for its saves, and its own settings.
  - iCloud sync covers Mel in its own folder.
  - Community Maps and Maps & Mods stay Portal 2 only.
  - The version line names the game.
- **Natif Installer > Devices** (step 4 of the installer window):
  - It lists the iPhones and iPads plugged in or paired on the Wi-Fi, with
    free space and the games already on each.
  - **Download** puts on Portal 2, Mel or your Mac's Workshop maps, with
    progress and Cancel. It installs the app first when needed and checks
    free space before any copy.
  - Every error is a plain sentence: locked, not trusted, Developer Mode
    off, not reachable, not enough space, not built on this Mac.
  - Underneath it runs the same `tools/ios_device.sh` commands.
- **New commands:**
  - `tools/ios_device.sh mel data push` now copies Mel into the Portal app.
  - `tools/ios_device.sh mel saves migrate` copies Mel's saves from the old
    standalone Mel app through the Mac, and checks every file arrived.

### On your iPad mini (end of the queue)

- **Backed up first** (`build/ios/device/ipad-mini-backup-20261124-2146`):
  - Portal 2's user folder: 4 saves, settings and the Workshop maps;
  - the old Mel app's folder: settings only.
- **Installed** the Portal app (`0.4.0-fork.2 (ba46d72)`). Portal 2's 4
  saves were all still there afterwards.
- **Mel's data** (9.3 GB, 39 copies, no errors) went into the Portal app's
  `Documents/mel`. The iPad has 112 GB free.
- **Both games start from the one app:**
  - Portal 2 kept its settings ("0 réglage(s) posé(s)") and its own folders;
  - Mel got its own first-launch defaults and its own folders.
- **Not seen yet:** both then paused in the background before drawing a
  frame, because the iPad's screen was asleep (it was the middle of the
  night). So actually playing them from the new app is the first item on
  the morning checklist. Earlier tonight, on the same iPad, standalone Mel
  ran its menu and first chamber at 60 fps.
- **iCloud:** "conteneur indisponible", as expected until the setup's last
  step (Needs Caspian 1).

### Mel's saves moved over?

The old Mel app on your iPad holds **no Mel saves**, only settings from my
test runs, so the migration correctly said "nothing to move". Keep the old
app until you've played Mel from the new one; then remove it (Needs Caspian).

---

## Round 7: back-tap calibration, the layout editor, the Community button

### Back-tap calibration (plain words)

In the settings menu, under Back taps:

- **Back taps** switches them on or off, in sync with the Settings app.
  When they're off, the portal buttons show.
- **Calibrate back taps** walks you through tapping the back on the left 8
  times, on the right 8 times, then holding still for 5 seconds. Each tap
  shows as it lands.

From those taps, the app works out three things for your device:

1. **How hard a tap must be:** 60 % of your softest taps, never below 3× the
   shaking measured while you held still, never harder than today's defaults.
2. **Which way the device turns when you tap each side.** It picks the
   blend of the two rotations that puts the most of your taps on their side.
3. **A wrong-side guard.** A tap too close to the line between left and
   right fires nothing, since a missed tap beats the wrong portal. The line
   sits just above the worst wrong-side tap you made while calibrating, as
   long as at least 3 good taps in 4 stay clear of it.

After calibrating:

- it shows how many of your taps the defaults got right, against the new
  values;
- **Save and test** opens a counter of what the game would fire, left and
  right;
- the result is stored for your device model ("Calibrated on …"), with
  **Re-calibrate** and **Reset to defaults**.

**Before and after numbers:** no recordings from your devices were on the
Mac, so these come from simulated taps shaped like the iPad mini's problem
(softer taps, a twisting motion that pulls the side the wrong way). On
those, the defaults caught under half and the calibrated values caught all
of them on the right side, none wrong. Played through the real detector,
soft taps went from **0 of 8 to 8 of 8**. The real test is yours: calibrate
on the iPad mini.

### Button layout editor

**Edit button layout**, in the settings menu:

- the game's buttons show where they are;
- drag one to move it, or its white dot to resize it;
- the sticks' areas show as dashed circles;
- Reset, Cancel and Save.

Buttons stay whole on screen, snap to a grid and are pushed out of the
sticks' areas. Layouts are saved separately for iPhone and iPad, and per
game. The opacity and size sliders still apply on top.

### The gear on the main menu (your addition)

The settings gear now shows on the **main menu** as well as the pause menu,
for Portal 2 and Mel. Every item works from there: settings apply at once,
maps and Community Maps start from there, and calibration and the editor
need no level. So no item needed a "start a level first" note.

### The game's own Community button

On iOS, **Community Test Chambers → Play Single Player Chambers** used to
open an empty Steam dialog.

- **What it does now:** the app spots that dialog by reading the game's
  menu screens, closes it the way a player would (Escape), and opens our
  Community Maps instead.
- **Back** returns to the game's Perpetual Testing screen, whose own Back
  reaches the main menu, so there's no dead end.
- **Nothing faked:** nothing in the game is changed and no Steam interface
  is faked. The Mac build is untouched.

---

## Round 8: Half-Life 2

- **Not built or run:** HL2's Linux binaries aren't on this Mac. Your
  depot-221 download failed twice ("No connection"), and 221 is game
  content anyway, which the Mac install already has. The binaries are
  **depot 224** (Needs Caspian).
- **What I did without them:** the fork's features are now in HL2's iOS code
  (built and checked on the Mac, compiled for iOS):
  - **the settings gear and menu**, in HL2's main and pause menus. Its engine
    is the newer Source 2013, whose layout I read from your Mac copy of
    HL2. The same method reproduces Portal 2's known values, and the app
    turns the gear off rather than guess if anything doesn't match.
  - **HL2's own controls:** no back-tap or portal items, gyro on by default.
  - **the version line** naming the game, **the older-phone defaults**, and
    **Edit button layout**.
  - **iCloud saves** in HL2's own folder, once its app gets the iCloud
    setup. That needs its own App ID, a permanent step, so it's yours.
- HL2 stays its own app.
- **What's rough, from upstream's notes:** one short session on an iPhone so
  far; sound, memory and long play untested; the ammo readout sits partly
  under the crouch and use buttons.
- **Realistic list for a full playthrough on the iPad mini:**
  1. Download depot 224, then build and install (half a day with you).
  2. Check the gear, menu and layout editor live, and move the ammo readout
     clear of the buttons (with the editor).
  3. Play through, fixing what comes up: missing call targets, memory in
     the big outdoor maps, and saves across chapter changes (2–4 days spread
     over play).
  4. Then the episodes.

---

## Round 9: Left 4 Dead 2 (local and offline only)

- **Its Linux binaries are depot 554.** I didn't guess: Steam's own app-info
  cache on your Mac lists 554 as `linux` (539 MB) and 553 as `macos`. It's
  not downloaded (Needs Caspian).
- **Sized up from your Mac copy** (details in `docs/fork/L4D2_RESEARCH.md`):
  - It's **i386, like Portal 2**, and its engine is the branch Portal 2 grew
    from, so most interfaces are the same versions. The differences are the
    engine-client and filesystem versions.
  - It has a recent Steam SDK, VScript and Miles audio, as Portal 2 does.
  - It renders through DXVK on Windows, so its Linux build very likely uses
    the same Vulkan path that Portal 2's Metal bridge already handles.
- **Offline only:** it will start with `-insecure`, `+sv_lan 1` and
  `-nomaster`, and never connect to Valve's servers.
- **Estimate once 554 is here:**
  - Mac, main menu: half a day to a day.
  - Mac, a campaign map with bots: 1–2 days more.
  - iOS: 3–5 days more.
- Nothing else in the fork changed for it.

---

## Round 10: P2CE and Portal: Revolution

- **P2CE** (Strata Source), sized up from its Linux build (details in
  `docs/fork/P2CE_RESEARCH.md`):
  - **64-bit x86-64** throughout, where the pipeline only knows 32-bit;
  - **Direct3D 11** through DXVK, where the fork's Metal bridge only speaks
    Direct3D 9;
  - its menus are **Panorama, running JavaScript on V8**. V8's JIT can't be
    recompiled ahead of time, so it would have to run interpreter-only;
  - plus Jolt physics, Steam Audio, ffmpeg and Pango.
- **First attempt:** the pipeline's first step stops at once with "ELF32
  attendu" (expected 32-bit). Every later stage assumes 32-bit x86 too.
- **Size:** about **10–15 rounds** to a Mac main menu (a new 64-bit lifter
  and runtime, D3D11 through DXVK and MoltenVK, V8 without a JIT), then iOS.
  Worth doing only as its own project.
- **Revolution:** its content depot has **no binaries**. Steam lists no
  Linux depot for it, only an untagged 601363 (263 MB), which should hold
  its engine, so comparing it with P2CE needs that download.
- **Backlog:** written to `Prompts/backlog.md` (kept out of git), marked DO
  NOT BUILD YET: the drop-in JIT kit, Half-Life 1 via Xash3D, and Aperture
  Tag and other mods.

---

## Bugs found tonight, and their causes

1. **Mel's iCloud switch was hidden in its own menu.** Round 6 gated the
   iCloud row to Portal 2 by name. Found while bringing the menu to HL2
   (round 8); the row now follows the game's iCloud support.
2. **The iCloud one-time setup can't finish from the command line.** The
   signing project didn't mark the iCloud capability, so automatic signing
   ignored it. Marked now: Xcode turned iCloud on for the identifier, but
   still attached no container (the profile came back with an empty
   container list). Ticking the container in Xcode is the documented step 3.
3. **The menu's test hook would have opened on the start-up main menu**
   once the gear showed there, which would have changed what the older
   simulator checks see. It now opens only where each check expects
   (`=main` for the main menu).
4. **A test of mine had too tight a float tolerance:** the stored button
   size is a `float` (1.1f), compared against a `double` 1.1. The code was
   right; the check now allows 1e-3.

## Needs Caspian

1. **iCloud saves, the last step** (you approved the container):
   - open `ios/signing/P2Signing.xcodeproj` in Xcode;
   - select the Portal2 target, then Signing & Capabilities;
   - under iCloud, tick **iCloud Documents**, press **+** and create the
     container `iCloud.local.portal2recomp.vn8jwve7jx`, then tick it;
   - press ⌘B;
   - tell me, and I rebuild and install on both devices (or run
     `python3 tools/build_ios.py --target device --release --deps deps/ios-deps/device`,
     then `P2_DEVICE=<UDID> tools/ios_device.sh install`).

   Until then the app says "iCloud isn't available", and nothing changes.
2. **iPhone:** unlock it (it was locked all night). Then:
   - back up its saves;
   - install the Portal app;
   - Workshop push;
   - Mel data push into the Portal app (`tools/ios_device.sh mel data push`,
     9.3 GB; check free space first).

   Or use the installer's Devices section.
3. **Remove the old standalone Mel app** from the iPad, once you've opened
   Mel from the Portal app and you're happy (it holds no saves).
4. **Depot downloads** (Steam console):
   - `download_depot 220 224` (Half-Life 2 Linux binaries)
   - `download_depot 550 554` (Left 4 Dead 2 Linux binaries)
   - `download_depot 601360 601363` (Portal: Revolution binaries)
   - `download_depot 440000 440003` (P2CE; needed to run it, not to study it)
5. **Half-Life 2's own App ID and iCloud container** (permanent), when you
   want HL2 on a device with iCloud.
6. **Left from earlier rounds:**
   - a Steam Web API key for Community Maps;
   - the controller test and the upstream PR OK;
   - the 120 Hz measurement;
   - the `portal2_dlc3` addon test.

## Morning hand-test checklist (most fun first)

1. **Mel from the Portal app on the iPad:** open it; the picker shows both
   covers. Tap Mel: the menu, then New Game (the tram ride).
2. **Calibrate back taps** on the iPad mini (pause or main menu > gear >
   Back taps > Calibrate). Then Save and test, then play a Portal 2 chamber:
   are portals reliable now, and never the wrong colour?
3. **Edit button layout:** move the jump button somewhere comfy, Save, play.
4. **The game's own Community button:** Community Test Chambers > Play
   Single Player Chambers opens Community Maps. Back returns to the game's
   screen, whose Back reaches the main menu.
5. **The gear on the main menu:** it's there before you start a level, and
   everything in it works.
6. **Manage games:** the sizes show. Don't remove anything you want to keep.
7. **Remember my choice** on, reopen (no picker), then Switch game (twice):
   the picker comes back.
8. **Natif Installer > Devices** on the Mac: your devices with their free
   space; try Download for the Workshop maps on the iPhone.
9. **Portal 2's saves:** all four are still in Load on the iPad.
