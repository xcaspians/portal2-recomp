"""tools/run64.py: the game-folder mirror (symlinks, binaries depot first)."""
import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import run64  # noqa: E402


class Mirror(unittest.TestCase):
    def test_mirror_links_every_file_and_keeps_the_first_depot(self):
        with tempfile.TemporaryDirectory() as root:
            binaries, content, mirror = (os.path.join(root, n) for n in ('bin', 'content', 'mirror'))
            for base, name, text in ((binaries, 'bin/linux64/p2ce', 'program'), (content, 'p2ce/gameinfo.txt', 'game'),
                                     (content, 'bin/linux64/p2ce', 'not this one')):
                os.makedirs(os.path.dirname(os.path.join(base, name)), exist_ok=True)
                Path(base, name).write_text(text)
            self.assertEqual(run64.mirror(binaries, mirror), 1)
            self.assertEqual(run64.mirror(content, mirror), 1)       # p2ce/gameinfo.txt only
            self.assertTrue(os.path.islink(os.path.join(mirror, 'bin/linux64/p2ce')))
            self.assertEqual(Path(mirror, 'bin/linux64/p2ce').read_text(), 'program')
            self.assertEqual(Path(mirror, 'p2ce/gameinfo.txt').read_text(), 'game')
            self.assertEqual(run64.mirror(binaries, mirror), 0)      # nothing new the second time


if __name__ == '__main__':
    unittest.main()
