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
import elfprep64  # noqa: E402
import lift64  # noqa: E402
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


class Exceptions(unittest.TestCase):
    """Landing pads of tests/x64_except.cpp become resume points (the run
    itself needs P2CE's C++ runtime: `make p2ce-modules-check`)."""
    def test_landing_pads_are_resume_points(self):
        with tempfile.TemporaryDirectory() as work:
            obj, so = os.path.join(work, 'x.o'), os.path.join(work, 'libx64except.so')
            subprocess.run(['clang++', '-target', 'x86_64-linux-gnu', '-fPIC', '-O1', '-fexceptions', '-c',
                            str(ROOT / 'tests/x64_except.cpp'), '-o', obj], check=True)
            x64_link.link(obj, so, 'libx64except.so', ['libmimalloc.so'])
            elf = elfprep64.Elf64(so)
            pads = elfprep64.landing_pads(elf)
            exports = elf.exports()
            for name in ('x64e_catch_here', 'x64e_catch_two_frames_up', 'x64e_catch_all'):
                self.assertIn(exports[name]['value'], pads, name)
            self.assertEqual(elf.needed, ['libmimalloc.so'])
            self.assertTrue(any(p['type'] == 0x6474e550 for p in elf.phdrs))      # PT_GNU_EH_FRAME
            info = elfprep64.prepare(so, work)
            bodies, _ = lift64.lift_module(info, [exports['x64e_catch_here']['value']], sweep=False)
            body = next(iter(bodies.values()))
            for pad in pads[exports['x64e_catch_here']['value']]:
                self.assertIn(f'case {pad:#x}: goto L{pad:x};', body)


if __name__ == '__main__':
    unittest.main()


class LiftCacheTest(unittest.TestCase):
    def test_key_follows_the_module_the_lifter_and_the_options(self):
        with tempfile.TemporaryDirectory() as tmp:
            module = os.path.join(tmp, 'libx.so')
            with open(module, 'wb') as f:
                f.write(b'one')
            key = build64.lift_key(module, None, 100, 'm_x')
            self.assertEqual(key, build64.lift_key(module, None, 100, 'm_x'))
            self.assertNotEqual(key, build64.lift_key(module, ['Export'], 100, 'm_x'))
            self.assertNotEqual(key, build64.lift_key(module, None, 200, 'm_x'))
            with open(module, 'wb') as f:
                f.write(b'two')
            self.assertNotEqual(key, build64.lift_key(module, None, 100, 'm_x'))
        # The lifter's own sources are part of it (a lifter fix relifts everything).
        self.assertIn('lift64.py', build64.LIFTER)
        self.assertIn('simd64.py', build64.LIFTER)


if __name__ == '__main__':
    unittest.main()
