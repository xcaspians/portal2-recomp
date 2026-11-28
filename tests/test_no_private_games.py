"""Games built for personal use only live in the private repository
(sauce-private), never in Sauce: no tracked file may name them, in its path
or its text. Generic fixes they led to (the x86-64 lifter, the loader) stay,
described without the game's name. .gitignore is the one exception: it lists
their paths so they can't be added again."""
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
# Spelled in pieces so this file doesn't name them either.
PRIVATE = ['sky' + 'rim']
ALLOWED = {'.gitignore'}


def tracked():
    r = subprocess.run(['git', 'ls-files', '-z'], cwd=ROOT, capture_output=True)
    if r.returncode != 0:
        return None   # not a git checkout (the installer's unpacked source)
    return [p for p in r.stdout.decode().split('\0') if p]


class NoPrivateGamesTests(unittest.TestCase):
    def test_no_tracked_file_names_a_private_game(self):
        files = tracked()
        if files is None:
            self.skipTest('not a git checkout')
        found = []
        for rel in files:
            if rel in ALLOWED:
                continue
            path = ROOT / rel
            if any(w in rel.lower() for w in PRIVATE):
                found.append(f'{rel} (its name)')
                continue
            try:
                text = path.read_bytes().decode('utf-8', 'ignore').lower()
            except (OSError, IsADirectoryError):
                continue
            for i, line in enumerate(text.splitlines(), 1):
                if any(w in line for w in PRIVATE):
                    found.append(f'{rel}:{i}')
        self.assertEqual(found, [], 'private game named in Sauce: move it to sauce-private, '
                                    'or describe the fix without the name')

    def test_gitignore_keeps_their_docs_out(self):
        text = (ROOT / '.gitignore').read_text().lower()
        for w in PRIVATE:
            self.assertIn(f'docs/*{w}*', text)
            self.assertIn(f'prompts/*{w}*', text)


if __name__ == '__main__':
    unittest.main()
