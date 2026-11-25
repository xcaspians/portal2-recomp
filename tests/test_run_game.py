"""tools/run_game.py: a game played by hand (--show) is never stopped by the
diagnostic timeout unless one is asked for."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import run_game  # noqa: E402


class LaunchTimeout(unittest.TestCase):
    def test_playing_by_hand_has_no_timeout(self):
        self.assertIsNone(run_game.launch_timeout(None, True))
        self.assertEqual(run_game.launch_timeout(None, False), 60)   # diagnostic launch
        self.assertEqual(run_game.launch_timeout(300, True), 300)    # asked for explicitly


if __name__ == '__main__':
    unittest.main()
