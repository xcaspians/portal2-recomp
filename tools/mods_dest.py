#!/usr/bin/env python3
"""Where `tools/ios_device.sh mods push <path> [name]` copies a map or an addon
on the device, relative to the app's data container (docs/mods.md):

    a .bsp file                -> Documents/game/portal2/maps/custom/<file>
    a folder of .bsp files     -> each one into Documents/game/portal2/maps/custom/
    a folder named portal2_dlcN, N >= 3 -> Documents/game/portal2_dlcN
    another folder and a name  -> Documents/game/<name>, name = portal2_dlcN, N >= 3

Map names are typed into the console (`map custom/<name>`), so they must be
plain: letters, digits, '_', '-', '.'. Prints one "source<TAB>destination" line
per copy; a refusal goes to stderr with exit code 2.
"""
import re
import sys
from pathlib import Path

GAME = 'Documents/game'
CUSTOM_MAPS = f'{GAME}/portal2/maps/custom'
ADDON = re.compile(r'^portal2_dlc([0-9]+)$')
PLAIN = re.compile(r'^[A-Za-z0-9_.-]+$')


class Refused(Exception):
    pass


def addon_name(name):
    match = ADDON.match(name)
    if not match:
        raise Refused(f'"{name}" is not an addon folder name: use portal2_dlc3, portal2_dlc4... (numbers with no gaps)')
    if int(match.group(1)) < 3:
        raise Refused(f'{name} belongs to the game (portal2_dlc1, portal2_dlc2): addons start at portal2_dlc3')
    return name


def map_destination(path):
    if not PLAIN.match(path.stem):
        raise Refused(f'"{path.name}": rename it with only letters, digits, "_", "-" (the console loads it by name)')
    return str(path), f'{CUSTOM_MAPS}/{path.name}'


def destinations(path, name=None):
    p = Path(path)
    if p.is_file():
        if p.suffix.lower() != '.bsp':
            raise Refused(f'{p.name}: push a .bsp map or a folder')
        if name:
            raise Refused('a name is only for addon folders (portal2_dlcN)')
        return [map_destination(p)]
    if not p.is_dir():
        raise Refused(f'{path}: no such file or folder')
    if name or ADDON.match(p.name):
        return [(str(p), f'{GAME}/{addon_name(name or p.name)}')]
    entries = [e for e in p.iterdir() if not e.name.startswith('.')]
    maps = sorted(e for e in entries if e.is_file() and e.suffix.lower() == '.bsp')
    if maps and len(maps) == len(entries):
        return [map_destination(e) for e in maps]
    raise Refused(f'{p.name} holds more than .bsp maps, so it is an addon: name it, '
                  f'e.g. tools/ios_device.sh mods push "{path}" portal2_dlc3')


def main(argv):
    if not 1 <= len(argv) <= 2:
        print('usage: mods_dest.py <map.bsp | folder> [portal2_dlcN]', file=sys.stderr)
        return 2
    try:
        pairs = destinations(*argv)
    except Refused as e:
        print(f'mods push: {e}', file=sys.stderr)
        return 2
    for source, destination in pairs:
        print(f'{source}\t{destination}')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
