#!/usr/bin/env python3
"""Portal Stories: Mel's slim pack: Mel without the Portal 2 content it carries.

Mel ships its own copy of Portal 2's `portal2` and `portal2_dlc2` folders,
about 2.4 GB, almost all of it in their VPKs. In the Portal app, Portal 2
itself is always on the device (it can't be removed), so the slim pack keeps
Mel's VPK *directories* and borrows Portal 2's *archives*:

- for each shared folder it writes `pak01_slim.vpk`: a VPK directory listing
  exactly the entries of Mel's own `pak01_dir.vpk` (version 1, as the engine
  wants), each pointing into Portal
  2's archives (`pak01_NNN.vpk`) when Portal 2 has the same bytes under the
  same path, otherwise carrying the bytes itself (archive 0x7fff, data after
  the tree, as Valve's format allows);
- the runtime's base-game layer (`P2_BASE_GAME_ROOT`, runtime/paths.c) gives
  the engine that file when it opens `<folder>/pak01_dir.vpk`, and Portal 2's
  archives when it opens `<folder>/pak01_NNN.vpk`;
- every loose file stays Mel's own, exactly as in the full copy.

So Mel sees the same entries with the same bytes as with its full copy, and no
Portal 2 entry it never had (Source searches every VPK before any loose file,
so an extra entry could hide one of Mel's loose files).

    mel_slim.py plan  [--mel DIR] [--portal2 DIR] [--json]   sizes
    mel_slim.py build [--mel DIR] [--portal2 DIR] --out DIR   writes DIR/<folder>/pak01_slim.vpk
    mel_slim.py check [--mel DIR] [--portal2 DIR] --overlay DIR  every entry of the full copy, same bytes
    mel_slim.py layout --overlay DIR --out DIR   the device's file layout on the Mac, for a boot test:
                                                 DIR/mel/game (the slim pack, as clones and links); give it
                                                 P2_BASE_GAME_ROOT=<Portal 2's folder>
    mel_slim.py boot-ok --log FILE               a boot test's log: Mel's menu, then a save or map loaded

Directories default to config/local.json (games.mel.game_root, game_root).
"""
import argparse
import json
import os
import struct
import sys
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))

SHARED = ('portal2', 'portal2_dlc2')    # Mel's folders read from Portal 2's install
MOD = 'portal_stories'                  # Mel's own folder, searched first
# Folders the slim pack carries whole, as the full copy does (ios_data of the profile).
OWN = ('bin', 'platform', MOD)


# ---- VPK (version 1 and 2) directories ----

def read_vpk_dir(path):
    """{lower-case path: (crc32, size, archive index, offset, length, preload bytes)}."""
    data = Path(path).read_bytes()
    sig, ver, tree = struct.unpack_from('<III', data, 0)
    if sig != 0x55aa1234:
        raise ValueError(f'{path}: not a VPK')
    off = 12 if ver == 1 else 28
    out = {}

    def string():
        nonlocal off
        end = data.index(b'\0', off)
        s = data[off:end].decode('latin-1')
        off = end + 1
        return s

    while True:
        ext = string()
        if not ext:
            break
        while True:
            folder = string()
            if not folder:
                break
            while True:
                name = string()
                if not name:
                    break
                crc, pre, arc, o, ln, _term = struct.unpack_from('<IHHIIH', data, off)
                off += 18
                preload = data[off:off + pre]
                off += pre
                rel = (f'{folder}/' if folder != ' ' else '') + name + (f'.{ext}' if ext != ' ' else '')
                out[rel.lower()] = (crc, pre + ln, arc, o, ln, preload)
    return out


def vpk_entry_bytes(dir_path, entry):
    """The bytes of one entry (preload + archive data)."""
    _crc, _size, arc, off, ln, preload = entry
    if not ln:
        return preload
    dir_path = Path(dir_path)
    if arc == 0x7fff:   # data after the directory tree, in the _dir file itself
        data = dir_path.read_bytes()
        _sig, ver, tree = struct.unpack_from('<III', data, 0)
        base = (12 if ver == 1 else 28) + tree
        return preload + data[base + off:base + off + ln]
    archive = dir_path.with_name(dir_path.name.replace('_dir.vpk', f'_{arc:03d}.vpk'))
    with open(archive, 'rb') as f:
        f.seek(off)
        return preload + f.read(ln)


