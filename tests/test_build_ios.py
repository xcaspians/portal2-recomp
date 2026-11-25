"""iOS build paths and identifiers of each game (tools/build_ios.py,
tools/ios_bundle_id.py, tools/ios_game.py): one checkout, one folder and one
app identifier per game, never another game's config keys."""
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import build_ios  # noqa: E402
import games  # noqa: E402
import ios_bundle_id  # noqa: E402
import ios_game  # noqa: E402
import localconfig  # noqa: E402

# Portal 2's keys at the top level, the other games' in their section (setup.sh).
BOTH = {'game_root': '/p2', 'linux_root': '/p2-linux', 'ios_bundle_id': 'local.portal2recomp.team',
        'games': {'hl2': {'game_root': '/hl2', 'linux_root': '/hl2-linux',
                          'ios_bundle_id': 'local.hl2recomp.team'}}}
THREE = {'game_root': '/p2', 'linux_root': '/p2-linux', 'ios_bundle_id': 'local.portal2recomp.team',
         'games': {'hl2': {'game_root': '/hl2', 'linux_root': '/hl2-linux',
                           'ios_bundle_id': 'local.hl2recomp.team'},
                   'portal1': {'game_root': '/p1', 'linux_root': '/p1-linux',
                               'ios_bundle_id': 'local.portal1recomp.team'}}}


def no_bundle_env():
    env = {k: v for k, v in os.environ.items() if k not in ('P2_BUNDLE_ID', 'P2_GAME', 'P2_LANGUAGE')}
    return mock.patch.dict(os.environ, env, clear=True)


class IosPaths(unittest.TestCase):
    def test_app_folders(self):
        p2, hl2, p1 = games.GAMES['portal2'], games.GAMES['hl2'], games.GAMES['portal1']
        self.assertTrue(p2['ios'] and hl2['ios'] and p1['ios'])
        # Portal 2 keeps the paths of existing docs and tools.
        self.assertEqual(games.ios_app(p2, 'sim'), Path('build/ios/sim/Portal2.app'))
        self.assertEqual(games.ios_app(p2, 'device'), Path('build/ios/device/Portal2.app'))
        self.assertEqual(games.ios_app(hl2, 'sim'), Path('build/hl2/ios/sim/HalfLife2.app'))
        self.assertEqual(games.ios_app(hl2, 'device'), Path('build/hl2/ios/device/HalfLife2.app'))
        self.assertEqual(games.ios_app(p1, 'sim'), Path('build/portal1/ios/sim/Portal.app'))
        self.assertEqual(games.ios_app(p1, 'device'), Path('build/portal1/ios/device/Portal.app'))
        # Each iPhone build stays inside its game's build tree (make clean, uninstall).
        parents = [games.ios_app(g, 'sim').parent for g in (p2, hl2, p1)]
        self.assertEqual(len(set(parents)), 3)
        for game in (hl2, p1):
            self.assertTrue(str(games.ios_app(game, 'sim')).startswith(game['build_dir'] + '/'))

    def test_shell_view_uses_each_games_keys(self):
        with no_bundle_env():
            p2 = ios_game.values('portal2', 'sim', BOTH)
            hl2 = ios_game.values('hl2', 'device', BOTH)
        self.assertEqual((p2['GAME_ROOT'], p2['LINUX_ROOT'], p2['PREP'], p2['IOS_DIR'], p2['IOS_APP'], p2['MOD']),
                         ('/p2', '/p2-linux', 'build/game/prep', 'build/ios/sim', 'build/ios/sim/Portal2.app', 'portal2'))
        self.assertEqual((hl2['GAME_ROOT'], hl2['LINUX_ROOT'], hl2['PREP'], hl2['IOS_DIR'], hl2['IOS_APP'],
                          hl2['MOD']),
                         ('/hl2', '/hl2-linux', 'build/hl2/prep', 'build/hl2/ios/device',
                          'build/hl2/ios/device/HalfLife2.app', 'hl2_complete'))
        self.assertEqual((p2['APP_ID'], hl2['APP_ID'], hl2['PROBE_ID']),
                         ('local.portal2recomp.team', 'local.hl2recomp.team', 'local.hl2recomp.team.probe'))
        self.assertEqual(hl2['IOS_DATA'].split(), list(games.GAMES['hl2']['ios_data']))
        # HL2 not set up: empty folders, never Portal 2's.
        with no_bundle_env():
            alone = ios_game.values('hl2', 'sim', {'game_root': '/p2', 'linux_root': '/p2-linux'})
        self.assertEqual((alone['GAME_ROOT'], alone['LINUX_ROOT']), ('', ''))
        with no_bundle_env():
            p1 = ios_game.values('portal1', 'device', THREE)
        # Portal 1 lives in the Portal app (Portal 2's ios_guests): that app, its
        # ID, and its own data in Documents/portal1.
        self.assertEqual((p1['GAME_ROOT'], p1['LINUX_ROOT'], p1['PREP'], p1['IOS_DIR'], p1['IOS_APP'], p1['MOD']),
                         ('/p1', '/p1-linux', 'build/portal1/prep', 'build/portal1/ios/device',
                          'build/ios/device/Portal2.app', 'portal'))
        self.assertEqual((p1['APP_ID'], p1['HOST'], p1['DATA_ROOT']),
                         ('local.portal2recomp.team', 'portal2', 'Documents/portal1'))
        self.assertIn('portal1', games.GAMES['portal2']['ios_guests'])
        self.assertEqual(p1['IOS_DATA'].split(), list(games.GAMES['portal1']['ios_data']))
        # Portal 1 not set up: empty folders, never Portal 2's.
        with no_bundle_env():
            alone = ios_game.values('portal1', 'sim', {'game_root': '/p2', 'linux_root': '/p2-linux'})
        self.assertEqual((alone['GAME_ROOT'], alone['LINUX_ROOT']), ('', ''))
        with no_bundle_env():   # its own former app (P2_STANDALONE=1) keeps its own ID
            own = ios_game.values('portal1', 'sim', THREE, standalone=True)
        self.assertEqual((own['APP_ID'], own['DATA_ROOT']), ('local.portal1recomp.team', 'Documents'))


