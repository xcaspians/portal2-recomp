#!/usr/bin/env python3
"""iPhones and iPads for the Natif Installer's Devices section: which are
reachable, what is on them, and what "Download" has to do for a game. The
work itself is tools/ios_device.sh (install, data push, mel data push,
workshop push); this only plans it, so there is one implementation.

    devices.py list                                 JSON: [{udid, name, model, kind, link, ready, problem}]
    devices.py pick                                 the UDID setup.sh installs to (exit 1 if none)
    devices.py status --device UDID                 JSON: apps on it, free space, each game's data
    devices.py status --device UDID --updates       ... plus updates: {game: [folders to copy again] or null}
    devices.py plan --device UDID --game portal2|mel|portal1|hl2|l4d2|workshop [--update] [--backup DIR]
                                                    JSON: {steps: [{label, command}], need_bytes, problem}
                                                    (--update: only the folders that changed on the Mac;
                                                    --backup: the device's saves into DIR first)

Every problem is a plain sentence for the window (device locked, not trusted,
not paired, Developer Mode off, not enough space, game not built on this Mac).
"""
import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import games  # noqa: E402
import localconfig  # noqa: E402
from ios_bundle_id import bundle_id  # noqa: E402

BASE_GAME_NAME = games.DEFAULT   # Portal 2: the one app's own game
MARGIN = 1 << 30   # kept free on the device (tools/workshop_push.py)
# Every game of the installer's table (tools/games.py installer_games), then the Workshop maps.
DOWNLOADS = (*games.installer_games(), 'workshop')
TITLES = {**{g: games.GAMES[g]['installer']['title'] for g in games.installer_games()},
          'workshop': "Workshop maps (the Mac's)"}
# Games with their own app on the device (none since round 19: every game is in
# the one app); the others live in the Portal app.
OWN_APP = tuple(g for g, p in games.GAMES.items() if p.get('ios') and g != 'portal2' and not p.get('ios_host'))
# Games copied with their own data alone (Portal 2 has its own step, Mel needs Portal 2's).
SIZES = {g: f"about {games.GAMES[g]['installer']['device_bytes'] / 1e9:g} GB" for g in games.installer_games()
         if g != BASE_GAME_NAME and not games.GAMES[g]['installer'].get('needs')}


# The games whose data lives in the one app (round 19): status asks each. Before
# round 28 only Portal 2, Mel and Portal were asked, so Half-Life 2 and Left 4
# Dead 2 always showed as missing from the device.
IN_APP = tuple(g for g in DOWNLOADS if g != 'workshop' and g not in OWN_APP)


def app_game(game):
    """The game whose app holds `game` on the device: its own, or the Portal app's."""
    return game if game in OWN_APP else 'portal2'
# Portal 2 is the base the other games in the Portal app run on (Mel borrows
# its files): nothing offers to remove it, on the device (game_picker.c) or here.
BASE_GAME = 'portal2'


def removable(game):
    return game in TITLES and game != BASE_GAME


# ---- devices ----

def device_fix(d):
    """What stands between the device and an install: (fix, problem sentence,
    steps), or (None, None, []) when it's ready. fix: 'pair' (not trusted by
    this Mac yet), 'developer-mode' (off), 'restarting' (not booted or not
    reachable, as during the restart Developer Mode asks for). The window shows
    the steps and, for every fix, waits for the device and installs by itself."""
    cp, dp, hp = d.get('connectionProperties') or {}, d.get('deviceProperties') or {}, d.get('hardwareProperties') or {}
    kind = hp.get('deviceType') or 'device'
    if cp.get('pairingState') != 'paired':
        return 'pair', ('Not paired with this Mac yet: pair it and tap Trust on it (pairing is also what makes '
                        'the Developer Mode switch appear).'), [
            f'Plug the {kind} into this Mac with a cable and unlock it.',
            'Click Pair with this Mac below.',
            f'On the {kind}, tap Trust in the “Trust This Computer?” message, then enter its passcode.',
        ]
    if dp.get('developerModeStatus') not in (None, 'enabled'):
        return 'developer-mode', 'Developer Mode is off: turn it on once, then the install carries on by itself.', [
            f'Keep the {kind} plugged into this Mac and unlocked. (The Developer Mode switch only appears once the '
            f'{kind} has been paired with a Mac, which is done.)',
            f'On the {kind}, open Settings → Privacy & Security, scroll to the bottom and tap Developer Mode.',
            'Turn Developer Mode On, then tap Restart.',
            f'When the {kind} has restarted, unlock it, tap Turn On in the message that appears, and enter the passcode.',
            'That’s all: the installer sees it come back and starts the install by itself.',
        ]
    if cp.get('tunnelState') == 'unavailable' or dp.get('bootState') not in (None, 'booted'):
        return 'restarting', 'Not reachable right now (restarting, asleep or out of reach).', [
            f'Wait for the {kind} to finish starting up, then unlock it.',
            'Keep it plugged into this Mac, or on the same Wi-Fi.',
        ]
    return None, None, []


