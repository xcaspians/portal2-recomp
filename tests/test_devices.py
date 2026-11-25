"""tools/devices.py: the Natif Installer's Devices planning, with devicectl-shaped listings."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import devices  # noqa: E402


def dev(name, udid, kind='iPhone', paired='paired', tunnel='disconnected', link='localNetwork', boot='booted',
        devmode='enabled', reality='physical'):
    return {'hardwareProperties': {'reality': reality, 'deviceType': kind, 'udid': udid, 'marketingName': name + ' model'},
            'connectionProperties': {'pairingState': paired, 'tunnelState': tunnel, 'transportType': link},
            'deviceProperties': {'name': name, 'bootState': boot, 'developerModeStatus': devmode}}


class DevicesTest(unittest.TestCase):
    def test_devices_and_plain_problems(self):
        report = {'result': {'devices': [
            dev('Phone', 'A'), dev('Pad', 'B', kind='iPad', link='wired'),
            dev('Old', 'C', tunnel='unavailable', boot=None), dev('New', 'D', paired='unpaired'),
            dev('Dev', 'E', devmode='disabled'), dev('Sim', 'F', reality='simulated'), dev('Watch', 'G', kind='appleWatch')]}}
        found = {d['udid']: d for d in devices.parse_devices(report)}
        self.assertEqual(set(found), {'A', 'B', 'C', 'D', 'E'})          # no simulator, no watch
        self.assertTrue(found['A']['ready'] and found['A']['link'] == 'Wi-Fi')
        self.assertEqual(found['B']['link'], 'USB')
        self.assertIn('Not reachable', found['C']['problem'])
        self.assertIn('Trust', found['D']['problem'])
        self.assertIn('Developer Mode', found['E']['problem'])
        self.assertEqual([d['ready'] for d in devices.parse_devices(report)][:2], [True, True])   # ready ones first

    def test_plain_errors(self):
        self.assertIn('locked', devices.plain_error('ERROR: The device is passcode locked'))
        self.assertIn('Trust', devices.plain_error('device is not paired'))
        self.assertIn('stopped answering', devices.plain_error('Operation timed out'))

    def test_plan(self):
        built = {'portal2': None, 'mel': None, 'workshop': None}
        fresh = {'app_installed': False, 'free_bytes': None, 'data': {}}
        self.assertIn('Portal 2 first', devices.plan('mel', fresh, built, 6_800_000_000)[1])   # Mel's slim pack uses its files
        fresh['data'] = {'portal2': True}
        steps, problem = devices.plan('mel', fresh, built, 6_800_000_000)
        self.assertIsNone(problem)
        self.assertEqual([c for _, c in steps], [['install'], ['mel', 'data', 'push']])   # the app first
        ready = {'app_installed': True, 'free_bytes': 50_000_000_000, 'data': {'portal2': True}}
        self.assertEqual([c for _, c in devices.plan('portal2', ready, built, 11_500_000_000)[0]], [['data']])
        self.assertEqual([c for _, c in devices.plan('workshop', ready, built, 800_000_000)[0]], [['workshop', 'push']])

    def test_plan_refusals(self):
        built = {'portal2': None, 'mel': 'Mel isn\'t built', 'workshop': None}
        tight = {'app_installed': True, 'free_bytes': 9_800_000_000, 'data': {}}
        steps, problem = devices.plan('portal2', tight, built, 11_500_000_000)
        self.assertEqual(steps, [])
        self.assertIn('Not enough space', problem)
        self.assertEqual(devices.plan('mel', tight, built, 1)[1], "Mel isn't built")
        self.assertIn('Portal 2 first', devices.plan('workshop', tight, built, 1)[1])   # maps play in Portal 2
        self.assertIn('unknown', devices.plan('hl3', tight, built, 1)[1])


class BaseGameTest(unittest.TestCase):
    def test_portal2_is_never_removable(self):
        self.assertFalse(devices.removable('portal2'))
        self.assertTrue(devices.removable('mel'))
        self.assertFalse(devices.removable('nonsense'))


if __name__ == '__main__':
    unittest.main()
