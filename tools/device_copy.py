#!/usr/bin/env python3
"""Copies a game's folders into the app on the device, and survives the
connection dropping on the way (round 31).

    device_copy.py --device UDID --app BUNDLE_ID --journal FILE < pairs
        pairs: one "source<TAB>destination" line per folder or file
        (destination relative to the app's data container).

Before round 31, tools/ios_device.sh data sent each folder in one devicectl
call: a Left 4 Dead 2 folder is several GB, so any hiccup over Wi-Fi (the
phone locking, Wi-Fi napping, a timeout) failed the whole install, and Retry
started that folder over. Now:

- each folder is cut into parts (a file, or a folder under PART_BYTES), one
  devicectl call each;
- a part that fails is looked into: the device locked (it waits for the
  unlock: "Your iPhone locked. Unlock it to carry on."), the device gone (it
  waits for it to come back, by cable or Wi-Fi), or neither (it tries again,
  up to RETRIES times, a little later each time);
- every part that arrived is written in the journal on the Mac, so a new run
  (Retry, or the next launch) skips what is already on the device, checked
  against the device's own listing (same files, same sizes), and carries on
  from where it stopped. The journal is removed once everything is there.

Lines for the installer: "copy i/n DESTINATION" (its progress bar), and
"status: <sentence>" when it waits or tries again (shown under the game).
P2_DEVICE_KIND names the device in those sentences (iPhone, iPad).
"""
import argparse
import json
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path

ENV = os.environ.get
PART_BYTES = int(ENV('P2_COPY_PART_BYTES', 200 * 1000 * 1000))   # a folder below this goes in one call
RETRIES = 3                                                        # failures with the device there and unlocked
LOCK_WAIT = float(ENV('P2_COPY_LOCK_WAIT', 30 * 60))              # how long it waits for an unlock
GONE_WAIT = float(ENV('P2_COPY_GONE_WAIT', 15 * 60))              # ... and for the device to come back
POLL = float(os.environ.get('P2_COPY_POLL', '5'))
KIND = os.environ.get('P2_DEVICE_KIND') or 'device'


def say(line):
    print(line, flush=True)


def size_and_stamp(path):
    """(bytes, newest mtime_ns, files) of a file or a folder."""
    p = Path(path)
    if p.is_file():
        st = p.stat()
        return st.st_size, st.st_mtime_ns, 1
    total = newest = count = 0
    for root, _dirs, files in os.walk(p):
        for f in files:
            try:
                st = (Path(root) / f).stat()
            except OSError:
                continue
            total += st.st_size
            newest = max(newest, st.st_mtime_ns)
            count += 1
    return total, newest, count


def parts(src, dest, limit=PART_BYTES):
    """[(source, destination, bytes)]: files, and folders under `limit` whole."""
    p = Path(src)
    if p.is_file():
        return [(str(p), dest, p.stat().st_size)]
    if not p.is_dir():
        return []
    size, _newest, count = size_and_stamp(p)
    if count == 0:
        return []   # devicectl refuses an empty folder (error 7000), and the game needs none (Portal's portal/bin)
    if size <= limit:
        return [(str(p), dest, size)]
    out = []
    for child in sorted(p.iterdir()):
        out += parts(child, f'{dest}/{child.name}', limit)
    return out


def local_files(src):
    """{path relative to src: size} of a part's files."""
    p = Path(src)
    if p.is_file():
        return {'': p.stat().st_size}
    out = {}
    for root, _dirs, files in os.walk(p):
        for f in files:
            full = Path(root) / f
            try:
                out[full.relative_to(p).as_posix()] = full.stat().st_size
            except OSError:
                pass
    return out


# ---- devicectl ----

class Device:
    def __init__(self, udid, app):
        self.udid, self.app = udid, app

    def devicectl(self, *args, timeout=60, json_out=False):
        """(ok, json or None, last line of its output)."""
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / 'out.json'
            cmd = ['xcrun', 'devicectl', *args[:3], '--device', self.udid, *args[3:]]
            if args[:2] == ('list', 'devices'):
                cmd = ['xcrun', 'devicectl', *args]
            if json_out:
                cmd += ['--json-output', str(out)]
            try:
                r = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, stdin=subprocess.DEVNULL)
            except subprocess.TimeoutExpired:
                return False, None, f'timed out after {timeout:g} s'
            data = None
            if json_out and out.is_file() and out.stat().st_size:
                try:
                    data = json.loads(out.read_text())
                except ValueError:
                    data = None
            lines = [l for l in (r.stderr + r.stdout).splitlines() if l.strip()]
            return r.returncode == 0, data, (lines[-1].strip() if lines else '')

    def copy(self, src, dest, size):
        # Long enough for a slow Wi-Fi (2 MB/s) plus devicectl's own start.
        ok, _d, last = self.devicectl('device', 'copy', 'to', '--domain-type', 'appDataContainer',
                                      '--domain-identifier', self.app, '--source', src, '--destination', dest,
                                      timeout=120 + size / 2e6)
        return ok, last

    def listing(self, dest):
        """{path: size} of the files under dest on the device, or None when it can't be listed."""
        ok, data, _last = self.devicectl('device', 'info', 'files', '--domain-type', 'appDataContainer',
                                         '--domain-identifier', self.app, '--subdirectory', dest,
                                         timeout=300, json_out=True)
        if not ok or data is None:
            return None
        out = {}
        for item in (data.get('result') or {}).get('files') or []:
            res = item.get('resources') or {}
            meta = item.get('metadata') or {}
            if res.get('isDirectory') or 'dir' in str(meta.get('type', '')).lower():
                continue
            path = str(item.get('relativePath') or item.get('name') or '').strip('/')
            if path and meta.get('size') is not None:
                out[path] = int(meta['size'])
        return out

    def state(self):
        """'ok', 'locked' or 'gone' (not reachable: asleep, out of Wi-Fi, unplugged)."""
        ok, data, _last = self.devicectl('device', 'info', 'lockState', timeout=30, json_out=True)
        if ok and data is not None:
            r = data.get('result') or {}
            return 'locked' if r.get('passcodeRequired') else 'ok'
        ok, data, _last = self.devicectl('list', 'devices', timeout=30, json_out=True)
        for d in ((data or {}).get('result') or {}).get('devices') or []:
            hp, cp = d.get('hardwareProperties') or {}, d.get('connectionProperties') or {}
            if (hp.get('udid') or d.get('identifier')) == self.udid:
                if cp.get('tunnelState') in ('connected',) or cp.get('transportType') == 'wired':
                    return 'ok'
        return 'gone'


