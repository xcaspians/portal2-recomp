"""tools/mods_dest.py: where `tools/ios_device.sh mods push` puts maps and addons."""
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import mods_dest  # noqa: E402
from mods_dest import Refused, destinations  # noqa: E402


class ModsDestTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def make(self, relative, data=b'x'):
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        return path

    def test_a_map_goes_to_custom_maps(self):
        bsp = self.make('downloads/sp_laser_maze.bsp')
        self.assertEqual(destinations(bsp), [(str(bsp), 'Documents/game/portal2/maps/custom/sp_laser_maze.bsp')])

    def test_a_folder_of_maps_is_split_into_custom_maps(self):
        a = self.make('pack/b_two.bsp'); b = self.make('pack/a_one.BSP'); self.make('pack/.DS_Store')
        self.assertEqual(destinations(self.root / 'pack'), [
            (str(b), 'Documents/game/portal2/maps/custom/a_one.BSP'),
            (str(a), 'Documents/game/portal2/maps/custom/b_two.bsp')])

    def test_an_addon_folder_keeps_its_number(self):
        self.make('portal2_dlc3/materials/x.vmt')
        folder = self.root / 'portal2_dlc3'
        self.assertEqual(destinations(folder), [(str(folder), 'Documents/game/portal2_dlc3')])
        # a trailing slash, as a shell completion leaves it
        self.assertEqual(destinations(f'{folder}/'), [(str(folder), 'Documents/game/portal2_dlc3')])

    def test_another_folder_needs_a_name(self):
        self.make('Cool Skin/materials/x.vmt')
        folder = self.root / 'Cool Skin'
        with self.assertRaises(Refused):
            destinations(folder)
        self.assertEqual(destinations(folder, 'portal2_dlc4'), [(str(folder), 'Documents/game/portal2_dlc4')])

    def test_names_that_would_escape_or_replace_the_game_are_refused(self):
        self.make('skin/materials/x.vmt')
        for name in ('portal2_dlc1', 'portal2_dlc2', 'portal2', '../portal2_dlc3', 'portal2_dlc3/../x', 'dlc3'):
            with self.subTest(name=name), self.assertRaises(Refused):
                destinations(self.root / 'skin', name)

    def test_map_names_must_be_typeable(self):
        with self.assertRaises(Refused):
            destinations(self.make('My Map (final).bsp'))
        with self.assertRaises(Refused):
            destinations(self.make('readme.txt'))
        with self.assertRaises(Refused):
            destinations(self.root / 'missing.bsp')
        with self.assertRaises(Refused):
            destinations(self.make('one.bsp'), 'portal2_dlc3')

    def test_main_prints_tab_separated_pairs_or_refuses(self):
        bsp = self.make('x.bsp')
        self.assertEqual(mods_dest.main([str(bsp)]), 0)
        self.assertEqual(mods_dest.main([str(self.root / 'nothing')]), 2)
        self.assertEqual(mods_dest.main([]), 2)


if __name__ == '__main__':
    unittest.main()
