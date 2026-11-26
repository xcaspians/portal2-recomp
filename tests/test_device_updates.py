import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import device_updates  # noqa: E402


class DeviceUpdatesTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        for rel, data in (('game/portal2/a.vpk', b'aaaa'), ('game/portal2/sub/b.txt', b'b'), ('prep/x.so', b'xx')):
            (self.root / rel).parent.mkdir(parents=True, exist_ok=True)
            (self.root / rel).write_bytes(data)
        self.rows = [(str(self.root / 'prep'), 'Documents/prep', 2),
                     (str(self.root / 'game/portal2'), 'Documents/game/portal2', 5)]

    def tearDown(self):
        self.tmp.cleanup()

    def test_signature_counts_bytes_files_and_newest(self):
        sig = device_updates.signature(self.rows)['folders']['Documents/game/portal2']
        self.assertEqual((sig['bytes'], sig['files']), (5, 2))

    def test_unchanged_then_one_folder_changed(self):
        before = device_updates.signature(self.rows)
        self.assertEqual(device_updates.changed(device_updates.signature(self.rows), before), [])
        f = self.root / 'game/portal2/a.vpk'
        f.write_bytes(b'aaaaa')   # Steam updated the game
        os.utime(f, (2_000_000_000, 2_000_000_000))
        self.assertEqual(device_updates.changed(device_updates.signature(self.rows), before), ['Documents/game/portal2'])

    def test_new_folder_and_no_manifest(self):
        before = device_updates.signature(self.rows[:1])
        self.assertEqual(device_updates.changed(device_updates.signature(self.rows), before), ['Documents/game/portal2'])
        self.assertIsNone(device_updates.changed(device_updates.signature(self.rows), None))   # copied before round 28
        self.assertIsNone(device_updates.changed(device_updates.signature(self.rows), {'version': 0}))

    def test_keep_matches_split_folders(self):
        self.assertTrue(device_updates.keep('Documents/prep', []))   # no --only: everything
        self.assertTrue(device_updates.keep('Documents/game/portal2', ['Documents/game/portal2/maps']))
        self.assertFalse(device_updates.keep('Documents/linux', ['Documents/game/portal2']))
        self.assertFalse(device_updates.keep('Documents/game/portal', ['Documents/game/portal2']))


if __name__ == '__main__':
    unittest.main()
