#!/usr/bin/env python3
"""Prepare the real game's Linux modules using a local installation config."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from elfprep import Elf  # noqa: E402
import games  # noqa: E402
import localconfig  # noqa: E402


def plan(config, tool=None):
    game = games.profile(config)
    root = Path(config['linux_root'])
    paths = [p for d in game['lib_dirs'] for p in sorted((root / d).glob('*.so'))]
    # Game libraries that ship in the data depot, not the Linux one (Portal Stories: Mel).
    if game.get('game_lib_dirs'):
        paths += [p for d in game['game_lib_dirs'] for p in sorted((Path(config['game_root']) / d).glob('*.so'))]
    # System libraries the game links dynamically (Portal Stories: Mel's 2021 engine needs
    # libstdc++.so.6 and libgcc_s.so.1): i386 copies from Debian, downloaded by setup.sh into
    # system_lib_dir, recompiled like the game's own modules.
    for name in game.get('system_libs', ()):
        if not config.get('system_lib_dir'):
            raise ValueError(f'{game["name"]} needs {name}: no system_lib_dir in the config (run ./setup.sh --game {game["name"]})')
        lib = Path(config['system_lib_dir']).expanduser() / name
        if not lib.is_file():
            raise ValueError(f'{game["name"]} needs {lib} (run ./setup.sh --game {game["name"]})')
        paths.append(lib)
    # Excluded modules (tools, DXVK...): see tools/games.py.
    excluded = set(game['excluded'])
    if tool:
        if game['name'] != 'portal2':
            raise ValueError('Puzzle Maker compilers require --game portal2')
        excluded |= {'client.so', 'server.so', 'launcher.so', 'matchmaking.so', 'puzzlemaker_dll.so'}
        excluded -= {'bsppack.so', 'vrad_dll.so', 'vvis_dll.so'}
    paths = [p for p in paths if p.name not in excluded]
    order = ['libtier0.so', 'libvstdlib.so', 'libsteam_api.so', 'libtogl.so']
    by_name = {p.name: p for p in paths}
    paths = [by_name.pop(name) for name in order if name in by_name] + sorted(by_name.values())
    result = []
    # vbsp_linux is ET_EXEC and occupies 0x08048000..0x1075e324. Shared
    # libraries must start above it; the game retains its original addresses.
    base = 0x11000000 if tool else 0x10000000
    for p in paths:
        module = Elf(p, base, len(result) + 1)
        extent = max(load[2] + load[5] for load in module.loads)
        result.append((p, base, extent))
        step = 0x1000000 if len(result) <= 2 else 0x100000
        base = (base + extent + step - 1) & ~(step - 1)
    p = root / 'bin/linux32' / f'{tool}_linux' if tool else root / game['program']
    if tool and not p.is_file():
        raise ValueError(f'map compiler missing: {p}')
    if p.is_file():
        # A non-PIE program (ET_EXEC, Portal Stories: Mel's portal2_linux at 0x08048000)
        # keeps its link-time addresses, like the map compiler.
        program_base = 0 if tool or Elf(p, 0, 0).e_type == 2 else base
        module = Elf(p, program_base, len(result) + 1)
        extent = max(load[2] + load[5] for load in module.loads)
        result.append((p, program_base, extent))
        base = max(base, program_base + extent)
    # Match the corresponding RT_VM_BEGIN in runtime/rt.h.
    if base > (0x30000000 if tool else 0x21000000):
        raise ValueError('module plan overlaps reserved guest mmap space')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    games.add_argument(parser)
    parser.add_argument('--tool', choices=('vbsp',), help='prepare the Portal 2 map compiler host')
    parser.add_argument('--config', type=Path, default=localconfig.DEFAULT_PATH)
    parser.add_argument('--out', type=Path, help='default: <build_dir>/prep of the game (tools/games.py)')
    args = parser.parse_args()
    config = localconfig.game_config(localconfig.load(args.config), args.game)
    if not config.get('linux_root'):
        parser.exit(1, f'no linux_root for {config["game"]} in {args.config}: run ./setup.sh --game {config["game"]}\n')
    default_build = 'build/tools/vbsp' if args.tool else games.profile(config)['build_dir']
    args.out = args.out or games.ROOT / default_build / 'prep'
    modules = plan(config, args.tool)
    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / 'plan.json').write_text(json.dumps([{'path': str(p), 'base': hex(base), 'size': size}
                                                 for p, base, size in modules], indent=2) + '\n')
    command = [sys.executable, str(Path(__file__).with_name('elfprep.py')), '--out', str(args.out),
               '--program', f'{args.tool}_linux' if args.tool else games.profile(config)['program']]
    command.extend(f'{p}@{base:#x}' for p, base, _ in modules)
    subprocess.run(command, check=True)


if __name__ == '__main__':
    main()
