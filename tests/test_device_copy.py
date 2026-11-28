"""tools/device_copy.py with a fake devicectl that drops the connection, locks
and fails on cue (round 31: "The iPhone has stopped answering" over Wi-Fi in
the middle of Left 4 Dead 2's copy, and Retry starting it over)."""
import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# The device's container is $FAKE/device. $FAKE/plan.json: what each "copy to"
# does in turn (ok, fail, drop:N, lock:N: the state then stays gone or locked
# for N polls). $FAKE/state.json: {"state": ok|gone|locked, "polls": N}.
FAKE_XCRUN = r'''#!/usr/bin/env python3
import json, os, shutil, sys
from pathlib import Path
fake = Path(os.environ['FAKE'])
args = sys.argv[1:]
assert args[0] == 'devicectl'
args = args[1:]
opt = lambda k: args[args.index(k) + 1] if k in args else None
with open(fake / 'calls.log', 'a') as f:
    f.write(' '.join(args[:3]) + ' ' + (opt('--destination') or opt('--subdirectory') or '') + '\n')
state_file = fake / 'state.json'
state = json.loads(state_file.read_text()) if state_file.is_file() else {'state': 'ok', 'polls': 0}
def save(): state_file.write_text(json.dumps(state))
def tick():
    if state['state'] != 'ok':
        state['polls'] -= 1
        if state['polls'] < 0:
            state['state'] = 'ok'
        save()
dev = fake / 'device'
cmd = ' '.join(args[:3])
if cmd == 'device copy to':
    if state['state'] != 'ok':
        print('ERROR: The connection was interrupted.', file=sys.stderr); sys.exit(1)
    plan_file = fake / 'plan.json'
    plan = json.loads(plan_file.read_text()) if plan_file.is_file() else []
    step = plan.pop(0) if plan else 'ok'
    plan_file.write_text(json.dumps(plan))
    src, dst = Path(opt('--source')), dev / opt('--destination')
    if step == 'fail':
        print('ERROR: The operation couldn\'t be completed. (timed out)', file=sys.stderr); sys.exit(1)
    if step.startswith(('drop', 'lock')):
        kind, n = step.split(':')
        state.update(state='gone' if kind == 'drop' else 'locked', polls=int(n)); save()
        if src.is_file():   # half a file arrived
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.write_bytes(src.read_bytes()[: max(1, src.stat().st_size // 2)])
        print('ERROR: The device is no longer connected (com.apple.dt.CoreDeviceError error 4).', file=sys.stderr)
        sys.exit(1)
    if src.is_dir() and not any(p.is_file() for p in src.rglob('*')):   # as the real devicectl does
        print(f'ERROR: Failed to retrieve the file node for {opt("--destination")} (com.apple.dt.CoreDeviceError error 7000 (0x1B58))', file=sys.stderr)
        sys.exit(1)
    dst.parent.mkdir(parents=True, exist_ok=True)
    if src.is_dir():
        shutil.copytree(src, dst, dirs_exist_ok=True)
    else:
        shutil.copyfile(src, dst)
    sys.exit(0)
out = opt('--json-output')
if cmd == 'device info lockState':
    if state['state'] == 'gone':
        tick(); print('ERROR: timed out', file=sys.stderr); sys.exit(1)
    locked = state['state'] == 'locked'
    tick()
    Path(out).write_text(json.dumps({'result': {'passcodeRequired': locked}})); sys.exit(0)
if args[:2] == ['list', 'devices']:
    devices = [] if state['state'] == 'gone' else [{'hardwareProperties': {'udid': 'UDID'},
                                                     'connectionProperties': {'tunnelState': 'connected'}}]
    Path(out).write_text(json.dumps({'result': {'devices': devices}})); sys.exit(0)
if cmd == 'device info files':
    if state['state'] != 'ok':
        sys.exit(1)
    base = dev / opt('--subdirectory')
    if not base.is_dir():
        print('ERROR: (com.apple.dt.CoreDeviceError error 7000)', file=sys.stderr); sys.exit(1)
    files = [{'relativePath': p.relative_to(base).as_posix(), 'metadata': {'size': p.stat().st_size if p.is_file() else 64},
              'resources': {'isDirectory': p.is_dir()}} for p in sorted(base.rglob('*'))]
    Path(out).write_text(json.dumps({'result': {'files': files}})); sys.exit(0)
sys.exit(1)
'''


class DeviceCopyTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.tmp = Path(tmp.name)
        self.fake = self.tmp / 'fake'
        (self.fake / 'bin').mkdir(parents=True)
        (self.fake / 'device').mkdir()
        xcrun = self.fake / 'bin/xcrun'
        xcrun.write_text(FAKE_XCRUN.replace('/usr/bin/env python3', sys.executable))
        xcrun.chmod(0o755)
        # A game: one folder in parts (four 30-byte VPKs over a 50-byte part limit), a small one whole.
        self.game = self.tmp / 'Left 4 Dead 2'
        for i in range(4):
            f = self.game / f'left4dead2/pak01_00{i}.vpk'
            f.parent.mkdir(parents=True, exist_ok=True)
            f.write_bytes(bytes([65 + i]) * 30)
        (self.game / 'left4dead2/bin').mkdir()   # empty, like Portal's portal/bin (devicectl refuses those)
        (self.game / 'platform/cfg').mkdir(parents=True)
        (self.game / 'platform/cfg/config.cfg').write_text('bind w +forward\n')
        self.journal = self.tmp / 'journal.json'

    def plan(self, *steps):
        (self.fake / 'plan.json').write_text(json.dumps(list(steps)))

    def copy(self, **env_extra):
        env = dict(os.environ, PATH=f"{self.fake / 'bin'}:/usr/bin:/bin", FAKE=str(self.fake), P2_COPY_POLL='0.01',
                   P2_COPY_PART_BYTES='50', P2_DEVICE_KIND='iPhone', **env_extra)
        pairs = ''.join(f'{self.game / d}\tDocuments/l4d2/game/{d}\n' for d in ('left4dead2', 'platform'))
        return subprocess.run([sys.executable, str(ROOT / 'tools/device_copy.py'), '--device', 'UDID', '--app', 'app.id',
                               '--journal', str(self.journal)], input=pairs, capture_output=True, text=True, env=env,
                              timeout=60)

    def device_has_everything(self):
        for f in self.game.rglob('*'):
            if f.is_file():
                d = self.fake / 'device/Documents/l4d2/game' / f.relative_to(self.game)
                self.assertTrue(d.is_file() and d.read_bytes() == f.read_bytes(), d)

    def copies(self):
        return [l for l in (self.fake / 'calls.log').read_text().splitlines() if l.startswith('device copy to')]

    def test_a_big_folder_goes_in_parts(self):
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.device_has_everything()
        self.assertEqual(len(self.copies()), 5)   # four VPKs one by one, platform whole
        self.assertIn('copy 5/5 Documents/l4d2/game/platform', r.stdout)
        self.assertFalse(self.journal.exists())   # complete: nothing to resume

    def test_a_dropped_connection_is_waited_out(self):
        self.plan('ok', 'drop:3')
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn('status: The iPhone dropped off the connection. Plug it in with a cable, or bring it near the Mac',
                      r.stdout)
        self.assertIn('status: Back: carrying on where it stopped.', r.stdout)
        self.device_has_everything()   # the half-copied VPK was sent again

    def test_a_lock_is_waited_out(self):
        self.plan('lock:2')
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn('status: Your iPhone locked. Unlock it to carry on', r.stdout)
        self.device_has_everything()

    def test_a_hiccup_is_tried_again(self):
        self.plan('ok', 'fail', 'fail')
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn('status: The copy hiccupped: trying again (1 of 3).', r.stdout)
        self.assertIn('trying again (2 of 3)', r.stdout)
        self.device_has_everything()

    def test_retry_picks_up_where_it_stopped(self):
        self.plan('ok', 'ok', 'fail', 'fail', 'fail', 'fail')
        r = self.copy()
        self.assertEqual(r.returncode, 1)
        self.assertIn('error: The copy to the iPhone kept failing', r.stdout)
        self.assertIn('press Retry: it picks up where it stopped', r.stdout)
        self.assertTrue(self.journal.is_file())
        (self.fake / 'calls.log').unlink()
        r = self.copy()   # Retry
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn('Picking up where it stopped: 2 of 5 parts', r.stdout)
        self.assertEqual(len(self.copies()), 3)   # only what hadn't arrived
        self.device_has_everything()

    def test_nothing_is_skipped_when_the_device_lost_it(self):
        self.plan('ok', 'ok', 'fail', 'fail', 'fail', 'fail')
        self.copy()
        import shutil
        shutil.rmtree(self.fake / 'device/Documents')   # Sauce deleted and installed again in between
        (self.fake / 'calls.log').unlink()
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertNotIn('Picking up', r.stdout)
        self.assertEqual(len(self.copies()), 5)
        self.device_has_everything()

    def test_a_changed_file_on_the_mac_is_sent_again(self):
        self.plan('ok', 'ok', 'fail', 'fail', 'fail', 'fail')
        self.copy()
        first = self.game / 'left4dead2/pak01_000.vpk'
        first.write_bytes(b'Z' * 30)   # a Steam update in between, same size
        os.utime(first, ns=(first.stat().st_atime_ns, first.stat().st_mtime_ns + 10**9))
        r = self.copy()
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn('Picking up where it stopped: 1 of 5 parts', r.stdout)
        self.device_has_everything()

    def test_a_device_that_never_comes_back_stops_with_what_to_do(self):
        self.plan('drop:100000')
        r = self.copy(P2_COPY_GONE_WAIT='0.3')
        self.assertEqual(r.returncode, 1)
        self.assertIn("error: The iPhone didn't come back. Plug it in with a cable, unlock it, then press Retry", r.stdout)

    def test_ios_device_sh_copies_through_it(self):
        text = (ROOT / 'tools/ios_device.sh').read_text()
        self.assertEqual(text.count('python3 tools/device_copy.py --device "$DEVICE" --app "$APP_ID"'), 2)  # data, mel


if __name__ == '__main__':
    unittest.main()
