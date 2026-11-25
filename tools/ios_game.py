#!/usr/bin/env python3
"""Shell view of one game's iOS build, for tools/run_ios_sim.sh,
tools/ios_device.sh and setup.sh: prints KEY='value' lines to eval.

    eval "$(python3 tools/ios_game.py [--game portal2|hl2] --target sim|device)"

GAME, TITLE, MOD, GAME_ROOT, LINUX_ROOT (localconfig.game_config: never
another game's keys), PREP (the prep folder the app was built with), IOS_DIR
(<ios_dir>/<target>), IOS_APP, APP_ID, PROBE_ID, IOS_DATA (space-separated
folders copied to the device), USERDATA (its saves folder in Documents), HOST and DATA_ROOT (a game hosted in another
game's app: Mel in the Portal app, data in Documents/mel).
"""
import argparse
import shlex
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import localconfig  # noqa: E402
from ios_bundle_id import bundle_id  # noqa: E402


def values(name, target, config, standalone=False):
    """A game hosted in another game's app (ios_host: Mel in the Portal app) uses
    that app (IOS_APP, APP_ID) and keeps its data in Documents/<name>
    (DATA_ROOT); HOST names the app's game. standalone: its own app instead."""
    merged = localconfig.game_config(config, name)
    game = games.profile(merged)
    missing = games.missing_ios_data(game, merged.get('game_root', ''))
    if missing:   # the device copy would lack them (L4D2 without hl2: no shaders, black screen)
        raise SystemExit(f'{name}: gameinfo.txt searches {", ".join(missing)}, missing from ios_data '
                         '(tools/games.py)')
    host = None if standalone else game.get('ios_host')
    app_game = games.profile(localconfig.game_config(config, host)) if host else game
    return dict(GAME=game['name'], TITLE=game['ios_name'], MOD=game['mod'],
                GAME_ROOT=merged.get('game_root', ''), LINUX_ROOT=merged.get('linux_root', ''),
                PREP=str(Path(game['build_dir']) / 'prep'),
                IOS_DIR=str(Path(game['ios_dir']) / target), IOS_APP=str(games.ios_app(app_game, target)),
                APP_ID=bundle_id(game=app_game['name'], config=config),
                PROBE_ID=bundle_id(True, app_game['name'], config),
                IOS_DATA=' '.join(game['ios_data']), USERDATA=games.ios_userdata(game['name']),
                HOST=host or '', DATA_ROOT=f'Documents/{game["name"]}' if host else 'Documents')


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    games.add_argument(ap)
    ap.add_argument('--target', choices=('sim', 'device'), required=True)
    ap.add_argument('--standalone', action='store_true', help="a hosted game's own app (P2_STANDALONE=1 in the tools)")
    args = ap.parse_args()
    for key, value in values(args.game, args.target, localconfig.load(), args.standalone).items():
        print(f'{key}={shlex.quote(value)}')
