"""tools/mel_slim.py: Mel's slim pack covers every file of its full copy.

Synthetic VPKs (no game files needed); with the real Mel and Portal 2 on this
Mac (config/local.json), the same check runs on them too."""
import os
import struct
import sys
import tempfile
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import mel_slim  # noqa: E402


def write_vpk(folder, files):
    """A version-2 VPK: pak01_dir.vpk plus one archive pak01_000.vpk."""
    folder.mkdir(parents=True, exist_ok=True)
    tree = bytearray()
    data = bytearray()
    by_ext = {}
    for rel, content in files.items():
        d, _, name = rel.rpartition('/')
        stem, _, ext = name.rpartition('.')
        by_ext.setdefault(ext, {}).setdefault(d or ' ', []).append((stem, content))
    for ext, dirs in by_ext.items():
        tree += ext.encode() + b'\0'
        for d, names in dirs.items():
            tree += d.encode() + b'\0'
            for stem, content in names:
                tree += stem.encode() + b'\0'
                tree += struct.pack('<IHHIIH', zlib.crc32(content) & 0xffffffff, 0, 0, len(data), len(content), 0xffff)
                data += content
            tree += b'\0'
        tree += b'\0'
    tree += b'\0'
    header = struct.pack('<IIIIIII', 0x55aa1234, 2, len(tree), 0, 0, 0, 0)
    (folder / 'pak01_dir.vpk').write_bytes(header + tree)
    (folder / 'pak01_000.vpk').write_bytes(bytes(data))


def put(path, content):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(content)


class SlimPackTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        t = Path(self.tmp.name)
        self.mel, self.p2, self.out = t / 'mel', t / 'p2', t / 'slim'
        write_vpk(self.mel / 'portal_stories', {'maps/mel.bsp': b'mel map'})
        # Mel's copy of Portal 2: mostly Portal 2's bytes, one older file, one of Mel's own.
        write_vpk(self.mel / 'portal2', {'models/cube.mdl': b'cube', 'sound/old.wav': b'old version',
                                          'materials/mel_only.vtf': b'only in mel'})
        put(self.mel / 'portal2/scripts/hudlayout.res', b'mel loose')
        write_vpk(self.p2 / 'portal2', {'models/cube.mdl': b'cube', 'sound/old.wav': b'new version',
                                         'scripts/hudlayout.res': b'p2 packed', 'models/p2_only.mdl': b'p2'})
        write_vpk(self.p2 / 'portal2_dlc2', {'models/dlc.mdl': b'dlc'})
        put(self.p2 / 'portal2/save/quick.sav', b'a portal 2 save')

    def tearDown(self):
        self.tmp.cleanup()

    def test_slim_directory_lists_exactly_mels_entries(self):
        mel_slim.build(self.mel, self.p2, self.out)
        slim = mel_slim.read_vpk_dir(self.out / 'portal2' / mel_slim.SLIM_DIR)
        # Mel's entries only: Portal 2's own (its hudlayout.res, which Source would
        # search before Mel's loose one) never appears.
        self.assertEqual(sorted(slim), ['materials/mel_only.vtf', 'models/cube.mdl', 'sound/old.wav'])
        self.assertNotEqual(slim['models/cube.mdl'][2], mel_slim.EMBEDDED)   # read from Portal 2's archive
        self.assertEqual(slim['sound/old.wav'][2], mel_slim.EMBEDDED)        # Mel's older bytes, carried
        self.assertFalse((self.out / 'portal2_dlc2').exists())               # Mel has no VPK there
        header = (self.out / 'portal2' / mel_slim.SLIM_DIR).read_bytes()[:8]
        self.assertEqual(struct.unpack('<II', header), (0x55aa1234, 1))     # the engine refuses version 2

    def test_slim_pack_plus_portal2_covers_every_entry(self):
        mel_slim.build(self.mel, self.p2, self.out)
        self.assertEqual(mel_slim.check(self.mel, self.p2, self.out), [])
        a = mel_slim.analyse(self.mel, self.p2)
        self.assertEqual((a['same'], a['carried']), (1, 2))
        self.assertLess(a['slim_bytes'], a['full_bytes'])
        # A stale slim directory (Portal 2 updated its archives): the check says so.
        write_vpk(self.p2 / 'portal2', {'models/cube.mdl': b'CUBE', 'sound/old.wav': b'new version'})
        self.assertTrue(mel_slim.check(self.mel, self.p2, self.out))
        # Portal 2 missing: said plainly.
        self.assertIn('missing', mel_slim.check(self.mel, self.out / 'none', self.out)[0])

    def test_boot_verdict(self):
        self.assertTrue(mel_slim.boot_ok('Loading game from SAVE/x/quick.sav...')[0])
        self.assertTrue(mel_slim.boot_ok('Host_NewGame on map sp_a1_tramride')[0])
        self.assertFalse(mel_slim.boot_ok('This game needs Portal 2: ...')[0])
        self.assertFalse(mel_slim.boot_ok('')[0])
        self.assertFalse(mel_slim.boot_ok("Can't load '7656/1795587607', file missing!")[0])
        # The console keeps every launch: only this one's part counts.
        with tempfile.TemporaryDirectory() as d:
            log = Path(d) / 'console.log'
            earlier = b'Loading game from SAVE/x/old.sav...\n'
            log.write_bytes(earlier + b"Can't load 'x/1795587607', file missing!\n")
            self.assertEqual(mel_slim.main(['boot-ok', '--log', str(log), '--since', str(len(earlier))]), 1)
            self.assertEqual(mel_slim.main(['boot-ok', '--log', str(log)]), 0)   # the whole file would wrongly pass

    @unittest.skipUnless(os.environ.get('P2_REAL_MEL_SLIM'), 'P2_REAL_MEL_SLIM=1: the real Mel and Portal 2 (slow)')
    def test_real_games(self):
        mel, p2 = mel_slim.default_roots()
        with tempfile.TemporaryDirectory() as out:
            mel_slim.build(mel, p2, out)
            self.assertEqual(mel_slim.check(mel, p2, out), [])


if __name__ == '__main__':
    unittest.main()
