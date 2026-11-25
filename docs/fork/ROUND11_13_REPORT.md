# Fork rounds 11 to 13 report (24–25 November 2026)

Fork: [xcaspians/portal2-recomp](https://github.com/xcaspians/portal2-recomp).
Everything below is merged into `main` on this Mac. It is **not pushed to the
fork yet**: the push was blocked by this session's permissions (Needs Caspian
6). Nothing went upstream, and `Prompts/` was never committed.

`make check` on `main`: **320 C checks, 288 Python tests and 62 new x86-64
checks pass** (308 C checks and 264 Python tests at the start).

## In short

| Round | Status |
|---|---|
| 11: fixes from your iPad mini test | **Shipped, all five items.** Installed on the iPad mini, iPhone and iPad Pro. |
| 12: Left 4 Dead 2 | **Mac: main menu, and the first campaign map loads offline** with the Director running, at about 120 fps. Bots and a horde need someone playing; iOS not started. |
| 13: P2CE part 1 | **Shipped.** A separate 64-bit lifter lifts P2CE's own `libtier0.so` (99.9 % of its instructions) and runs 9 of its functions correctly on the Mac. |

**Devices at the end:** all three got the new Portal app, with saves and
settings backed up first (`build/ios/device/<device>-backup-20261125-*`:
the iPad mini's 4 Portal 2 saves and 1 Mel save, the iPad Pro's 2, the
iPhone's none). Every save was still there afterwards. The app is the round 11
build (`bb45c81`); rounds 12 and 13 change nothing on iOS.

**Your mid-queue requests:**

- **Frame rate:** it now applies at once. I didn't hunt for a 60 cap.
- **Memory:** after your message, everything ran one heavy job at a time
  with 2 compile jobs, and I shut the simulator down. Swap went from 18 GB
  to under 4 GB.

---

## Round 11: fixes from your iPad mini test

### 1. Back taps: Start and a moving hand no longer count as taps

**Why it happened** (found in the code, then replayed in recorded motion):

1. **Touches on buttons were invisible to the back-tap detector.** Only the
   touch overlay reported touches. The settings menu's buttons, and the
   calibration screen's own **Start**, shook the device without the detector
   knowing a finger was on the glass.
2. **The calibration screen counted every jolt, even rejected ones.** It
   kept any strong jolt, even those the detector had already thrown out (a
   touch nearby, a hand moving, the device turning). It also listened with
   a gate set to a quarter of the in-game thresholds, so the smallest jolt
   passed.
3. **Nothing asked "was the device steady just before?"** A short grip
   shift right before a jolt passed the old 300 ms average.

**What it looks like** (traces replayed through the detector, old rules
against new):

| Trace | Old rules | New rules |
|---|---|---|
| Real back taps, left ×4 and right ×4 | 8 of 8, right side | 8 of 8, right side |
| Pressing Start: a jolt at the touch, another as the hand lets go 130 ms later | a portal fired (calibration: both counted) | both ignored ("touch on screen") |
| A grip shift: the device turned at 1 rad/s just before a jolt | a portal fired | ignored ("not steady before") |

**The fix** (same rules in calibration and in play):

- Every touch on the screen, on any button, now reaches the detector.
- A jolt is ignored from 300 ms after a touch starts or ends.
- A jolt counts only if the device was steady for the 150 ms before it.
  A smooth turn while aiming with the gyro still counts as steady.
- Calibration listens at **half** the in-game thresholds (was a quarter),
  and a calibration never sets them below that.
- Every window and threshold is in **`ios/device_defaults.txt`** (`backtap`
  lines).
- With the Log setting on, every jolt logs its strength, the nearest touch,
  how steady the device was, and the verdict.

The traces live in `tests/data/backtap/`. A real recording from **Record
taps**, with an `# expect` line added, is checked the same way.

### 2. Portal 2 can't be removed

In Manage games, Portal 2 has no Remove: one line says it's the base the
other games run on. The Mac installer's Devices section says the same. Mel
keeps Remove.

### 3. Mel borrows Portal 2's files: 2.36 GB less

**Measured:**

- Mel's copy of Portal 2's `portal2` folder is 2.4 GB, almost all of it
  packed in one VPK. Of its 14,977 files inside, **14,656 are byte for
  byte in Portal 2's own VPK**.
- Of the rest, 49 are older versions of Portal 2 files and 272 are Mel's own
  (27 MB together).
- `platform` and `portal2_dlc2` are tiny, so they stay as they are.
- **Mel goes from 9.18 GB to 6.83 GB.**

**How it works:**

- The slim pack keeps Mel's VPK *index* as `portal2/pak01_slim.vpk`. It
  lists exactly Mel's original files. Each one points into Portal 2's
  archives, or carries its own bytes when Portal 2's differ.
- The runtime gives the engine that index, and Portal 2's archives, when
  Mel opens its own (`P2_BASE_GAME_ROOT`).
- Mel's loose files stay its own.
- So Mel sees exactly the files and bytes of its full copy, and nothing of
  Portal 2's.

**A bug caught on the way:** my first design put Mel's differing files as
loose files in front. The Mac test showed Source searches every VPK before
any loose file, so a Portal 2 HUD layout won over Mel's. The index design
avoids that.

**Checked on the Mac with the device's file layout:**

- I made a save in Mel with the slim pack, then loaded it.
- The game console was identical to the full copy's.
- `lsof` showed Mel reading the slim index plus 26 of Portal 2's archives.

A test checks that every file of the full copy is covered, with the same
bytes (`tests/test_mel_slim.py`; `P2_REAL_MEL_SLIM=1` runs it on the real
games).

**Missing Portal 2:** if Portal 2's files are gone, the picker shows "Needs
Portal 2" with a sentence, and the game refuses to start with the same
sentence instead of crashing.

**From now on:**

- `tools/ios_device.sh mel data push` and the installer's Download send the
  slim pack (about 6.8 GB, Portal 2 first).
- `--full` still sends the whole copy.

**Your iPad mini keeps its full Mel copy** until you run the switch-over
(Needs Caspian 1).

### 4. One set of controls and settings for every game

**Shared by every game:**

- the touch layout
- button size and opacity
- back taps: on/off, sensitivity and calibration
- gyro and camera speed
- frame rate
- the FPS counter, and every other general setting of the gear menu

**Still per game:** saves, `launch.args`, the version line and the iCloud
saves switch.

**Migration:**

- Portal 2's settings became the shared set.
- Mel's old settings are untouched in the app's preferences, and copied
  once to `Documents/mel-userdata/settings-before-shared.plist`.

**Layouts:** saving the layout in one game never moves a button only the
other game has. Checked by a test that changes the layout in one game and
reads it from the other.

### 5. Frame rate applies at once, and an FPS counter

**Applies without a restart:**

- Metal reads its frame cap once, at start. So it now starts at the
  screen's maximum (120 on ProMotion, 60 otherwise), and the app paces
  any lower cap itself after each frame.
- On a ProMotion screen, the display's rate follows every change too.
- A test checks that Auto asks for 120 on a 120 Hz screen and 60 on a 60 Hz
  one.
- **On the iPad Pro tonight:** cap 120, the display measured at 120 Hz, and
  **120 fps** on the menu.

**FPS counter:** gear menu > Display > **FPS counter**, off by default and
shared by every game. It shows:

- the renderer;
- frames per second over half a second;
- the display's current refresh rate and the cap asked for, e.g.
  `120 HZ CAP 120`.

**Where it sits:**

- **Top centre** in Portal's layout, because your blue and orange portal
  buttons sit under the old top-left spot.
- Top left in Half-Life 2's layout.
- A test checks it never covers a button, at any button size.
- A tap on it hides it.

---

## Round 12: Left 4 Dead 2 (local and offline only)

**Inputs:** depot 554 was on the Mac and complete (68 files, all i386).

**Offline, always:**

- Every launch gets `-insecure +sv_lan 1 -nomaster`, whatever the arguments.
- The Steam bridge refuses server lists, server pings, lobby searches and
  lobby joins before Steam sees them (`P2_STEAM_OFFLINE`).
- Nothing reached Valve's servers.

### How far it got (Mac)

- **Main menu:** reached. The menu's background video plays, at about
  **150 fps**.
- **`map c1m1_hotel`:** the map loads ("Host_NewGame on map c1m1_hotel",
  "Redownloading all lightmaps"). The client joins the local server, the
  Director's script starts, and it places the wandering infected
  ("Initiating Reserved Wanderers").
  - Frame rate: about **120 fps** with the window focused. An unfocused
    window drops to about 14 fps, because the engine sleeps 50 ms a frame
    when it isn't focused.
  - Memory: **2.0 GB**.
  - It ran 4 minutes without a crash.
- **Not seen yet:** survivor bots and a horde. Both need a player actually
  in the game, and an unattended test run stays at the intro.
  `director_force_panic_event` had nothing to act on. That's the first thing
  to try with a keyboard (Needs Caspian 3).
- **iOS (step 4): not started.** It's gated on step 3 working fully. Left
  for it: L4D2's own app, a touch layout (move, look, shoot, shove, use,
  reload, crouch, item slots, flashlight), its engine-interface numbers for
  the gear menu (`VEngineClient013`), and memory checks under the 4 GB guard.

