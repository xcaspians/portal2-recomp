"""Update Sauce (round 31): the latest release's source, rebuilt, installed over
the old Sauce with saves, settings and games kept; a failed build leaves the
working install (and this Mac's previous source and build) as they were.
Driven through the installer itself (--autotest update) in a temporary HOME,
with a fake device and a local archive standing in for GitHub's."""
import json
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import build_ios  # noqa: E402
import devices  # noqa: E402

FAKE_DEVICES = r'''#!/usr/bin/env python3
import json, sys
cmd = sys.argv[1]
if cmd == 'list':
    print(json.dumps([{"udid": "FAKE-UDID", "name": "Test iPhone", "model": "iPhone 18 Pro", "kind": "iPhone",
                       "link": "USB", "ready": True, "problem": None}]))
elif cmd == 'status':
    print(json.dumps({"app_installed": True, "free_bytes": 100000000000, "data": {"portal2": True},
                      "apps": {"portal2": True}, "updates": {"portal2": []}, "problem": None, "sauce_version": "0.5.1"}))
'''
FAKE_IOS_DEVICE = '''#!/bin/bash
echo "$1" >> "$HOME/device.log"
[ "$1" = saves-backup ] && echo "backed up portal2-userdata"
exit 0
'''
SETUP_OK = '''#!/bin/bash
echo "==> Building the Portal 2 iPhone app"
echo new > build/ios/device/Portal2.app/version.txt
tools/ios_device.sh install
'''
SETUP_FAILS = '''#!/bin/bash
echo "==> Building the Portal 2 iPhone app"
echo half-new > build/ios/device/Portal2.app/version.txt
echo "error: compiling the generated code failed"
exit 1
'''


def write_source(root, version, setup):
    (root / 'tools').mkdir(parents=True, exist_ok=True)
    (root / 'setup.sh').write_text(setup)
    for name, text in (('devices.py', FAKE_DEVICES), ('ios_device.sh', FAKE_IOS_DEVICE),
                       ('build_tools.py', 'print("[]")\n')):
        (root / 'tools' / name).write_text(text)
        (root / 'tools' / name).chmod(0o755)
    shutil.copy(ROOT / 'tools/installer_runs.py', root / 'tools/installer_runs.py')
    if version:
        (root / 'SOURCE_VERSION').write_text(version + '\n')


