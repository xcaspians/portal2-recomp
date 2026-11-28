"""The Natif Installer itself (no window: --autotest), against a fake checkout and
a fake device whose copy never ends (round 31, bugs 1, 2 and 5):

- Stop and Quit mid-install: no build process survives, and the reopened
  installer shows the games stopped with Retry, never "Waiting";
- Force Quit (SIGKILL: no cleanup code runs in the app): the same;
- a leftover from an older installer (no record, running from its source
  folder): the next launch finds it and stops it.

Everything lives in a temporary HOME; nothing touches a real device."""
import json
import os
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

FAKE_DEVICES = r'''#!/usr/bin/env python3
import json, sys
cmd = sys.argv[1]
if cmd == 'list':
    print(json.dumps([{"udid": "FAKE-UDID", "name": "Test iPhone", "model": "iPhone 18 Pro", "kind": "iPhone",
                       "link": "USB", "ready": True, "problem": None}]))
elif cmd == 'status':
    print(json.dumps({"app_installed": True, "free_bytes": 100000000000, "data": {}, "apps": {"portal2": True},
                      "updates": {}, "problem": None, "sauce_version": "0.5.1"}))
elif cmd == 'plan':
    game = sys.argv[sys.argv.index('--game') + 1]
    print(json.dumps({"steps": [{"label": "Copy " + game, "command": ["--game", game, "data"]}],
                      "need_bytes": 1, "problem": None}))
'''
# The copy: a child and a grandchild that never end, like devicectl under device_copy.py.
FAKE_IOS_DEVICE = '''#!/bin/bash
echo "copy 1/4 Documents/game"
bash -c "sleep 600; : MARKER-copy" &
bash -c "bash -c 'sleep 600; : MARKER-deep'; :" &
wait
'''
FAKE_BUILD_TOOLS = '''#!/usr/bin/env python3
print("[]")
'''


def marker_running(marker):
    return subprocess.run(['pgrep', '-f', marker], capture_output=True).returncode == 0


def wait_until(check, seconds):
    end = time.time() + seconds
    while time.time() < end:
        if check():
            return True
        time.sleep(0.1)
    return check()


@unittest.skipUnless(shutil.which('swiftc'), 'needs swiftc (Xcode or the command line tools)')
class InstallerQuitTests(unittest.TestCase):
    binary = None

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
        self.repo = self.tmp / 'repo'
        (self.repo / 'tools').mkdir(parents=True)
        self.marker = f'r31quit{os.getpid()}x{time.time_ns() % 10**8}'
        shutil.copy(ROOT / 'tools/installer_runs.py', self.repo / 'tools/installer_runs.py')
        for name, text in (('devices.py', FAKE_DEVICES), ('ios_device.sh', FAKE_IOS_DEVICE.replace('MARKER', self.marker)),
                           ('build_tools.py', FAKE_BUILD_TOOLS)):
            (self.repo / 'tools' / name).write_text(text)
            (self.repo / 'tools' / name).chmod(0o755)
        self.addCleanup(lambda: subprocess.run(['pkill', '-9', '-f', self.marker], capture_output=True))

    def env(self, report):
        return dict(os.environ, HOME=str(self.home), P2_REPO=str(self.repo), P2_GAMES_JSON=str(self.table),
                    P2_AUTOTEST_REPORT=str(report), P2_SAUCE_LATEST='', P2_NO_TOOLS_PROMPT='1')

    def reopen(self):
        report = self.tmp / 'reopen.json'
        subprocess.run([str(self.binary), '--autotest', 'reopen'], env=self.env(report), timeout=120,
                       capture_output=True)
        return json.loads(report.read_text())

    def check_reopened(self, r):
        states = {g: j['state'] for g, j in r['jobs'].items()}
        self.assertTrue(states, r)
        self.assertNotIn('waiting', states.values(), r)
        self.assertNotIn('running', states.values(), r)
        self.assertEqual(r['activity'], '', r)
        self.assertEqual(r['leftover'], '', r)
        self.assertTrue(r['retry'], 'Retry shows on the stopped game')
        first = r['jobs'][r['retry']]
        self.assertEqual(first['state'], 'stopped')
        self.assertIn('Press Retry to carry on from here', first['text'])

    def test_stop_and_quit_mid_install(self):
        report = self.tmp / 'quit.json'
        run = subprocess.run([str(self.binary), '--autotest', 'install:6'], env=self.env(report), timeout=180,
                             capture_output=True, text=True)
        self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
        r = json.loads(report.read_text())
        self.assertIn('being installed on the iPhone', r['busy_at_quit'], 'Cmd+Q would have asked first')
        self.assertTrue(wait_until(lambda: not marker_running(self.marker), 10), 'build processes outlived the quit')
        self.check_reopened(self.reopen())

    def test_force_quit_mid_install(self):
        report = self.tmp / 'force.json'
        app = subprocess.Popen([str(self.binary), '--autotest', 'install:600'], env=self.env(report),
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        self.addCleanup(lambda: app.poll() is None and app.kill())
        self.assertTrue(wait_until(lambda: marker_running(self.marker + '-deep'), 60), 'the fake copy never started')
        app.send_signal(signal.SIGKILL)   # Force Quit: nothing in the app runs
        app.wait()
        self.assertTrue(wait_until(lambda: not marker_running(self.marker), 10), 'build processes outlived a Force Quit')
        runs = self.home / 'Library/Application Support/Natif Installer/runs'
        self.assertTrue(wait_until(lambda: not list(runs.glob('*.json')), 5), 'a lock with no owner stayed')
        self.check_reopened(self.reopen())

    def test_an_older_installers_leftover_is_stopped_at_launch(self):
        source = self.home / 'Library/Application Support/Portal 2 Natif Build'
        (source / 'tools').mkdir(parents=True)
        (source / 'tools/ios_device.sh').write_text(FAKE_IOS_DEVICE.replace('MARKER', self.marker))
        subprocess.Popen(['/bin/bash', str(source / 'tools/ios_device.sh')], start_new_session=True,
                         stdout=subprocess.DEVNULL)
        self.assertTrue(wait_until(lambda: marker_running(self.marker + '-deep'), 10))
        r = self.reopen()
        self.assertIn('Stopped what an earlier run had left going', r['notice'])
        self.assertTrue(wait_until(lambda: not marker_running(self.marker), 10), 'the leftover still runs')


if __name__ == '__main__':
    unittest.main()