# ---- one search path: its VPK first, then its loose files ----

def is_vpk(folder, p):
    return p.parent == folder and p.name.lower().startswith('pak01_') and p.suffix.lower() == '.vpk'


class Layer:
    """A search path folder: pak01_dir.vpk (searched first, as Source does) then loose
    files. vpk_folder: where its VPK comes from, when not the folder itself (the slim
    pack: Portal 2's archives, Mel's own loose files)."""

    def __init__(self, folder, vpk_folder=None):
        self.folder = Path(folder)
        vpk = Path(vpk_folder or folder) / 'pak01_dir.vpk'
        self.vpk_path = vpk if vpk.is_file() else None
        self.vpk = read_vpk_dir(vpk) if self.vpk_path else {}
        self.loose = {}
        if self.folder.is_dir():
            for p in self.folder.rglob('*'):
                if p.is_file() and not is_vpk(self.folder, p):
                    self.loose[p.relative_to(self.folder).as_posix().lower()] = p
        self._crc = {}

    def files(self):
        return set(self.vpk) | set(self.loose)

    def signature(self, rel):
        """(crc32, size) of what this layer gives for rel, or None."""
        if rel in self.vpk:
            e = self.vpk[rel]
            return (e[0], e[1])
        p = self.loose.get(rel)
        if p is None:
            return None
        if rel not in self._crc:
            crc = 0
            with open(p, 'rb') as f:
                for block in iter(lambda: f.read(1 << 22), b''):
                    crc = zlib.crc32(block, crc)
            self._crc[rel] = (crc & 0xffffffff, p.stat().st_size)
        return self._crc[rel]

    def read(self, rel):
        if rel in self.vpk:
            return vpk_entry_bytes(self.vpk_path, self.vpk[rel])
        return self.loose[rel].read_bytes()

    def bytes_on_disk(self, vpks=True):
        if not self.folder.is_dir():
            return 0
        return sum(p.stat().st_size for p in self.folder.rglob('*') if p.is_file() and (vpks or not is_vpk(self.folder, p)))


def resolve(layers, rel):
    """(layer, signature) of the first layer that has rel."""
    for layer in layers:
        s = layer.signature(rel)
        if s is not None:
            return layer, s
    return None, None


SLIM_DIR = 'pak01_slim.vpk'   # the slim pack's directory of each shared folder
EMBEDDED = 0x7fff               # VPK archive index: the bytes follow the tree in the directory file


def write_vpk_dir(path, entries):
    """A version-1 VPK directory, as Portal 2's engine reads (it refuses version 2):
    entries {path: (crc, preload bytes, archive, offset, length, data)};
    archive EMBEDDED: offset into `embedded` (bytes passed as entries' data, see plan_dir)."""
    by_ext = {}
    for rel in sorted(entries):
        folder, _, name = rel.rpartition('/')
        stem, dot, ext = name.rpartition('.')
        if not dot:
            stem, ext = name, ' '
        by_ext.setdefault(ext, {}).setdefault(folder or ' ', []).append((stem, rel))
    tree = bytearray()
    data = bytearray()
    for ext, folders in by_ext.items():
        tree += ext.encode('latin-1') + b'\0'
        for folder, names in folders.items():
            tree += folder.encode('latin-1') + b'\0'
            for stem, rel in names:
                crc, preload, arc, off, ln, payload = entries[rel]
                if arc == EMBEDDED:
                    off, ln = len(data), len(payload)
                    data += payload
                tree += stem.encode('latin-1') + b'\0'
                tree += struct.pack('<IHHIIH', crc, len(preload), arc, off, ln, 0xffff) + preload
            tree += b'\0'
        tree += b'\0'
    tree += b'\0'
    header = struct.pack('<III', 0x55aa1234, 1, len(tree))
    Path(path).write_bytes(header + bytes(tree) + bytes(data))


