# Fork rounds 4 and 5 report (24 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp).
Nothing was pushed upstream (`origin` still refuses pushes). Your iPhone was
offline the whole time. **The iPad mini was plugged in partway through**, so it
got real-device tests (marked "iPad" below); everything else was checked on
the Mac and in the iOS Simulator, and the rest is under "Needs Jacksen".

**In one paragraph:** an independent review of rounds 1–3 found real ways
iCloud sync could overwrite a save. Those are fixed, along with the other
confirmed findings, and each fix has a check. The fork now has upstream's
latest (docs only). `tools/ios_device.sh workshop push` puts the Mac's 25
Workshop chambers on a device, and did so on your iPad (they load at 60 fps).
Older phones get sensible first-launch defaults from one tunable table. Mel
builds for iOS as its own app. It reaches its main menu and loads its first
chamber in the simulator, and it's installed on your iPad. Its data needs
about 1.1 GB more free space there before it can run.

`make check` on the fork's `main`: 297 C checks and 256 Python tests pass
(286 and 236 before these rounds). `feat/mel` is merged into `main`; Portal 2
was checked unchanged on the Mac, in the simulator and on the iPad.

---

## Round 4

### 1. Audit of rounds 1–3

A separate reviewer, given only the diffs since `daeda05^` and the three
reports, found 1 critical, 3 high, 8 medium and 11 low items. I checked each
one against the code and reproduced it where I could (a small program running
the old record rule, or a failing check) before fixing it.

**Confirmed and fixed**

