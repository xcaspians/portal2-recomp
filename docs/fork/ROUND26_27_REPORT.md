# Rounds 26 and 27: combined report

Most of both rounds is done, but two things are stuck:
- **PR #22 (phone controller) isn't merged.** GitHub's macOS runner never picked up its test job, twice, and auto mode blocked me from merging over the red status. `make check` passes locally.
- **P2CE can't reach the console banner yet.** Valve's own Steam start-up call waits forever for the Steam client on this Mac.

The new build (`0.6.0-fork.1 (efeb311)`) is on the iPhone, iPad mini and iPad Pro, with saves backed up first. It includes the unmerged Part B code. The Natif Installer was rebuilt (`Natif-Installer-v0.4.0-240-gefeb311`). Nothing was tested on the iPhone or the mini.

## Round 26

### Causes and what changed

- **L4D2 touch buttons:**
  - **Cause:** L4D2's `config.cfg` has controller binds only, on both iPads. The touch buttons sent keys and mouse clicks, which L4D2 had nothing bound to. The stick worked because it goes through the virtual controller.
  - **Fix (as you approved):** in L4D2 the touch buttons now press the virtual controller's buttons and triggers. Slots 1 and 2 have no controller bind, so they go as `slot1`/`slot2` commands. No `config.cfg` was changed.
  - **Checked on the Pro:** crouch lowers the view and comes back; flashlight toggles.
- **"Nothing works", then "stick works":** last round's build re-creates the virtual controller after L4D2 resets it, which brought the stick back. The buttons were never covered by that.
- **Missing Log switch:**
  - The app on the mini does contain it: last item under Diagnostics, in French "Diagnostic › Journal". I couldn't find anything that hides it. My best guess is the Settings app showing an out-of-date copy of the page.
  - **Change:** the Log switch is now also in the gear menu ("applies on next launch").
  - **New test:** every game's Settings page must end with Log.
- **"Remember my game" turning itself on:**
  - **Cause:** the launcher scrolls, and its buttons are UIKit controls. By default, a slow drag that starts on a control still counts as a tap, even when the finger lifts up to about 70 points away. A scroll that began on the Remember pill flipped it.
  - **Fix:** drags now always scroll, a pill only reacts to a real tap, and every change is logged. Tested.
- **Customize controls in L4D2 (item 2):**
  - **Cause:** the editor row was tied to the "Show portal buttons" setting, which L4D2 and HL2 don't have.
  - **Fix:** the editor is now in every game's gear menu, and the launcher's Controls lists one row per installed game. Each game keeps its own layout, so moving L4D2's jump no longer moves Portal 2's.
  - **Checked on the Pro:** edited from the launcher, and the change was still there after a relaunch.
- **Play Online (item 8):** the notice now shows the moment the game asks for a lobby. It explains local play and has a **Play nearby** button that opens Host/Join. Checked on the Pro.
- **Layouts through iCloud (item 10):**
  - They use the existing container: `Documents/Layouts/iPad.txt` or `iPhone.txt`, newest wins. iPhones and iPads never read each other's file (tested).
  - Without iCloud, the editor says layouts are saved on this device only.
  - **On the Pro:** it uploaded, and after I wiped its local copy it got the layout back from iCloud. The hop between your two iPads is untested.
- **Phone controller (items 4–7):**
  - **Per-game buttons:** the page now shows whichever game is running, in the same style and sizes as the in-game touch controls. The sticks follow your thumb, the move stick walks from a slight push, and haptics work where the phone allows.
  - **Using the Sauce app as the controller:** "Use as controller" on the iPhone finds the iPad over the network and connects with one tap. The QR page still works for phones without Sauce.
  - **What I verified:**
    - The Pro advertises itself correctly; I checked from the Mac.
    - A simulated iPhone connected in-app at 48 ms and showed L4D2's buttons.
    - The simulator itself couldn't discover the iPad, so on a real iPhone discovery is untested.
  - **HL2 and Portal:** they have no trigger binds, so firing from the phone may do nothing there.
- **Emote button (item 7):**
  - It appears only in co-op, at the bottom centre, and the layout editor can move and resize it.
  - It sends `+mouse_menu_taunt`, but in my split-screen test **the gesture wheel didn't open**. That needs your hand check.

### Switching games without quitting (item 9)
Not possible. Each game runs inside the same app process, and its engine can't be shut down and replaced by another while that process keeps running. iOS also doesn't let an app relaunch itself.

The closest options, which I haven't built:
- "Switch game" closes Sauce and leaves a notification that reopens it straight to the launcher with one tap.
- Home-screen quick actions: long-press the Sauce icon and pick a game.

Tell me which one, if either.

