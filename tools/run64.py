#!/usr/bin/env python3
"""Run a lifted x86-64 program (P2CE) in a private mirror of its game folder.

    run64.py --depot DEPOT [--depot CONTENT] --mirror build/p2ce/game --program bin/linux64/p2ce BINARY [args...]

The mirror has the depot's folders and a symlink for every file, so the
depot (Steam's own download) is never written: the runtime replaces a
symlink by a private copy before writing to it (RT64_COPY_ON_WRITE).
BINARY (src/main64.c linked with the modules) runs with the mirror as its
working directory, like the game's own p2ce.sh, and RT64_EXE set to the
program's path in the mirror. Output goes to the terminal; --timeout stops
it after that many seconds (exit code 124).
"""
import argparse
import os
import subprocess
import sys


def mirror(depot, target):
    """Create or refresh the mirror; returns the number of new links."""
    made = 0
    for root, dirs, files in os.walk(depot):
        relative = os.path.relpath(root, depot)
        destination = os.path.join(target, relative) if relative != '.' else target
        os.makedirs(destination, exist_ok=True)
        for name in files:
            link = os.path.join(destination, name)
            if not os.path.lexists(link):
                os.symlink(os.path.join(root, name), link)
                made += 1
    return made


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--depot', required=True, action='append',
                    help='game folder to mirror (repeatable: binaries, then shared content)')
    ap.add_argument('--mirror', required=True)
    ap.add_argument('--program', default='bin/linux64/p2ce', help='program path inside the game folder')
    ap.add_argument('--timeout', type=float, default=0)
    ap.add_argument('binary')
    ap.add_argument('args', nargs=argparse.REMAINDER)
    a = ap.parse_args()
    for depot in a.depot:
        made = mirror(depot, a.mirror)
        if made:
            print(f'run64: mirrored {made} files of {depot} into {a.mirror}', file=sys.stderr)
    env = dict(os.environ, RT64_EXE=os.path.abspath(os.path.join(a.mirror, a.program)), RT64_COPY_ON_WRITE='1')
    try:
        return subprocess.run([os.path.abspath(a.binary), *a.args], cwd=a.mirror, env=env,
                              timeout=a.timeout or None).returncode
    except subprocess.TimeoutExpired:
        print(f'run64: stopped after {a.timeout:.0f} s', file=sys.stderr)
        return 124


if __name__ == '__main__':
    sys.exit(main())
