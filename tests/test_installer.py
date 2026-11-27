"""The Natif Installer's packaging (tools/build_installer.sh) and window
(installer/Installer.swift): static checks, no build needed."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPT = (ROOT / 'tools/build_installer.sh').read_text()
SWIFT = (ROOT / 'installer/Installer.swift').read_text()
DEVICES = (ROOT / 'installer/Devices.swift').read_text()


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

    def test_release_is_never_ad_hoc(self):
        # --release stops unless signed with a Developer ID, notarized, stapled and accepted by spctl.
        self.assertIn('installer-release:\n\ttools/build_installer.sh --release', (ROOT / 'Makefile').read_text())
        self.assertIn('PROFILE="${P2_NOTARY_PROFILE:-sauce-notary}"', SCRIPT)
        self.assertIn('[ "$IDENTITY" != - ] || fail "--release: no Developer ID', SCRIPT)
        self.assertIn('xcrun stapler validate "$DMG_FILE"', SCRIPT)
        self.assertIn("flags=0x10000(runtime)", SCRIPT)
        self.assertIn('--options runtime --timestamp', SCRIPT)
        # The published notes have no Open Anyway step.
        for notes in (ROOT / 'docs/fork/releases').glob('v*.md'):
            self.assertNotIn('Open Anyway', notes.read_text(), notes.name)

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
    # The game list itself: tests/test_installer_table.py.
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