| # | What could go wrong | Fix | Check |
|---|---|---|---|
| C1 (critical) | **A save iCloud hadn't downloaded yet counted as "absent".** On the first sync of a second device that already had its own `quick.sav`, or when two devices saved close together, this device **uploaded over the other device's newer save**. Reproduced: the old rule answers UPLOAD. | Files still downloading are skipped until they arrive. | flow checks below |
| H1 | The same placeholder's date was recorded as "synced", so the next sync saw a false conflict and the **older** save took the real name. | Same fix: a waiting file is never recorded. | |
| H2 | **A failed copy (full iCloud, app killed) still updated the record**, so the next sync replaced a save that had never synced. Reproduced: DOWNLOAD over the unsynced save. | The record update is now one tested rule (`rt_sync_after`): a failed copy keeps the old record. | `save_sync_failed_copy_keeps_the_record` |
| M1 | **A save deleted on this device came back on the second sync.** Reproduced. | A local deletion stays recorded. | `save_sync_local_deletion_stays_deleted_on_the_next_syncs` |
| M2 | Two conflicts in the same minute gave the same name, and the second overwrote the first; iCloud's own conflict versions were dropped even when copying them failed. | `-2`, `-3`… when a name is taken; versions dropped only once copied. | |
| M3 | Every sync with files still downloading started its own 15-second retry loop, and they piled up. | One retry at a time. | |
| L3 | "Synced" showed even when copies failed. | The menu says some saves couldn't be copied, and it retries. | |
| L6 | A file in iCloud named exactly `.icloud` crashed the sync. | Ignored. | |
| L7 | A temp file left by a kill mid-copy (`.p2sync-quick.sav`) could show in the game's Load list. | Swept at each sync. | |
| L8 | Backup folder dates depended on the phone's calendar setting (early pruning possible); a second backup the same day replaced the first. | Fixed calendar; unique names. | |
| H3 | **A Community Maps details page re-fetched itself forever** for a map with no description or no download link (and Delete never armed). | Details fetched once per map. | |
| M4 | The download session kept the page alive: leaving the page didn't cancel a download, and reopening it could start a duplicate. | Leaving the page cancels its download and frees it. | |
| L1 | An update deleted the old map before the new one was in place. | New file in place first, old names removed after. | |
| L2 | Keyed API requests could be stored in the disk cache, API key included in the URL. | Ephemeral session for the API. | |
| L4 | No size cap while downloading; a lying `file_size` was only caught after the whole body arrived. | Capped live at the announced size (1 GB max); https only. | |
| L5 | A pasted link with `?appid=620&id=123` opened item 620. | `id=` must be its own parameter. | `workshop_pasted_links` |
| L9, L10, L11 | 148 KB arrays on the main thread's stack; odd text from Steam could crash a download (invalid UTF-8 → nil URL); dead controller-focus entries piled up. | Heap; text always valid UTF-8 and URLs checked; focus cleaned before cards go. | `workshop_text_is_always_valid_utf8` |
| M5 | Changes made in the Settings app might not apply until relaunch, and an open menu showed stale values: the notice used only fires for this process's own changes. | Settings read again on returning to the game; the open menu redraws. The docs now say which settings apply only at the next launch. | needs a device |
| M6 | The gear could switch itself off for the whole session (its "level loaded?" sanity check counted from start-up, before Portal 2's GameUI exists). | Counted only once a level has loaded. | simulator: gear and panel work |
| M7 | A stick held when the menu opened stayed "held" for the game after it closed; the options button press also reached the game's pause menu. | Returns to centre and releases pass; the options button is the menu's in the pause menu. | `ios_menu_pad_filter_lets_releases_and_centring_through` |

**Dismissed**

- **M8, "batched Workshop requests return HTTP 400":** tested live, a
  two-item request in the app's exact format returns 200 with both items.
  The round 3 failure was a different request format.
- **Clocks that differ between devices, iCloud turned off, the app killed
  mid-copy, path traversal, console injection, the API key in logs:** the
  reviewer checked these and found them safe; I agree from the code. Each side
  is compared only with its own recorded state, copies go through a temp file
  then an atomic replace, a missing container never deletes anything, and map
  names are cleaned twice before reaching the console.

### 2. Upstream

`kanapitsas/source-recomp` (the old URL still redirects, so `origin`'s fetch URL
was left as is; pushes to it stay blocked). Three commits came in, **docs
only**; nothing touched the iOS runtime or files the fork changed:

- Half-Life 2 is now called playable on the Mac (player reports, no open game bugs).
- docs/portal1.md records a user's Portal playthrough to chamber 16 on an iPhone 18 Pro (#53), with unreliable back taps.
- The README's status section says the same.

Checked: `make check` green; the Mac Portal 2 app rebuilt and a 100-second
`+map sp_a1_intro3` smoke test loaded the map on Metal with no crash
(`Host_NewGame on map sp_a1_intro3`); the iOS app built for device (Metal
linked) and simulator. Merged into `main`. `pr-52-mel` was left alone here.

### 3. Workshop maps from the Mac to the phone

`tools/ios_device.sh workshop push [--dry-run]`:

- **Copies** the Mac game's downloaded chambers into the device's Community
  Maps folder (`Documents/portal2-userdata/portal2/maps/workshop/<Workshop ID>/`)
  with Community Maps' note (title, update date), so they show as
  **Downloaded** with working update badges.
- **The Mac names its folders by Steam content handle**, not Workshop ID. The
  tool reads the ID from the `thumb<ID>.jpg` next to each map and fetches the
  details from the keyless API; the API's `hcontent_file` matches the folder
  name.
- **Skips** maps already on the device with the same size (adding only the
  note if it's missing), and **leaves alone** a map with another size (another
  version downloaded on the device). **It never deletes anything** on the
  device, and the Mac folder stays.
- **Free space:** `devicectl` only knows a device's total capacity, so the
  app gained a mode (`P2_DEVICE_REPORT=1`) that writes free space, model and
  memory to a file and quits before the game starts. The push stops with a
  plain message if the maps plus 1 GB to spare don't fit.
  `tools/ios_device.sh space` prints that report.
- **Dry run on your Mac today: 25 maps, 794 MB, all resolved** (Portal Plus
  chapters 2–5, Minecraft in Portal 2 parts 1–5, Unreal Chamber 1–4,
  Batcature Laboratories Part 2, and 7 more).
- **iPad: done.** The push copied all 25 (25 maps and 25 notes on the device).
  A second run skipped all 25. Portal Plus: Chapter 2 then loaded from the
  device's copy, at 59.9 fps on Metal.
- Checks: `tests/test_workshop_push.py` (what to copy, skips, notes, names,
  staging, dry run, free space) and `checks_device_ios.c`.

### 4. Defaults for older phones

At **first launch only**, and only for settings you never changed, the app
picks Resolution, Upscaling (MetalFX) and Frames per second from the device
model and memory. Everything is in one file, **`ios/device_defaults.txt`**,
to tune and rebuild.

| Tier | Devices | Resolution | MetalFX | Frames per second |
|---|---|---|---|---|
| weak | A13, A14 (SE 2, 12 family) | ×1 | Quality | 30 |
| a15 | **iPhone 13, 13 mini** (4 GB, 60 Hz), SE 3, iPad mini 6 | ×1.5 | Quality | 30 |
| a15pro | **iPhone 13 Pro, 13 Pro Max** (6 GB, ProMotion); 14, 14 Plus | ×1.5 | Quality | 40 (30 on 60 Hz screens) |
| a16 | 14 Pro, 15, 15 Plus; **any unknown model** (the safe middle) | ×2 | Quality | 60 |
| strong | A17 Pro and newer (15 Pro, 16–18 families, iPad mini A17 Pro and A20 Pro) | ×2 | Off | 60 |

- **Conservative:** nothing below an A17 Pro has been measured. Auto
  (120 Hz) is nobody's default, per the round 1 rule.
- **4 GB phones** also get a low-memory guard: textures at half resolution
  (`+mat_picmip 1`) and lower Metal memory caps. In the simulator the engine
  confirmed it: "System configuration ignoring mat_picmip due to command line
  override".
- **The log records the memory footprint at each level load**, with the peak
  and the headroom left before iOS's limit, and the tier picked at launch.
- **In the game:** Display shows "Defaults for iPhone 13: …" with a **Use
  recommended** button. Reset to defaults goes back to these values.
- Checks: table lookup, unknown model → safe middle, never over a changed
  setting, memory readout.
- **Simulator, set up as an iPhone 13** (new simulator "R4 iPhone 13", iOS
  27.2, with 4 GB simulated): `[ios] appareil : iPhone 13 (iPhone14,5, 3700
  Mo, mémoire réduite), palier a15 : résolution 1.5, MetalFX 0.75, 30 i/s ; 3
  réglage(s) posé(s)`; a chamber loaded, the pause menu opened and the
  settings panel drew over it.
- One finding from that run: a chamber used about **1.9 GB** in the
  simulator. Portal 2's own profile has Apple's increased-memory entitlement,
  which matters on a 4 GB phone; see Risks.
- **iPad (A20 Pro, 11.8 GB):** "strong" tier picked, Metal, 59.9–60.1 fps.
  - On a later launch, "0 réglage(s) posé(s)": the defaults are written once
    and then left alone.
  - Memory at a level load: **2.7–2.9 GB**, with 9.4–9.6 GB left before
    iOS's limit.
  - That device figure is what an iPhone 13 has to fit under; the
    low-memory guard is there for it.
- `docs/ios.md`: tested devices (iPhone 15 Pro Max by upstream, your iPhone
  18 Pro and iPad mini A20 Pro) and an "Older phones" section.

---

## Round 5: Portal Stories: Mel on iOS, phase 1

### The plan's choices ([MEL_IOS_PLAN.md](MEL_IOS_PLAN.md))

- **A separate app, as you leaned.** "Portal Stories Mel" has its own
  identifier (`local.melrecomp.vn8jwve7jx`), data and saves
  (`Documents/mel-userdata`), like Half-Life 2 and Portal. A game switcher
  would mean two 300 MB game binaries in one app, and Mel's problems could
  never reach Portal 2's saves this way.
- **No new paid or permanent step.** Your team already has Xcode's wildcard
  development profile (valid to November 2027, both your devices in it).
  `sign.py` now falls back to it when an app has no profile of its own, so
  Mel signs **without registering an App ID**. Portal 2 still uses its own
  profile.
  - I also made `setup.sh --ios` skip Xcode's provisioning step for Mel.
    Otherwise turning Mel's iOS build on would have registered that App ID
    the next time you ran setup.
  - The catch: the wildcard can't carry the increased-memory entitlement.
    Fine on your 12 GB devices; a 4 GB phone will need the real App ID
    later (Needs Jacksen 4).
- **Data over the cable:** `tools/ios_device.sh mel data push [--dry-run]`,
  about **9.3 GB**, free space checked first.
  - **Nothing worth trimming:** 1.2 GB of Bink videos are the menu
    backgrounds, cutscenes and credits; there's one set of voices; and the
    other languages are only a few MB of text.
  - What *is* left out: the Mac player's own leftovers in Mel's game folder
    (`SAVE`, screenshots, logs).
- **`feat/mel` = PR #52's commits with the fork's `main` merged in** (not
  rebased, so they stay identical to upstream's). To stay in sync, fetch
  `pull/52/head` into `pr-52-mel` and merge it into `feat/mel` again. The fork's
  iOS work sits in separate commits on top.
- **Mac build kept working:** `./setup.sh --game mel` is unchanged on the Mac
  side.
- **Risks** (4 GB phones, the 2021 engine against the iOS layer, indirect call
  roots, controls): in the plan. One checked already: Mel's engine asks for
  exactly the same three interface versions the iOS layer uses for Portal 2
  (`VEngineCvar007`, `VEngineClient015`, `VEngineVGui001`), so the pause-menu
  gear and settings menu are on for Mel too, with the same safety checks.

### How far Mel got

**In the simulator: the main menu and the first chamber.**

- **Builds:** for the simulator (407 files, 388 MB executable) and for the
  device (Metal linked, signed, 400 MB). The Mac build from `feat/mel` still
  works: `Host_NewGame on map sp_a1_tramride`, no crash in 150 s.
- **Main menu:** reached and held for 7 minutes, with no crash and the
  "Portal Stories: MEL" logo, the menu background and Load / New Game /
  Options / Quit:

  ```
  [ios] jeu : mel (mod portal_stories, mel-userdata)
  [ios] état du jeu : IEngineVGui à 0x136cf360
  [ios] état du jeu : IVEngineClient à 0x1351d398
  [steam-stub] hors ligne : stub iOS, app 317400
  [ios] interface du jeu affichée
  ```

- **First chamber:** the first try stopped at a dialog, *"Disconnected: STEAM
  certificate length error! 0/2048"*. After the fix (below), the tram ride
  of `sp_a1_tramride` loads and runs, with its subtitles:

  ```
  Host_NewGame on map sp_a1_tramride
  [ios] niveau chargé
  [ios] mémoire (niveau chargé) : empreinte 4493 Mo, pic 4549 Mo, limite d'iOS inconnue (iPhone19,2, 16384 Mo)
  [ios] joy_response_move 5 -> 0 (stick tactile linéaire)
  ```

- **On the iPad mini (you plugged it in):** Mel installed, signed with the
  team wildcard profile, and it starts. It answered the free-space report,
  which runs inside the app.
  - Its data isn't on the iPad yet: `mel data push` stopped as designed, with
    *"this copy needs 8911 MB and keeps 1024 MB free, but only 8868 MB is
    free"*.
  - So Mel's menu on a real device is still to see: about 1.1 GB has to be
    freed first.

### Blockers, in plain words

- **Fixed: "STEAM certificate length error".**
  - **What happened:** even in single player, the game joins its own small
    local server and shows it a Steam ticket. On iOS a stand-in for Steam
    hands out that ticket.
  - **Why Mel got none:** Mel uses an older Steam version, where the call has
    a slightly different name (no `_DEPRECATED` at the end), so the
    stand-in didn't recognise it and gave an empty ticket. The server then
    turned the player away.
  - **Fix:** the stand-in now recognises the old names too. Only two Mel
    entries changed; Portal 2's are identical.
- **Fixed: the touch stick's straight-line response was off for Mel.**
  - At start-up Mel looks like "in gameplay" before its game code has
    loaded, so a lookup failed once and gave up for good.
  - It now waits for a level, for Portal 2 too.
- **Open: memory.**
  - In the simulator Mel uses about 4.5 GB in the tram ride, against
    Portal 2's 1.9 GB there.
  - The simulator renders in software and isn't a device measure. On the
    iPad, Portal 2 uses 2.9 GB with 9.4 GB of headroom, so Mel should fit
    on your devices.
  - A 4 GB phone is the real question, and the wildcard profile has no
    increased-memory entitlement (Needs Jacksen 4).
- **Not checked yet:**
  - whether Mel's touch controls draw during gameplay;
  - whether the Bink menu video animates (the menu background showed; the
    screenshots can't tell motion);
  - anything past the tram ride.

### What's left before Mel is playable on a device

Realistic estimate: **2–4 days of work once its data is on a device**, plus
your play time.

1. **Free about 1.1 GB on the iPad mini**, then `mel data push`
   (5–10 minutes). See Mel's menu and first chamber on Metal.
   Half an hour, with you.
2. **Play through chapter 1 on the device**, fixing what comes up:
   - missing call targets (`target not recompiled`; each takes one root added
     to `config/mel-roots.json` and a rebuild, as on the Mac);
   - Metal differences from the simulator's OpenGL;
   - the touch layout in Mel's own maps.
   1–2 days.
3. **Memory and speed:** the footprint at each level load (now logged),
   frame rate on the A20 Pro, and the defaults table for Mel if it needs
   lower ones. Half a day to a day.
4. **Chapters 2–5 and the ending:** nobody has finished Mel even on the Mac
   yet; new call roots are likely. Open-ended, shared with the Mac work.
5. **For 4 GB phones:** Mel's own App ID with the increased-memory
   entitlement (your decision), then a test on an iPhone 13-class device.

### Was `feat/mel` merged into `main`?

**Yes.** With `feat/mel` merged in, Portal 2 was checked unchanged:

- `make check` (297 checks);
- the Mac app with `+map sp_a2_laser_intro`;
- the iOS simulator (iPhone 13 set-up, level loaded, settings menu);
- the iPad mini: Metal, 60.1 fps, a pushed Workshop map loaded, 2.7 GB.

PR #52's own runtime changes (12-byte D3D9 vertex elements, FreeType and
fortify shims, the Steam SDK 1.40 tables) only add paths that Portal 2
doesn't take.

---

## Bugs found and their causes

From the audit (details in the table above), the ones that mattered most:

1. **iCloud could overwrite a save on another device.** A save iCloud
   hadn't finished downloading looked "missing", so this device uploaded its
   own over it. That happened on the first sync of a second device, or when
   both saved close together.
2. **iCloud could later replace a save that never synced.** A failed copy
   was still written down as "synced".
3. **A deleted save came back** on the second sync.
4. **A Community Maps page could reload itself forever** for a map with no
   description.

Found this round outside the audit:

5. **Mel couldn't load a chamber on iOS** ("STEAM certificate length
   error"). The iOS stand-in for Steam only knew the newer name of the
   "join a server" call, and Mel's older Steam version uses the old name.
6. **The touch stick's straight-line response could switch itself off**
   for the session. Mel (and possibly Portal 2) looks "in gameplay" at
   start-up before its game code is loaded, and a lookup made then gave up
   for good.
7. **Without iCloud set up, the game re-checked iCloud and logged it every
   10 seconds** (seen on the iPad mini). The failed check never noted the
   saves as seen.
8. **The new defaults slowed down the simulator.** They gave the simulator
   its own model's ×2 resolution, while it renders in software at ×1. The
   simulator now skips them unless asked.
9. **Memory headroom printed "0 MB left" in the simulator**, which has no
   such limit; it now says "unknown".
10. **"Nothing to copy" printed as "1 KB"** in the Workshop push.
11. **Turning Mel's iOS build on would have made `setup.sh --ios` register
    a new App ID** on your account, through Xcode's provisioning step.
    Caught before anything ran: `setup.sh` now skips that step for Mel,
    which signs with the wildcard profile.
12. **"Defaults for iPhone 13: resolution √ó1.5"**: a formatting call read
    the "×" as the wrong character set (seen in a simulator screenshot; fixed
    and re-checked: "×1.5").

## Needs Jacksen (everything, batched with rounds 1–3)

Always set `P2_DEVICE=<UDID>` for the commands below: an **iPad Pro is also
reachable from this Mac**, and without `P2_DEVICE` the tools would pick it.
Your iPhone 18 Pro is `00008160-0018345E10034036` and your iPad mini is
`00008160-001C693804C14036`.

**Done on the iPad mini while it was plugged in:** saves backed up first
(`build/ios/device/ipad-mini-backup-20261124-1905`, 4 saves), the final
Portal 2 build installed (your saves and settings kept), the 25 Workshop maps
pushed, Mel installed. A temporary `Documents/launch.args` I used for testing
was emptied again.

**New this round**

1. **iPad mini: free about 1.1 GB**, then push Mel's data. It has 8.9 GB
   free, and Mel needs 8.9 GB plus 1 GB to spare. Then, from
   `../portal2-recomp-mel`: `P2_DEVICE=00008160-001C693804C14036
   tools/ios_device.sh mel data push` (roughly 5–10 minutes).
2. **iPhone: plug in and unlock**, then from the main checkout:
   - `P2_DEVICE=00008160-0018345E10034036 tools/ios_device.sh install`
     (Portal 2; data and saves stay);
   - `… workshop push` (794 MB);
   - from `../portal2-recomp-mel`: `… --game mel install`, then
     `… mel data push` (about 9.3 GB, if there's room).
3. **Decide later, not needed tonight: a real App ID for Mel.** Your devices
   don't need it. An iPhone 13 would: only a profile made for Mel's own
   identifier can carry the increased-memory entitlement that Portal 2's has.
   It's a permanent App ID on your account, so I didn't create it. When you
   want it: `cd ios/signing && P2_BUNDLE_ID=local.melrecomp.vn8jwve7jx xcodegen`,
   then the `xcodebuild … -allowProvisioningUpdates` line from docs/ios.md.
4. **Mel upstream:** nothing to do now. When PR #52 changes or merges, I fetch
   it into `pr-52-mel` and merge that into `feat/mel`.

**Left over from rounds 1–3 (unchanged)**

5. The one-time iCloud setup (docs/ios.md, "iCloud saves"). This round's
   fixes make it safer to turn on, so it's a good time.
6. A free Steam Web API key, pasted into Community Maps on each device.
7. The controller test of the round 1 overlay fix, and the OK on the
   upstream PR (E) after it.
8. A back-tap recording with light taps (Settings > Portal 2 > Record taps).
9. The 120 Hz measurement on both devices (decides whether Auto becomes a
    default).
10. The `portal2_dlc3` addon test.

## Hand-test checklist for tonight

**Install**
- [ ] Portal 2 on the iPhone (already on the iPad): the menu's version line
  reads `0.4.0-fork.2 (6d99195)` or later. In pause > gear > Display, "Defaults for iPhone 18 Pro" (or
  "iPad mini (A20 Pro)") shows ×2, no MetalFX, 60 fps; your existing settings
  are unchanged (the defaults never override a setting you set).
- [ ] "Portal Stories Mel" on the home screen with its icon (installed on
  the iPad; the icon was made from Mel's own `portal2.ico`, not checked by eye).

**Workshop maps**
- [ ] iPhone: `workshop push --dry-run` lists 25 maps, about 794 MB; then
  push. (iPad: done.)
- [ ] Pause > gear > Maps & Mods > Community Maps > **Downloaded**: the 25
  chambers with their titles; Play one (e.g. Portal Plus: Chapter 2) and it
  loads.
- [ ] Run the push again: everything is "already on the device", nothing
  copied.

**Mel**
- [ ] `mel data push` refuses cleanly if space is short, otherwise copies
  about 9.3 GB.
- [ ] Open Mel: Prism Studios logo / menu video, then the **main menu**.
- [ ] New Game: the first chamber loads (that's phase 2's real test; tell me
  how far it gets). With Settings > Portal Stories Mel > Log on,
  `P2_DEVICE=<UDID> tools/ios_device.sh --game mel run 300` brings back the log.
- [ ] In a chamber: the touch controls show (not confirmed yet), and in the
  pause menu the gear shows, without Maps & Mods or iCloud (Portal 2 only).
- [ ] The menu background moves (the Bink video); the simulator screenshots
  couldn't tell.

**Portal 2 saves (after the iCloud setup, item 6)**
- [ ] Save on the iPhone, wait ten seconds, open the game on the iPad mini:
  the save is in Load.
- [ ] Both devices in airplane mode, save "quick" on each, airplane mode off,
  open each game: both copies are in Load (one with the other device's name),
  none lost.
- [ ] Delete a save on the iPhone, sync twice (leave and reopen the game twice):
  it stays deleted on the iPhone and stays on the iPad.
- [ ] Nothing older than before this round went missing: compare Load with
  what you had.

**Round 3 checklist** (unchanged, docs/fork/ROUND3_REPORT.md): browse with
your key (Popular, Newest, Trending, Search, Co-op), Details, Download and
Cancel, Play, Downloaded, Delete, offline message, pasted links and
collections, a wrong key, the controller. Two items are new from the audit:
- [ ] Open a map with no description (some short test chambers): the page
  stays still and Delete works with two presses.
- [ ] Start a download and press Back: it stops, and reopening Community Maps
  shows Download again (no hidden download running).
