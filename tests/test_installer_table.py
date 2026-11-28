"""The one game table (tools/games.py installer entries) and its four readers:
tools/steam_library.py, setup.sh, installer/Installer.swift and
installer/Devices.swift (through games.json), plus tools/devices.py. Left 4 Dead
2 fell through in round 28 because each kept its own list: these fail when one
of them disagrees with the table or keeps a copy of it again."""
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import devices  # noqa: E402
import games  # noqa: E402
import steam_library  # noqa: E402

# Every game Sauce plays, the installer's whole job (round 29).
EXPECTED = ['portal2', 'hl2', 'portal1', 'mel', 'l4d2']
SWIFT = {name: (ROOT / 'installer' / name).read_text() for name in ('Installer.swift', 'Devices.swift')}


class TableTests(unittest.TestCase):
    def test_every_game_sauce_plays_is_in_it(self):
        self.assertEqual(games.installer_games(), EXPECTED)
        # Portal 2: Community Edition stays out until it plays.
        self.assertNotIn('p2ce', games.installer_games())

    def test_entries_are_complete(self):
        for name in games.installer_games():
            entry = games.installer_entry(name)
            for key in ('title', 'folder', 'markerFile', 'linuxSize', 'appName'):
                self.assertTrue(entry[key], (name, key))
            self.assertGreater(entry['minutes'], 0, name)
            self.assertGreater(entry['deviceBytes'], 10 ** 9, name)
            self.assertTrue(entry['linuxFiles'], name)
            self.assertTrue(games.GAMES[name]['steamworks'][1], name)
            if entry['needs']:
                self.assertIn(entry['needs'], games.installer_games(), name)
        self.assertEqual([e['title'] for e in games.installer_entry('hl2')['extras']],
                         ['Episode One', 'Episode Two', 'Lost Coast'])

    def test_json_and_shell_exports(self):
        out = subprocess.run([sys.executable, str(ROOT / 'tools/games.py'), '--installer-json'],
                             capture_output=True, text=True, check=True).stdout
        self.assertEqual(json.loads(out), [games.installer_entry(n) for n in EXPECTED])
        ids = subprocess.run([sys.executable, str(ROOT / 'tools/games.py'), '--installer-ids'],
                             capture_output=True, text=True, check=True).stdout.split()
        self.assertEqual(ids, EXPECTED)
        bad = subprocess.run([sys.executable, str(ROOT / 'tools/games.py'), '--shell', 'p2ce'], capture_output=True)
        self.assertEqual(bad.returncode, 2)


class SteamLibraryReadsTheTable(unittest.TestCase):
    def test_rows_are_the_table(self):
        self.assertEqual([row[0] for row in steam_library.GAMES], EXPECTED)
        for name, title, app, linux, data, folder, marker in steam_library.GAMES:
            entry = games.installer_entry(name)
            self.assertEqual((title, app, linux, data, folder, marker),
                             (entry['title'], entry['appID'], entry['linuxDepot'], entry['dataDepot'],
                              entry['folder'], entry['markerFile']))

    def test_no_copy_of_its_own(self):
        source = (ROOT / 'tools/steam_library.py').read_text()
        for name in EXPECTED:
            self.assertNotIn(f"('{name}', '", source)
        self.assertNotRegex(source, r'\b(620|317400|550)\b')


class DevicesReadsTheTable(unittest.TestCase):
    def test_downloads_titles_and_sizes(self):
        self.assertEqual(devices.DOWNLOADS, (*EXPECTED, 'workshop'))
        for name in EXPECTED:
            self.assertEqual(devices.TITLES[name], games.GAMES[name]['installer']['title'])
        self.assertEqual(devices.SIZES, {'hl2': 'about 10 GB', 'portal1': 'about 4.4 GB', 'l4d2': 'about 14 GB'})


