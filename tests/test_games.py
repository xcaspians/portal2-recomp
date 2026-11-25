import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import build_game  # noqa: E402
import games  # noqa: E402


class GameProfiles(unittest.TestCase):
    def test_default_is_portal2(self):
        game = games.profile({})
        self.assertEqual(game['name'], 'portal2')
        self.assertEqual(game['program'], 'portal2_linux')
        self.assertEqual(game['roots'], 'config/game-roots.json')

    def test_hl2_profile(self):
        game = games.profile({'game': 'hl2'})
        self.assertEqual(game['program'], 'hl2_linux')
        self.assertEqual(game['lib_dirs'], ('bin', 'hl2_complete/bin'))
        # The host FreeType/Fontconfig replace the bundled ones (runtime/fonts.c).
        self.assertIn('libfreetype.so', game['excluded'])
        self.assertIn('libdxvk_d3d9.so', game['excluded'])

    def test_unknown_game_is_rejected(self):
        with self.assertRaises(SystemExit):
            games.profile({'game': 'quake'})

    def test_portal1_profile(self):
        game = games.profile({'game': 'portal1'})
        self.assertEqual(game['program'], 'hl2_linux')
        self.assertEqual(game['lib_dirs'], ('bin', 'portal/bin'))
        self.assertEqual(game['mod'], 'portal')
        self.assertEqual((game['steam_appid'], game['depot_id']), (400, 404))
        self.assertEqual(game['roots'], 'config/portal1-roots.json')
        self.assertEqual((game['ios_name'], game['ios_app']), ('Portal', 'Portal.app'))
        # Source 2013 takes the Miles plugins from the Linux depot, like HL2.
        self.assertNotIn('bin', game['ios_data'])
        self.assertIn('portal', game['ios_data'])

    def test_mel_profile(self):
        game = games.profile({'game': 'mel'})
        self.assertEqual(game['program'], 'portal2_linux')
        self.assertEqual(game['lib_dirs'], ('bin',))
        self.assertEqual(game['game_lib_dirs'], ('portal_stories/bin',))
        self.assertEqual(game['system_libs'], ('libstdc++.so.6', 'libgcc_s.so.1'))
        self.assertEqual(game['mod'], 'portal_stories')
        self.assertEqual((game['steam_appid'], game['depot_id']), (317400, 317404))
        self.assertEqual(game['roots'], 'config/mel-roots.json')
        self.assertTrue(game['ios'])
        self.assertEqual(game['ios_app'], 'PortalStoriesMel.app')

    def test_build_paths(self):
        # Portal 2 keeps the paths of single-game checkouts; other games live under build/<game>.
        portal2, hl2, portal1, mel = (games.GAMES[k] for k in ('portal2', 'hl2', 'portal1', 'mel'))
        self.assertEqual((portal2['build_dir'], portal2['binary'], portal2['ios_dir'], portal2['executable']),
                         ('build/game', 'build/portal2-native', 'build/ios', 'portal2-native'))
        self.assertEqual((hl2['build_dir'], hl2['binary'], hl2['ios_dir'], hl2['executable']),
                         ('build/hl2', 'build/hl2/hl2-native', 'build/hl2/ios', 'hl2-native'))
        self.assertEqual((portal1['build_dir'], portal1['binary'], portal1['ios_dir'], portal1['executable']),
                         ('build/portal1', 'build/portal1/portal1-native', 'build/portal1/ios', 'portal1-native'))
        self.assertEqual((mel['build_dir'], mel['binary'], mel['ios_dir'], mel['executable']),
                         ('build/mel', 'build/mel/mel-native', 'build/mel/ios', 'mel-native'))
        self.assertEqual(games.EXECUTABLES, ('portal2-native', 'hl2-native', 'portal1-native', 'mel-native', 'l4d2-native'))
        # No two games share a build tree, binary or data folder.
        for key in ('build_dir', 'binary', 'ios_dir', 'executable', 'app_name', 'log_name', 'bundle_id'):
            self.assertEqual(len({g[key] for g in games.GAMES.values()}), len(games.GAMES), key)

    def test_selection_precedence(self):
        both = {'game_root': '/p2', 'games': {'hl2': {'game_root': '/hl2'}}}
        self.assertEqual(games.selected(None, {}, environ={}), 'portal2')
        self.assertEqual(games.selected(None, both, environ={}), 'portal2')
        self.assertEqual(games.selected(None, dict(both, game='hl2'), environ={}), 'hl2')
        self.assertEqual(games.selected(None, dict(both, game='hl2'), environ={'P2_GAME': 'portal2'}), 'portal2')
        self.assertEqual(games.selected('hl2', {'game': 'portal2'}, environ={'P2_GAME': 'portal2'}), 'hl2')
        self.assertEqual(games.selected(None, {}, environ={'P2_GAME': ''}), 'portal2')
        # Only Half-Life 2 set up: no --game needed.
        self.assertEqual(games.selected(None, {'games': {'hl2': {'game_root': '/hl2'}}}, environ={}), 'hl2')
        self.assertEqual(games.selected(None, {'games': {'hl2': {}}}, environ={}), 'portal2')
        for name, environ in (('quake', {}), (None, {'P2_GAME': 'quake'})):
            with self.assertRaises(SystemExit):
                games.selected(name, {}, environ=environ)
        with self.assertRaises(SystemExit):
            games.selected(None, {'game': 'quake'}, environ={})

    def test_roots_follow_the_program(self):
        layout = {'program': 'hl2_linux', 'modules': [
            {'name': 'hl2_linux', 'init': 0x100, 'init_array': [1, 2]},
            {'name': 'engine.so', 'init': 0x200, 'init_array': [3]},
        ]}
        roots = build_game.roots(layout, roots_file='/nonexistent/roots.json')
        self.assertEqual(roots, ['hl2_linux:symbol:main', 'hl2_linux:init:dt', 'hl2_linux:init:0', 'hl2_linux:init:1'])

    def test_roots_need_the_program_key(self):
        # tools/elfprep.py always writes it (--program); no silent Portal 2 default.
        with self.assertRaises(KeyError):
            build_game.roots({'modules': []}, roots_file='/nonexistent/roots.json')

    def test_roots_file_is_relative_to_the_checkout(self):
        import json
        import tempfile
        layout = {'program': 'hl2_linux', 'modules': []}
        (build_game.ROOT / 'build').mkdir(exist_ok=True)   # absent on a fresh clone (CI python-3-9 job)
        with tempfile.TemporaryDirectory(dir=build_game.ROOT / 'build') as tmp:
            path = Path(tmp) / 'roots.json'
            path.write_text(json.dumps(['engine.so:code:0x10']))
            relative = path.relative_to(build_game.ROOT)
            self.assertEqual(build_game.roots(layout, roots_file=str(relative)),
                             ['hl2_linux:symbol:main', 'engine.so:code:0x10'])
            self.assertEqual(build_game.roots(layout, roots_file=str(path))[-1], 'engine.so:code:0x10')


