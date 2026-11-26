"""ios/signing/project-icloud.yml: the generated P2Signing.xcodeproj marks iCloud
as a real dictionary (round 25), so Xcode's Signing & Capabilities tab can read
it, and asks for the one existing container, iCloud.<bundle id>."""
import importlib.util
import json
import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('ios_capabilities', ROOT / 'ios/signing/capabilities.py')
capabilities = importlib.util.module_from_spec(spec)
spec.loader.exec_module(capabilities)

# What XcodeGen 2.46 wrote (round 25 diagnosis), between two untouched entries.
BROKEN = '''\t\t\t\tTargetAttributes = {
\t\t\t\t\t2011B873BBDFD23261217D53 = {
\t\t\t\t\t\tProvisioningStyle = Automatic;
\t\t\t\t\t\tSystemCapabilities = "[\\"com.apple.iCloud\\": [\\"enabled\\": 1]]";
\t\t\t\t\t};
\t\t\t\t\tBCBE4371E8937C3237D2A8F7 = {
\t\t\t\t\t\tProvisioningStyle = Automatic;
\t\t\t\t\t};
'''


class FixText(unittest.TestCase):
    def test_stringified_capabilities_become_a_dictionary(self):
        fixed = capabilities.fix(BROKEN)
        self.assertNotIn('SystemCapabilities = "', fixed)
        self.assertIn('\t\t\t\t\t\tSystemCapabilities = {\n'
                      '\t\t\t\t\t\t\tcom.apple.iCloud = {\n'
                      '\t\t\t\t\t\t\t\tenabled = 1;\n'
                      '\t\t\t\t\t\t\t};\n'
                      '\t\t\t\t\t\t};\n', fixed)
        # Everything else unchanged, and running it again changes nothing.
        lines = BROKEN.splitlines(True)
        self.assertTrue(fixed.startswith(''.join(lines[:3])) and fixed.endswith(''.join(lines[4:])))
        self.assertEqual(capabilities.fix(fixed), fixed)

    def test_swift_dictionary_text(self):
        self.assertEqual(capabilities.parse('["com.apple.iCloud": ["enabled": 1], "com.apple.Push": ["enabled": false]]'),
                         {'com.apple.iCloud': {'enabled': 1}, 'com.apple.Push': {'enabled': 0}})
        self.assertEqual(capabilities.parse('[:]'), {})
        with self.assertRaises(ValueError):
            capabilities.parse('["a": 1')


@unittest.skipUnless(shutil.which('xcodegen') and shutil.which('plutil'), 'needs xcodegen and plutil (macOS)')
class GeneratedProject(unittest.TestCase):
    def test_icloud_capability_is_a_dictionary_and_the_container_is_reused(self):
        bundle = 'local.portal2recomp.vn8jwve7jx'
        with tempfile.TemporaryDirectory() as tmp:
            work = Path(tmp) / 'signing'
            shutil.copytree(ROOT / 'ios/signing', work, ignore=shutil.ignore_patterns('P2Signing.xcodeproj', '*.entitlements'))
            subprocess.run(['xcodegen', '-q', '--spec', 'project-icloud.yml'], cwd=work, check=True,
                           env={**os.environ, 'P2_BUNDLE_ID': bundle, 'P2_TEAM_ID': 'TEAM123456'})
            pbxproj = work / 'P2Signing.xcodeproj/project.pbxproj'
            data = json.loads(subprocess.run(['plutil', '-convert', 'json', '-o', '-', str(pbxproj)],
                                             check=True, capture_output=True, text=True).stdout)
            objects = data['objects']
            project = next(o for o in objects.values() if o.get('isa') == 'PBXProject')
            names = {k: o.get('name') for k, o in objects.items() if o.get('isa') == 'PBXNativeTarget'}
            attributes = {names[k]: v for k, v in project['attributes']['TargetAttributes'].items()}
            caps = attributes['Portal2']['SystemCapabilities']
            self.assertIsInstance(caps, dict)
            self.assertEqual(caps['com.apple.iCloud'], {'enabled': '1'})   # OpenStep plists keep numbers as text
            self.assertNotIn('SystemCapabilities', attributes['Probe'])
            # The team is in the project (setup.sh's P2_TEAM_ID): without it Xcode's Signing &
            # Capabilities tab showed no iCloud and + Capability > iCloud did nothing.
            configs = [o for o in objects.values() if o.get('isa') == 'XCBuildConfiguration']
            self.assertTrue(configs and all(c['buildSettings'].get('DEVELOPMENT_TEAM', 'TEAM123456') == 'TEAM123456' for c in configs))
            self.assertTrue(any(c['buildSettings'].get('DEVELOPMENT_TEAM') == 'TEAM123456' for c in configs))
            # One container, the existing one: iCloud.<the app's identifier>.
            entitlements = subprocess.run(['plutil', '-convert', 'json', '-o', '-', str(work / 'Portal2-icloud.entitlements')],
                                          check=True, capture_output=True, text=True).stdout
            ent = json.loads(entitlements)
            self.assertEqual(ent['com.apple.developer.icloud-container-identifiers'], [f'iCloud.{bundle}'])
            self.assertEqual(ent['com.apple.developer.ubiquity-container-identifiers'], [f'iCloud.{bundle}'])


if __name__ == '__main__':
    unittest.main()
