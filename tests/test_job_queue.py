"""installer/JobQueue.swift's queue, the loop the window runs, checked by
tests/job_queue_check.swift (round 31: a multi-game install wedged after one
game failed; a quit left the next launch on "Waiting")."""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


@unittest.skipUnless(shutil.which('swiftc'), 'needs swiftc (Xcode or the command line tools)')
class JobQueueTests(unittest.TestCase):
    def test_failure_retry_quit_and_reopen(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe = Path(tmp) / 'job-queue-check'
            build = subprocess.run(['swiftc', '-parse-as-library', '-swift-version', '5',
                                    str(ROOT / 'installer/JobQueue.swift'), str(ROOT / 'tests/job_queue_check.swift'),
                                    '-o', str(exe)], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stderr[-3000:])
            run = subprocess.run([str(exe)], capture_output=True, text=True, timeout=60)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
            self.assertIn('job-queue-check: PASS', run.stdout)


if __name__ == '__main__':
    unittest.main()
