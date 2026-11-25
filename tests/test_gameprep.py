from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import gameprep


class GamePrepTests(unittest.TestCase):
    def test_game_addresses_stay_stable_and_tools_have_separate_layout(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'bin/linux32').mkdir(parents=True)
            (root / 'portal2_linux').touch()
            names = ['libtier0.so', 'libvstdlib.so', 'client.so', 'server.so', 'engine.so']
            for name in names:
                (root / 'bin/linux32' / name).touch()
            def elf(path, *_):
                start = 0x08048000 if path.name == 'vbsp_linux' else 0
                size = 0x08800000 if start else 0x12345
                return mock.Mock(loads=[(1, 0, start, 0, 0, size)])
            with mock.patch.object(gameprep, 'Elf', side_effect=elf):
                before = gameprep.plan({'linux_root': str(root)})
                authoring = ['puzzlemaker_dll.so', 'vrad_dll.so', 'vvis_dll.so', 'bsppack.so']
                for name in authoring + ['vbsp_linux']:
                    (root / 'bin/linux32' / name).touch()
                self.assertEqual(gameprep.plan({'linux_root': str(root)}), before)
                tool = gameprep.plan({'linux_root': str(root)}, 'vbsp')
            names = {p.name for p, _, _ in tool}
            self.assertTrue({'vrad_dll.so', 'vvis_dll.so', 'bsppack.so', 'vbsp_linux'} <= names)
            self.assertFalse({'client.so', 'server.so', 'portal2_linux', 'puzzlemaker_dll.so'} & names)
            self.assertTrue(all(base >= 0x11000000 for p, base, _ in tool if p.suffix == '.so'))
            self.assertEqual(tool[-1][1], 0)  # ET_EXEC keeps its linked addresses.
            self.assertTrue(all(base + size < 0x30000000 for _, base, size in tool))

    def test_mel_et_exec_game_lib_dirs_and_system_libs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            linux = root / 'depot_317404'
            game_dir = root / 'Portal Stories Mel'
            syslib = root / 'debian-i386'
            (linux / 'bin').mkdir(parents=True)
            (game_dir / 'portal_stories/bin').mkdir(parents=True)
            syslib.mkdir(parents=True)
            (linux / 'portal2_linux').touch()
            for name in ('libtier0.so', 'libvstdlib.so', 'engine.so'):
                (linux / 'bin' / name).touch()
            for name in ('client.so', 'server.so'):
                (game_dir / 'portal_stories/bin' / name).touch()
            for name in ('libstdc++.so.6', 'libgcc_s.so.1'):
                (syslib / name).touch()
            def elf(path, *_):
                if path.name == 'portal2_linux':
                    return mock.Mock(e_type=2, loads=[(1, 0, 0x08048000, 0, 0, 0x10000)])
                return mock.Mock(e_type=3, loads=[(1, 0, 0, 0, 0, 0x20000)])
            config = {'game': 'mel', 'linux_root': str(linux), 'game_root': str(game_dir), 'system_lib_dir': str(syslib)}
            with mock.patch.object(gameprep, 'Elf', side_effect=elf):
                planned = gameprep.plan(config)
            by_name = {p.name: (p, base) for p, base, _ in planned}
            self.assertEqual(by_name['portal2_linux'][1], 0)  # ET_EXEC keeps its linked base
            self.assertEqual(by_name['server.so'][0].parent, game_dir / 'portal_stories/bin')
            self.assertEqual(by_name['libstdc++.so.6'][0].parent, syslib)
            self.assertEqual(by_name['libgcc_s.so.1'][0].parent, syslib)
            # No two relocatable modules overlap.
            self.assertEqual(len({base for p, base, _ in planned if p.name != 'portal2_linux'}), len(planned) - 1)
