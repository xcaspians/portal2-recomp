"""tools/ios_device.sh saves-backup on a fresh device, with a fake xcrun (no device).

Round 29: on a device where Sauce was deleted, or installed and never played,
Documents/portal2-userdata is empty or missing; devicectl refuses to copy it
(error 7000) and the install stopped with "couldn't back up". Nothing saved
means nothing to back up: skip it and carry on. A folder that holds files and
won't copy still stops the install, before anything is replaced."""
import json
import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Plays devicectl from files in $FAKE: listing.json (Documents), inner-<folder>.json,
# copy-<folder> (that folder copies). Anything absent fails like devicectl does.
FAKE_XCRUN = r'''#!/bin/bash
[ "$1" = devicectl ] || exit 1
shift
args=("$@"); out=""; sub=""; src=""; dst=""
for ((i = 0; i < ${#args[@]}; i++)); do
  case "${args[$i]}" in
    --json-output) out="${args[$((i + 1))]}" ;;
    --subdirectory) sub="${args[$((i + 1))]}" ;;
    --source) src="${args[$((i + 1))]}" ;;
    --destination) dst="${args[$((i + 1))]}" ;;
  esac
done
case "$1 $2 $3" in
  "device info files")
    if [ "$sub" = Documents ]; then f="$FAKE/listing.json"; else f="$FAKE/inner-${sub#Documents/}.json"; fi
    [ -f "$f" ] || { echo "ERROR: Failed to retrieve the file node for $sub (com.apple.dt.CoreDeviceError error 7000)" >&2; exit 1; }
    cp "$f" "$out" ;;
  "device copy from")
    name="${src#Documents/}"
    [ -f "$FAKE/copy-$name" ] || { echo "ERROR: Failed to retrieve the file node for $src (com.apple.dt.CoreDeviceError error 7000)" >&2; exit 1; }
    mkdir -p "$dst" && echo saved > "$dst/save.sav" ;;
  *) exit 1 ;;
esac
'''


def listing(*names, kind='directory'):
    # devicectl's own shape (Xcode 26/27): a folder is resources.isDirectory, metadata has no type.
    return {'result': {'files': [{'relativePath': n, 'name': n, 'metadata': {'size': 64 if kind == 'directory' else 1234},
                                  'resources': {'isDirectory': kind == 'directory'}} for n in names]}}


class SavesBackupTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.tmp = Path(tmp.name)
        # A bare checkout: no config/local.json, as on CI.
        self.repo = self.tmp / 'repo'
        shutil.copytree(ROOT / 'tools', self.repo / 'tools')
        (self.repo / 'runtime/ios').mkdir(parents=True)
        shutil.copy(ROOT / 'runtime/ios/game_ios.c', self.repo / 'runtime/ios/game_ios.c')
        self.fake = self.tmp / 'fake'
        (self.fake / 'bin').mkdir(parents=True)
        xcrun = self.fake / 'bin/xcrun'
        xcrun.write_text(FAKE_XCRUN)
        xcrun.chmod(0o755)
        self.dest = self.tmp / 'backup'

    def device(self, documents=None, inner=None, copies=()):
        if documents is not None:
            (self.fake / 'listing.json').write_text(json.dumps(documents))
        for name, content in (inner or {}).items():
            (self.fake / f'inner-{name}.json').write_text(json.dumps(content))
        for name in copies:
            (self.fake / f'copy-{name}').write_text('')

    def backup(self):
        env = {k: v for k, v in os.environ.items() if not k.startswith('P2_')}
        env.update(PATH=f"{self.fake / 'bin'}:{env['PATH']}", FAKE=str(self.fake), P2_DEVICE='00008142-000000000000001C')
        return subprocess.run(['bash', str(self.repo / 'tools/ios_device.sh'), 'saves-backup', str(self.dest)],
                              capture_output=True, text=True, env=env, cwd=self.repo)

    def test_sauce_never_installed(self):
        result = self.backup()   # no app container: Documents can't be listed
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("no saves to back up (the app isn't on the device yet)", result.stdout)

    def test_fresh_sauce_with_an_empty_saves_folder(self):
        # Installed (or reinstalled after being deleted), opened once, nothing played.
        self.device(listing('portal2-userdata'), inner={'portal2-userdata': listing('portal2-userdata/cfg')})
        result = self.backup()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('nothing to back up in portal2-userdata (empty)', result.stdout)
        self.assertIn('no saves to back up (nothing saved on the device yet)', result.stdout)
        self.assertNotIn("couldn't back up", result.stdout)

    def test_folder_gone_after_the_listing(self):
        self.device(listing('portal2-userdata'))   # listed, then not there to copy or list
        result = self.backup()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('nothing to back up in portal2-userdata', result.stdout)

    def test_real_saves_are_backed_up_and_empty_ones_skipped(self):
        self.device(listing('portal2-userdata', 'hl2-userdata'),
                    inner={'hl2-userdata': listing()}, copies=['portal2-userdata'])
        result = self.backup()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('backed up portal2-userdata', result.stdout)
        self.assertIn('nothing to back up in hl2-userdata', result.stdout)
        self.assertTrue((self.dest / 'portal2-userdata/save.sav').is_file())

    def test_saves_that_wont_copy_still_stop_the_install(self):
        self.device(listing('portal2-userdata'),
                    inner={'portal2-userdata': listing('portal2-userdata/SAVE/quick.sav', kind='file')})
        result = self.backup()
        self.assertEqual(result.returncode, 1)
        self.assertIn("couldn't back up Documents/portal2-userdata: stopping before anything is replaced", result.stdout)


if __name__ == '__main__':
    unittest.main()
