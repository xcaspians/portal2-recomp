"""tools/coverage64.py and tools/build64.py on the x86-64 test fixture
(tests/x64_programs.s linked by tests/x64_link.py): the linear sweep, the
split C output and its object cache. A few seconds, no game files."""
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'tests'))
import build64  # noqa: E402
import coverage64  # noqa: E402
import x64_link  # noqa: E402


class Fixture(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.dir = tempfile.TemporaryDirectory()
        obj = os.path.join(cls.dir.name, 'x64_programs.o')
        subprocess.run(['clang', '-target', 'x86_64-linux-gnu', '-c', str(ROOT / 'tests/x64_programs.s'), '-o', obj],
                       check=True)
        cls.so = os.path.join(cls.dir.name, 'libx64fixture.so')
        x64_link.link(obj, cls.so, 'libx64fixture.so')

    @classmethod
    def tearDownClass(cls):
        cls.dir.cleanup()

    def test_sweep_handles_every_fixture_instruction(self):
        total, missing, why, traps = coverage64.sweep(self.so)
        self.assertGreater(total, 300)
        self.assertEqual(dict(missing), {}, why)
        self.assertEqual(dict(traps), {})

    def test_symbol_names(self):
        self.assertEqual(build64.symbol_for('libtier0.so'), 'm_tier0')
        self.assertEqual(build64.symbol_for('libQt6Core.so.6'), 'm_qt6core')
        self.assertEqual(build64.symbol_for('filesystem_stdio.so'), 'm_filesystem_stdio')

    def test_build_splits_compiles_and_caches(self):
        out = os.path.join(self.dir.name, 'lib')
        logs = []
        first = build64.build_module(self.so, out, jobs=2, shard_size=20_000, opt='0', limit_gb=0, log=logs.append)
        self.assertGreaterEqual(first['c_files'], 3)
        self.assertEqual(first['compiled'], first['c_files'])
        symbols = subprocess.run(['nm', first['library']], capture_output=True, text=True, check=True).stdout
        self.assertIn('m_x64fixture_f1000', symbols)       # x64_sum8, module-unique name
        self.assertIn('m_x64fixture_base', symbols)
        again = build64.build_module(self.so, out, jobs=2, shard_size=20_000, opt='0', limit_gb=0, log=logs.append)
        self.assertEqual(again['compiled'], 0)               # unchanged C: every object from the cache


if __name__ == '__main__':
    unittest.main()