**What's left for full play on the iPad mini:**

1. Play c1m1 on the Mac by hand: bots, a horde, a save.
2. Keep running discovery for new code paths through a whole campaign.
3. The iOS app and touch layout.
4. Memory on the iPad mini: 2 GB on the Mac for one map, so the 4 GB guard
   settings will matter.

### Log lines from the bring-up

```
initializing client.so (820 constructors) ... initializing server.so (867 constructors)
guest trap ...: Steam: STEAMTIMELINE_INTERFACE_V001::slot3 has no flat function in SDK 1.53a
guest trap ...: Steam: SteamNetworking006::slot8 has no flat function in SDK 1.53a
guest trap at 0x1522fc80: target not recompiled          (valve_avi.so: a new root)
guest trap ...: longjmp to a frame that has already returned (called from 0x1100cb0a, libvstdlib)
guest trap at 0: target not recompiled                   (SteamGameServer014 slot 43)
guest trap ...: SteamMatchMakingServers002::PingServer is not bridged
Host_NewGame on map c1m1_hotel ... Initiating Reserved Wanderers ... [diag] 120 fps
```

### How this went (plain English)

**What each stage did:**

- **Gameprep (2 seconds)** takes L4D2's Linux files (31 libraries plus the
  `hl2_linux` program) and lays them out in memory the way Linux would. It
  also connects each library's references to the others ("this calls
  `memcpy` from libc").
