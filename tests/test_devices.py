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
        self.assertEqual((found['A']['fix'], found['A']['steps']), (None, []))

    def test_developer_mode_off_walks_through_turning_it_on(self):
        found = {d['udid']: d for d in devices.parse_devices({'result': {'devices': [
            dev('Pad', 'E', kind='iPad', devmode='disabled'), dev('New', 'D', paired='unpaired', devmode=None),
            dev('Rebooting', 'R', tunnel='unavailable', boot='booting')]}})}
        pad = found['E']
        self.assertFalse(pad['ready'])
        self.assertEqual(pad['fix'], 'developer-mode')
        steps = ' '.join(pad['steps'])
        # Settings → Privacy & Security → Developer Mode → On → restart → confirm, in that order.
        order = [steps.index(w) for w in ('Settings → Privacy & Security', 'tap Developer Mode', 'On, then tap Restart',
                                          'restarted', 'tap Turn On', 'by itself')]
        self.assertEqual(order, sorted(order))
        self.assertIn('only appears once the iPad has been paired with a Mac', steps)
        # Not paired yet: pairing comes first (Developer Mode isn't even reported before it).
        self.assertEqual(found['D']['fix'], 'pair')
        self.assertIn('Trust', ' '.join(found['D']['steps']))
        self.assertIn('Developer Mode', found['D']['problem'])
        # During the restart: waited for, not an error.
        self.assertEqual(found['R']['fix'], 'restarting')

    def test_pick_for_setup(self):
        # devicectl lists an Apple TV first on the maintainer's Mac: setup.sh picked it.
        tv = dev('Bedroom', 'TV', kind='appleTV')
        report = {'result': {'devices': [tv, dev('Pad', 'P', kind='iPad', link='wired'), dev('Phone', 'W'),
                                         dev('Phone cable', 'U', link='wired'), dev('Locked', 'L', tunnel='unavailable', boot=None)]}}
        self.assertEqual(devices.pick(devices.parse_devices(report)), 'U')   # an iPhone, on the cable
        report['result']['devices'] = [tv, dev('Pad', 'P', kind='iPad'), dev('Locked phone', 'L', tunnel='unavailable', boot=None)]
        self.assertEqual(devices.pick(devices.parse_devices(report)), 'P')   # a ready iPad before a phone that isn't
        self.assertIsNone(devices.pick(devices.parse_devices({'result': {'devices': [tv]}})))

    def test_plain_errors(self):
        self.assertIn('locked', devices.plain_error('ERROR: The device is passcode locked'))
        self.assertIn('Trust', devices.plain_error('device is not paired'))
        self.assertIn('stopped answering', devices.plain_error('Operation timed out'))
        # Wi-Fi dropping mid-copy (seen on the iPad Pro, round 28).
        self.assertIn('stopped answering', devices.plain_error('The connection to the remote device is no longer valid.'))

    def test_plan(self):
        built = {'portal2': None, 'mel': None, 'workshop': None}
        fresh = {'app_installed': False, 'free_bytes': None, 'data': {}}
        self.assertIn('Portal 2 first', devices.plan('mel', fresh, built, 6_800_000_000)[1])   # Mel's slim pack uses its files
        fresh['data'] = {'portal2': True}
        steps, problem = devices.plan('mel', fresh, built, 6_800_000_000)
        self.assertIsNone(problem)
        self.assertEqual([c for _, c in steps], [['install'], ['mel', 'data', 'push'], ['--game', 'mel', 'manifest']])   # the app first
        ready = {'app_installed': True, 'free_bytes': 50_000_000_000, 'data': {'portal2': True}}
        self.assertEqual([c for _, c in devices.plan('portal2', ready, built, 11_500_000_000)[0]],
                         [['data'], ['--game', 'portal2', 'manifest']])
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

    def test_plan_for_portal_and_the_games_with_their_own_app(self):
        built = dict.fromkeys(devices.DOWNLOADS)
        # Portal lives in the Portal app and needs no other game's data.
        bare = {'app_installed': True, 'apps': {'portal2': True}, 'free_bytes': 50_000_000_000, 'data': {}}
        steps, problem = devices.plan('portal1', bare, built, 4_400_000_000)
        self.assertIsNone(problem)
        self.assertEqual([c for _, c in steps], [['--game', 'portal1', 'data'], ['--game', 'portal1', 'manifest']])
        # Half-Life 2 and Left 4 Dead 2 live in the one app too since round 19:
        # their data into it, the app installed first when missing.
        self.assertEqual(devices.OWN_APP, ())
        steps, problem = devices.plan('l4d2', bare, built, 14_000_000_000)
        self.assertIsNone(problem)
        self.assertEqual([c for _, c in steps], [['--game', 'l4d2', 'data'], ['--game', 'l4d2', 'manifest']])
        no_app = dict(bare, app_installed=False, apps={'portal2': False})
        self.assertEqual([c for _, c in devices.plan('hl2', no_app, built, 10_000_000_000)[0]],
                         [['install'], ['--game', 'hl2', 'data'], ['--game', 'hl2', 'manifest']])
        # Not built on this Mac: the sentence, no steps.
        self.assertEqual(devices.plan('hl2', bare, dict(built, hl2='HL2 not built'), 1), ([], 'HL2 not built'))
        self.assertEqual(devices.app_game('portal1'), 'portal2')
        self.assertEqual(devices.app_game('l4d2'), 'portal2')


    def test_plan_backs_up_saves_before_replacing_and_updates_only_what_changed(self):
        built = dict.fromkeys(devices.DOWNLOADS)
        on = {'app_installed': True, 'apps': {'portal2': True}, 'free_bytes': 50_000_000_000,
              'data': {'portal2': True}}
        steps, problem = devices.plan('portal2', on, built, 1, backup='/tmp/b')
        self.assertIsNone(problem)
        self.assertEqual(steps[0][1], ['saves-backup', '/tmp/b'])   # before the copy replaces anything
        steps, _ = devices.plan('portal2', on, built, 1, only=['Documents/prep', 'Documents/game/portal2'])
        self.assertEqual(steps[0][1], ['data', '--only', 'Documents/prep Documents/game/portal2'])
        self.assertIn('2 folders changed', steps[0][0])
        # Nothing on the device yet: nothing to back up.
        fresh = dict(on, data={})
        self.assertNotIn('saves-backup', [c[0] for _, c in devices.plan('portal2', fresh, built, 1, backup='/tmp/b')[0]])


    def test_status_asks_every_game_in_the_app(self):
        self.assertEqual(devices.IN_APP, tuple(devices.games.installer_games()))

    def test_device_tools_get_home(self):
        # Round 28: status ran ios_device.sh without HOME, so has-data failed and
        # every game showed as missing from the device.
        env = devices.device_env('UDID')
        self.assertEqual(env['P2_DEVICE'], 'UDID')
        self.assertIn('HOME', env)
        self.assertNotIn("'PATH': '/usr/bin", (devices.ROOT / 'tools/devices.py').read_text())


