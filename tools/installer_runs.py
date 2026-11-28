#!/usr/bin/env python3
"""The Natif Installer's runs: every tool it starts (setup.sh, devices.py,
ios_device.sh, build_tools.py, devicectl pair) goes through `run`, which makes
it the leader of its own process group, records who owns it, and takes the
whole group down with it.

Round 31: before this, quitting the installer (Cmd+Q, a crash, Force Quit)
left setup.sh, ninja, build_game.py and devicectl copies running with no
window, still talking to the iPhone; the next launch then sat on "Waiting"
behind them until the Mac was restarted.

    installer_runs.py run --state DIR --label TEXT [--game ID] [--timeout S] -- CMD...
        CMD in a new process group; DIR/<pid>.json says who holds it (the
        installer's pid and start time, the label, when it started). The group
        is stopped when the owner goes away (quit, crash, Force Quit: checked
        every second), on SIGTERM/SIGINT/SIGHUP, or after --timeout seconds
        (exit 124, "error: ... didn't answer within S seconds").
    installer_runs.py list --state DIR [--repo DIR]
        JSON: the runs still going: {pid, label, game, started, owner_alive}.
        With --repo, also processes of an older installer (no record) running
        from that folder with no installer above them.
    installer_runs.py clean --state DIR [--repo DIR]
        Stops every run whose owner is gone (and, with --repo, those older
        leftovers), removes their records. Runs of a live installer are never
        touched. Prints what it stopped, as JSON.
    installer_runs.py stop --state DIR --owner PID
        Stops every run of that installer (its quit).

A record is a lock: it is held while its owner lives, and a record whose
owner is dead (or whose pid now belongs to another program: the start time
is compared) is cleared.
"""
import argparse
import json
import os
import signal
import subprocess
import sys
import time
from pathlib import Path

GRACE = 5   # seconds between SIGTERM and SIGKILL
INSTALLER_EXE = 'Contents/MacOS/installer'   # tools/build_installer.sh's CFBundleExecutable


def start_time(pid):
    """The process's start time as ps prints it, or None when there is no such process."""
    try:
        out = subprocess.run(['/bin/ps', '-o', 'lstart=', '-p', str(pid)], capture_output=True, text=True, timeout=10)
    except (OSError, subprocess.TimeoutExpired):
        return None
    return out.stdout.strip() or None


def alive(pid, started=None):
    """pid is running, and is the same process as when `started` was taken."""
    if pid <= 1:
        return False
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    except PermissionError:
        pass
    try:   # a zombie (killed, not reaped yet) is gone
        stat = subprocess.run(['/bin/ps', '-o', 'stat=', '-p', str(pid)], capture_output=True, text=True, timeout=10)
        if stat.stdout.strip().startswith('Z'):
            return False
    except (OSError, subprocess.TimeoutExpired):
        pass
    return started is None or start_time(pid) == started


def group_alive(pgid):
    try:
        os.killpg(pgid, 0)
        return True
    except ProcessLookupError:
        return False
    except PermissionError:
        return True


def stop_group(pgid, grace=GRACE):
    """SIGTERM to the group, then SIGKILL to what is left after `grace` seconds."""
    for sig in (signal.SIGTERM, signal.SIGKILL):
        try:
            os.killpg(pgid, sig)
        except ProcessLookupError:
            return True
        except PermissionError:
            return False
        end = time.time() + grace
        while time.time() < end:
            if not group_alive(pgid):
                return True
            time.sleep(0.1)
    return not group_alive(pgid)


def records(state):
    out = []
    for path in sorted(Path(state).glob('*.json')):
        try:
            r = json.loads(path.read_text())
        except (OSError, ValueError):
            path.unlink(missing_ok=True)
            continue
        r['path'] = path
        out.append(r)
    return out


