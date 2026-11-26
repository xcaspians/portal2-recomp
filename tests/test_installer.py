"""The Natif Installer's packaging (tools/build_installer.sh) and window
(installer/Installer.swift): static checks, no build needed."""
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPT = (ROOT / 'tools/build_installer.sh').read_text()
SWIFT = (ROOT / 'installer/Installer.swift').read_text()
DEVICES = (ROOT / 'installer/Devices.swift').read_text()
sys.path.insert(0, str(ROOT / 'tools'))
import steam_library  # noqa: E402


class PackagingTests(unittest.TestCase):
    def test_make_target_builds_the_dmg(self):
        self.assertIn('installer-dmg:\n\ttools/build_installer.sh --dmg', (ROOT / 'Makefile').read_text())

    def test_universal_binary(self):
        self.assertIn('for arch in arm64 x86_64', SCRIPT)
        self.assertIn('lipo -create', SCRIPT)

    def test_dmg_has_the_applications_link_and_is_notarized_and_stapled(self):
        self.assertIn('ln -s /Applications', SCRIPT)
        self.assertIn('hdiutil create', SCRIPT)
        self.assertRegex(SCRIPT, r'notarytool submit "\$DMG_FILE"')
        self.assertIn('stapler staple "$DMG_FILE"', SCRIPT)

    def test_no_secret_in_the_script(self):
        # The notary password lives in the keychain profile only.
        self.assertNotRegex(SCRIPT, r'--password\s')
        self.assertIn('--keychain-profile "$PROFILE"', SCRIPT)

    def test_both_swift_files_are_compiled(self):
        self.assertIn('installer/Installer.swift installer/Devices.swift', SCRIPT)

    def test_version_is_shown(self):
        self.assertIn('P2SourceVersion', SCRIPT)
        self.assertRegex(SWIFT, r'Text\("Version \\\(installerVersion\)"\)')


class WindowTests(unittest.TestCase):
    def test_swift_games_match_steam_library(self):
        rows = re.findall(r'SauceGame\(id: "(\w+)",\s+title: "[^"]+",\s+appID: (\d+),\s+linuxDepot: (\d+),\s+'
                          r'dataDepot: (\d+),\s+folder: "([^"]+)",\s+marker: "([^"]+)"', DEVICES)
        self.assertEqual([(g, int(a), int(l), int(d), f, m) for g, a, l, d, f, m in rows],
                         [(g, a, l, d, f, m) for g, _t, a, l, d, f, m in steam_library.GAMES])

    def test_never_asks_for_a_password(self):
        for source in (SWIFT, DEVICES):
            self.assertNotIn('SecureField', source)
            self.assertNotIn('loginusers', source)
        # Signing goes through Xcode's own account (xcodebuild -allowProvisioningUpdates in setup.sh).
        self.assertIn('never sees your password', DEVICES)

    def test_saves_backed_up_before_replacing(self):
        self.assertIn('"--backup", backup.path', DEVICES)
        self.assertIn('["saves-backup", backup.path]', DEVICES)

    def test_updates_are_offered_never_installed_alone(self):
        self.assertIn('releases/latest', DEVICES)
        self.assertNotIn('installer.app', DEVICES.lower().replace('natif installer', ''))   # nothing replaces itself
        self.assertIn('NSWorkspace.shared.open(newer.url)', SWIFT)   # a click opens the download


if __name__ == '__main__':
    unittest.main()
