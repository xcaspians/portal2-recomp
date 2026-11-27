#!/usr/bin/env python3
"""The Sauce games in this Mac's Steam libraries, for the Natif Installer.

Reads only what the Steam client already wrote: libraryfolders.vdf (every
library, the default one and any on another disk), each library's
appmanifest_<app>.acf (installed or not, size on disk) and the folders of
the Steam console's download_depot (content/app_<app>/depot_<depot>, the
Linux binaries). No Steam login, no password, no network.

    steam_library.py [--steam DIR]      JSON: [{game, title, app_id, installed, folder,
                                                  size_bytes, linux, linux_command, extras, hint}]
"""
import argparse
import json
import os
import re
import sys
from pathlib import Path

STEAM = Path.home() / 'Library/Application Support/Steam'

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402

# Every game Sauce plays, in the installer's order, from tools/games.py (its
# installer entries, the one table): (game, title, Steam app, Linux binaries
# depot, data depot, steamapps/common folder, file only a complete install has).
GAMES = tuple((n, i['title'], games.GAMES[n]['steam_appid'], games.GAMES[n]['depot_id'], i['data_depot'],
               i['steam_folder'], i['data_files'][0])
              for n in games.installer_games() for i in [games.GAMES[n]['installer']])
# What comes with a game's folder, each a game in Sauce's picker (Half-Life 2's
# episodes and Lost Coast): {game: [(id, title, folder)]}.
EXTRAS = {n: games.extras(n) for n in games.installer_games() if games.extras(n)}
# A game that runs on another's files on the device (Mel on Portal 2's).
NEEDS = {n: games.GAMES[n]['installer']['needs'] for n in games.installer_games()
         if games.GAMES[n]['installer'].get('needs')}


def parse_vdf(text):
    """Valve's KeyValues text (libraryfolders.vdf, appmanifest_*.acf) as nested dicts."""
    tokens = re.findall(r'"((?:[^"\\]|\\.)*)"|([{}])', text)
    root, stack, key = {}, [], None
    node = root
    for quoted, brace in tokens:
        if brace == '{':
            child = {}
            node[key if key is not None else ''] = child
            stack.append(node)
            node, key = child, None
        elif brace == '}':
            node = stack.pop() if stack else root
            key = None
        elif key is None:
            key = quoted.replace('\\\\', '\\')
        else:
            node[key] = quoted.replace('\\\\', '\\')
            key = None
    return root


def libraries(steam=STEAM):
    """Every Steam library folder (the one holding steamapps), the default first, no duplicates."""
    out = []
    for vdf in (steam / 'steamapps/libraryfolders.vdf', steam / 'config/libraryfolders.vdf'):
        try:
            data = parse_vdf(vdf.read_text(errors='replace'))
        except OSError:
            continue
        for entry in (data.get('libraryfolders') or data.get('LibraryFolders') or {}).values():
            path = entry.get('path') if isinstance(entry, dict) else entry
            if path and Path(path) not in out:
                out.append(Path(path))
    if steam not in out:
        out.insert(0, steam)
    return out


def manifest(library, app_id):
    """The appmanifest of `app_id` in `library`, or None."""
    try:
        return parse_vdf((library / f'steamapps/appmanifest_{app_id}.acf').read_text(errors='replace')).get('AppState')
    except OSError:
        return None


def depot_folder(steam, app_id, depot):
    """download_depot's folder for (app, depot) if it holds files (setup.sh's find_depot)."""
    for base in (steam / 'steamapps/content', steam / 'Steam.AppBundle/Steam/Contents/MacOS/steamapps/content'):
        d = base / f'app_{app_id}/depot_{depot}'
        if d.is_dir() and any(d.iterdir()):
            return d
    return None


def folder_size(path):
    total = 0
    for root, _dirs, files in os.walk(path):
        for f in files:
            try:
                total += os.lstat(os.path.join(root, f)).st_size
            except OSError:
                pass
    return total


def scan(steam=STEAM):
    """One row per Sauce game (see the module's doc)."""
    libs = libraries(steam)
    rows = []
    for game, title, app, linux_depot, data_depot, folder_name, marker in GAMES:
        folder, size, installed = None, None, False
        for lib in libs:
            m = manifest(lib, app)
            candidate = lib / 'steamapps/common' / ((m or {}).get('installdir') or folder_name)
            if (candidate / marker).is_file():
                folder = candidate
                # StateFlags 4: fully installed; anything else (update queued, downloading) still plays the old files.
                installed = True
                size = int((m or {}).get('SizeOnDisk') or 0) or None
                break
        if folder is None:   # the data depot fetched with the Steam console instead
            d = depot_folder(steam, app, data_depot)
            if d is not None and (d / marker).is_file():
                folder, installed = d, True
        if folder is not None and size is None:   # no manifest: measure the folder
            size = folder_size(folder)
        linux = depot_folder(steam, app, linux_depot)
        extras = []
        if folder is not None:
            extras = [{'id': e, 'title': t, 'installed': (folder / sub).is_dir()} for e, t, sub in EXTRAS.get(game, ())]
        hint = None
        if not installed:
            hint = 'Install it in Steam first.'
        elif linux is None:
            hint = f'One more file set from Steam: paste download_depot {app} {linux_depot} in the Steam console.'
        rows.append({'game': game, 'title': title, 'app_id': app, 'installed': installed,
                     'folder': str(folder) if folder else None, 'size_bytes': size,
                     'linux': str(linux) if linux else None, 'linux_command': f'download_depot {app} {linux_depot}',
                     'needs': NEEDS.get(game), 'extras': extras, 'hint': hint})
    return rows


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--steam', type=Path, default=Path(os.environ.get('P2_STEAM_DIR') or STEAM))
    a = ap.parse_args(argv)
    print(json.dumps(scan(a.steam), indent=1))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