def plan_dir(mel_folder, p2_folder):
    """Entries of a shared folder's slim directory, and counts: Mel's own entries,
    from Portal 2's archives when the bytes match, carried otherwise."""
    mel_vpk = Path(mel_folder) / 'pak01_dir.vpk'
    if not mel_vpk.is_file():
        return {}, 0, 0, 0
    mel = read_vpk_dir(mel_vpk)
    p2_vpk = Path(p2_folder) / 'pak01_dir.vpk'
    p2 = read_vpk_dir(p2_vpk) if p2_vpk.is_file() else {}
    out, borrowed, carried, carried_bytes = {}, 0, 0, 0
    for rel, e in mel.items():
        theirs = p2.get(rel)
        if theirs and theirs[0] == e[0] and theirs[1] == e[1]:
            out[rel] = (e[0], theirs[5], theirs[2], theirs[3], theirs[4], b'')
            borrowed += 1
        else:
            payload = vpk_entry_bytes(mel_vpk, e)
            out[rel] = (e[0], b'', EMBEDDED, 0, len(payload), payload)
            carried += 1
            carried_bytes += len(payload)
    return out, borrowed, carried, carried_bytes


def archives_bytes(folder):
    folder = Path(folder)
    return sum(p.stat().st_size for p in folder.iterdir() if is_vpk(folder, p)) if folder.is_dir() else 0


def analyse(mel_root, p2_root):
    """Sizes and counts of the slim pack (no file written)."""
    mel_root, p2_root = Path(mel_root), Path(p2_root)
    borrowed = carried = carried_bytes = 0
    for f in SHARED:
        _e, b, c, cb = plan_dir(mel_root / f, p2_root / f)
        borrowed += b; carried += c; carried_bytes += cb
    own_bytes = sum(Layer(mel_root / f).bytes_on_disk() for f in OWN)
    loose_bytes = sum(Layer(mel_root / f).bytes_on_disk(vpks=False) for f in SHARED)
    shared_bytes = sum(archives_bytes(mel_root / f) for f in SHARED)
    return {'same': borrowed, 'carried': carried, 'overlay_bytes': carried_bytes, 'shared_bytes': shared_bytes,
            'full_bytes': own_bytes + loose_bytes + shared_bytes,
            'slim_bytes': own_bytes + loose_bytes + carried_bytes}


def build(mel_root, p2_root, out):
    """Writes out/<folder>/pak01_slim.vpk for each shared folder (and a manifest)."""
    mel_root, p2_root, out = Path(mel_root), Path(p2_root), Path(out)
    for f in SHARED:
        entries, _b, _c, _cb = plan_dir(mel_root / f, p2_root / f)
        if entries:
            (out / f).mkdir(parents=True, exist_ok=True)
            write_vpk_dir(out / f / SLIM_DIR, entries)
    a = analyse(mel_root, p2_root)
    (out / 'mel-slim.json').write_text(json.dumps({'shared': list(SHARED), **a}, indent=1) + '\n')
    return a


def check(mel_root, p2_root, overlay_dir):
    """Every entry of Mel's own VPKs in the shared folders, read through the slim
    directory (Portal 2's archives, or the bytes it carries), gives the same
    bytes: path, size and CRC-32 of the data. Loose files are Mel's own in both.
    Returns the problems (empty: covered)."""
    mel_root, p2_root, overlay_dir = Path(mel_root), Path(p2_root), Path(overlay_dir)
    problems = []
    for f in SHARED:
        mel_vpk = mel_root / f / 'pak01_dir.vpk'
        if not mel_vpk.is_file():
            continue
        if not (p2_root / f / 'pak01_dir.vpk').is_file():
            return [f"Portal 2's archives missing under {p2_root / f}"]
        slim_path = overlay_dir / f / SLIM_DIR
        if not slim_path.is_file():
            problems.append(f'{f}: no {SLIM_DIR}')
            continue
        mel, slim = read_vpk_dir(mel_vpk), read_vpk_dir(slim_path)
        extra = set(slim) - set(mel)
        if extra:
            problems.append(f'{f}: {len(extra)} entries Mel never had, e.g. {sorted(extra)[0]}')
        for rel, e in mel.items():
            s = slim.get(rel)
            if s is None:
                problems.append(f'{f}/{rel}: missing')
                continue
            # The bytes the engine will read: archives are Portal 2's, the rest is in the slim file.
            where = slim_path if s[2] == EMBEDDED else p2_root / f / 'pak01_dir.vpk'
            data = vpk_entry_bytes(where, s)
            if len(data) != e[1] or (zlib.crc32(data) & 0xffffffff) != e[0] or s[0] != e[0]:
                problems.append(f'{f}/{rel}: different bytes')
    return problems


