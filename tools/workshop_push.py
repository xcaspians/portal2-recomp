#!/usr/bin/env python3
"""The Mac's Workshop maps onto an iPhone or iPad, over the cable
(`tools/ios_device.sh workshop push [--dry-run]`, docs/mods.md).

The Mac game (real Steam) keeps its subscribed chambers in
<user_dir>/portal2/maps/workshop/<content handle>/<name>.bsp, next to
thumb<Workshop ID>.jpg. Community Maps on iOS keeps them in
Documents/portal2-userdata/portal2/maps/workshop/<Workshop ID>/<name>.bsp with
p2workshop.txt (updated date, size, title: runtime/ios/workshop.c,
rt_ws_meta_write), so this tool renames the folders by Workshop ID and writes
that note, from Valve's keyless GetPublishedFileDetails.

    workshop_push.py plan --listing L.json --stage DIR [--details D.json] [--dry-run]
        prints one "copy<TAB>staged folder<TAB>device folder" line per map to
        copy (stdout) and what is skipped and why (stderr), then the total;
        --dry-run stages nothing. The device listing is devicectl's JSON of
        Documents/portal2-userdata/portal2/maps/workshop ("" or a missing file:
        no folder there yet).
    workshop_push.py space --report R.txt --need BYTES [--margin BYTES]
        exit 0 when the device has room (report written by the app:
        P2_DEVICE_REPORT=1, ios/main_ios.c), 1 with a plain message otherwise.

Nothing is ever deleted on the device: a map already there with the same
size is skipped, and one with another size (another version, downloaded on
the device) is left alone.
"""
import argparse
import json
import os
import re
import shutil
import sys
import urllib.parse
import urllib.request
from pathlib import Path

DEVICE_DIR = 'Documents/portal2-userdata/portal2/maps/workshop'
DETAILS_URL = 'https://api.steampowered.com/ISteamRemoteStorage/GetPublishedFileDetails/v1/'
THUMB = re.compile(r'^thumb([0-9]{1,20})\.jpg$', re.I)
MARGIN = 1 << 30     # left free on the device after the copy (saves, caches, iOS itself)


class PushError(Exception):
    pass


def mac_workshop_dir():
    """<user_dir>/portal2/maps/workshop of the Mac game (config/local.json, else the app's default)."""
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    try:
        import localconfig
        config = localconfig.for_game('portal2')
        return localconfig.user_dir(config) / 'portal2/maps/workshop'
    except Exception:
        return Path.home() / 'Library/Application Support/Portal 2 Natif/userdata/portal2/maps/workshop'


# ---- the Mac side ----

def scan(root):
    """[{handle, bsp: Path, size, id}] of the Mac's downloaded maps; id from thumb<id>.jpg (None if absent).

    Folders without a .bsp (thumbnails of maps not downloaded) are not maps."""
    found = []
    root = Path(root)
    if not root.is_dir():
        return found
    for folder in sorted(p for p in root.iterdir() if p.is_dir() and not p.name.startswith('.')):
        maps = sorted(f for f in folder.iterdir() if f.is_file() and f.suffix.lower() == '.bsp')
        if not maps:
            continue
        ids = [m.group(1) for m in (THUMB.match(f.name) for f in folder.iterdir()) if m]
        found.append({'handle': folder.name, 'bsp': maps[0], 'size': maps[0].stat().st_size,
                      'id': ids[0] if len(ids) == 1 else None})
    return found


# ---- Valve's keyless API ----

def fetch_details(ids, timeout=20):
    """{id: details dict} from GetPublishedFileDetails, one id per request (batches failed, ROUND3_REPORT)."""
    out = {}
    for i in ids:
        body = urllib.parse.urlencode({'itemcount': 1, 'publishedfileids[0]': i}).encode()
        req = urllib.request.Request(DETAILS_URL, data=body, headers={'User-Agent': 'portal2-recomp workshop push'})
        try:
            with urllib.request.urlopen(req, timeout=timeout) as r:
                answer = json.load(r)
        except Exception as e:  # offline, refused, unreadable
            raise PushError(f"Steam's Workshop details didn't answer ({e}); try again when online")
        for d in (answer.get('response') or {}).get('publishedfiledetails') or []:
            if str(d.get('publishedfileid')) == str(i):
                out[str(i)] = d
    return out