class IosProfiles(unittest.TestCase):
    """tools/build_ios.py and runtime/ios/game_ios.c agree on each game."""
    ROOT = Path(__file__).resolve().parent.parent

    def c_table(self):
        import re
        text = (self.ROOT / 'runtime/ios/game_ios.c').read_text()
        rows = re.findall(r'\{"(\w+)", "(\w+)", "([\w-]+)", "(\d+)", (\d), (\w+), "(\w+)", (\d+), "(\w*)",\s*"([^"]+)",\s*(NULL|"\w+"),\s*(NULL|"\w+")\}', text)
        return {r[0]: r for r in rows}

    def test_every_game_has_an_ios_profile(self):
        table = self.c_table()
        ios_games = {k: g for k, g in games.GAMES.items() if g.get('ios', True)}
        variants = {v: base for base, g in games.GAMES.items() for v, _mod, _t in g.get('ios_variants', ())}
        self.assertEqual(set(table), set(ios_games) | set(variants))
        for v, base in variants.items():   # round 21: on its base's data, under its own saves
            self.assertEqual((table[v][1], table[v][8], table[v][11]), (games.GAMES[base]['mod'], base, f'"{base}"'))
        for name, game in ios_games.items():
            row = table[name]
            self.assertEqual(row[1], game['mod'])
            self.assertEqual(int(row[3]), game['steam_appid'])
            self.assertTrue(game['ios_name'] and game['ios_executable'].isidentifier())
            self.assertIn(game['mod'], game['ios_data'])
        # Portal 2 keeps the folder of existing installs.
        self.assertEqual(table['portal2'][2], 'portal2-userdata')
        # The shell tools read the same saves folders (ios_game.py USERDATA, ios_device.sh migrate).
        for name in ios_games:
            self.assertEqual(games.ios_userdata(name), table[name][2])
        self.assertEqual(games.ios_userdata('nonsense'), '')
        self.assertEqual(table['hl2'][5], 'RT_IOS_LAYOUT_HL2')
        self.assertEqual(table['portal1'][5], 'RT_IOS_LAYOUT_PORTAL2')
        self.assertEqual(table['portal1'][4], '1')   # back taps on, like Portal 2

    def test_settings_page_follows_the_game(self):
        import plistlib
        import shutil
        import tempfile
        import build_ios
        with tempfile.TemporaryDirectory() as tmp:
            for game in ('portal2', 'portal1', 'hl2'):
                path = Path(tmp) / f'{game}.plist'
                shutil.copy(self.ROOT / 'ios/Settings.bundle/Root.plist', path)
                build_ios.settings_for_game(path, game)
                items = plistlib.loads(path.read_bytes())['PreferenceSpecifiers']
                keys = [i.get('Key') for i in items]
                # Gyro aiming: off by default in Portal 2 (back taps shake the
                # phone), on in Half-Life 2, as rt_gyro_enabled's default.
                gyro = next(i for i in items if i.get('Key') == 'P2Gyro')
                self.assertEqual(gyro['DefaultValue'], game == 'hl2')
                self.assertIn('P2GyroSpeed', keys)
                if game in ('portal2', 'portal1'):
                    self.assertIn('P2BackTap', keys)
                    self.assertIn('P2PortalButtons', keys)
                else:
                    for key in ('P2BackTap', 'P2Haptics', 'P2BackTapRecord', 'P2PortalButtons'):
                        self.assertNotIn(key, keys)
                    for key in ('P2Renderer', 'P2TouchControls', 'P2TouchTrackpad', 'P2Log'):
                        self.assertIn(key, keys)

    def test_info_plist_names_the_game(self):
        plist = (self.ROOT / 'ios/Info.plist.in').read_text()
        self.assertIn('<key>P2Game</key><string>@GAME@</string>', plist)
        self.assertNotIn('Portal 2', plist)   # the name comes from the profile (@NAME@)


