"""Live x86-64 milestone on P2CE's own libtier0.so (depot 440005):
P2_P2CE_TESTS=1 runs `make p2ce-tier0-check` (lift, compile, call exports).
Skipped without the variable or without the depot (P2CE_ROOT)."""
import os
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_DEPOT = (Path.home() / 'Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS'
                 / 'steamapps/content/app_440000/depot_440005')


@unittest.skipUnless(os.environ.get('P2_P2CE_TESTS'), 'P2_P2CE_TESTS=1: lifts P2CE libtier0.so (needs depot 440005)')
class P2CETier0(unittest.TestCase):
    def test_lift_and_call_libtier0(self):
        depot = Path(os.environ.get('P2CE_ROOT', DEFAULT_DEPOT))
        if not (depot / 'bin/linux64/libtier0.so').is_file():
            self.skipTest(f'no P2CE depot 440005 at {depot}')
        result = subprocess.run(['make', '-s', 'p2ce-tier0-check', f'P2CE_ROOT={depot}'], cwd=ROOT,
                                capture_output=True, text=True, timeout=600)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('p2ce-tier0: PASS', result.stdout)


if __name__ == '__main__':
    unittest.main()