def plain_title(text):
    return ' '.join(str(text or '').replace('\r', ' ').replace('\n', ' ').split())[:255]


def map_name(filename, item_id):
    """The .bsp name Community Maps would give this file (rt_ws_map_path, runtime/ios/workshop.c)."""
    base = str(filename or '').rsplit('/', 1)[-1][:127]
    chars = []
    for k, c in enumerate(base):
        nxt = base[k + 1] if k + 1 < len(base) else ''
        ok = (c.isascii() and c.isalnum()) or c in '_-' or (c == '.' and nxt != '.')
        chars.append(c if ok else '_')
    name = ''.join(chars)
    if name.endswith('.bsp'):
        name = name[:-4]
    if not name or name.startswith('.') or name.endswith('.'):
        name = str(item_id)
    return name + '.bsp'


def note_text(details, size):
    """p2workshop.txt as rt_ws_meta_write writes it. size is the copied file's: a Mac copy older than the
    Workshop's then shows "Update available" (rt_ws_update_available: same date, other size)."""
    updated = int(details.get('time_updated') or details.get('time_created') or 0)
    return f"updated={updated}\nsize={size}\ntitle={plain_title(details.get('title'))}\n"


# ---- the device side ----

def _field(item, names):
    for place in [item] + [item[k] for k in ('metadata', 'attributes') if isinstance(item.get(k), dict)]:
        for n in names:
            if n in place:
                return place[n]
    return None


def parse_listing(report):
    """{id: {name: size}} of the device's workshop folder, from devicectl's JSON (None: no listing = empty)."""
    if report is None:
        return {}
    if not isinstance(report, dict) or report.get('error'):
        raise PushError('devicectl reported an error instead of a file listing')
    files = (report.get('result') or {}).get('files')
    if not isinstance(files, list):
        raise PushError('not a devicectl file listing (no result.files)')
    found = {}
    for item in files:
        if not isinstance(item, dict):
            continue
        path = str(item.get('relativePath') or item.get('path') or item.get('name') or '').rstrip('/')
        parts = path.split('/')
        if len(parts) < 2 or not parts[-2].isdigit():
            continue
        kind = str(_field(item, ('type', 'fileType')) or '').lower()
        if 'dir' in kind:
            continue
        found.setdefault(parts[-2], {})[parts[-1]] = int(_field(item, ('size', 'fileSize')) or 0)
    return found


# ---- the plan ----

def plan(local, details, device):
    """(copies, skipped): copies = [{id, bsp, name, size, note, title}], skipped = [(what, why)]."""
    copies, skipped, seen = [], [], set()
    for m in local:
        label = m['bsp'].name
        d = details.get(m['id']) if m['id'] else None
        if not m['id']:
            skipped.append((label, 'no Workshop ID next to it (thumb<ID>.jpg)'))
            continue
        if not d or int(d.get('result', 1)) != 1:
            skipped.append((label, f'Workshop item {m["id"]} not found (removed or private)'))
            continue
        if str(d.get('hcontent_file') or m['handle']) != m['handle']:
            # The folder holds another version of the item (the Workshop's file changed since): still that map.
            pass
        if int(d.get('consumer_app_id') or 620) != 620:
            skipped.append((label, f'{m["id"]} is not a Portal 2 item'))
            continue
        if m['id'] in seen:
            skipped.append((label, f'{m["id"]} twice on the Mac: the first copy is used'))
            continue
        seen.add(m['id'])
        title = plain_title(d.get('title')) or label
        on_device = device.get(m['id'], {})
        maps_there = {n: s for n, s in on_device.items() if n.lower().endswith('.bsp')}
        if any(s == m['size'] for s in maps_there.values()):
            if 'p2workshop.txt' in on_device:
                skipped.append((title, 'already on the device'))
                continue
            copies.append({'id': m['id'], 'bsp': None, 'name': None, 'size': 0, 'title': title,
                           'note': note_text(d, m['size'])})   # map there, note missing: the note only
            continue
        if maps_there:
            skipped.append((title, 'another version is on the device (kept; update it in Community Maps)'))
            continue
        copies.append({'id': m['id'], 'bsp': m['bsp'], 'name': map_name(d.get('filename') or label, m['id']),
                       'size': m['size'], 'title': title, 'note': note_text(d, m['size'])})
    return copies, skipped