class Left4Dead2Profile(unittest.TestCase):
    def test_offline_arguments_are_always_added(self):
        l4d2 = dict(games.GAMES['l4d2'], name='l4d2')
        self.assertEqual(l4d2['program'], 'hl2_linux')
        self.assertEqual(l4d2['roots'], 'config/l4d2-roots.json')
        args = games.forced_args(l4d2, ['-game', 'left4dead2', '+map', 'c1m1_hotel'])
        self.assertEqual(args[-4:], ['-insecure', '+sv_lan', '1', '-nomaster'])   # never Valve's servers or VAC
        self.assertEqual(games.forced_args(dict(games.GAMES['portal2']), ['-novid']), ['-novid'])   # others unchanged
        self.assertTrue((games.ROOT / l4d2['roots']).is_file())
        import launch
        self.assertEqual(launch.environment({}, l4d2, base={})['P2_STEAM_OFFLINE'], '1')   # no server lists or lobbies
        self.assertNotIn('P2_STEAM_OFFLINE', launch.environment({}, dict(games.GAMES['portal2']), base={}))


class GameinfoSearchPaths(unittest.TestCase):
    # Left 4 Dead 2's own gameinfo.txt (abridged): its shaders live in hl2.
    L4D2 = """"GameInfo" { FileSystem { SteamAppId 550
        SearchPaths
        {
            Game                update
            Game                left4dead2_dlc3   // DLC
            Game                |gameinfo_path|.
            Game                hl2
        } } }"""

    def test_folders_named_by_search_paths(self):
        self.assertEqual(games.gameinfo_dirs(self.L4D2, 'left4dead2'),
                         ['update', 'left4dead2_dlc3', 'left4dead2', 'hl2'])
        text = 'SearchPaths\n{\nGame |all_source_engine_paths|hl2\nGameBin |gameinfo_path|bin\n}'
        self.assertEqual(games.gameinfo_dirs(text, 'ep2'), ['hl2', 'ep2'])

    def test_l4d2_device_copy_has_its_shaders(self):
        # The black screen on both iPads: hl2 (shaders) was never copied.
        import tempfile
        with tempfile.TemporaryDirectory() as root:
            for d in ('update', 'left4dead2_dlc3', 'left4dead2', 'hl2', 'sdk'):
                (Path(root) / d).mkdir()
            (Path(root) / 'left4dead2' / 'gameinfo.txt').write_text(self.L4D2)
            l4d2 = dict(games.GAMES['l4d2'], name='l4d2')
            self.assertEqual(games.missing_ios_data(l4d2, root), [])
            short = dict(l4d2, ios_data=tuple(d for d in l4d2['ios_data'] if d != 'hl2'))
            self.assertEqual(games.missing_ios_data(short, root), ['hl2'])
            self.assertEqual(games.missing_ios_data(l4d2, ''), [])   # no game files: nothing to check