def device_problem(d):
    """A sentence when the device can't be used now, else None."""
    return device_fix(d)[1]


def parse_devices(report):
    """The physical iPhones and iPads of `devicectl list devices --json-output`."""
    out = []
    for d in ((report or {}).get('result') or {}).get('devices') or []:
        hp, cp, dp = d.get('hardwareProperties') or {}, d.get('connectionProperties') or {}, d.get('deviceProperties') or {}
        if hp.get('reality') != 'physical' or hp.get('deviceType') not in ('iPhone', 'iPad'):
            continue
        fix, problem, steps = device_fix(d)
        out.append({'udid': hp.get('udid') or d.get('identifier'), 'name': dp.get('name') or hp.get('marketingName'),
                    'model': hp.get('marketingName') or hp.get('productType'), 'kind': hp.get('deviceType'),
                    'link': 'USB' if cp.get('transportType') == 'wired' else 'Wi-Fi',
                    'ready': problem is None, 'problem': problem, 'fix': fix, 'steps': steps})
    out.sort(key=lambda x: (not x['ready'], x['name'] or ''))
    return out


def pick(found):
    """The device setup.sh signs for and installs to when P2_DEVICE isn't set: a
    ready iPhone first, then a ready iPad, cable before Wi-Fi; never an Apple
    TV or a Mac (parse_devices lists only iPhones and iPads). None if no
    iPhone or iPad is ready. Before this, setup.sh took the first paired
    device in devicectl's list, which can be an Apple TV."""
    ready = [d for d in found if d['ready']]
    ready.sort(key=lambda d: (d['kind'] != 'iPhone', d['link'] != 'USB'))
    return ready[0]['udid'] if ready else None


def device_env(udid):
    """The environment for tools/ios_device.sh on one device: the user's own
    (devicectl and the config need HOME; without it every game read as
    missing from the device), the device pinned, a plain PATH."""
    import os
    return dict(os.environ, P2_DEVICE=udid, PATH='/usr/bin:/bin:/usr/sbin:/sbin')


def devicectl_json(*args):
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / 'out.json'
        result = subprocess.run(['xcrun', 'devicectl', *args, '--json-output', str(path)], capture_output=True, text=True)
        data = json.loads(path.read_text()) if path.is_file() and path.stat().st_size else None
        return data, result


def plain_error(text):
    """devicectl's error, as a sentence for the window."""
    t = (text or '').lower()
    if 'locked' in t or 'passcode' in t:
        return 'The device is locked: unlock it and try again.'
    if 'pair' in t or 'trust' in t:
        return 'The device doesn\'t trust this Mac yet: plug it in, unlock it and tap Trust.'
    if 'developer mode' in t:
        return 'Developer Mode is off on the device (Settings > Privacy & Security).'
    if 'unavailable' in t or 'not connected' in t or 'timed out' in t or 'no longer valid' in t:
        return 'The device stopped answering: plug it in or bring it near the Mac, unlocked.'
    return 'The device didn\'t answer as expected. Plug it in, unlock it, and try again.'


# ---- one device ----

def games_in_app(app_path):
    """The games a built app holds: its own (P2Game) and hosted ones (covers/<name>.png)."""
    app = Path(app_path)
    if not (app / 'Info.plist').is_file():
        return []
    import plistlib
    own = plistlib.loads((app / 'Info.plist').read_bytes()).get('P2Game', 'portal2')
    hosted = sorted(p.stem for p in (app / 'covers').glob('*.png') if p.stem != own) if (app / 'covers').is_dir() else []
    return [own, *hosted]


