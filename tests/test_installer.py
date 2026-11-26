"""The Natif Installer's packaging (tools/build_installer.sh) and window
(installer/Installer.swift): static checks, no build needed."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPT = (ROOT / 'tools/build_installer.sh').read_text()
SWIFT = (ROOT / 'installer/Installer.swift').read_text()


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

    def test_version_is_shown(self):
        self.assertIn('P2SourceVersion', SCRIPT)
        self.assertRegex(SWIFT, r'Text\("Version \\\(installerVersion\)"\)')


if __name__ == '__main__':
    unittest.main()
