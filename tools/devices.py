#!/usr/bin/env python3
"""iPhones and iPads for the Natif Installer's Devices section: which are
reachable, what is on them, and what "Download" has to do for a game. The
work itself is tools/ios_device.sh (install, data push, mel data push,
workshop push); this only plans it, so there is one implementation.

    devices.py list                                 JSON: [{udid, name, model, kind, link, ready, problem}]
    devices.py status --device UDID                 JSON: apps on it, free space, each game's data
    devices.py plan --device UDID --game portal2|mel|workshop
                                                    JSON: {steps: [{label, command}], need_bytes, problem}

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

MARGIN = 1 << 30   # kept free on the device (tools/workshop_push.py)
DOWNLOADS = ('portal2', 'mel', 'workshop')
TITLES = {'portal2': 'Portal 2', 'mel': 'Portal Stories: Mel', 'workshop': "Workshop maps (the Mac's)"}


# ---- devices ----

def device_problem(d):
    """A sentence when the device can't be used now, else None."""
    cp, dp = d.get('connectionProperties') or {}, d.get('deviceProperties') or {}
    if cp.get('pairingState') != 'paired':
        return 'Not trusted yet: unlock it, plug it in, and tap Trust on its screen.'
    if dp.get('developerModeStatus') not in (None, 'enabled'):
        return 'Developer Mode is off: Settings > Privacy & Security > Developer Mode, then restart it.'
    if cp.get('tunnelState') == 'unavailable' or dp.get('bootState') not in (None, 'booted'):
        return "Not reachable: plug it in, or wake it on the same Wi-Fi as this Mac, and unlock it."
    return None


def parse_devices(report):
    """The physical iPhones and iPads of `devicectl list devices --json-output`."""
    out = []
    for d in ((report or {}).get('result') or {}).get('devices') or []:
        hp, cp, dp = d.get('hardwareProperties') or {}, d.get('connectionProperties') or {}, d.get('deviceProperties') or {}
        if hp.get('reality') != 'physical' or hp.get('deviceType') not in ('iPhone', 'iPad'):
            continue
        problem = device_problem(d)
        out.append({'udid': hp.get('udid') or d.get('identifier'), 'name': dp.get('name') or hp.get('marketingName'),
                    'model': hp.get('marketingName') or hp.get('productType'), 'kind': hp.get('deviceType'),
                    'link': 'USB' if cp.get('transportType') == 'wired' else 'Wi-Fi',
                    'ready': problem is None, 'problem': problem})
    out.sort(key=lambda x: (not x['ready'], x['name'] or ''))
    return out


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
    if 'unavailable' in t or 'not connected' in t or 'timed out' in t:
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
    workshop = localconfig.user_dir(p2) / 'portal2/maps/workshop'
    out['workshop'] = None if any(workshop.glob('*/*.bsp')) else \
        'No Workshop maps on this Mac: subscribe to chambers in Portal 2 on the Mac first.'
    return out


def plan(game, status, built, need_bytes):
    """What Download does for `game` on a device in `status`
    ({app_installed, free_bytes or None, data: {game: bool}}): [(label, ios_device.sh words)]
    and a problem sentence (None when it can go)."""
    if game not in DOWNLOADS:
        return [], f'unknown download {game!r}'
    if built.get(game):
        return [], built[game]
    steps = []
    if not status.get('app_installed'):
        steps.append(('Install the Portal app', ['install']))
    if game == 'portal2':
        steps.append(('Copy Portal 2 (about 11.5 GB)', ['data']))
    elif game == 'mel':
        steps.append(('Copy Portal Stories: Mel (about 9.3 GB)', ['mel', 'data', 'push']))
    else:
        if not status.get('data', {}).get('portal2'):
            return [], 'Download Portal 2 first: the Workshop maps play in it.'
        steps.append(("Copy the Mac's Workshop maps", ['workshop', 'push']))
    free = status.get('free_bytes')
    if free is not None and need_bytes and free < need_bytes + MARGIN:
        return [], (f'Not enough space on the device: this needs {need_bytes / 1e9:.1f} GB plus 1 GB to spare, '
                    f'and {free / 1e9:.1f} GB is free. Free some space (Settings > General > Storage).')
    return steps, None


def status(udid):
    """{app_installed, free_bytes, data: {portal2, mel}, problem}"""
    portal = bundle_id(game='portal2')
    apps, result = devicectl_json('device', 'info', 'apps', '--device', udid)
    if apps is None:
        return {'problem': plain_error(result.stderr + result.stdout)}
    installed = any(a.get('bundleIdentifier') == portal for a in (apps.get('result') or {}).get('apps') or [])
    out = {'app_installed': installed, 'free_bytes': None, 'data': {}, 'problem': None}
    if installed:
        env = {'P2_DEVICE': udid, 'PATH': '/usr/bin:/bin:/usr/sbin:/sbin'}
        space = subprocess.run([str(ROOT / 'tools/ios_device.sh'), 'space'], capture_output=True, text=True, env=env)
        for line in space.stdout.splitlines():
            if line.startswith('free_bytes='):
                out['free_bytes'] = int(line.split('=', 1)[1])
        for game in ('portal2', 'mel'):
            has = subprocess.run([str(ROOT / 'tools/ios_device.sh'), '--game', game, 'has-data'], env=env,
                                 capture_output=True)
            out['data'][game] = has.returncode == 0
    return out


def need_bytes(game, config=None):
    """About how much a download writes on the device."""
    config = localconfig.load() if config is None else config
    if game == 'workshop':
        import workshop_push
        return sum(m['size'] for m in workshop_push.scan(workshop_push.mac_workshop_dir()))
    cfg = localconfig.game_config(config, game)
    profile = games.profile(cfg)
    import ios_data_plan
    root = Path(cfg.get('game_root', ''))
    rows = ios_data_plan.plan(root, Path(cfg.get('linux_root', '')), Path(profile['build_dir']) / 'prep',
                              profile['ios_data'])
    return sum(n for _, _, n in rows)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    sub.add_parser('list')
    st = sub.add_parser('status')
    st.add_argument('--device', required=True)
    pl = sub.add_parser('plan')
    pl.add_argument('--device', required=True)
    pl.add_argument('--game', required=True, choices=DOWNLOADS)
    a = ap.parse_args(argv)
    if a.cmd == 'list':
        report, result = devicectl_json('list', 'devices')
        print(json.dumps(parse_devices(report) if report else [], indent=1))
        return 0
    if a.cmd == 'status':
        print(json.dumps(status(a.device), indent=1))
        return 0
    st = status(a.device)
    if st.get('problem'):
        print(json.dumps({'steps': [], 'need_bytes': 0, 'problem': st['problem']}))
        return 0
    need = need_bytes(a.game)
    steps, problem = plan(a.game, st, mac_builds(), need)
    print(json.dumps({'steps': [{'label': l, 'command': c} for l, c in steps], 'need_bytes': need, 'problem': problem},
                     indent=1))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
