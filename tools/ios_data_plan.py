#!/usr/bin/env python3
"""What `tools/ios_device.sh mel data push` copies to the device, and how much
(the slim pack by default: --shared and --overlay, tools/mel_slim.py):
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


def is_archive(path):
    """A shared folder's VPK archive (pak01_dir.vpk, pak01_000.vpk...): the slim pack reads Portal 2's."""
    n = path.name.lower()
    return path.is_file() and n.startswith('pak01_') and n.endswith('.vpk')


def plan(game_root, linux_root, prep, folders, dest_root='Documents', shared=(), overlay=None):
    """[(source, destination, bytes)]; a folder missing on the Mac is skipped (as `data` does).
    dest_root: Documents, or Documents/mel for Mel hosted in the Portal app.
    shared: the slim pack's folders read from Portal 2 (tools/mel_slim.py): copied
    without their VPK archives. overlay: the slim pack's own extra files
    (mel_slim.py build), copied over the game folders after them."""
    out = [(str(prep), f'{dest_root}/prep', size_of(prep)), (str(linux_root), f'{dest_root}/linux', size_of(linux_root))]
    for name in folders:
        src = Path(game_root) / name
        if not src.exists():
            continue
        entries = list(src.iterdir()) if src.is_dir() else []
        if name in shared:
            for e in sorted(entries):
                if is_archive(e) or e.name.lower() in LEFTOVERS or e.name.startswith('.'):
                    continue
                out.append((str(e), f'{dest_root}/game/{name}/{e.name}', size_of(e)))
        elif any(e.name.lower() in LEFTOVERS for e in entries):
            for e in sorted(entries):
                if e.name.lower() in LEFTOVERS or e.name.startswith('.'):
                    continue
                out.append((str(e), f'{dest_root}/game/{name}/{e.name}', size_of(e)))
        else:
            out.append((str(src), f'{dest_root}/game/{name}', size_of(src)))
    if overlay:
        for folder in sorted(p for p in Path(overlay).iterdir() if p.is_dir()):
            for e in sorted(folder.iterdir()):
                out.append((str(e), f'{dest_root}/game/{folder.name}/{e.name}', size_of(e)))
    return out


def device_files(report):
    """{relative path: size} of the files in a devicectl JSON listing."""
    out = {}
    for item in ((report or {}).get('result') or {}).get('files') or []:
        meta = item.get('metadata') if isinstance(item.get('metadata'), dict) else {}
        if (item.get('resources') or {}).get('isDirectory') or 'dir' in str(meta.get('type', item.get('type', ''))).lower():
            continue
        path = str(item.get('relativePath') or item.get('path') or item.get('name') or '').rstrip('/')
        size = meta.get('size', item.get('size', item.get('fileSize')))
        if path and size is not None:
            out[path] = int(size)
    return out


def missing_after_copy(local, listing):
    """Files of the local folder not on the device with the same size afterwards
    (paths matched from their end, since devicectl may list them relative to the
    folder or to the container): [] when the copy is complete."""
    on_device = device_files(listing)
    missing = []
    for path in sorted(p for p in Path(local).rglob('*') if p.is_file()):
        rel = path.relative_to(local).as_posix()
        size = path.stat().st_size
        if not any((d == rel or d.endswith('/' + rel)) and s == size for d, s in on_device.items()):
            missing.append(rel)
    return missing


def verify(argv):
    ap = argparse.ArgumentParser(description='every file of --local is on the device (--listing: devicectl JSON)')
    ap.add_argument('--local', type=Path, required=True)
    ap.add_argument('--listing', type=Path, required=True)
    a = ap.parse_args(argv)
    import json
    missing = missing_after_copy(a.local, json.loads(a.listing.read_text()))
    if missing:
        print(f'{len(missing)} file(s) did not arrive: ' + ', '.join(missing[:10]), file=sys.stderr)
        return 1
    print(f'all {sum(1 for p in a.local.rglob("*") if p.is_file())} files arrived, same sizes')
    return 0


def main(argv):
    if argv[:1] == ['verify']:
        return verify(argv[1:])
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--game-root', required=True)
    ap.add_argument('--linux-root', required=True)
    ap.add_argument('--prep', required=True)
    ap.add_argument('--folders', required=True, help='space-separated (ios_data of the game profile)')
    ap.add_argument('--dest-root', default='Documents', help='Documents/mel for Mel hosted in the Portal app')
    ap.add_argument('--shared', default='', help='slim pack: folders read from Portal 2 (copied without their VPKs)')
    ap.add_argument('--overlay', help='slim pack: its extra files (tools/mel_slim.py build --out)')
    a = ap.parse_args(argv)
    for p in (a.game_root, a.linux_root, a.prep):
        if not Path(p).is_dir():
            print(f'{p}: no such folder (config/local.json, gameprep)', file=sys.stderr)
            return 2
    rows = plan(a.game_root, a.linux_root, a.prep, a.folders.split(), a.dest_root, tuple(a.shared.split()), a.overlay)
    for src, dest, n in rows:
        print(f'copy\t{src}\t{dest}\t{n}')
    print(f'total\t{sum(n for _, _, n in rows)}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
