"""ios/deps/patch_state.sh: applies the gl4es patch to a clean tree, accepts a
tree that already has it whatever git's hash abbreviation, refuses other edits."""
import os
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPT = ROOT / 'ios' / 'deps' / 'patch_state.sh'


def git(tree, *args, abbrev=None):
    config = ['-c', f'core.abbrev={abbrev}'] if abbrev else []
    return subprocess.run(['git', '-C', str(tree), *config, *args], capture_output=True, text=True, check=True).stdout


class PatchStateTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.tree = Path(self.tmp.name) / 'gl4es'
        self.tree.mkdir()
        env = {'GIT_AUTHOR_NAME': 't', 'GIT_AUTHOR_EMAIL': 't@t', 'GIT_COMMITTER_NAME': 't', 'GIT_COMMITTER_EMAIL': 't@t'}
        os.environ.update(env)
        git(self.tree, 'init', '-q')
        (self.tree / 'a.c').write_text('int a;\n')
        git(self.tree, 'add', 'a.c')
        git(self.tree, 'commit', '-qm', 'base')
        # The patch file as written with 7-character hashes.
        (self.tree / 'a.c').write_text('int a;\nint b;\n')
        self.patch = Path(self.tmp.name) / 'fix.patch'
        self.patch.write_text(git(self.tree, 'diff', abbrev=7))
        git(self.tree, 'checkout', '-q', '--', 'a.c')

    def tearDown(self):
        self.tmp.cleanup()

    def run_script(self):
        return subprocess.run(['bash', str(SCRIPT), str(self.tree), str(self.patch)], capture_output=True, text=True)

    def test_clean_tree_gets_the_patch(self):
        self.assertEqual(self.run_script().returncode, 0)
        self.assertEqual((self.tree / 'a.c').read_text(), 'int a;\nint b;\n')

    def test_already_patched_tree_passes_with_longer_hashes(self):
        self.assertEqual(self.run_script().returncode, 0)
        # A second target (sim after device) on a git that abbreviates to 12.
        git(self.tree, 'config', 'core.abbrev', '12')
        result = self.run_script()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual((self.tree / 'a.c').read_text(), 'int a;\nint b;\n')

    def test_other_edits_are_refused(self):
        self.assertEqual(self.run_script().returncode, 0)
        (self.tree / 'a.c').write_text('int a;\nint b;\nint c;\n')
        result = self.run_script()
        self.assertEqual(result.returncode, 1)
        self.assertIn('réconcilier', result.stderr)


if __name__ == '__main__':
    unittest.main()