- **Lifting (about 2.5 minutes)** reads every machine instruction it can
  find (135,000 functions, 17 million instructions) and writes an
  equivalent line of C for each, about 1.6 GB of C. Compiling that C into
  one Mac program took 13 minutes, and gave a 413 MB executable.
- **Imports** are the things the game asks of Linux or other libraries
  (maths, files, sound, Steam). The runtime answers each with a small
  translator called a **shim**. The first lift listed 27 missing, and each
  got a shim:
  - fast-maths versions of `log`, `exp` and `pow`;
  - `bsearch`, `unsetenv` and `wcsdup`;
  - voice-capture calls, answered "no microphone";
  - two Steam start-up calls.
- **Roots** are where code starts. The lifter finds most of them by itself,
  but some code is only reached through a pointer worked out while the game
  runs. When the game jumps somewhere that wasn't lifted, it stops with
  "target not recompiled". `discover_game.py` then adds that address and
  rebuilds just that file. L4D2 needed only 2 so far (in the video player).

**What broke, and why:**

- **Steam versions.** L4D2 uses Portal 2's Steam interfaces, plus two newer
  ones:
  - **Timeline**, which only feeds Steam's game-recording timeline. Its
    calls are now ignored.
  - **The old Steam socket calls.** Offline, they answer "none".
