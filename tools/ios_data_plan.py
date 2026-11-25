#!/usr/bin/env python3
"""What `tools/ios_device.sh mel data push` copies to the device, and how much:
one "copy<TAB>source<TAB>destination<TAB>bytes" line per folder or file, then
"total<TAB>bytes" (destinations relative to the app's data container).

    ios_data_plan.py --game-root DIR --linux-root DIR --prep DIR --folders "bin platform ..."

The game folders go to Documents/game/<name>, the Linux depot to
Documents/linux and the prepared modules (the i386 libstdc++ and libgcc_s
included) to Documents/prep, as `tools/ios_device.sh data` does. A mod folder
holding the player's own leftovers from the Mac (Mel keeps SAVE, screenshots
and logs in its game folder) is copied entry by entry without them: saves on
the device live in Documents/<game>-userdata, and the device never needs the
Mac's.
"""
import argparse
import os
import sys
from pathlib import Path

# The player's files and logs, never game data (docs/fork/MEL_IOS_PLAN.md).
LEFTOVERS = {'save', 'screenshots', 'console.log', 'workshop_log.txt', 'demoheader.tmp', 'voice_ban.dt',
             'modelsounds.cache', 'test.txt'}


def size_of(path):
    path = Path(path)
    if path.is_file():
        return path.stat().st_size
    total = 0
    for root, _dirs, files in os.walk(path):
        for f in files:
            try:
                total += (Path(root) / f).stat().st_size
            except OSError:
                pass
    return total


def plan(game_root, linux_root, prep, folders):
    """[(source, destination, bytes)]; a folder missing on the Mac is skipped (as `data` does)."""
    out = [(str(prep), 'Documents/prep', size_of(prep)), (str(linux_root), 'Documents/linux', size_of(linux_root))]
    for name in folders:
        src = Path(game_root) / name
        if not src.exists():
            continue
        entries = list(src.iterdir()) if src.is_dir() else []
        if any(e.name.lower() in LEFTOVERS for e in entries):
            for e in sorted(entries):
                if e.name.lower() in LEFTOVERS or e.name.startswith('.'):
                    continue
                out.append((str(e), f'Documents/game/{name}/{e.name}', size_of(e)))
        else:
            out.append((str(src), f'Documents/game/{name}', size_of(src)))
    return out


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--game-root', required=True)
    ap.add_argument('--linux-root', required=True)
    ap.add_argument('--prep', required=True)
    ap.add_argument('--folders', required=True, help='space-separated (ios_data of the game profile)')
    a = ap.parse_args(argv)
    for p in (a.game_root, a.linux_root, a.prep):
        if not Path(p).is_dir():
            print(f'{p}: no such folder (config/local.json, gameprep)', file=sys.stderr)
            return 2
    rows = plan(a.game_root, a.linux_root, a.prep, a.folders.split())
    for src, dest, n in rows:
        print(f'copy\t{src}\t{dest}\t{n}')
    print(f'total\t{sum(n for _, _, n in rows)}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
