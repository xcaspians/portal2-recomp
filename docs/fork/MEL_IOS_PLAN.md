# Portal Stories: Mel on iOS: plan (round 5, phase 1)

Written 24 November 2026. Phase 1 gets Mel building for iOS and reaching its
main menu in the simulator. It does not need to be playable yet. The report
for this round is [ROUND4_5_REPORT.md](ROUND4_5_REPORT.md).

## 1. A separate app

**Recommendation: a separate "Portal Stories Mel" app**, like Half-Life 2 and
Portal already are. That was Caspian's leaning, and nothing argues against it:

- **One game per app is how the iOS build works today.** The game is fixed at
  build time (`P2Game` in the Info.plist, `runtime/ios/game_ios.c`). The
  lifted code is one binary per game (Mel's engine is its own 2021 build, not
  Portal 2's), so a switcher would mean two 300 MB executables in one app.
- **Its own data and saves:** `Documents/mel-userdata` (data and prep in the
  Mel app's own container), so a problem in Mel can never touch Portal 2's
  saves, iCloud sync or Community Maps.
- **Portal 2's app is not changed** by any of this: the Portal 2 profile, its
  identifier and its signing stay as they are.
- **Identifier:** `local.melrecomp.<team>` (`games.mel.ios_bundle_id` in
  `config/local.json`). On Caspian's team it is signed with the team's existing
  wildcard development profile ("iOS Team Provisioning Profile: *", valid to
  November 2027, both devices already in it). `ios/signing/sign.py` now falls
  back to a wildcard profile of the same team when no exact profile exists.
  That needs **no new App ID and no new paid or permanent step**, so it is not
  a "stop and ask" item. Mel needs no iCloud, which a wildcard profile can't
  give. A free account would still need one of its 3 app slots.
- **Later, for 4 GB phones:** a wildcard profile can't carry
  `com.apple.developer.kernel.increased-memory-limit`, which Portal 2's own
  profile has. Caspian's devices have plenty of memory, but an iPhone 13 will
  need Mel registered as its own App ID with that capability (the
  `xcodebuild` step in docs/ios.md, with the Mel identifier). That creates a
  permanent App ID on the account, so it is listed under "Needs Caspian" and
  not done here.

## 2. Getting the data onto the device

**Over the cable, like Portal 2's data:** `tools/ios_device.sh mel data push
[--dry-run]` (the same as `tools/ios_device.sh --game mel data`, plus a
free-space check first). It copies:

| What | Size | Where on the device |
|---|---|---|
| `portal_stories` (Mel's maps, VPKs, Bink videos) | 6.2 GB | `Documents/game/portal_stories` |
| `portal2` (the Portal 2 content Mel ships with its own engine) | 2.3 GB | `Documents/game/portal2` |
| `bin`, `platform`, `portal2_dlc2` | 140 MB | `Documents/game/…` |
| Linux binaries (depot 317404) | 90 MB | `Documents/linux` |
| prepared modules (with i386 `libstdc++` and `libgcc_s`) | 71 MB | `Documents/prep` |

About **8.8 GB** in all, plus 1 GB kept free.

**Trimming:** almost nothing is safe to drop.

- The 1.2 GB of Bink videos in `portal_stories/media` are the menu
  backgrounds, the cutscenes (`aegis_interior` alone is 554 MB) and the
  credits, all played by the game.
- Mel has one set of voices (English, inside its VPKs) and one video format
  (Bink). Its other languages are subtitle and menu text files of a few MB in
  all (`portal_stories/resource`, 27 MB with fonts), not worth the risk of a
  missing string.
- Left out: the Mac-side leftovers in the game folder (`SAVE`, `screenshots`,
  `console.log`, `workshop_log.txt`) and `sdk_content` (empty), all a few MB.
- Mel's `portal2` folder can't be shared with the Portal 2 app's copy: iOS
  apps can't read each other's containers, and Mel's engine build expects its
  own.

## 3. What comes from the Mac-side Mel work

Everything in PR #52 is needed, and phase 1 uses it as is:

- the game profile (`tools/games.py`), the non-PIE program, and the dynamic
  i386 `libstdc++` and `libgcc_s`, which are lifted like any other module, so
  "packing them" is just the prep folder;
- the Steamworks SDK 1.40 slot tables, which the PR already regenerated into
  the iOS Steam stub (`runtime/ios/steam_stub_generated.c`);
- the 12-byte D3D9 vertex elements, Bink code roots and FreeType and fortify
  shims.

**Bringing it in:** `feat/mel` is `pr-52-mel` with the fork's `main` (after
round 4) merged into it, not rebased, so PR #52's commits stay as they are
upstream. **Keeping it in sync:** when upstream changes PR #52 (or merges it),
fetch `pull/52/head` into `pr-52-mel` and merge `pr-52-mel` into `feat/mel`
again. The fork's iOS changes are in separate commits on top, easy to rebase
onto upstream's merged version later. Mel's 3.3 GB build in
`../portal2-recomp-mel/build/mel` is reused as is: the worktree now has
`feat/mel` checked out.

## 4. Risks

- **4 GB phones:** Mel's engine is Portal 2's, with larger custom maps and
  more VPKs. The round 4 low-memory guard (half-resolution textures, lower
  mtld3d caps, a memory log at each level load) applies to Mel too, since it
  is in `ios/main_ios.c`. An iPhone 13 may still run out of memory in the
  biggest maps; the log will show it.
- **Mel's 2021 engine build versus the iOS runtime:** the iOS layer reaches
  into the engine by interface versions and vtable slots (IEngineVGui,
  IVEngineClient015, ICvar). They are read from the symbols of Portal 2's
  build. Mel's build may use other versions or slots. `game_state.c` checks
  every answer and switches the feature off if it doesn't behave, so a
  mismatch turns into "no gear" rather than a crash. Phase 1 checks this in
  the simulator.
- **Indirect call roots not found yet:** a `target not recompiled` stop.
  The Mac roots (`config/mel-roots.json`) cover chapters 1–4; later chapters
  may hit new ones on any platform. The same roots serve iOS, so a fix found on
  the Mac fixes iOS too.
- **Back taps and the touch layout:** Mel plays like Portal 2 (one portal
  gun, two portals, no new buttons), so Portal 2's touch layout and back taps
  fit as they are. The in-game settings menu needs the engine checks above.
  Community Maps stays Portal 2 only.
- **Shader cache and first-run time:** Mel's maps compile many shaders on the
  first visit (as on the Mac). Not a blocker, but first loads will be slow.
