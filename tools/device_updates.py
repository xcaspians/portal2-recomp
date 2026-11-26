#!/usr/bin/env python3
"""Which of a game's folders on the device are out of date with the Mac.

After a copy, the installer leaves a manifest next to the game's data on the
device (<data root>/.p2-data-manifest.json): for every folder it copied
(tools/ios_data_plan.py's rows: prep, linux, game/<folder>), the bytes, the
file count and the newest modification time on the Mac. When Steam updates
the game, or a new Sauce build changes the prepared modules, the Mac's
signature changes and only those folders are offered again ("Update").

    device_updates.py signature --game G            JSON manifest of the Mac's copy now
    device_updates.py changed --game G --device-manifest FILE
                                                    JSON: [destinations to copy again]
"""
import argparse
import json
import os
import sys
from pathlib import Path

MANIFEST = '.p2-data-manifest.json'
VERSION = 1


def folder_signature(path):
    """{bytes, files, newest} of a file or folder on the Mac (symlinks followed, as devicectl copies)."""
    path = Path(path)
    total = files = newest = 0
    if path.is_file():
        st = path.stat()
        return {'bytes': st.st_size, 'files': 1, 'newest': int(st.st_mtime)}
    for root, _dirs, names in os.walk(path, followlinks=True):
        for n in names:
            try:
                st = os.stat(os.path.join(root, n))
            except OSError:
                continue
            total += st.st_size
            files += 1
            newest = max(newest, int(st.st_mtime))
    return {'bytes': total, 'files': files, 'newest': newest}


def signature(rows):
    """The manifest for ios_data_plan rows [(source, destination, bytes)]."""
    return {'version': VERSION, 'folders': {dest: folder_signature(src) for src, dest, _ in rows}}


def changed(mac, device):
    """Destinations whose Mac signature differs from the device's manifest (missing
    ones included), in the Mac's order. None when the device has no usable
    manifest: copied before manifests existed, so nothing is known."""
    if not isinstance(device, dict) or device.get('version') != VERSION:
        return None
    on_device = device.get('folders') or {}
    return [dest for dest, sig in (mac.get('folders') or {}).items() if on_device.get(dest) != sig]


def keep(dest, only):
    """ios_device.sh's P2_DATA_ONLY rule: copy `dest` when it is one of `only`,
    or holds one of them (a folder the plan split into entries)."""
    return not only or any(o == dest or o.startswith(dest + '/') for o in only)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    for name in ('signature', 'changed'):
        p = sub.add_parser(name)
        p.add_argument('--game', required=True)
        p.add_argument('--out', type=Path)
        if name == 'changed':
            p.add_argument('--device-manifest', type=Path, required=True)
    a = ap.parse_args(argv)
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import devices
    mac = signature(devices.data_rows(a.game))
    if a.cmd == 'signature':
        text = json.dumps(mac, indent=1, sort_keys=True)
        if a.out:
            a.out.write_text(text)
        else:
            print(text)
        return 0
    try:
        device = json.loads(a.device_manifest.read_text())
    except (OSError, ValueError):
        device = None
    print(json.dumps(changed(mac, device)))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