### L4D2 split-screen (item 3)
It's partly there:
- The engine reports "Game supporting (2) split screen players", and `ss_map c1m1_hotel` loads a level.
- With two controllers connected, it still showed a single view. Pressing Start or A on the second controller didn't bring player 2 in.
- L4D2's "-insecure" warning also stays on screen over the level.

The missing piece is how L4D2 on PC lets a second player join. All of this used existing test hooks, so no code changed and no branch was made. I'd estimate about a round to finish, with your real controller for testing.

## Round 27

- **`make p2ce-launch`:**
  - `inputsystem.so` and 12 more systems now lift and compile (about 3 million instructions, one trap). Those are the material system, data cache, studio render, both physics libraries, audio, sound, scripting, VScript, localisation, the scene cache and VGUI.
  - **But the run stops earlier than in round 23.** The last line is:
    ```
    [S_API] SteamAPI_Init(): Loaded '.../steamclient.dylib' OK.
    run64: stopped after 150 s
    ```
  - **What's blocking:** the program sits inside Valve's own `SteamInternal_SteamAPI_Init`, waiting for the Steam client to answer. The same thing happens with `main`'s code, so it isn't this round's change.
  - **My guess:** the Steam client's state on this Mac. Restarting Steam should clear it.
- **Steam bridge (item 2):** native calls now handle structures returned through a hidden pointer, and more than eight arguments, packed the way Apple's ABI wants. Tested in `make check`; no key or password is read anywhere.
  - Valve's library has no method names, so a method needing this has to be listed by its function address. None of P2CE's calls so far needs it.
- **Renderer (item 3, proposal only):** `docs/fork/P2CE_RENDERER_DECISION.md` recommends building DXVK natively for Apple arm64 and running it on MoltenVK. That's about 3 rounds, against 6–10 for a Direct3D 11 to Metal bridge of our own. Nothing is built; it waits for your OK.
- **Piece E:** not started, because the Steam block stops the launcher before any window.

## PRs
- [#20](https://github.com/xcaspians/portal2-recomp/pull/20) L4D2 controls, Log switch, editor: **merged** (green CI).
- [#21](https://github.com/xcaspians/portal2-recomp/pull/21) Play Online, Remember game, iCloud layouts: **merged** (green CI).
- [#22](https://github.com/xcaspians/portal2-recomp/pull/22) phone controller and emote: **open** (runner never ran; local `make check` passes).
- [#23](https://github.com/xcaspians/portal2-recomp/pull/23) round 27: **open**, CI pending.

## Needs you
1. **PR #22:** merge it yourself, or tell me it's OK to merge on the local `make check` under the runner-outage rule.
2. **Restart Steam on the Mac,** then I can continue P2CE from the systems.
3. **Renderer route:** your OK for DXVK on MoltenVK, or pick the Metal bridge. Also whether `setup.sh` may fetch and build DXVK and MoltenVK from source.
4. **Switching games:** pick a fallback, or none.

## Hand re-test checklist
1. **Mini, L4D2 touch:** in a level, try jump, reload, use, crouch (tap on, tap off), fire, shove, flashlight and slots 1–5. All should work.
2. **Mini, Log switch:** force-quit the Settings app, then look at Sauce › Diagnostic (Diagnostics in English): the switch is "Journal" in French, "Log" in English. It's also in the gear menu.
3. **L4D2 customize controls:**
   - gear menu › Edit button layout, then move jump and Save, and check it stays after a relaunch;
   - launcher › Controls › "Edit button layout: Left 4 Dead 2" should open the same editor;
   - Portal 2's buttons shouldn't move.
4. **iPhone as controller:**
   - on the iPad, gear menu › Phone controller;
   - on the iPhone, launcher › Use as controller: the iPad should be listed, and one tap connects;
   - Close should return you to the list.
5. **Phone controller:** compare its look with the in-game buttons in Portal 2, L4D2 and HL2. Each game should show its own buttons, and the page should change when you relaunch Sauce in another game. Check haptics.
6. **Emote:** in Portal 2 split-screen co-op, the wave button appears at the bottom centre. Hold it and slide: does the gesture wheel open? It didn't in my test. Out of co-op the button should be gone.
7. **Play Online:** Play Cooperative Game › Online should show the notice at once, and its Play nearby button should open Host/Join.
8. **Remember game:** scroll the launcher starting on the pill. It shouldn't change unless you tap it.
9. **Switching games:** for now gear › Switch game still closes the app.
10. **Layouts between your iPads:** edit a layout on the Pro, open Sauce on the mini, and it should match. iPhone layouts stay separate.

Screenshots are in `/Users/caspian/Documents/Claude Code/portal2-recomp/docs/fork/round26/`. They aren't committed yet because a round report would need another PR.