def processes():
    """[(pid, ppid, pgid, command)] of this user's processes."""
    out = subprocess.run(['/bin/ps', '-x', '-o', 'pid=,ppid=,pgid=,command='], capture_output=True, text=True)
    rows = []
    for line in out.stdout.splitlines():
        parts = line.split(None, 3)
        if len(parts) >= 3 and all(p.isdigit() for p in parts[:3]):
            rows.append((int(parts[0]), int(parts[1]), int(parts[2]), parts[3] if len(parts) > 3 else ''))
    return rows


def orphans(repo, rows=None):
    """Processes running from `repo` (an older installer's, with no record)
    with no installer and no run of ours above them: the roots of what was
    left behind, as [(pid, pgid, command)]."""
    rows = processes() if rows is None else rows
    names = {str(repo), str(Path(repo).resolve())}   # /var and /private/var: either spelling
    by_pid = {pid: (ppid, pgid, cmd) for pid, ppid, pgid, cmd in rows}
    me = os.getpid()

    def attached(pid):
        seen = set()
        while pid in by_pid and pid not in seen and pid > 1:
            seen.add(pid)
            ppid, _pgid, cmd = by_pid[pid]
            if INSTALLER_EXE in cmd or 'installer_runs.py run' in cmd or pid == me:
                return True
            pid = ppid
        return False

    found = []
    for pid, (ppid, pgid, cmd) in by_pid.items():
        if not any(n in cmd for n in names) or 'installer_runs.py' in cmd or attached(pid):
            continue
        if ppid in by_pid and any(n in by_pid[ppid][2] for n in names) and not attached(ppid):
            continue   # its parent is the root of the same leftover
        found.append((pid, pgid, cmd))
    return found


def descendants(pid, rows):
    kids = {}
    for p, pp, _g, _c in rows:
        kids.setdefault(pp, []).append(p)
    out, todo = [], [pid]
    while todo:
        p = todo.pop()
        out.append(p)
        todo.extend(kids.get(p, []))
    return out


def stop_tree(pid, rows):
    """A leftover with no group of its own: its whole tree, children first."""
    tree = descendants(pid, rows)
    for sig in (signal.SIGTERM, signal.SIGKILL):
        for p in reversed(tree):
            try:
                os.kill(p, sig)
            except (ProcessLookupError, PermissionError):
                pass
        end = time.time() + GRACE
        while time.time() < end and any(alive(p) for p in tree):
            time.sleep(0.1)
        if not any(alive(p) for p in tree):
            return True
    return not any(alive(p) for p in tree)


def describe(r):
    return {'pid': r['pid'], 'label': r.get('label', ''), 'game': r.get('game'), 'started': r.get('started'),
            'owner': r.get('owner'), 'owner_alive': alive(r.get('owner', 0), r.get('owner_started'))}


def live_runs(state):
    """Records whose group still runs (the others are cleared on the way)."""
    out = []
    for r in records(state):
        if group_alive(r['pid']):
            out.append(r)
        else:
            r['path'].unlink(missing_ok=True)
    return out


def signal_group_but_me(sig):
    me = os.getpid()
    group = os.getpgid(0)
    for pid, _pp, pgid, _c in processes():
        if pgid == group and pid != me:
            try:
                os.kill(pid, sig)
            except (ProcessLookupError, PermissionError):
                pass


# ---- commands ----