@unittest.skipUnless(shutil.which('swiftc'), 'needs swiftc (Xcode or the command line tools)')
class UpdateSauceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.binary = Path(cls.build.name) / 'installer'
        out = subprocess.run(['swiftc', '-parse-as-library', '-swift-version', '5',
                              *sorted(str(p) for p in (ROOT / 'installer').glob('*.swift')), '-o', str(cls.binary)],
                             capture_output=True, text=True)
        if out.returncode != 0:
            raise AssertionError(out.stderr[-3000:])
        cls.table = Path(cls.build.name) / 'games.json'
        cls.table.write_text(subprocess.run([sys.executable, str(ROOT / 'tools/games.py'), '--installer-json'],
                                            capture_output=True, text=True, check=True).stdout)

    @classmethod
    def tearDownClass(cls):
        cls.build.cleanup()

    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.tmp = Path(tmp.name)
        self.home = self.tmp / 'home'
        self.source = self.home / 'Library/Application Support/Portal 2 Natif Build'
        # This Mac today: v0.5.1's source, its Sauce build and the config.
        write_source(self.source, 'v0.5.1', '#!/bin/bash\necho old setup\n')
        (self.source / '.portal2-installer').touch()
        (self.source / 'build/ios/device/Portal2.app').mkdir(parents=True)
        (self.source / 'build/ios/device/Portal2.app/version.txt').write_text('old')
        (self.source / 'config').mkdir()
        (self.source / 'config/local.json').write_text('{"games": {"portal2": {}}}')

    def update(self, setup):
        release = self.tmp / 'release/portal2-recomp-0.5.2'
        write_source(release, None, setup)   # GitHub's tag archive has no SOURCE_VERSION
        archive = self.tmp / 'v0.5.2.tar.gz'
        with tarfile.open(archive, 'w:gz') as t:
            t.add(release, arcname='portal2-recomp-0.5.2')
        report = self.tmp / 'report.json'
        env = dict(os.environ, HOME=str(self.home), P2_GAMES_JSON=str(self.table), P2_AUTOTEST_REPORT=str(report),
                   P2_SAUCE_LATEST='v0.5.2', P2_SAUCE_ARCHIVE=str(archive), P2_TEST_SOURCE='1', P2_NO_TOOLS_PROMPT='1')
        env.pop('P2_REPO', None)
        run = subprocess.run([str(self.binary), '--autotest', 'update'], env=env, timeout=180, capture_output=True, text=True)
        self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
        return json.loads(report.read_text())

    def device_log(self):
        log = self.home / 'device.log'
        return log.read_text().split() if log.is_file() else []

    def test_a_newer_release_builds_and_installs_keeping_everything(self):
        r = self.update(SETUP_OK)
        self.assertTrue(r['could_update'], 'Update Sauce offered: 0.5.1 on the device, v0.5.2 out')
        self.assertEqual(r['update_state'], 'done', r)
        self.assertIn('Saves, settings and games are as they were', r['update_text'])
        self.assertEqual((self.source / 'SOURCE_VERSION').read_text().strip(), 'v0.5.2')
        self.assertEqual((self.source / 'build/ios/device/Portal2.app/version.txt').read_text().strip(), 'new')
        self.assertEqual((self.source / 'config/local.json').read_text(), '{"games": {"portal2": {}}}')
        self.assertEqual(self.device_log(), ['saves-backup', 'install'], 'saves backed up first, then installed')
        self.assertEqual([p.name for p in self.source.parent.iterdir() if p.name.startswith('.')], [])

    def test_a_failed_build_leaves_the_working_install(self):
        r = self.update(SETUP_FAILS)
        self.assertEqual(r['update_state'], 'failed', r)
        self.assertIn("didn't build (compiling the generated code failed)", r['update_text'])
        self.assertIn("wasn't touched: it still works as before", r['update_text'])
        self.assertNotIn('install', self.device_log(), 'nothing was installed on the device')
        # This Mac is back to v0.5.1: its source and the Sauce build that works.
        self.assertEqual((self.source / 'SOURCE_VERSION').read_text().strip(), 'v0.5.1')
        self.assertIn('old setup', (self.source / 'setup.sh').read_text())
        self.assertEqual((self.source / 'build/ios/device/Portal2.app/version.txt').read_text(), 'old')
        self.assertEqual((self.source / 'config/local.json').read_text(), '{"games": {"portal2": {}}}')
        self.assertEqual([p.name for p in self.source.parent.iterdir() if p.name.startswith('.')], [])


class VersionTests(unittest.TestCase):
    def test_sauce_carries_its_release(self):
        self.assertEqual(build_ios.bundle_version('v0.5.2'), '0.5.2')
        self.assertEqual(build_ios.bundle_version('v0.5.2-3-gabc1234'), '0.5.2')
        self.assertEqual(build_ios.bundle_version(''), '0.1')
        plist = (ROOT / 'ios/Info.plist.in').read_text()
        self.assertIn('<key>CFBundleShortVersionString</key><string>@BUNDLE_VERSION@</string>', plist)
        self.assertIn('<key>P2SauceVersion</key><string>@SAUCE_VERSION@</string>', plist)
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / 'SOURCE_VERSION').write_text('v0.5.2\n')
            self.assertEqual(build_ios.sauce_version(Path(tmp)), 'v0.5.2')

    def test_the_device_reports_it(self):
        from ios_bundle_id import bundle_id
        apps = [{'bundleIdentifier': 'com.apple.other', 'version': '9'},
                {'bundleIdentifier': bundle_id(game='portal2'), 'version': '0.5.1', 'bundleVersion': '0.5.1'}]
        self.assertEqual(devices.sauce_version(apps), '0.5.1')
        self.assertIsNone(devices.sauce_version([]))

    def test_the_installer_packs_the_release_with_the_source(self):
        script = (ROOT / 'tools/build_installer.sh').read_text()
        self.assertIn('--add-virtual-file="SOURCE_VERSION:$VERSION', script)
        self.assertIn('cp tools/installer_runs.py "$APP/Contents/Resources/installer_runs.py"', script)


if __name__ == '__main__':
    unittest.main()
