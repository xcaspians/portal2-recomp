import os
import sys
import tempfile
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import link64  # noqa: E402


class Link64Tests(unittest.TestCase):
    def test_plan_one_dylib_per_module_then_the_executable(self):
        steps = link64.plan('out', 'bin/p2ce-run', ['src/main64.c'], 'reg.c',
                            ['lib/m_engine/libm_engine.a', 'lib/m_tier0/libm_tier0.a'])
        targets = [t for t, _, _ in steps]
        self.assertEqual(targets, ['out/librt64.dylib', 'out/libm_engine.dylib', 'out/libm_tier0.dylib', 'bin/p2ce-run'])
        engine = steps[1][2]
        self.assertIn('-dynamiclib', engine)
        self.assertIn('-Wl,-force_load,lib/m_engine/libm_engine.a', engine)   # nothing references its objects
        self.assertIn('@rpath/libm_engine.dylib', engine)
        self.assertIn('out/librt64.dylib', engine)                            # the runtime, once, shared
        self.assertNotIn('out/libm_tier0.dylib', engine)                      # modules never link each other
        exe = steps[-1][2]
        self.assertIn('-Wl,-rpath,@executable_path/../out', exe)
        self.assertEqual(exe[-1], 'bin/p2ce-run')

    def test_symbol_of(self):
        self.assertEqual(link64.symbol_of('x/libm_vgui2.a'), 'm_vgui2')
        with self.assertRaises(ValueError):
            link64.symbol_of('x/engine.so')

    def test_only_stale_modules_relink(self):
        with tempfile.TemporaryDirectory() as tmp:
            a, d = os.path.join(tmp, 'libm_x.a'), os.path.join(tmp, 'libm_x.dylib')
            Path(a).write_bytes(b'a')
            self.assertTrue(link64.stale(d, [a]))
            Path(d).write_bytes(b'd')
            os.utime(a, (time.time() - 100,) * 2)
            self.assertFalse(link64.stale(d, [a]))
            os.utime(a, (time.time() + 100,) * 2)
            self.assertTrue(link64.stale(d, [a]))

    def test_i386_games_never_use_it(self):
        root = Path(link64.ROOT)
        for tool in ('build_game.py', 'build_ios.py', 'make_app.py'):
            self.assertNotIn('link64', (root / 'tools' / tool).read_text())


if __name__ == '__main__':
    unittest.main()