class IosSteamStub(unittest.TestCase):
    def test_stub_answers_every_flat_function_the_bridge_looks_up(self):
        # runtime/steam.c looks each of these up in Valve's dylib on the Mac and in
        # the app's own stub on iOS (runtime/ios/steam_stub.c): a missing one stops
        # the game at launch on iOS only (Left 4 Dead 2: SteamAPI_Init, round 15).
        import re
        root = Path(__file__).resolve().parent.parent
        steam = (root / 'runtime/steam.c').read_text()
        stub = (root / 'runtime/ios/steam_stub.c').read_text()
        looked_up = set(re.findall(r'FLAT\([^"]*"(\w+)"\)', steam)) | {'SteamAPI_Init', 'SteamAPI_InitSafe'}
        defined = set(re.findall(r'EXPORT [^(]*?\b(\w+)\(', stub))
        self.assertEqual(sorted(looked_up - defined), [])


if __name__ == '__main__':
    unittest.main()


class MelSteamStub(unittest.TestCase):
    """The iOS Steam stub answers Mel's SDK 1.40 slots like Portal 2's flat
    functions: a non-empty ticket to join the local server (else "STEAM
    certificate length error! 0/2048" when a map loads), and the server accepts
    the player."""
    def test_pre_rename_methods_get_the_answers(self):
        text = (Path(__file__).resolve().parent.parent / 'runtime/ios/steam_stub_generated.c').read_text()
        line = lambda name: next(l for l in text.splitlines() if l.startswith(f'static ') and f' {name}(' in l)
        self.assertIn('steam_stub_ticket(a1, a2)', line('v_SteamUser019_3'))          # InitiateGameConnection
        self.assertIn('return true;', line('v_SteamGameServer012_24'))                  # SendUserConnectAndAuthenticate
