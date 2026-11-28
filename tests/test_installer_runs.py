"""tools/installer_runs.py: the installer's runs die with it, leave a record
saying who holds them, and leftovers are found and stopped (round 31: after
Cmd+Q the build kept running, and the next launch sat on "Waiting")."""
import json
import os
import signal
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
RUNS = str(ROOT / 'tools/installer_runs.py')


def gone(marker, wait=8):
    end = time.time() + wait
    while time.time() < end:
        if subprocess.run(['pgrep', '-f', marker], capture_output=True).returncode != 0:
            return True
        time.sleep(0.1)
    return False


def running(marker):
    return subprocess.run(['pgrep', '-f', marker], capture_output=True).returncode == 0


class RunTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.state = Path(tmp.name) / 'runs'
        self.repo = Path(tmp.name) / 'Portal 2 Natif Build'
        (self.repo / 'tools').mkdir(parents=True)
        self.marker = f'r31marker{os.getpid()}{time.time_ns() % 100000}'
        # A build that starts a child and a grandchild, like setup.sh > build_game.py > clang.
        self.script = self.repo / 'tools/build.sh'
        self.script.write_text(f'#!/bin/bash\nbash -c "sleep 300; : {self.marker}a" &\n'
                               f'bash -c "bash -c \\"sleep 300; : {self.marker}b\\"; :" &\nwait\n')
        self.script.chmod(0o755)
        self.addCleanup(lambda: subprocess.run(['pkill', '-9', '-f', self.marker]))

    def owner(self, *run_args):
        """A stand-in installer: a process that starts one run and waits."""
        cmd = [sys.executable, RUNS, 'run', '--state', str(self.state), *run_args, '--', str(self.script)]
        return subprocess.Popen([sys.executable, '-c', 'import subprocess, sys; subprocess.run(sys.argv[1:])', *cmd])

    def wait_for_record(self):
        end = time.time() + 5
        while time.time() < end:
            recs = list(self.state.glob('*.json'))
            if recs and running(self.marker + 'b'):
                return json.loads(recs[0].read_text())
            time.sleep(0.05)
        self.fail('the run never started')

    def test_the_record_says_who_holds_it(self):
        owner = self.owner('--label', 'Left 4 Dead 2: copying', '--game', 'l4d2')
        self.addCleanup(owner.kill)
        r = self.wait_for_record()
        self.assertEqual(r['owner'], owner.pid)
        self.assertTrue(r['owner_started'])
        self.assertEqual(r['label'], 'Left 4 Dead 2: copying')
        listed = json.loads(subprocess.run([sys.executable, RUNS, 'list', '--state', str(self.state)],
                                           capture_output=True, text=True).stdout)
        self.assertEqual([(x['game'], x['owner_alive']) for x in listed], [('l4d2', True)])

    def test_force_quit_takes_the_whole_build_down(self):
        owner = self.owner()
        self.wait_for_record()
        owner.send_signal(signal.SIGKILL)   # Force Quit, or a crash: no cleanup code runs in the installer
        owner.wait()
        self.assertTrue(gone(self.marker), 'the build outlived the installer')
        end = time.time() + 3
        while list(self.state.glob('*.json')) and time.time() < end:
            time.sleep(0.05)
        self.assertEqual(list(self.state.glob('*.json')), [], 'the lock stayed behind')

    def test_quit_stops_every_run_of_that_installer(self):
        owner = self.owner()
        self.addCleanup(owner.kill)
        self.wait_for_record()
        out = subprocess.run([sys.executable, RUNS, 'stop', '--state', str(self.state), '--owner', str(owner.pid)],
                             capture_output=True, text=True)
        self.assertTrue(json.loads(out.stdout)[0]['stopped'])
        self.assertTrue(gone(self.marker))

    def test_a_timeout_stops_it_and_says_so(self):
        out = subprocess.run([sys.executable, RUNS, 'run', '--state', str(self.state), '--label', 'Reading the iPhone',
                              '--timeout', '1', '--', str(self.script)], capture_output=True, text=True, timeout=30)
        self.assertEqual(out.returncode, 124)
        self.assertIn("error: Reading the iPhone didn't answer within 1 seconds", out.stderr)
        self.assertTrue(gone(self.marker))

    def test_a_lock_with_no_live_owner_is_cleared(self):
        self.state.mkdir(parents=True)
        dead = subprocess.Popen(['true']); dead.wait()
        (self.state / '99999.json').write_text(json.dumps({'pid': 99999, 'owner': dead.pid, 'owner_started': 'x',
                                                           'label': 'old', 'started': 0}))
        subprocess.run([sys.executable, RUNS, 'clean', '--state', str(self.state)], capture_output=True, check=True)
        self.assertEqual(list(self.state.glob('*.json')), [])

    def test_a_live_installers_run_is_never_touched(self):
        owner = self.owner()
        self.addCleanup(owner.kill)
        self.wait_for_record()
        out = subprocess.run([sys.executable, RUNS, 'clean', '--state', str(self.state), '--repo', str(self.repo)],
                             capture_output=True, text=True)
        self.assertEqual(json.loads(out.stdout), [])
        self.assertTrue(running(self.marker + 'a'))

    def test_an_older_installers_leftover_is_found_and_stopped(self):
        # v0.5.1 started setup.sh without a record; after its quit the build ran on, under launchd.
        subprocess.Popen([str(self.script)], start_new_session=True, stdout=subprocess.DEVNULL)
        end = time.time() + 5
        while not running(self.marker + 'b') and time.time() < end:
            time.sleep(0.05)
        listed = json.loads(subprocess.run([sys.executable, RUNS, 'list', '--state', str(self.state), '--repo',
                                            str(self.repo)], capture_output=True, text=True).stdout)
        self.assertEqual(len(listed), 1, listed)   # the root only, not each of its children
        self.assertFalse(listed[0]['owner_alive'])
        out = subprocess.run([sys.executable, RUNS, 'clean', '--state', str(self.state), '--repo', str(self.repo)],
                             capture_output=True, text=True)
        self.assertTrue(json.loads(out.stdout)[0]['stopped'])
        self.assertTrue(gone(self.marker))


if __name__ == '__main__':
    unittest.main()
