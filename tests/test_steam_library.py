import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import steam_library  # noqa: E402

VDF = '''"libraryfolders"
{
\t"0"
\t{
\t\t"path"\t\t"%s"
\t\t"apps" { "620" "12824996329" }
\t}
\t"1"
\t{
\t\t"path"\t\t"%s"
\t}
}
'''


def touch(path, data=b'x'):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


class SteamLibraryTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.steam = Path(self.tmp.name) / 'Steam'
        self.other = Path(self.tmp.name) / 'External/SteamLibrary'
        (self.steam / 'steamapps').mkdir(parents=True)
        (self.steam / 'steamapps/libraryfolders.vdf').write_text(VDF % (self.steam, self.other))

    def tearDown(self):
        self.tmp.cleanup()

    def rows(self):
        return {r['game']: r for r in steam_library.scan(self.steam)}

    def test_parse_vdf_nested_and_escaped(self):
        data = steam_library.parse_vdf('"A" { "path" "C:\\\\x" "B" { "k" "v" } }')
        self.assertEqual(data, {'A': {'path': 'C:\\x', 'B': {'k': 'v'}}})

    def test_every_library_default_first(self):
        self.assertEqual(steam_library.libraries(self.steam), [self.steam, self.other])

    def test_nothing_installed_is_greyed_with_a_hint(self):
        rows = self.rows()
        # Every game of the shared table (tools/games.py), in its order; Left 4 Dead 2 included.
        self.assertEqual(list(rows), ['portal2', 'hl2', 'portal1', 'mel', 'l4d2'])
        for r in rows.values():
            self.assertFalse(r['installed'])
            self.assertEqual(r['hint'], 'Install it in Steam first.')

    def test_game_on_a_second_library_with_its_manifest_size(self):
        touch(self.other / 'steamapps/common/Left 4 Dead 2/left4dead2/pak01_dir.vpk')
        (self.other / 'steamapps/appmanifest_550.acf').write_text(
            '"AppState" { "appid" "550" "installdir" "Left 4 Dead 2" "SizeOnDisk" "15537775628" }')
        r = self.rows()['l4d2']
        self.assertTrue(r['installed'])
        self.assertEqual(r['size_bytes'], 15537775628)
        self.assertIn('download_depot 550 554', r['hint'])   # the Linux binaries are still missing

    def test_empty_marker_folder_is_not_installed(self):
        (self.steam / 'steamapps/common/Portal 2/portal2').mkdir(parents=True)
        self.assertFalse(self.rows()['portal2']['installed'])

    def test_linux_depot_and_measured_size_without_manifest(self):
        touch(self.steam / 'steamapps/common/Portal/portal/gameinfo.txt', b'12345')
        touch(self.steam / 'steamapps/content/app_400/depot_404/hl2_linux')
        r = self.rows()['portal1']
        self.assertTrue(r['installed'])
        self.assertEqual(r['size_bytes'], 5)
        self.assertIsNone(r['hint'])
        self.assertTrue(r['linux'].endswith('depot_404'))

    def test_hl2_lists_its_episodes(self):
        touch(self.steam / 'steamapps/common/Half-Life 2/hl2_complete/gameinfo.txt')
        (self.steam / 'steamapps/common/Half-Life 2/ep2').mkdir()
        extras = {e['id']: e['installed'] for e in self.rows()['hl2']['extras']}
        self.assertEqual(extras, {'ep1': False, 'ep2': True, 'lostcoast': False})

    def test_l4d2_needs_its_linux_depot_then_is_ready(self):
        touch(self.steam / 'steamapps/common/Left 4 Dead 2/left4dead2/pak01_dir.vpk')
        row = self.rows()['l4d2']
        self.assertTrue(row['installed'])
        self.assertIsNone(row['linux'])
        self.assertEqual(row['linux_command'], 'download_depot 550 554')
        self.assertIn('download_depot 550 554', row['hint'])
        # Steam's console puts the depot inside its app bundle on a Mac.
        touch(self.steam / 'Steam.AppBundle/Steam/Contents/MacOS/steamapps/content/app_550/depot_554/hl2_linux')
        row = self.rows()['l4d2']
        self.assertTrue(row['linux'].endswith('app_550/depot_554'))
        self.assertIsNone(row['hint'])
        self.assertEqual(row['extras'], [])

    def test_l4d2_data_depot_without_a_steam_install(self):
        touch(self.steam / 'steamapps/content/app_550/depot_551/left4dead2/pak01_dir.vpk')
        row = self.rows()['l4d2']
        self.assertTrue(row['installed'])
        self.assertTrue(row['folder'].endswith('app_550/depot_551'))

    def test_never_reads_credentials(self):
        source = (Path(steam_library.__file__)).read_text()
        for word in ('config.vdf', 'loginusers', 'ssfn', 'urlopen', 'import requests'):
            self.assertNotIn(word, source)


if __name__ == '__main__':
    unittest.main()