def mac_builds(config=None):
    """{game: problem or None} for the downloads: is it built on this Mac, with its data?"""
    config = localconfig.load() if config is None else config
    p2 = localconfig.game_config(config, 'portal2')
    app = games.ios_app(games.profile(p2), 'device')
    in_app = games_in_app(app)
    out = {}
    out['portal2'] = None if 'portal2' in in_app and p2.get('game_root') else \
        'Portal 2 isn\'t built for iPhone on this Mac yet: run the installer for Portal 2 with iPhone ticked.'
    mel = localconfig.game_config(config, 'mel') if (config.get('games') or {}).get('mel') else {}
    out['mel'] = None if 'mel' in in_app and mel.get('game_root') else \
        'Portal Stories: Mel isn\'t in this Mac\'s iPhone app yet: install Mel on the Mac, then build Portal 2 for iPhone again.'
    for game in SIZES:   # in the one app (round 19), when this Mac has them
        if game in OWN_APP:
            continue
        cfg = localconfig.game_config(config, game) if (config.get('games') or {}).get(game) else {}
        out[game] = None if game in in_app and cfg.get('game_root') else \
            (f'{TITLES[game]} isn\'t in this Mac\'s iPhone app yet: install {TITLES[game]} on the Mac, '
             'then build Portal 2 for iPhone again.')
    for game in OWN_APP:   # their own apps (tools/build_ios.py --game hl2 | l4d2)
        cfg = localconfig.game_config(config, game) if (config.get('games') or {}).get(game) else {}
        built = cfg.get('game_root') and game in games_in_app(games.ios_app(games.profile(dict(cfg, game=game)), 'device'))
        out[game] = None if built else \
            f'{TITLES[game]} isn\'t built for iPhone on this Mac yet: build it with tools/build_ios.py --game {game}.'
    workshop = localconfig.user_dir(p2) / 'portal2/maps/workshop'
    out['workshop'] = None if any(workshop.glob('*/*.bsp')) else \
        'No Workshop maps on this Mac: subscribe to chambers in Portal 2 on the Mac first.'
    return out


def plan(game, status, built, need_bytes, only=None, backup=None):
    """What Download does for `game` on a device in `status`
    ({app_installed, free_bytes or None, data: {game: bool}}): [(label, ios_device.sh words)]
    and a problem sentence (None when it can go). only: the destinations an
    Update copies again (device_updates.changed); backup: a folder on the Mac
    that gets the device's saves first, whenever the app or data on it is
    going to be replaced. Every game copy ends by leaving its manifest."""
    if game not in DOWNLOADS:
        return [], f'unknown download {game!r}'
    if built.get(game):
        return [], built[game]
    steps = []
    owner = app_game(game)
    installed = (status.get('apps') or {}).get(owner, status.get('app_installed') if owner == 'portal2' else False)
    if backup and installed and (status.get('data') or {}).get(game):
        steps.append(('Back up the saves on the device', ['saves-backup', str(backup)]))
    if not installed:
        steps.append((f'Install the {games.app_name(localconfig.load())} app', ['install']) if owner == 'portal2' else
                     (f'Install the {TITLES[game]} app', ['--game', game, 'install']))
    limit = ['--only', ' '.join(only)] if only else []
    verb = 'Update' if only else 'Copy'
    if game == 'portal2':
        steps.append((f'{verb} Portal 2 (about 11.5 GB)' if not only else f'{verb} Portal 2 ({len(only)} folders changed)',
                      ['data', *limit]))
    elif game in SIZES:   # its own data, no other game's needed
        steps.append((f'{verb} {TITLES[game]} ({SIZES[game] if not only else f"{len(only)} folders changed"})',
                      ['--game', game, 'data', *limit]))
    elif game == 'mel':
        # The slim pack (tools/mel_slim.py) reads Portal 2's archives: Portal 2 first.
        if not status.get('data', {}).get('portal2'):
            return [], 'Download Portal 2 first: Mel uses its files.'
        steps.append((f'{verb} Portal Stories: Mel ({"about 6.8 GB" if not only else f"{len(only)} folders changed"})',
                      ['mel', 'data', 'push', *limit]))
    else:
        if not status.get('data', {}).get('portal2'):
            return [], 'Download Portal 2 first: the Workshop maps play in it.'
        steps.append(("Copy the Mac's Workshop maps", ['workshop', 'push']))
    if game != 'workshop':
        steps.append(('Remember what was copied (for updates)', ['--game', game, 'manifest']))
    free = status.get('free_bytes')
    if free is not None and need_bytes and free < need_bytes + MARGIN:
        return [], (f'Not enough space on the device: this needs {need_bytes / 1e9:.1f} GB plus 1 GB to spare, '
                    f'and {free / 1e9:.1f} GB is free. Free some space (Settings > General > Storage).')
    return steps, None


def status(udid):
    """{app_installed (the Portal app), apps: {portal2, hl2, l4d2}, free_bytes, data: {game: bool}, problem}"""
    apps, result = devicectl_json('device', 'info', 'apps', '--device', udid)
    if apps is None:
        return {'problem': plain_error(result.stderr + result.stdout)}
    ids = {a.get('bundleIdentifier') for a in (apps.get('result') or {}).get('apps') or []}
    on = {g: bundle_id(game=g) in ids for g in ('portal2', *OWN_APP)}
    installed = on['portal2']
    out = {'app_installed': installed, 'apps': on, 'free_bytes': None, 'data': {}, 'problem': None}
    env = device_env(udid)
    for game in OWN_APP:
        if on[game]:
            has = subprocess.run([str(ROOT / 'tools/ios_device.sh'), '--game', game, 'has-data'], env=env,
                                 capture_output=True)
            out['data'][game] = has.returncode == 0
    if installed:
        space = subprocess.run([str(ROOT / 'tools/ios_device.sh'), 'space'], capture_output=True, text=True, env=env)
        for line in space.stdout.splitlines():
            if line.startswith('free_bytes='):
                out['free_bytes'] = int(line.split('=', 1)[1])
        for game in IN_APP:
            has = subprocess.run([str(ROOT / 'tools/ios_device.sh'), '--game', game, 'has-data'], env=env,
                                 capture_output=True)
            out['data'][game] = has.returncode == 0
    return out


