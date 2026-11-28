"""Sauce's setting-up screen (round 31): never a pure black screen while a game
starts. Checked by hand in the iOS Simulator (the screen stays up and its ring
keeps turning while the main thread is blocked, then fades once frames come);
here: it compiles for iOS, main_ios.c shows it as soon as the game is known,
and it goes on the frames both renderers count."""
import shutil
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = (ROOT / 'runtime/ios/loading_ios.m').read_text()


class LoadingScreenTests(unittest.TestCase):
    def test_shown_once_the_game_is_chosen(self):
        main = (ROOT / 'ios/main_ios.c').read_text()
        show = main.index('rt_ios_loading_show(rt_ios_current_game()->name, rt_ios_current_game()->title)')
        self.assertLess(main.index('rt_ios_choose_game()'), show)          # after the picker (which has its own screen)
        self.assertLess(show, main.index('return p2_main(count, arguments)'))
        self.assertLess(main.index('P2_DEVICE_REPORT'), show)               # the installer's quick report never shows it

    def test_alive_while_the_engine_blocks_the_main_thread(self):
        self.assertIn('[CATransaction flush]', SRC)                          # committed before the engine starts
        self.assertIn('animationWithKeyPath:@"transform.rotation.z"', SRC)   # turns in the render server
        self.assertIn('Setting up %@…', SRC)
        self.assertIn('Starting %@…', SRC)
        self.assertIn('P2SauceVersion', SRC)                                 # the version in a corner
        self.assertIn('userInteractionEnabled = NO', SRC)                    # never in the way of the game's touches
        self.assertNotIn('blackColor', SRC)

    def test_gone_when_the_game_draws(self):
        self.assertIn('fps_overlay_presented() >= frames_at_show + 3', SRC)
        fps = (ROOT / 'runtime/fps_overlay.c').read_text()
        self.assertIn('atomic_fetch_add(&presented, 1);', fps.split('int fps_overlay_tick(void)')[1].split('\n}')[0])

    @unittest.skipUnless(shutil.which('xcrun'), 'needs Xcode')
    def test_it_compiles_for_ios(self):
        sdk = subprocess.run(['xcrun', '--sdk', 'iphoneos', '--show-sdk-path'], capture_output=True, text=True)
        if sdk.returncode != 0:
            self.skipTest('no iOS SDK')
        out = subprocess.run(['xcrun', '-sdk', 'iphoneos', 'clang', '-fsyntax-only', '-fobjc-arc', '-Werror',
                              '-target', 'arm64-apple-ios17.0', '-I', str(ROOT / 'runtime/ios'),
                              str(ROOT / 'runtime/ios/loading_ios.m')], capture_output=True, text=True)
        self.assertEqual(out.returncode, 0, out.stderr)


class CommandLineToolsTests(unittest.TestCase):
    """Bug 7: no command line tools on the Mac: Apple's installer starts by itself, first."""
    def test_apples_installer_starts_by_itself(self):
        swift = (ROOT / 'installer/Installer.swift').read_text()
        block = swift.split('await MainActor.run {\n                self.tools = t')[1].split('for (game, g, l) in found')[0]
        self.assertIn('if !t, !self.askedForTools', block)
        self.assertIn('self.installTools()', block)
        self.assertIn('func installTools() { run("/usr/bin/xcode-select", ["--install"]) }', swift)

    def test_nothing_calls_python3_before_them(self):
        # /usr/bin/python3 is a shim without them: it would pop its own prompt and fail silently.
        devices = (ROOT / 'installer/Devices.swift').read_text()
        for fn in ('private func devicesTool', 'nonisolated func runsTool', 'func stopAllRuns', 'nonisolated func checkTools'):
            body = devices.split(fn)[1][:400]
            self.assertIn('hasCommandLineTools()', body, fn)


if __name__ == '__main__':
    unittest.main()
