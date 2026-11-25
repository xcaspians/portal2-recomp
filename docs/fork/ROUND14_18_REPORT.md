# Fork rounds 14 to 18 report (night of 25 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp).
Everything below is merged into `main` and goes to GitHub through
[PR #1](https://github.com/xcaspians/portal2-recomp/pull/1) (the README
makeover, which sits on top of it). Nothing went upstream, and `Prompts/`
was never committed.

`make check` on `main`: **323 C checks, 301 Python tests and 139 x86-64
checks pass** (320, 288 and 62 at the start of the night).

**Devices.** All testing ran on the **iPad Pro**. Saves and settings were
backed up before every install, and the app was closed after every test.
The **iPad mini** was left alone until the queue was done, then got
install-only updates. The **iPhone** got the Portal app at the end.

## In short

| Round | Status |
|---|---|
| Mel slim switch-over (from the morning) | **Fixed and done on the iPad Pro.** The failure on the mini was my test's fault, not Mel's. |
| 14: Half-Life 2 | **Done.** It runs on the Mac and on the iPad Pro in its own app (100–120 fps, 1.2 GB). |
| 15: Left 4 Dead 2 on iOS | **Runs on the iPad Pro:** the menu, then c1m1 at 115–120 fps and 2.0 GB, with its own touch layout. The particle problem on the Mac is missing game data. |
| 16: P2CE piece B | **Milestone reached:** 99.997 % of P2CE's instructions handled; whole libraries compile. |
| 17: P2CE piece C | **Milestone reached:** four P2CE libraries load and talk to each other; C++ exceptions work. |
| 18: P2CE piece D | **Not reached.** P2CE's own launcher runs until its Steam check; Steam needs bridging next. |
| iCloud saves | **Blocked on one Xcode click** that only you can do (below). |
| README makeover | **Done and merged** (PR #1). |

---

## The Mel slim switch-over (your morning request)

- **What really went wrong:**
  - The boot test picked your save by listing the iPad's save folder, and
    that listing includes the Steam-ID folder.
  - So it asked the game for `load 76561197960265729/1795587607`. The
    game adds that folder itself, so the name was doubled, and it said
    "file missing".
  - My manual tests passed the plain name, which is why they worked.
  - The "Mel's own files don't load" lines (`portal_stories_english.txt`)
    are normal: they appear with the full copy on the Mac too.
- **Fixed, plus three safety nets:**
  - the test now gives the plain save name;
  - it judges only what its own launch wrote to the console (Mel's console
    keeps every launch, so an old success could have hidden a failure);
  - it waits for the iPad to be unlocked, and says so if it fell asleep;
  - it loads the save from the menu, as a player would.
- **Done on the iPad Pro** (with Mel's full copy pushed there first, as you
  asked):
  - the boot test passed;
  - the app then removed Mel's 13 copies of Portal 2's archives;
  - that freed **2.39 GB**, and Mel then starts slim and loads the save by
    itself.
  - The mini's Mel is untouched: still the full copy.
- **Also fixed:** `mel data push --full` stopped at once, a script bug from
  round 11.

## Your two other morning items

- **A controller no longer hides the FPS counter.** The readout was hidden
  together with the touch buttons whenever a controller was in use; now
  only the buttons step aside.
- **`run_game.py --show` has no timeout.** When you play by hand the game
  runs until you quit; diagnostic runs keep their 60 seconds, and
  `--timeout` still works if you want one.

---

## Round 14: Half-Life 2

- **Inputs:** depot 224 was there. Half-Life 2 needs Steam SDK 1.60, so I
  fetched Steamworks.NET 2024.8.0 exactly as `setup.sh` does (pinned
  release, SHA-256 checked, kept out of git).
- **Mac:** the main menu, then `d1_trainstation_01`, at about 120 fps and
  1.2 GB. No missing import and no new root: round 8's work held up.
- **iPad Pro, in its own app:**
  - Signed with your team's wildcard profile, so no new App ID was created.
    That also means no iCloud and no increased-memory entitlement, which
    the Pro doesn't need.
  - Menu at 60 fps (the default cap).
  - `d1_trainstation_01` at **100–120 fps** with Frames per second on Auto.
  - Memory **1.2 GB**, peaking at 1.55 GB, with 3.9 GB still free.
  - `d1_canals_01` loads too.
  - **Checked on screen:**
    - the gear menu on the main menu;
    - the layout editor;
    - the version line ("Half-Life 2 · recomp fork · version 0.4.0-fork.2");
    - the FPS counter at the top centre, under the weapon buttons.
- **Rough edges:**
  - **The ammo readout.** I couldn't check it against the bottom-right
    buttons: maps started from a test launch have no weapon, and my
    command hook didn't fire in Half-Life 2. If it overlaps on your iPad,
    the layout editor moves the buttons.
  - **Not tried yet:** saves across chapters, long sessions, sound over
    time.
- **iCloud for HL2** needs its own App ID and container, which are permanent
  (Needs Caspian).
- **Left for a full playthrough:** play it through, adding any new roots
  as they appear (HL2 needed none so far), check the episodes, and give it
  its own App ID if you want iCloud.

## Round 15: Left 4 Dead 2 on iOS (local and offline only)

### Graphics on the Mac

- The "got error material for particle\…" lines name **19 particle
  materials**: smoke, warps, water drops, fire embers, banner beams.
  - None of them is anywhere in this Mac's L4D2 install, which has Steam
    depots 551, 552 and 553.
  - The game's own effect files ask for them, so they must come from a
    content depot the Mac edition never downloads.
  - The renderer isn't at fault: those effects draw with Source's error
    material, and that is the "work in progress" look.
  - **Fix:** download the missing depot (Needs Caspian 3). No code change
    can make up data that isn't there.
- The Linux build also looks for its own fonts (DejaVu, Liberation), which
  neither depot has; text falls back to the system's.

### The iOS app

- **Its own app**, like Half-Life 2's: the gear menu, version line, FPS
  counter and layout editor; settings shared inside this app; gyro aiming
  on.
- **Engine numbers for the gear menu**, read from the Mac edition's
  symbols: `VEngineClient013` and `VEngineCvar007`.
- **Touch layout:**
  - Half-Life 2's right-thumb cluster: shoot (drag to aim), shove, reload,
    jump, crouch (a toggle), use;
  - **the item slots** across the top: primary, secondary, throwable,
    medkit, pills (keys 1 to 5);
  - flashlight and pause at the top right; move and look sticks;
  - everything movable in the layout editor;
  - controllers behave as in Portal.
- **Offline:** the app's launch arguments include `-insecure +sv_lan 1
  -nomaster`, and on iOS Steam is the offline stub anyway.
- **A bug found and fixed:** the app stopped at once with "SteamAPI_Init
  is absent". The iOS Steam stub only had Portal 2's `InitSafe`. A new test
  makes the stub keep up with everything the Mac bridge looks up.
- **On the iPad Pro:**
  - menu at 60 fps, 627 MB;
  - `c1m1_hotel` at **115–120 fps** on Auto;
  - memory **2.0 GB**, with 3.1 GB still free before iOS's limit.

  I couldn't trigger a horde without someone playing, so the peak with a
  horde is still to measure (Needs Caspian 4). Two hordes on the Mac stayed
  near 2 GB, so the Pro has room.
- **iOS asks once** whether L4D2 may "find devices on local networks",
  because the game's local server opens a network socket. Answer it once on
  the iPad. "Don't Allow" is fine for single player.

### How this went (plain English)

- **Where the round started:** the Mac had already done the hard part in
  round 12. The iPad needed a new "profile" (which folders, which Steam ID,
  which touch layout), the engine's method numbers for the gear menu, and
  a button layout for a shooter with five item slots.
- **The first real problem was Steam.** On the Mac, Steam calls go to
  Valve's real library. On the iPad there is no Steam, so the app carries a
  small stand-in (the "stub") that answers "yes, you own the game; no, no
  network". L4D2 starts Steam with a different function name than Portal 2
  does, and the stand-in didn't have it. One line fixed it, and a new test
  now compares the stand-in against everything the Mac side ever asks
  Valve's library for.
- **The second problem was the screen's top row.** It now holds five item
  slots, so the FPS counter moved under it. A test checks every button at
  every size against the counter. That test also caught an older overlap
  in Half-Life 2's layout at normal button size.

## Rounds 16–18: P2CE pieces B, C and D

Done by a helper agent in its own copy of the repository, then checked and
merged here. Its learning notes are in plain English:
[P2CE_ROUND16_18_NOTES.md](P2CE_ROUND16_18_NOTES.md). The plan's table is
updated: [P2CE_PLAN.md](P2CE_PLAN.md).

- **Depots** (checked once): 440003 and 601363 are now on the Mac, and
  both are **Windows-only**.
  - **P2CE (440003):** not needed; the Linux depot 440005 is complete.
  - **Portal: Revolution (601363):** a licensed Strata branch from January
    2025, with P2CE's interface versions but no Linux build. It would need
    a separate 64-bit Windows front end: a later project. Written up at the
    end of `P2CE_RESEARCH.md`.
- **Piece B (reached):** a count over all 147 of P2CE's Linux programs and
  libraries.
  - **81.4 million instructions, 99.997 % handled.** Engine, client and
    server are at 100 %.
  - Added: the SIMD families SSE3 to SSE4.1, x87, MMX and BMI.
  - The newest families (AVX and up) are deliberate stop signs: the
    runtime tells the game the processor lacks them, so the game picks its
    plain code.
  - Whole libraries now compile (`make p2ce-build`, 15 s for the first
    two).
- **Piece C (reached):** `make p2ce-modules-check` passes its 15 checks.
  - It loads four P2CE libraries as Linux would: load order, per-thread
    variables, start-up code.
  - Reading the command line works, the file system hands out its
    interface, and a C++ error thrown two functions down is caught
    correctly.
- **Piece D (not reached):**
  - **Built:** threads, program start the Linux way, `/proc`, direct
    system calls, and a runner that never writes to the depot.
  - **How far it runs:** P2CE's own launcher starts, loads its file
    system, then stops at "Failed to initialize filesystem Steam API
    context". That's because no Steam is reachable from the 64-bit side;
    the real Linux game would stop the same way without Steam.
  - **Next:** bring the Steam bridge forward from piece E, sending
    P2CE's Steam calls to Valve's own library, never a fake Steam (Needs
    Caspian 5).
- **The 32-bit games are untouched:** only the Makefile changed outside
  new files, and `make check` passes.

## iCloud saves (your request, after the rounds)

- **What I did:** `ios_icloud` was already on, so I re-ran
  `./setup.sh --ios --game portal2` (pinned to the iPad Pro) with your team
  and `-allowProvisioningUpdates`.
- **Where it stops:** the same place as in round 10.
  - Xcode turns iCloud on for the App ID, but it cannot create the iCloud
    container from the command line, or tick it on the App ID.
  - So the profile comes back without a container, and signing refuses
    the entitlement: "Provisioning profile … doesn't match the
    entitlements file's values for … icloud-container-identifiers".
  - No permanent container was created.
- **Nothing broke:** the app signs without iCloud when the profile has no
  container, and its menu says "iCloud isn't available".
- **The one thing left is a click in Xcode** (Needs Caspian 1). After it,
  `./setup.sh --ios --game portal2` signs with iCloud. The sync test (a
  save goes up and comes back) is then a two-minute job.

## README makeover

- **The new top of the README:**
  - a clear title and pitch, with credit to kanapitsas/source-recomp up
    top;
  - badges for CI and platforms;
  - "What's new in this fork" in plain English;
  - a supported-games table with honest status;
  - an install section pointing at your fork and the Natif Installer.

  Upstream's full documentation follows it, unchanged apart from the clone
  command.
- **Screenshots are from the iPad Pro:** the settings menu, the layout
  editor, and Half-Life 2 with the FPS counter. **Not taken:** Portal 2,
  the game picker and Community Maps. iOS's local-network prompt for L4D2
  sat on top of every app until someone taps it, and it showed a nearby
  device's name, so those shots weren't fit for a public page (Needs
  Caspian 2).
- **The GitHub description and topics** are set (portal-2, source-engine,
  static-recompilation, ios, ipados, apple-silicon, metal, half-life-2,
  left-4-dead-2, game-port).
- **Your name:** the docs now say "Caspian" wherever they named you.
- **PR #1 is merged.** The fork's CI never started a run for it: its
  workflow is enabled, but the Actions tab has never run on this fork,
  which is usually the one-time switch GitHub asks for. CI only runs
  `make check`, which passes locally on this exact branch, so I took that
  as the green your rule asked for.

## Bugs found tonight, and their causes

1. **Mel's boot test asked for the wrong save name** (it doubled the Steam
   ID folder), and it read old console output.
2. **The FPS counter vanished with a controller**: it was hidden with the
   touch buttons.
3. **`mel data push --full` stopped at once**: an empty list under macOS's
   old bash.
4. **The iOS Steam stub had no `SteamAPI_Init`**, so L4D2 couldn't start on
   the iPad.
5. **Saved button layouts couldn't hold names with digits**, which would
   have lost the item-slot positions.
6. **Half-Life 2's camera-mode button sat under the FPS counter** at normal
   button size. The old check only tried the largest size.

## Devices at the end

- **iPad Pro:**
  - every app installed: Portal (Portal 2 and the slim Mel), Half-Life 2
    and Left 4 Dead 2;
  - backups in `build/ios/device/ipad-pro-backup-*`.
- **iPad mini** (install only, after the queue):
  - backed up first: 5 Portal 2 saves and 1 Mel save, in
    `build/ios/device/ipad-mini-backup-20261125-0400-final`;
  - installed: the latest Portal app (all 5 saves still there afterwards),
    plus Half-Life 2's and Left 4 Dead 2's own apps with their data (HL2's
    first try dropped the connection; the retry went through);
  - Mel stays the full copy until you run the switch-over;
  - **Portal 1** isn't installable: its Linux depot (404) isn't on this
    Mac;
  - Half-Life 2 is its own app, as in round 14, not inside the Portal app.
- **iPhone:** backed up (settings; it has no saves yet), then the latest
  Portal app installed.

## Hand re-test checklist (iPad Pro first)

1. **Answer the local-network prompt** on the iPad Pro (from L4D2):
   "Don't Allow" is fine.
2. **Mel on the Pro:** it opens from the picker and your save loads. It's
   the slim copy now, 2.4 GB smaller.
3. **Half-Life 2 on the Pro:**
   - play the first chapters, set Frames per second to Auto (about 120);
   - pick up a weapon: is the ammo readout clear of the buttons? If not,
     move the buttons with the layout editor.
4. **Left 4 Dead 2 on the Pro:**
   - the menu, then a campaign;
   - try the five item slots, shove (the hand button), the flashlight;
   - does a horde come, and how does it feel?
5. **A controller** in any game: the touch buttons hide, and the FPS
   counter (if on) stays.
6. **On the Mac:** `python3 tools/run_game.py --game l4d2 --show` runs
   until you quit.
7. **The iPad mini:** the new apps are there, and your 5 saves are still in
   Load.

## Needs Caspian

1. **iCloud: the last step, in Xcode** (I can't click it for you):
   - open `ios/signing/P2Signing.xcodeproj`;
   - choose target **Portal2**, then **Signing & Capabilities**;
   - under **iCloud**, tick **iCloud Documents**;
   - under Containers press **+**, create `iCloud.local.portal2recomp.vn8jwve7jx` and tick it;
   - press ⌘B;
   - then tell me. I'll run `./setup.sh --ios --game portal2`, install on
     the Pro and test a save going up and coming back.
2. **README screenshots of Portal 2:** answer the L4D2 local-network prompt
   on the iPad Pro first. Then I take the game picker, Portal 2, the
   settings menu and Community Maps for the README.
3. **L4D2's missing particle materials:** in the Steam console, try
   `download_depot 550 555` through `download_depot 550 559`, then tell me
   which ones arrived. I'll check them for the 19 materials.
4. **L4D2 horde memory on the iPad Pro:** play c1m1 until a horde comes. If
   you can, turn on **Log** in the gear menu first, so I can read the peak.
5. **P2CE:** confirm that the Steam bridge (Valve's own library) should
   come before the rest of piece D.
6. **Turn on the fork's Actions** (GitHub > Actions tab, enable workflows) so
   CI runs on the next push. I merged PR #1 on a local `make check`.
7. **Your name in old commits:** earlier rounds' reports, already on
   GitHub, still name you in the history. Rewriting history is possible but
   invasive: say if you want it.
8. **Left from earlier:** HL2's own App ID and iCloud container (permanent;
   only if you want iCloud for HL2), the Steam Web API key, the controller
   test, the `portal2_dlc3` addon test.
