"""ios/signing/sign.py: the signing identity must be one the profile allows."""
import hashlib
import importlib.util
import unittest
from datetime import datetime, timezone
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



class ChooseProfile(unittest.TestCase):
    NOW = datetime(2026, 11, 24, tzinfo=timezone.utc)

    @staticmethod
    def profile(app_id, created, expires=2027):
        return {'Entitlements': {'application-identifier': app_id}, 'CreationDate': datetime(created, 1, 1),
                'ExpirationDate': datetime(expires, 1, 1, tzinfo=timezone.utc)}

    def test_exact_profile_wins_over_the_team_wildcard(self):
        exact, wild = self.profile('T.local.portal2recomp.t', 2025), self.profile('T.*', 2026)
        got = sign.choose_profile([('w', wild), ('e', exact)], 'local.portal2recomp.t', self.NOW)
        self.assertEqual(got[0], 'e')   # Portal 2 keeps its own profile (increased memory, iCloud)

    def test_wildcard_signs_an_app_without_its_own_profile(self):
        wild = self.profile('T.*', 2026)
        got = sign.choose_profile([('e', self.profile('T.local.portal2recomp.t', 2025)), ('w', wild)],
                                  'local.melrecomp.t', self.NOW)
        self.assertEqual(got[0], 'w')
        self.assertEqual(sign.app_entitlements(wild['Entitlements'], 'local.melrecomp.t')['application-identifier'],
                         'T.local.melrecomp.t')

    def test_expired_profiles_and_none(self):
        self.assertIsNone(sign.choose_profile([('w', self.profile('T.*', 2024, expires=2025))], 'x.y', self.NOW))
        self.assertIsNone(sign.choose_profile([], 'x.y', self.NOW))
        exact = {'application-identifier': 'T.x.y'}
        self.assertEqual(sign.app_entitlements(exact, 'x.y'), exact)

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