- **Coroutines.** L4D2's matchmaking runs small tasks that pause and resume
  ("coroutines"). Valve's Linux version does it by copying a slice of
  memory and jumping back into the middle of a function that has already
  finished. A recompiled program can't do that jump, because every function
  of the game is a real function on the Mac. So the runtime now runs each
  coroutine on its own thread. Only one runs at a time, passing control
  back and forth like a baton.
- **A table that was too short.** The local server asks Steam's game-server
  object for its 44th function, but the Steam SDK this project reads lists
  42. The call ran off the end of the table and jumped to address 0. A new
  diagnostic (`P2_TRAP_PEEK`) follows pointers at a crash and named the
  table. Calls past the end now answer 0 offline, and every Steam table
  has 16 spare named entries, so a future overrun says what it is.
- **Matchmaking pinged a server** as the map loaded. With the offline rule,
  server pings and lists are now refused outright.

**The renderer path:** L4D2 draws with Direct3D 9 through `shaderapivk`,
the same route as Portal 2. The runtime hands those calls to mtld3d, which
turns them into Metal. It worked first time.

---

## Round 13: P2CE part 1, a 64-bit (x86-64) lifter

(Built by a helper agent in its own copy of the repository, then checked
and merged here.)

- **Roadmap:** `docs/fork/P2CE_PLAN.md` splits the job into rounds A to K,
  each with a size and a milestone you can check. That's about 13–17 rounds
  to the main menu on the Mac, then 3–4 for iOS. This round was A.
- **ELF64 in:** `tools/elfprep64.py`, a new file. `tools/elfprep.py` (32-bit)
  is untouched.
- **The first 64-bit lifter** (`tools/lift64.py`) and **runtime**
  (`runtime/x64/`), both new files. They cover:
  - the 16 registers and their width rules;
  - addresses relative to the current instruction;
  - the 64-bit calling convention;
  - jump tables and import calls;
  - the common integer, atomic and SSE2 instructions.
- **Game memory is the Mac's memory:** a pointer in the game is a real
  pointer on the Mac, which works because both sides use the same sizes.