class SetupReadsTheTable(unittest.TestCase):
    def run_setup(self, *args, home):
        # A copy of the scripts, without this checkout's config/local.json.
        repo = Path(home) / 'repo'
        (repo / 'tools').mkdir(parents=True, exist_ok=True)
        shutil.copy(ROOT / 'setup.sh', repo / 'setup.sh')
        for name in ('games.py', 'localconfig.py'):
            shutil.copy(ROOT / 'tools' / name, repo / 'tools' / name)
        env = {k: v for k, v in os.environ.items() if not k.startswith('P2_')}
        env['HOME'] = str(home)
        return subprocess.run(['bash', str(repo / 'setup.sh'), '--detect', *args], capture_output=True, text=True, env=env)

    def test_every_game_is_a_setup_game_and_nothing_else(self):
        with tempfile.TemporaryDirectory() as home:
            for name in EXPECTED:
                result = self.run_setup('--game', name, home=home)
                self.assertNotEqual(result.returncode, 2, (name, result.stderr))   # known: fails later, data not found
                self.assertIn(f"{games.GAMES[name]['installer']['title']} data not found", result.stderr)
            result = self.run_setup('--game', 'p2ce', home=home)
            self.assertEqual(result.returncode, 2)
            self.assertIn(f"use --game {', '.join(EXPECTED)} or all", result.stderr)

    def test_no_copy_of_its_own(self):
        text = (ROOT / 'setup.sh').read_text()
        self.assertNotRegex(text, r'\bAPPID=\d')
        self.assertNotRegex(text, r'\bLINUX_DEPOT=\d')
        self.assertNotIn('for id in portal2 hl2', text)
        self.assertIn('python3 "$REPO/tools/games.py" --shell', text)
        usage = re.search(r'^# usage: \./setup\.sh \[--game ([\w|]+)\]', text, re.M).group(1)
        self.assertEqual(usage.split('|'), [*EXPECTED, 'all'])


class SwiftReadsTheTable(unittest.TestCase):
    def test_no_copy_of_its_own(self):
        for name, source in SWIFT.items():
            self.assertNotRegex(source, r'\bGame\(id: "', name)
            self.assertNotIn('SauceGame', source, name)
            self.assertNotIn('setupGame', source, name)   # "can't be built by the installer yet" is gone
            self.assertNotRegex(source, r'appID: \d', name)
        self.assertIn('Bundle.main.url(forResource: "games", withExtension: "json")', SWIFT['Installer.swift'])
        script = (ROOT / 'tools/build_installer.sh').read_text()
        self.assertIn('python3 tools/games.py --installer-json >"$APP/Contents/Resources/games.json"', script)

    def test_both_tabs_list_every_game(self):
        # The Mac tab (ContentView) and the iPad and iPhone tab (DeviceFlowView) both draw allGames.
        self.assertIn('ForEach(allGames) { GameRow(model: model, game: $0) }', SWIFT['Installer.swift'])
        self.assertIn('ForEach(allGames) { g in', SWIFT['Devices.swift'])
        self.assertNotIn('allGames.filter', SWIFT['Devices.swift'].split('var gamesList')[1].split('var deviceSection')[0])

    @unittest.skipUnless(shutil.which('swiftc'), 'needs swiftc (Xcode command line tools)')
    def test_the_app_decodes_the_table(self):
        with tempfile.TemporaryDirectory() as tmp:
            binary, table = Path(tmp) / 'installer', Path(tmp) / 'table.json'
            subprocess.run(['swiftc', '-parse-as-library', '-swift-version', '5',
                            *sorted(str(p) for p in (ROOT / 'installer').glob('*.swift')),
                            '-o', str(binary)], check=True, capture_output=True)
            table.write_text(json.dumps([games.installer_entry(n) for n in EXPECTED]))
            out = subprocess.run([str(binary), '--games'], capture_output=True, text=True, check=True,
                                 env=dict(os.environ, P2_GAMES_JSON=str(table))).stdout
        rows = [line.split('|') for line in out.splitlines()]
        self.assertEqual([r[0] for r in rows], EXPECTED)
        for r in rows:
            e = games.installer_entry(r[0])
            self.assertEqual(r[1:], [e['title'], str(e['appID']), str(e['dataDepot']), str(e['linuxDepot']), e['folder'],
                                     e['markerFile'], ','.join(e['linuxFiles']), str(e['minutes']), str(e['deviceBytes']),
                                     e['needs'] or '-', ','.join(x['folder'] for x in e['extras'])])


if __name__ == '__main__':
    unittest.main()
