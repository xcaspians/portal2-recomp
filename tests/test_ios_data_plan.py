"""tools/ios_data_plan.py: what `tools/ios_device.sh mel data push` copies, with a fake Mel folder."""
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import ios_data_plan as dp  # noqa: E402


class DataPlanTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.game, self.linux, self.prep = root / 'Mel', root / 'linux', root / 'prep'
        for d in (self.linux, self.prep):
            d.mkdir()
        (self.linux / 'portal2_linux').write_bytes(b'x' * 7)
        (self.prep / 'libstdc++.so.6').write_bytes(b'x' * 11)
        mod = self.game / 'portal_stories'
        (mod / 'maps').mkdir(parents=True)
        (mod / 'maps' / 'sp_a1_tramride.bsp').write_bytes(b'x' * 100)
        (mod / 'pak01_dir.vpk').write_bytes(b'x' * 50)
        (mod / 'SAVE' / '7656').mkdir(parents=True)
        (mod / 'SAVE' / '7656' / 'quick.sav').write_bytes(b'x' * 1000)
        (mod / 'screenshots').mkdir()
        (mod / 'console.log').write_bytes(b'x' * 500)
        (self.game / 'platform').mkdir()
        (self.game / 'platform' / 'a.vpk').write_bytes(b'x' * 30)

    def tearDown(self):
        self.tmp.cleanup()

    def test_leftovers_stay_on_the_mac_and_data_is_complete(self):
        rows = dp.plan(self.game, self.linux, self.prep, ['bin', 'platform', 'portal_stories'])
        dests = {d: n for _, d, n in rows}
        self.assertEqual(dests['Documents/prep'], 11)          # i386 system libraries come with prep
        self.assertEqual(dests['Documents/linux'], 7)
        self.assertEqual(dests['Documents/game/platform'], 30)  # a clean folder goes whole
        self.assertEqual(dests['Documents/game/portal_stories/maps'], 100)
        self.assertEqual(dests['Documents/game/portal_stories/pak01_dir.vpk'], 50)
        self.assertFalse(any('SAVE' in d or 'screenshots' in d or 'console.log' in d for d in dests))
        self.assertNotIn('Documents/game/bin', dests)            # missing on the Mac: skipped
        self.assertEqual(sum(dests.values()), 11 + 7 + 30 + 100 + 50)

    def test_missing_folders_are_refused(self):
        self.assertEqual(dp.main(['--game-root', str(self.game), '--linux-root', '/nonexistent',
                                  '--prep', str(self.prep), '--folders', 'platform']), 2)


if __name__ == '__main__':
    unittest.main()