class InstallerMatchesDevicesTest(unittest.TestCase):
    def test_installer_lists_every_download_and_names_the_one_app(self):
        swift = (devices.ROOT / 'installer/Devices.swift').read_text()
        self.assertIn(f'let deviceAppName = "{devices.games.APP_NAME}"', swift)   # rename: both, tests keep them equal


class DeveloperModeWindowTest(unittest.TestCase):
    def test_window_shows_the_steps_pairs_and_waits_to_install(self):
        swift = (devices.ROOT / 'installer/Devices.swift').read_text()
        self.assertIn('var steps: [String]? = nil', swift)            # decoded from devices.py list
        self.assertIn('Text("\\(i + 1). \\(s)")', swift)             # numbered, as devices.py orders them
        self.assertIn('["devicectl", "manage", "pair", "--device", d.udid]', swift)   # pair first
        self.assertIn('if !d.ready {   // Developer Mode, pairing or a restart first', swift)
        self.assertIn('waitForDevice(d.udid, then: games)', swift)
        self.assertIn('if let d = list.first(where: { $0.udid == udid }), d.ready {', swift)   # then carries on
        self.assertIn('if waitingFor == nil, selected == nil', swift)  # the device keeps its place while it restarts


class BaseGameTest(unittest.TestCase):
    def test_portal2_is_never_removable(self):
        self.assertFalse(devices.removable('portal2'))
        self.assertTrue(devices.removable('mel'))
        self.assertFalse(devices.removable('nonsense'))


if __name__ == '__main__':
    unittest.main()
