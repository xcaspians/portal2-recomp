# L4D2 black screen, history scrub, Portal and the L4D2 particles (25 November 2026)

Three jobs, in the order asked, with two standing-rule changes along the way
(the iPad mini is install-only, the Natif Installer is rebuilt with every
build) and two answers from Caspian (scrub the history, then the P2CE Steam
connection later; depots 556–559 and Portal's are downloaded).

## 1. Left 4 Dead 2: black screen on both iPads — fixed

**What you saw:** the app opened, the menu music played, the screen stayed
black, on the iPad mini and then on the iPad Pro.

**The cause, in plain English:** Left 4 Dead 2 keeps its shaders (the small
programs that draw every surface) in a folder called `hl2`, which its
`gameinfo.txt` lists as the last place to look. The list of folders our tools
copy to the iPad left `hl2` out. Without shaders nothing can be drawn, so the
game ran (music, 60 fps of black frames) with nothing on screen. The Mac never
hit this because it reads the Steam folder directly.

**A correction:** last night's report said L4D2 worked on the Pro. Round 15
measured its frame rate and memory there but never took a screenshot, so it
had probably never drawn on iOS. Nothing regressed after round 15; the Steam
stub change, the installs and the README work weren't involved.

**A second problem on the mini only (black hands):** once the shaders were
there, the mini showed the world but the player's hands were a black
silhouette. The graphics library reports an NVIDIA graphics card ID chosen
from the Apple chip, and the engine looks that ID up in Valve's
`dxsupport.cfg` to pick a quality level. The A20 Pro got 0x034B, which that
file lists as a 2003 GeForce FX, so the mini ran in DirectX 9.0 mode, where a
shader combination the hands need is missing. L4D2 now reports 0x051F on
every chip (the ID the M5 iPad Pro already had, so its saved video settings
didn't reset). Portal 2, Mel, Portal and Half-Life 2 keep their IDs, because a
new ID makes Source reset a player's saved video settings once.

**What shipped (PR #2):**
- `hl2` added to L4D2's iOS data list, and a guard: `tools/ios_game.py` now
  refuses any data list that leaves out a folder the game's `gameinfo.txt`
  searches, so this can't happen to another game (test with L4D2's own
  search paths).
- `gpu_device_id` in the iOS game table, set for L4D2 only (check that it is
  outside the GeForce FX range, and that the other games have none).
- Tested on the iPad Pro, then on the mini (the one-off allowed today): the
  menu draws, `c1m1_hotel` loads and plays with the bots, about 60 fps (the
  frame-rate setting on both is 60) and 1.7 GB of memory. The app was closed
  after every run.

## 2. Your real name scrubbed from the fork's history — done

**Backups first** (outside the repo, in
`Documents/Claude Code/portal2-recomp-history-backup-20261125/`): a git bundle
of every local branch, tag and remote ref, and a mirror clone of the fork as it
was on GitHub.

**How:** every author and committer with your real name or your Gmail
address (PR #1's merge included) became "Caspian
<208339910+xcaspians@users.noreply.github.com>"; every mention of your first
name in commit messages and in files across history became "Caspian" (old
reports, docs, a few code comments and test save names). Your surname only
appeared in the identities.

**Special handling, and why:**
- **Upstream's history had to stay identical.** The usual tool,
  `git-filter-repo`, strips commit signatures, and GitHub signs the merge
  commits of kanapitsas/source-recomp. Running it changed every upstream
  commit and tag, which would have cut the fork off from upstream. Instead I
  wrote a small filter over git's own export (`fast-export
  --signed-commits=verbatim`): commits that don't mention you come out byte
  for byte identical. Result: all five tags (v0.1.0 to v0.4.0, host-deps-1)
  and upstream's main keep their hashes.
- **One signature was dropped on purpose:** PR #1's merge commit, the only
  GitHub-signed commit that had to change (a changed commit's old signature
  would show as "Unverified").
- **Force-pushed to the fork only** (by its explicit URL): 30 branches
  rewritten, the upstream-only branches and the tags untouched.
- **Verified on GitHub** from a fresh clone: 668 commits and 2,209 file
  versions across all 35 branches and 5 tags, no trace of your name or your
  Gmail address anywhere (control words found, so the scan works).
- **What I can't change:** GitHub keeps its own read-only copies of pull
  request commits. `refs/pull/1/head` (PR #1's "Commits" tab) still shows the
  126 old-identity commits. Only GitHub Support can purge those (or deleting
  and re-creating the repository). See Needs Caspian.
- **GitHub's own merges use your account email:** merging PR #2 through
  GitHub put your Gmail address on the merge commit. I redid that merge
  commit locally as Caspian and force-pushed main (PR #2 still shows as
  merged). From now on I merge locally and push, so GitHub never writes the
  merge commit. To fix it at the source, see Needs Caspian.
- **Local repo:** the same filter ran over the local branches, so every
  branch the fork has matches it exactly; the repo's git config now commits
  as Caspian with the noreply address. (`feat/l4d2` locally is 23 commits
  ahead of the fork's copy, as before: that work reached the fork through
  main.)

## 3. Portal and the L4D2 particles

**Portal (depots 401 and 404): in the one Portal app — done (PR #3).**
- Built on the Mac first (18 minutes); the first chamber renders on Metal and
  ran 90 seconds without a crash.
- On iOS it joins Mel in the Portal app: the game picker offers it, with
  Portal 2's touch layout (both portal buttons), the settings menu and
  controller handling. Its data lives in `Documents/portal1` (4.4 GB), its
  saves in `Documents/portal-userdata`.
- **Bug found and fixed:** the first launch stopped on "Portal 2 game data is
  incomplete". The Portal app is built as Portal 2's program, and Portal is a
  different program (`hl2_linux`), so it ran Portal 2's data check. Its launch
  file now names its own program (`--program hl2_linux`), with a test.
- **Bug found and fixed:** the picker showed Portal 2 at "0 KB". Its size was
  counted over the folders "Remove" may delete, and Portal 2 is never
  removable (round 11). It now counts its own folders (12.5 GB on the Pro),
  with a check.
- **On the iPad Pro:** the picker lists Portal 2, Mel and Portal; Portal's
  first chamber at 120 fps, 1.4 GB; the settings panel opens on Portal's main
  menu.
- **On the iPad mini (install only):** saves backed up (6 saves), the new
  Portal app installed, Portal's data copied. Not launched there.

**L4D2 particles (depots 556–559): not there.** Those four depots are only
language packs (French, German, Spanish, Russian). None of the 18 missing
particle materials (round 15's 19 counted one twice) is in them, so there's
nothing to wire in. They are most likely in depot 555, which answered "no
subscription", or in content the Mac edition never gets. The effects still
draw as Source's error material; everything else is unaffected.

## GitHub Actions

You enabled Actions on the fork, so I tested them on PR #2:
- **They run, with no billing or minutes problem.** The main `test` job
  (make check and the UBSan checks) passed in 2 minutes 13 seconds on the
  first run.
- **But GitHub couldn't supply Apple Silicon Macs:** the second job (Python
  3.9 tests), and then every later job, was cancelled after waiting 15
  minutes: "The job was not acquired by Runner of type hosted even after
  multiple attempts", with "Due to capacity constraints, jobs targeting macOS
  arm64 runners may experience longer queue times". Five cancellations in a
  row, then PR #3's run sat queued for over an hour.
- I made the two jobs run one after the other (`needs: test`) in case one Mac
  at a time was the limit; it didn't help, so this is GitHub's capacity, not
  the account.
- **What I did:** merged on a passing local `make check` (which runs the same
  Python tests on Python 3.9.6, plus the runtime checks), with a note on each
  PR. I'll keep trying Actions on every push and merge on green whenever it
  runs.

## Natif Installer — rebuilt

`build/installer/Natif Installer.app`, version shown in its window
(`v0.4.0-158-g6bce301` at this build). Its Devices section now offers Portal,
Half-Life 2 and Left 4 Dead 2 next to Portal 2, Mel and Workshop maps (a test
keeps the window's list equal to the planner's).

## Standing rules recorded

`CLAUDE.md` has the fork's rules: Caspian only, merging (Actions, local
`make check` as the fallback), testing on the iPad Pro with the mini
install-only, and rebuilding the installer with every build. There is no
`PROGRESS.md` in the repository, so they live in `CLAUDE.md`.

## Needs Caspian

1. **GitHub email privacy:** github.com > Settings > Emails, tick "Keep my
   email addresses private" and "Block command line pushes that expose my
   email". Then GitHub's own merges and web edits use the noreply address.
2. **PR #1's old commits on GitHub:** `refs/pull/1/head` still holds the old
   identity. To remove it, ask GitHub Support (support.github.com, "remove
   cached views / pull request refs") to purge PR #1's refs on
   xcaspians/portal2-recomp, or delete and re-create the repository.
3. **The history backups** in
   `Documents/Claude Code/portal2-recomp-history-backup-20261125/` contain the
   old history: keep them until you're happy, then delete them.
4. **GitHub Actions:** nothing to do. If the macOS arm64 queue stays stuck,
   one option is to run the Python-only job on Linux (ubuntu-latest); say if
   you want that.
5. **L4D2 depot 555:** if it ever becomes available ("no subscription" now),
   `download_depot 550 555` and I'll check it for the particle materials.