class BundleIds(unittest.TestCase):
    def test_per_game_identifier(self):
        with no_bundle_env():
            self.assertEqual(ios_bundle_id.bundle_id(game='portal2', config=BOTH), 'local.portal2recomp.team')
            self.assertEqual(ios_bundle_id.bundle_id(game='hl2', config=BOTH), 'local.hl2recomp.team')
            self.assertEqual(ios_bundle_id.bundle_id(True, 'hl2', BOTH), 'local.hl2recomp.team.probe')
            # Without its own identifier HL2 gets an example one, not Portal 2's
            # (installing it would replace Portal 2's app).
            p2_only = {'ios_bundle_id': 'local.portal2recomp.team'}
            self.assertEqual(ios_bundle_id.bundle_id(game='hl2', config=p2_only), 'com.example.hl2-recomp')
            self.assertEqual(ios_bundle_id.bundle_id(game='portal1', config=THREE), 'local.portal1recomp.team')
            self.assertEqual(ios_bundle_id.bundle_id(game='portal1', config=p2_only), 'com.example.portal1-recomp')
            self.assertEqual(ios_bundle_id.bundle_id(game='portal2', config={}), ios_bundle_id.DEFAULT)
            # The game of the config when none is given (games.selected).
            self.assertEqual(ios_bundle_id.bundle_id(config=dict(BOTH, game='hl2')), 'local.hl2recomp.team')
        with mock.patch.dict(os.environ, {'P2_BUNDLE_ID': 'x.y'}):
            self.assertEqual(ios_bundle_id.bundle_id(game='hl2', config=BOTH), 'x.y')


class LaunchArgs(unittest.TestCase):
    def test_language_comes_from_the_games_own_folder(self):
        with tempfile.TemporaryDirectory() as tmp, no_bundle_env():
            hl2_root = Path(tmp) / 'hl2'
            (hl2_root / 'hl2/resource').mkdir(parents=True)
            (hl2_root / 'hl2/resource/gameui_french.txt').write_text('')
            config = {'game_root': str(Path(tmp) / 'p2'), 'language': 'french',
                      'games': {'hl2': {'game_root': str(hl2_root)}}}
            hl2 = localconfig.game_config(config, 'hl2')
            args = build_ios.launch_args(games.profile(hl2), hl2)
            self.assertEqual(args, ['--', '-game', 'hl2_complete', '-novid', '-fullscreen', '-language', 'french'])
            # Portal 2's folder has no French files here: english, as before.
            p2 = localconfig.game_config(config, 'portal2')
            self.assertEqual(build_ios.launch_args(games.profile(p2), p2)[-4:],
                             ['-novid', '-fullscreen', '-language', 'english'])

    def test_hosted_game_names_its_own_program(self):
        # Portal 1 in the Portal app: the app is built as portal2_linux, Portal
        # is hl2_linux (without it the launch stopped on Portal 2's data check).
        p2, p1, mel = (dict(games.GAMES[g], name=g) for g in ('portal2', 'portal1', 'mel'))
        with no_bundle_env():
            args = build_ios.launch_args(p1, {}, host=p2)
            self.assertEqual(args[:4], ['--program', 'hl2_linux', '--', '-game'])
            self.assertEqual(build_ios.launch_args(mel, {}, host=p2)[:2], ['--', '-game'])   # same program
            self.assertEqual(build_ios.launch_args(p1, {})[:2], ['--', '-game'])   # its own app


if __name__ == '__main__':
    unittest.main()


class AppName(unittest.TestCase):
    def test_one_place_for_the_name(self):
        # Round 19: the one app's name lives in games.APP_NAME (or ios_app_name),
        # never a Valve trademark.
        self.assertEqual(games.app_name({}), games.APP_NAME)
        self.assertEqual(games.app_name({'ios_app_name': 'Saurce'}), 'Saurce')
        for mark in ('source', 'portal', 'half-life', 'valve', 'steam'):
            self.assertNotIn(mark, games.APP_NAME.lower())
        self.assertTrue((games.ROOT / build_ios.APP_ICON).is_file())


class ForkVersionTests(unittest.TestCase):
    """Version shown at the bottom of the in-game settings menu."""
    def test_version_names_the_commit(self):
        import re
        self.assertRegex(build_ios.fork_version(), r'^' + re.escape(build_ios.FORK_VERSION) + r' \([0-9a-f]{7}\+?\)$')

    def test_outside_a_repository_the_bare_version(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            self.assertEqual(build_ios.fork_version(Path(tmp)), build_ios.FORK_VERSION)
