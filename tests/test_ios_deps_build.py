"""ios/deps/build.sh installs under P2_IOS_DEPS/<target>, resolved before the
script changes directory: a relative P2_IOS_DEPS (the docs/ios.md command)
must not nest the install in P2_IOS_DEPS/P2_IOS_DEPS/<target>."""
import os
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPT = ROOT / 'ios' / 'deps' / 'build.sh'


def prefix(cwd, target, deps=None):
    env = dict(os.environ)
    env.pop('P2_IOS_DEPS', None)
    if deps is not None: env['P2_IOS_DEPS'] = deps
    result = subprocess.run(['bash', str(SCRIPT), target, '--prefix'], cwd=cwd, env=env, capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    return result.stdout.strip()


class BuildPrefixTests(unittest.TestCase):
    def test_relative_deps_resolve_against_the_current_directory(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = os.path.realpath(tmp)
            self.assertEqual(prefix(tmp, 'sim', 'deps/ios-deps'), f'{tmp}/deps/ios-deps/sim')
            self.assertEqual(prefix(tmp, 'device', 'deps/ios-deps'), f'{tmp}/deps/ios-deps/device')

    def test_absolute_deps_are_kept(self):
        self.assertEqual(prefix(ROOT, 'device', '/opt/ios-deps'), '/opt/ios-deps/device')

    def test_default_is_the_work_folder_next_to_the_repository(self):
        self.assertEqual(prefix(ROOT, 'sim'), str(ROOT.parent / 'work' / 'ios-deps' / 'sim'))

    def test_prefix_builds_nothing(self):
        with tempfile.TemporaryDirectory() as tmp:
            prefix(tmp, 'sim', 'deps/ios-deps')
            self.assertEqual(os.listdir(tmp), [])


if __name__ == '__main__':
    unittest.main()