- **Test harness in `make check`:** small 64-bit test programs are
  assembled on the Mac, linked by a tiny linker written for the test
  (there's no Linux linker here), lifted and run. 62 answers are checked,
  in seconds.
- **The milestone:** P2CE's own `libtier0.so` (from depot 440005).
  - Across the whole library, **238,670 of 238,816 instructions are
    handled (99.94 %)**. What's left is rare: direct system calls, shadow-
    stack instructions, some packed and old x87 maths.
  - **Nine of its functions run correctly on the Mac**: string copy and
    compare, a timer, atomic exchanges, a memory clear handed to the Mac's
    `memset`, a settings flag.
  - All 12 live checks pass. Run them with `make p2ce-tier0-check`; they
    aren't part of the default `make check`.
- **Inputs:**
  - depot 440003 (needed to run P2CE) isn't on the Mac;
  - depot 601363 (Revolution's binaries) isn't either, so the comparison
    with P2CE couldn't be made.
- **The i386 path is untouched.** After both merges, Portal 2 loaded
  `sp_a1_intro1`, Mel loaded a save, L4D2 runs its map, and `make check`
  passes. Portal 2's prepared files come out the same as before.

**Learning notes:** in plain English, in
[P2CE_ROUND13_NOTES.md](P2CE_ROUND13_NOTES.md). They cover what's different
about 64-bit code (twice as many registers, and arguments passed in
registers instead of on the stack), why the lifter needed a separate 64-bit
version, and why game memory can simply be the Mac's memory.

**Next round (B in the plan):** the rest of the instruction set, faster
generated code, and whole libraries compiled.

---

## Bugs found, and their causes

1. **Back taps from Start and a moving hand.** There were three causes:
   - touches on buttons were invisible to the detector;
   - calibration counted jolts the detector had rejected, through a gate a
     quarter of the in-game one;
   - there was no "steady before" check.
2. **The slim pack's first design** would have let a Portal 2 file win over
   one of Mel's. Source searches every VPK before any loose file. The Mac
   boot test caught it.
3. **The engine refuses VPK version 2.** The slim index has to be written as
   version 1, like Valve's own.
4. **Every Steam proxy table ended exactly at the SDK's size**, so a game
   built against a longer one jumped to address 0. It now traps with a name.
5. **Metal's frame cap is fixed at start-up**, hence the old restart. It
   now starts at the screen's maximum and the app paces lower caps.

## Hand re-test checklist (iPad mini first)

1. **Calibrate back taps** (gear > Back taps > Calibrate):
   - pressing **Start** should no longer count as a tap;
   - shifting your grip shouldn't count either;
   - then tap 8 left, 8 right, and Save and test.
2. **Play a Portal 2 chamber** with back taps on. Shifting your grip or
   pressing an on-screen button should never fire a portal.
3. **Shared settings:** move a button in the layout editor in Portal 2,
   then open Mel: it's in the same place. Change Button size in Mel: Portal
   2 has it too.
4. **Manage games:** Portal 2 shows the "base game" line and no Remove; Mel
   has Remove (don't press it).
5. **FPS counter:** gear > Display > FPS counter, on. The readout sits at
   the top centre, clear of the portal buttons. A tap on it hides it.
6. **iPad Pro:** switch Frames per second between 60 and Auto *without
   restarting*. The counter's cap and Hz follow (60 ↔ 120), and so does
   Apple's Metal HUD.
7. **Mel** still opens from the picker and loads your save. This is still
   the full copy until you run the switch-over.
8. **iPhone:** opens and plays as before.

## Needs Caspian

1. **Mel slim switch-over on the iPad mini** (frees about **2.4 GB**), one
   command:

       P2_DEVICE=00008160-001C693804C14036 tools/ios_device.sh mel slim switch

   - It copies the slim index (30 MB).
   - It boots Mel with it and checks the console that your newest Mel save
     (or the first map) loaded.
   - Only then does the app delete Mel's own copies of Portal 2's archives.
   - If the boot test fails, nothing is deleted.
   - Keep the iPad unlocked and awake for the 2 minutes it takes.
2. **Depot downloads** (Steam console):
   - `download_depot 440000 440003`: P2CE, needed to run it later;
   - `download_depot 601360 601363`: Portal: Revolution's binaries;
   - (HL2's depot 224 is now on the Mac; thanks.)
3. **L4D2 on the Mac, by hand:**

       python3 tools/run_game.py --game l4d2 --show -- -game left4dead2 -novid -windowed -w 1280 -h 720 +map c1m1_hotel

   Walk out of the first room: do the survivor bots follow? Does a horde
   come? Tell me where it stops, if it does. It's offline whatever you type.
4. **Screen recording permission** for Terminal (System Settings > Privacy &
   Security > Screen Recording), if you want screenshots in the next
   report. My screenshot attempt was refused.
5. **Left from earlier rounds:**
   - the iCloud container tick in Xcode;
   - removing the old standalone Mel app from the iPad mini;
   - the Steam Web API key;
   - the controller test;
   - the `portal2_dlc3` addon test.
6. **Push to the fork:** `git push fork main` (local `main` is 27 commits
   ahead of `fork/main`; the auto-mode permission check refused the push).
