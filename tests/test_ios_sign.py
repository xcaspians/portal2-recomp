"""ios/signing/sign.py: the signing identity must be one the profile allows."""
import hashlib
import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('ios_sign', ROOT / 'ios/signing/sign.py')
sign = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sign)

FREE, PAID = b'free-team certificate', b'paid-team certificate'


def sha1(der):
    return hashlib.sha1(der).hexdigest().upper()


# Two Apple accounts in Xcode: a free team listed first, the paid team second.
FIND_IDENTITY = f'''  1) {sha1(FREE)} "Apple Development: someone@icloud.com (4289AHMJB2)"
  2) {sha1(PAID)} "Apple Development: Someone (CAU4T2RD85)"
  3) {'0' * 40} "Developer ID Application: SOMEONE (3RG2R8TYFA)"
     3 valid identities found
'''


class PickIdentity(unittest.TestCase):
    def test_identity_comes_from_the_profile_certificates(self):
        # A paid-team profile: signing with the free team's certificate made
        # the iPhone refuse the app ("A valid provisioning profile for this
        # executable was not found").
        self.assertEqual(sign.pick_identity(FIND_IDENTITY, [PAID]), sha1(PAID))
        self.assertEqual(sign.pick_identity(FIND_IDENTITY, [FREE]), sha1(FREE))

    def test_no_certificate_of_the_profile_in_the_keychain(self):
        with self.assertRaises(SystemExit):
            sign.pick_identity(FIND_IDENTITY, [b'another certificate'])


if __name__ == '__main__':
    unittest.main()


class NarrowEntitlements(unittest.TestCase):
    """iCloud saves: the profile's iCloud wildcards become the app's own values."""
    BASE = {'application-identifier': 'TEAM.local.portal2recomp.x', 'get-task-allow': True,
            'com.apple.developer.kernel.increased-memory-limit': True}

    def test_without_icloud_nothing_changes(self):
        self.assertEqual(sign.narrow_entitlements(self.BASE), self.BASE)

    def test_icloud_wildcards_become_the_container(self):
        profile = dict(self.BASE, **{
            'com.apple.developer.icloud-container-identifiers': ['iCloud.local.portal2recomp.x'],
            'com.apple.developer.icloud-services': '*',
            'com.apple.developer.ubiquity-container-identifiers': ['TEAM.*'],
            'com.apple.developer.icloud-container-environment': ['Development', 'Production'],
            'com.apple.developer.ubiquity-kvstore-identifier': 'TEAM.*'})
        narrowed = sign.narrow_entitlements(profile)
        self.assertEqual(narrowed['com.apple.developer.icloud-container-identifiers'], ['iCloud.local.portal2recomp.x'])
        self.assertEqual(narrowed['com.apple.developer.ubiquity-container-identifiers'], ['iCloud.local.portal2recomp.x'])
        self.assertEqual(narrowed['com.apple.developer.icloud-services'], ['CloudDocuments'])
        self.assertEqual(narrowed['com.apple.developer.icloud-container-environment'], 'Development')
        self.assertNotIn('com.apple.developer.ubiquity-kvstore-identifier', narrowed)
        self.assertTrue(all('*' not in str(v) for v in narrowed.values()))
        self.assertEqual(profile['com.apple.developer.icloud-services'], '*')   # the profile itself untouched

    def test_icloud_without_a_container_is_dropped(self):
        profile = dict(self.BASE, **{'com.apple.developer.icloud-container-identifiers': [],
                                     'com.apple.developer.icloud-services': '*'})
        self.assertEqual(sign.narrow_entitlements(profile), self.BASE)