def data_rows(game, config=None):
    """ios_data_plan's rows [(source, destination, bytes)] of what a copy of `game` sends."""
    config = localconfig.load() if config is None else config
    cfg = localconfig.game_config(config, game)
    profile = games.profile(cfg)
    import ios_data_plan
    root = Path(cfg.get('game_root', ''))
    shared, overlay, dest_root = (), None, 'Documents'
    if game == 'mel':   # the slim pack: Mel's own VPKs in portal2 stay on the Mac (tools/mel_slim.py)
        import mel_slim
        shared, dest_root = mel_slim.SHARED, 'Documents/mel'
        slim = ROOT / 'build/mel/slim'
        overlay = slim if slim.is_dir() else None
    elif game != 'portal2' and game not in OWN_APP:
        dest_root = f'Documents/{game}'
    return ios_data_plan.plan(root, Path(cfg.get('linux_root', '')), Path(profile['build_dir']) / 'prep',
                              profile['ios_data'], dest_root=dest_root, shared=shared, overlay=overlay)


def need_bytes(game, config=None):
    """About how much a download writes on the device."""
    config = localconfig.load() if config is None else config
    if game == 'workshop':
        import workshop_push
        return sum(m['size'] for m in workshop_push.scan(workshop_push.mac_workshop_dir()))
    rows = data_rows(game, config)
    return sum(n for _, _, n in rows) + (30_000_000 if game == 'mel' else 0)   # + the slim directory


def updates(udid, game, config=None):
    """Destinations of `game` to copy again ([] when up to date), or None when
    the device has no manifest for it (copied before round 28) or it isn't there."""
    import device_updates
    env = device_env(udid)
    with tempfile.TemporaryDirectory() as tmp:
        target = Path(tmp) / 'manifest.json'
        got = subprocess.run([str(ROOT / 'tools/ios_device.sh'), '--game', game, 'manifest-get', str(target)],
                             env=env, capture_output=True)
        if got.returncode != 0:
            return None
        try:
            device = json.loads(target.read_text())
        except (OSError, ValueError):
            return None
    return device_updates.changed(device_updates.signature(data_rows(game, config)), device)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('list')
    sub.add_parser('pick')
    st = sub.add_parser('status')
    st.add_argument('--device', required=True)
    st.add_argument('--updates', action='store_true')
    pl = sub.add_parser('plan')
    pl.add_argument('--device', required=True)
    pl.add_argument('--game', required=True, choices=DOWNLOADS)
    pl.add_argument('--update', action='store_true')
    pl.add_argument('--backup')
    a = ap.parse_args(argv)
    if a.cmd == 'list':
        report, result = devicectl_json('list', 'devices')
        print(json.dumps(parse_devices(report) if report else [], indent=1))
        return 0
    if a.cmd == 'pick':
        report, result = devicectl_json('list', 'devices')
        udid = pick(parse_devices(report) if report else [])
        if udid:
            print(udid)
        return 0 if udid else 1
    if a.cmd == 'status':
        st = status(a.device)
        if a.updates and not st.get('problem'):
            built = mac_builds()
            st['updates'] = {g: updates(a.device, g) for g, on in (st.get('data') or {}).items()
                             if on and not built.get(g)}
        print(json.dumps(st, indent=1))
        return 0
    st = status(a.device)
    if st.get('problem'):
        print(json.dumps({'steps': [], 'need_bytes': 0, 'problem': st['problem']}))
        return 0
    only = None
    if a.update:
        only = updates(a.device, a.game)
        if only == []:
            print(json.dumps({'steps': [], 'need_bytes': 0, 'problem': f'{TITLES[a.game]} is up to date on the device.'}))
            return 0
    if only:
        need = sum(n for _, dest, n in data_rows(a.game) if dest in only)
    else:
        need = need_bytes(a.game)
    steps, problem = plan(a.game, st, mac_builds(), need, only=only, backup=a.backup)
    print(json.dumps({'steps': [{'label': l, 'command': c} for l, c in steps], 'need_bytes': need, 'problem': problem},
                     indent=1))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
