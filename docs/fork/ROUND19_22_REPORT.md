# Fork rounds 19–22: Sauce, the TV picture, Half-Life 2's episodes, Apple TV research

25 November 2026. Rounds 19 to 22 back to back, after the L4D2 fix, the
history scrub and Portal (their own report: `L4D2_FIX_SCRUB_PORTAL1_REPORT.md`).
Tested on the iPad Pro (M5); the iPad mini got installs only.

## Round 19: Sauce, one app for every game — shipped (PR #6)

**What shipped**
- **The name:** the Portal app is now **Sauce** on the home screen. The name
  lives in one place (`APP_NAME` in `tools/games.py`, or `"ios_app_name"` in
  `config/local.json`), so "Saurce" is a one-line change. A test keeps Valve
  trademarks out of it.
- **The icon** is our own art (a white drop on a warm gradient), drawn by
  `tools/make_app_icon.swift`: no Valve image or logo. Game covers inside the
  picker keep the games' own icons.
- **The version line** in the settings menu reads "Sauce 0.5.0-fork.1
  (commit) · <game>".
- **Same app underneath:** same bundle ID, provisioning profile and
  increased-memory entitlement, so saves, settings and iCloud carried over.
- **Every game in the picker:** Portal 2, Portal Stories: Mel, Portal,
  Half-Life 2, Left 4 Dead 2 (and, after round 21, Episode One, Episode Two
  and Lost Coast). Each keeps its own data folder (`Documents/<game>`), saves
  folder, launch arguments and iCloud folder. L4D2's offline arguments are
  untouched (`-insecure +sv_lan 1 -nomaster`).