def cmd_run(a):
    if not a.command:
        print('error: nothing to run', file=sys.stderr)
        return 2
    try:
        os.setsid()   # our own group: everything below us goes with it
    except OSError:   # already a group leader (Foundation's Process can make it one)
        if os.getpgid(0) != os.getpid():
            os.setpgid(0, 0)
    state = Path(a.state)
    state.mkdir(parents=True, exist_ok=True)
    owner = os.getppid()
    record = {'pid': os.getpid(), 'owner': owner, 'owner_started': start_time(owner), 'label': a.label,
              'game': a.game, 'started': time.time(), 'command': a.command}
    path = state / f'{os.getpid()}.json'
    tmp = path.with_suffix('.tmp')
    tmp.write_text(json.dumps(record))
    tmp.replace(path)

    stopping = {'why': None}

    def on_signal(signum, _frame):
        stopping['why'] = stopping['why'] or f'signal {signum}'
    for s in (signal.SIGTERM, signal.SIGINT, signal.SIGHUP):
        signal.signal(s, on_signal)

    try:
        child = subprocess.Popen(a.command)
    except OSError as e:
        path.unlink(missing_ok=True)
        print(f'error: could not start {a.command[0]}: {e.strerror}', file=sys.stderr, flush=True)
        return 127
    deadline = time.time() + a.timeout if a.timeout else None
    code = None
    while code is None:
        try:
            code = child.wait(timeout=1)
            break
        except subprocess.TimeoutExpired:
            pass
        if stopping['why'] is None:
            if os.getppid() != owner or not alive(owner):
                stopping['why'] = 'owner gone'
            elif deadline and time.time() > deadline:
                stopping['why'] = 'timeout'
        if stopping['why']:
            # The children only: we stay to clear the record.
            signal.signal(signal.SIGTERM, signal.SIG_IGN)
            for s in (signal.SIGTERM, signal.SIGKILL):
                signal_group_but_me(s)
                try:
                    code = child.wait(timeout=GRACE)
                    break
                except subprocess.TimeoutExpired:
                    continue
            if code is None:
                code = -9
            # Grandchildren that outlived their parent (a copy left by a killed script).
            time.sleep(0.2)
            signal_group_but_me(signal.SIGKILL)
    path.unlink(missing_ok=True)
    if stopping['why'] == 'timeout':
        print(f"error: {a.label or a.command[0]} didn't answer within {a.timeout:g} seconds", file=sys.stderr, flush=True)
        return 124
    if stopping['why']:
        return 130
    return code if code >= 0 else 128 - code


def cmd_list(a):
    out = [describe(r) for r in live_runs(a.state)]
    if a.repo:
        out += [{'pid': pid, 'label': 'an older installer\'s build', 'game': None, 'started': None,
                 'owner': None, 'owner_alive': False, 'command': cmd[:200]} for pid, _g, cmd in orphans(a.repo)]
    print(json.dumps(out, indent=1))
    return 0


def cmd_clean(a):
    stopped = []
    for r in live_runs(a.state):
        if alive(r.get('owner', 0), r.get('owner_started')):
            continue   # a live installer's: never touched
        ok = stop_group(r['pid'])
        if ok:
            r['path'].unlink(missing_ok=True)
        stopped.append({**describe(r), 'stopped': ok})
    for r in records(a.state):   # records whose group is gone
        if not group_alive(r['pid']):
            r['path'].unlink(missing_ok=True)
    if a.repo:
        rows = processes()
        for pid, _pgid, cmd in orphans(a.repo, rows):
            stopped.append({'pid': pid, 'label': "an older installer's build", 'started': None,
                            'command': cmd[:200], 'stopped': stop_tree(pid, rows)})
    print(json.dumps(stopped, indent=1))
    return 0


def cmd_stop(a):
    stopped = []
    for r in live_runs(a.state):
        if r.get('owner') == a.owner:
            stopped.append({**describe(r), 'stopped': stop_group(r['pid'])})
            r['path'].unlink(missing_ok=True)
    print(json.dumps(stopped, indent=1))
    return 0


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run')
    r.add_argument('--state', required=True)
    r.add_argument('--label', default='')
    r.add_argument('--game')
    r.add_argument('--timeout', type=float, default=0)
    r.add_argument('command', nargs=argparse.REMAINDER)
    for name in ('list', 'clean'):
        p = sub.add_parser(name)
        p.add_argument('--state', required=True)
        p.add_argument('--repo')
    s = sub.add_parser('stop')
    s.add_argument('--state', required=True)
    s.add_argument('--owner', type=int, required=True)
    a = ap.parse_args(argv)
    if a.cmd == 'run' and a.command and a.command[0] == '--':
        a.command = a.command[1:]
    return {'run': cmd_run, 'list': cmd_list, 'clean': cmd_clean, 'stop': cmd_stop}[a.cmd](a)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