def stage(copies, root):
    """One folder per map under root (<id>/<name>.bsp + p2workshop.txt), copied as a whole by devicectl.
    The map is hard-linked when possible (no 800 MB duplicate)."""
    root = Path(root)
    out = []
    for c in copies:
        folder = root / c['id']
        if folder.exists():
            shutil.rmtree(folder)
        folder.mkdir(parents=True)
        if c['bsp']:
            target = folder / c['name']
            try:
                os.link(c['bsp'], target)
            except OSError:
                shutil.copy2(c['bsp'], target)
        (folder / 'p2workshop.txt').write_text(c['note'], encoding='utf-8')
        out.append((str(folder), f'{DEVICE_DIR}/{c["id"]}'))
    return out


def megabytes(n):
    if n <= 0:
        return '0 MB'
    return f'{n / 1048576:.0f} MB' if n >= 1048576 else f'{max(1, n // 1024)} KB'


# ---- free space ----

def parse_report(text):
    """{key: value} of the app's device report (lines key=value)."""
    out = {}
    for line in text.splitlines():
        if '=' in line:
            k, v = line.split('=', 1)
            out[k.strip()] = v.strip()
    return out


def check_space(report, need, margin=MARGIN):
    """None when there's room, else the sentence to print."""
    free = report.get('free_bytes')
    if free is None or not free.isdigit():
        return ("couldn't read the device's free space; check Settings > General > Storage "
                f"(this copy needs {megabytes(need)} plus {megabytes(margin)} to spare)")
    free = int(free)
    if free < need + margin:
        return (f'not enough room on the device: this copy needs {megabytes(need)} and keeps '
                f'{megabytes(margin)} free, but only {megabytes(free)} is free. '
                'Free some space (Settings > General > Storage), then run it again.')
    return None


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('plan')
    p.add_argument('--mac-dir', type=Path, default=None)
    p.add_argument('--listing', type=Path, help='devicectl JSON of the device folder (missing: none yet)')
    p.add_argument('--details', type=Path, help='saved GetPublishedFileDetails answers {id: details} (tests)')
    p.add_argument('--stage', type=Path, required=True)
    p.add_argument('--dry-run', action='store_true')
    s = sub.add_parser('space')
    s.add_argument('--report', type=Path, required=True)
    s.add_argument('--need', type=int, required=True)
    s.add_argument('--margin', type=int, default=MARGIN)
    a = ap.parse_args(argv)
    try:
        if a.cmd == 'space':
            text = a.report.read_text(encoding='utf-8', errors='replace') if a.report.is_file() else ''
            problem = check_space(parse_report(text), a.need, a.margin)
            if problem:
                print(problem, file=sys.stderr)
                return 1
            return 0
        root = a.mac_dir or mac_workshop_dir()
        local = scan(root)
        if not local:
            print(f'no downloaded Workshop maps in {root}', file=sys.stderr)
            return 0
        listing = None
        if a.listing and a.listing.is_file() and a.listing.stat().st_size:
            listing = json.loads(a.listing.read_text())
        device = parse_listing(listing)
        ids = sorted({m['id'] for m in local if m['id']})
        details = json.loads(a.details.read_text()) if a.details else fetch_details(ids)
        copies, skipped = plan(local, details, device)
        for what, why in skipped:
            print(f'  skip  {what}: {why}', file=sys.stderr)
        total = sum(c['size'] for c in copies)
        for c in copies:
            print(f'  copy  {c["title"]} ({megabytes(c["size"]) if c["bsp"] else "note only"})', file=sys.stderr)
        print(f'{len(copies)} to copy ({megabytes(total)}), {len(skipped)} skipped', file=sys.stderr)
        if a.dry_run:
            print(f'total\t{total}')
            return 0
        print(f'total\t{total}')
        for staged, dest in stage(copies, a.stage):
            print(f'copy\t{staged}\t{dest}')
        return 0
    except PushError as e:
        print(f'workshop push: {e}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