- **Shared settings** cover every game (round 11's model). Buttons only one
  game has keep their own positions.
- **Installer and setup:** `./setup.sh --ios` builds Sauce once and copies
  each selected game into it. The Natif Installer's Devices section downloads
  every game into Sauce.
- **Manage games** already listed size, saves size and last played; Portal 2
  stays non-removable, the others removable (the episodes aren't, see round 21).

**One binary or several?** One. Each game's generated code is linked into the
single executable, as Mel's was in round 6, and the runtime picks a game's
code at launch. iOS runs one executable per app, so this is the only way that
keeps one app ID and one memory entitlement. **Size:** the executable is about
1.7 GB, the app 1.6 GB on disk. All the games' data stays in Documents (about
51 GB on the Pro with everything).

**Moving Half-Life 2 and L4D2 from their old apps**
(`tools/ios_device.sh --game hl2|l4d2 migrate`): each game's saves folder and
the old app's preferences go through the Mac into Sauce, every file is
checked on arrival, and nothing is deleted in the old app. At the game's
next launch, its old button layout fills in any of its buttons the shared
layout doesn't place yet.
- iPad Pro: Half-Life 2, 22 files, all arrived; L4D2, 13 files, all arrived.
- iPad mini: Half-Life 2, 8 files; L4D2, 14 files; all arrived.

**Check on the Pro (each game from the picker: menu, then first map):**

| Game | Menu | First map | Frame rate | Memory (in map) |
|---|---|---|---|---|
| Portal 2 | yes | `sp_a1_intro1` | 120 | 2.4 GB (peak 3.2) |
| Portal Stories: Mel | yes | `sp_a1_tramride` | 120 | 2.0 GB (peak 2.8) |
| Portal | yes | `testchmb_a_00` | 120 | 1.4 GB |
| Half-Life 2 | yes | `d1_trainstation_01` | 120 (second launch) | 1.5 GB |
| Left 4 Dead 2 | yes | `c1m1_hotel` | 120 (second launch) | 1.8 GB |

The memory guard in Sauce is far away (about 10 GB left on the Pro). Each
game ran from a fresh launch through the picker, one after another, so
switching games works; switching from the settings menu's "Switch game" row
is on the hand checklist below.

**Bug found (plain English):** on the first run, Half-Life 2 and L4D2 dipped
to 75–97 fps where their old apps held 120. A new app starts with an empty
shader cache, so the graphics layer compiled shaders while playing (9 bursts
against 3 on the next launch). From the second launch both hold 120, the same
as the old apps. Nothing to fix; it's documented in `docs/ios.md`.

## Round 20: the game on a TV — shipped (PR #7)

**What shipped**
- With a TV connected (an HDMI adapter on USB-C, or AirPlay to an Apple TV),
  the game draws its own picture on the TV at the TV's shape and resolution:
  16:9, up to 4K over a cable.
- The iPad becomes the pad: the touch controls over black, or a dim
  "Playing on TV" when a controller is in use.
- Hot-plug both ways mid-game, without a crash or a restart.
- **Gear menu > TV** (and the Settings app), shared by every game: Picture on
  the TV (on), TV resolution (Auto, 1080p, 1440p, 4K), TV frame rate (the
  TV's rate, 30, 60, 120). Each applies at once.
- The FPS counter shows the game's frame rate, which is what the TV gets.

**How it was tested without a TV:** a test switch, `P2_TV_SIMULATE=WxH`,
makes a 16:9 band across the top of the iPad stand in for the TV (the same
code path, only the window's screen differs), and `P2_TV_SIMULATE_CYCLE=N`
unplugs and plugs it back every N seconds.
- Portal 2 at 1080p: correct 16:9 picture, 4 hot-plugs, 120 fps.
- L4D2 at 4K: correct picture with its HUD, 4 hot-plugs, **120 fps** in
  `c1m1_hotel`, 2.4 GB (2.0 GB at the iPad's own resolution).
- Half-Life 2 at 4K: held its 60 fps cap.
- **4K is not too heavy for any game tested on the Pro.** A real TV usually
  runs at 60 Hz, which caps the game at 60.

**Bug found and fixed (plain English):** with L4D2 and Half-Life 2 the TV
stayed black while the game kept drawing on the iPad. After a picture-size
change, those engines toggle fullscreen, and the SDL library answers by putting
its views back into its own window, taking the game's view off the TV. Portal
2's engine doesn't toggle, which is why it worked first. The app now checks
ten times a second that the game's view is on the TV and puts it back.

## Round 21: Episode One, Episode Two and Lost Coast — shipped (PR #8)

**Inputs:** all already on the Mac, nothing to download. The 20th anniversary
Half-Life 2 install holds `episodic`, `ep2` and `lostcoast`, which Steam shares
with Half-Life 2's install for an owner: the appmanifest lists apps 380, 420
and 340 under "SharedDepots", read from the Mac's own Steam files.

**Choice: games in the picker, not chapters.** The engine plays them as
expansions of Half-Life 2: its own menu switches with `launchmod <folder>`,
before any map. So each is a picker entry that runs Half-Life 2's code on
Half-Life 2's data, started with `+launchmod episodic`, `ep2` or `lostcoast`,
with its own saves folder. The picker is where a game gets chosen, so that's
cleaner than a chapter list. Each has the fork's features: shared settings,
Half-Life 2's touch layout, the gear menu and the version line. "Remove game
data" never offers them, because their files are Half-Life 2's.

**Bugs found and fixed (plain English):**
- `+map ep1_citadel_00` alone answered "No such map": the anniversary build
  only mounts an episode after `launchmod`. Found by printing the engine's
  search paths.
- On a second launch Episode One stopped with "No permissions to run
  'episodic'". Before switching, the engine asks Steam whether the player
  owns the episode, and the iPad's Steam stand-in only knew Half-Life 2.
  Now it also answers yes for the apps Steam installed with the game on the
  Mac (those three IDs, read from the appmanifest), and nothing else.

**Smoke test:**
- Mac: the first map of each loads with no crash, no new import or root.
- iPad Pro, second launch:

| Game | First map | Frame rate | Memory |
|---|---|---|---|
| Episode One | `ep1_citadel_00` | 80–90 (Citadel opening) | 1.6 GB |
| Episode Two | `ep2_outland_01` | 120 | 1.7 GB |
| Lost Coast | `d2_lostcoast` | 60, steady (its HDR harbour) | 1.6 GB |

- The picker shows them next to Half-Life 2, each with Half-Life 2's 11.1 GB
  (the same files, counted once on the device).
- iPad mini: installed (no data needed: it uses Half-Life 2's).

## Round 22: Apple TV research — written (PR #8)

`docs/fork/APPLETV_RESEARCH.md`, research only, every tvOS number with a
source (a helper agent did the web research; I checked the document but not
every press figure myself). The short version:
- **Your A2737 is the Wi-Fi-only, 64 GB model** (no Ethernet port; the A2843
  has it).
- **Memory is the hard limit:** tvOS apps are reported to be killed at about
  2 GB (9to5Mac, measured; Apple doesn't document it), and the
  increased-memory entitlement Sauce uses on iPad doesn't exist on tvOS.
- **Storage:** a 4 GB app bundle limit; only Caches and tmp are writable for
  big files, and tvOS may purge them, so game data must be refillable from
  your Mac. Your streaming idea fits the runtime's file layer
  (`runtime/paths.c`); a first visit to a map would take roughly 6–16 s over
  typical Wi-Fi 6.
- **No local-network prompt on tvOS.**
- **Verdict:** realistic for Portal and Half-Life 2, borderline for L4D2,
  not realistic for Portal 2 or Mel without cutting about 1 GB at peak.
  Portal first: about 3 rounds to play it on the TV, about 5 to stream
  Half-Life 2. Round 1 would be a probe on your box to measure the real
  memory limit.

## GitHub Actions

Still short of Apple Silicon runners today: most jobs were cancelled with
"not acquired by Runner … capacity constraints". Round 19's main `test` job
did get a runner and passed. Each PR was merged on what CI managed plus a
passing local `make check`, with a note on the PR. No billing or minutes
problem.

## Natif Installer — rebuilt

`build/installer/Natif Installer.app`, rebuilt from main after the merges of
rounds 19–22 (version `v0.4.0-176-g1e7f055` in its window, rebuilt once more
after this report's own merge). Its Devices section offers every game for
Sauce.

## Devices

- **iPad Pro:** Sauce with every game, Half-Life 2's and L4D2's data and
  saves moved in. The old Half-Life 2 and L4D2 apps are still installed.
- **iPad mini (install only):**
  - Sauce installed.
  - Portal 2, Mel, Portal and Half-Life 2 data present; Half-Life 2's and
    L4D2's saves moved in.
  - **L4D2's data isn't in Sauce yet:** the mini had 18 GB free, and the old
    apps still hold about 27 GB. Half-Life 2 (11 GB) went in; L4D2 (16 GB)
    waits for the old L4D2 app to go (see Needs Caspian).
  - Saves were backed up before every install.

## Hand re-test checklist (iPad Pro first)

1. Open **Sauce**: the new icon and name; the picker shows eight games with
   their sizes.
2. Play each game for a minute from the picker; open the gear menu and check
   the last line reads "Sauce … · <game>".
3. Gear menu > **Switch game**: back to the picker, pick another game.
4. **Half-Life 2 and L4D2:** load your saves (moved from the old apps);
   check your button layout survived.
5. **Episode One, Episode Two, Lost Coast:** start each, save, quit, load.
6. **TV:** plug the Pro into the TV over HDMI (USB-C adapter):
   - the game moves to the TV, filling it at 16:9, with no black bars;
   - the iPad shows the controls only, and the game plays from them;
   - with a controller, the iPad shows "Playing on TV";
   - unplug mid-game: the picture comes back to the iPad, no crash; plug in
     again: back to the TV;
   - gear menu > TV: switch resolution (1080p / 4K) and frame rate; check the
     FPS counter;
   - AirPlay from Control Center > Screen Mirroring to the Apple TV: the same.
7. **Manage games:** sizes and last played for each; Remove isn't offered for
   Portal 2 or the episodes.

## Needs Caspian

1. **Check your Half-Life 2 and L4D2 saves in Sauce**, then remove the old
   Half-Life 2 and Left 4 Dead 2 apps from both iPads (hold the icon > Remove
   App). On the mini that frees about 27 GB; then I'll copy L4D2's data into
   Sauce there.
2. **The TV checklist above** (step 6): I could only test with the iPad
   standing in for a TV.
3. **iOS's local-network question** now appears as "Sauce" the first time
   L4D2 starts: "Don't Allow" is fine for single player.
4. **Apple TV:** if you want the port, round 1 is a probe on your box: pair
   it with Xcode (Settings > Remotes and Devices > Remote App and Devices on
   the TV) so I can install a test app.
5. **GitHub email privacy** (from the earlier report), if not done yet.