# ---- the journal ----

class Journal:
    def __init__(self, path):
        self.path = Path(path)
        try:
            self.done = json.loads(self.path.read_text()) if self.path.is_file() else {}
        except (OSError, ValueError):
            self.done = {}

    @staticmethod
    def stamp(src):
        size, newest, count = size_and_stamp(src)
        return f'{size}:{newest}:{count}'

    def has(self, src, dest):
        e = self.done.get(dest)
        return bool(e) and e.get('src') == src and e.get('stamp') == self.stamp(src)

    def add(self, src, dest):
        self.done[dest] = {'src': src, 'stamp': self.stamp(src)}
        self.path.parent.mkdir(parents=True, exist_ok=True)
        tmp = self.path.with_suffix('.tmp')
        tmp.write_text(json.dumps(self.done))
        tmp.replace(self.path)

    def clear(self):
        self.path.unlink(missing_ok=True)


def on_device(src, dest, root_dest, listing):
    """Every file of the part is in the listing of root_dest, with its size."""
    if listing is None:
        return False
    prefix = dest[len(root_dest):].strip('/') if dest.startswith(root_dest) else dest
    for rel, size in local_files(src).items():
        want = '/'.join(x for x in (prefix, rel) if x)
        if not any((p == want or p.endswith('/' + want)) and s == size for p, s in listing.items()):
            return False
    return True


# ---- waiting ----

def wait_for(device, what, limit):
    """Polls until the device is 'ok'; False after `limit` seconds."""
    end = time.time() + limit
    while time.time() < end:
        time.sleep(POLL)
        s = device.state()
        if s == 'ok':
            return True
        if s != what:
            what = s
            say(status_line(s))
    return False


def status_line(state):
    if state == 'locked':
        return (f'status: Your {KIND} locked. Unlock it to carry on (with iPhone Mirroring, unlock the {KIND} '
                'itself). The copy picks up where it stopped.')
    return (f'status: The {KIND} dropped off the connection. Plug it in with a cable, or bring it near the Mac '
            'and unlock it: the copy carries on by itself.')


def copy_part(device, src, dest, size):
    """None when it arrived, else the sentence to stop with."""
    tries = 0
    while True:
        ok, last = device.copy(src, dest, size)
        if ok:
            return None
        state = device.state()
        if state in ('locked', 'gone'):
            say(f'note: copy of {dest} stopped ({last or "no message"}); the {KIND} is {state}')
            say(status_line(state))
            if not wait_for(device, state, LOCK_WAIT if state == 'locked' else GONE_WAIT):
                return (f'The {KIND} stayed locked, so the copy stopped. Unlock it and press Retry: it picks up '
                        'where it stopped.' if state == 'locked' else
                        f'The {KIND} didn\'t come back. Plug it in with a cable, unlock it, then press Retry: '
                        'it picks up where it stopped.')
            say('status: Back: carrying on where it stopped.')
            continue
        tries += 1
        say(f'note: copy of {dest} failed: {last or "no message"}')
        if tries > RETRIES:
            return f'The copy to the {KIND} kept failing ({last or "no message"}). Plug it in with a cable, then press Retry: it picks up where it stopped.'
        say(f'status: The copy hiccupped: trying again ({tries} of {RETRIES}).')
        time.sleep(POLL * tries)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--device', required=True)
    ap.add_argument('--app', required=True)
    ap.add_argument('--journal', required=True)
    a = ap.parse_args(argv)
    pairs = [tuple(l.rstrip('\n').split('\t', 1)) for l in sys.stdin if '\t' in l]
    device = Device(a.device, a.app)
    journal = Journal(a.journal)
    plan = [(src, root, part) for src, root in pairs for part in parts(src, root)]
    n = len(plan)
    # Resuming: what the journal says arrived, checked on the device, is skipped.
    skip = set()
    if journal.done:
        for root in dict.fromkeys(root for _s, root, _p in plan):
            mine = [(i, part) for i, (_s, r, part) in enumerate(plan) if r == root and journal.has(part[0], part[1])]
            if not mine:
                continue
            listing = device.listing(root)
            skip |= {i for i, part in mine if on_device(part[0], part[1], root, listing)}
        if skip:
            gb = sum(plan[i][2][2] for i in skip) / 1e9
            say(f'status: Picking up where it stopped: {len(skip)} of {n} parts ({gb:.1f} GB) are already on the {KIND}.')
    for i, (_src, root, (src, dest, size)) in enumerate(plan):
        say(f'copy {i + 1}/{n} {dest}')
        if i in skip:
            continue
        problem = copy_part(device, src, dest, size)
        if problem:
            say(f'error: {problem}')
            return 1
        journal.add(src, dest)
    journal.clear()
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