def boot_ok(log_text):
    """A boot test's verdict from the game's log (ios/main_ios.c P2_LOG_FILE or the
    console): Mel loaded a save or a map. Returns (ok, reason)."""
    if 'This game needs Portal 2' in log_text:
        return False, "Portal 2's files are missing"
    if 'Loading game from' in log_text or 'Host_NewGame on map' in log_text:
        return True, 'a save or map loaded'
    return False, 'no save or map loaded in the log'


def layout(mel_root, p2_root, overlay_dir, out):
    """The slim pack as it sits on the device, on the Mac: out/mel/game with Mel's
    own folders (APFS clones: no space used), its shared folders without their
    VPKs, and the overlay in portal_stories. Portal 2 stays where it is: give it
    as P2_BASE_GAME_ROOT. Returns the game folder."""
    import shutil
    import subprocess
    mel_root, out = Path(mel_root), Path(out)
    game = out / 'mel' / 'game'
    if game.exists():
        shutil.rmtree(game)
    game.mkdir(parents=True)
    for name in sorted(os.listdir(mel_root)):
        src = mel_root / name
        if name in SHARED:
            (game / name).mkdir()
            for e in sorted(src.iterdir()):
                if not is_vpk(src, e):
                    subprocess.run(['cp', '-cR', str(e), str(game / name / e.name)], check=True)
        elif name == MOD:
            subprocess.run(['cp', '-cR', str(src), str(game / name)], check=True)
        else:
            (game / name).symlink_to(src)
    for f in SHARED:
        slim = Path(overlay_dir) / f / SLIM_DIR
        if slim.is_file():
            shutil.copy(slim, game / f / SLIM_DIR)
    return game


def default_roots():
    import localconfig
    cfg = localconfig.load() if hasattr(localconfig, 'load') else None
    mel = localconfig.game_config(cfg, 'mel').get('game_root')
    p2 = localconfig.game_config(cfg, 'portal2').get('game_root')
    return mel, p2


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('command', choices=('plan', 'build', 'check', 'layout', 'boot-ok'))
    ap.add_argument('--log')
    ap.add_argument('--mel')
    ap.add_argument('--portal2')
    ap.add_argument('--out')
    ap.add_argument('--overlay')
    ap.add_argument('--json', action='store_true')
    a = ap.parse_args(argv)
    if a.command == 'boot-ok':
        ok, why = boot_ok(Path(a.log).read_text(errors='replace'))
        print(('boot test passed: ' if ok else 'boot test failed: ') + why)
        return 0 if ok else 1
    if not a.mel or not a.portal2:
        mel, p2 = default_roots()
        a.mel, a.portal2 = a.mel or mel, a.portal2 or p2
    for p in (a.mel, a.portal2):
        if not p or not Path(p).is_dir():
            print(f'{p}: no such folder (config/local.json game_root)', file=sys.stderr)
            return 2
    if a.command == 'layout':
        game = layout(a.mel, a.portal2, a.overlay or ROOT / 'build/mel/slim', a.out)
        print(f'{game}\nP2_BASE_GAME_ROOT={a.portal2}')
        return 0
    if a.command == 'check':
        problems = check(a.mel, a.portal2, a.overlay or a.out or ROOT / 'build/mel/slim')
        for p in problems[:20]:
            print(p)
        print(f'{len(problems)} file(s) not covered' if problems else 'every file of the full copy is covered, same bytes')
        return 1 if problems else 0
    r = build(a.mel, a.portal2, a.out) if a.command == 'build' else analyse(a.mel, a.portal2)
    r = dict(r, freed_bytes=r['full_bytes'] - r['slim_bytes'])
    if a.json:
        print(json.dumps(r, indent=1))
    else:
        gb = lambda n: f'{n / 1e9:.2f} GB'
        print(f"full copy {gb(r['full_bytes'])}, slim pack {gb(r['slim_bytes'])} (frees {gb(r['freed_bytes'])}); "
              f"{r['same']} VPK entries read from Portal 2's archives, {r['carried']} carried "
              f"({r['overlay_bytes'] / 1e6:.1f} MB)")
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
