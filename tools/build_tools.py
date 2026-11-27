#!/usr/bin/env python3
"""Every tool the Mac and device builds call, checked up front and set up in
one go (the Natif Installer's "Setting up tools" step, and setup.sh --ios).

    build_tools.py check [--for mac|device] [--json]   what's there, what's missing (exit 1 if anything is)
    build_tools.py install [--for mac|device]          installs every missing tool it can, with progress
    build_tools.py env                                 shell exports: PATH (and Rust, Xcode) for the builds

Where the tools come from:
- Apple's command line tools (clang, git, python3, objdump...): Apple's own
  installer (xcode-select --install), which the installer window triggers.
- Xcode (device builds): the App Store. Never installed from here; the window
  says so with a button. When it is installed but not selected (xcode-select
  points to the command line tools), DEVELOPER_DIR selects it: no sudo.
- xcodegen, cmake, ninja: Homebrew when it is installed (brew install), else
  pinned copies (SHA-256 checked) in the installer's own folder,
  ~/Library/Application Support/Natif Installer/tools. Homebrew itself is
  never installed: it needs an administrator password typed in Terminal.
- Rust (the Metal renderer on iPad and iPhone): rustup. The user's own rustup
  if there is one, else a private one in the same folder. Homebrew's Rust has
  no iOS target.

Nothing here needs a password. P2_TOOLS_DIR moves the folder (tests);
P2_NO_BREW=1 and P2_NO_USER_RUST=1 ignore Homebrew and ~/.cargo (tests of a
Mac without them).
"""
import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
import zipfile
from pathlib import Path

HOME = Path(os.environ.get('HOME') or Path.home())
TOOLS_DIR = Path(os.environ.get('P2_TOOLS_DIR') or HOME / 'Library/Application Support/Natif Installer/tools')
XCODE = Path('/Applications/Xcode.app/Contents/Developer')
RUST_MIN = (1, 99)   # mtld3d's rust-version (deps/mtld3d-native/native/Cargo.toml)
RUST_TOOLCHAIN = '1.100.0'

# Commands the builds call that come with macOS or Apple's command line tools
# (tests/test_build_tools.py checks that every command setup.sh, ios/deps/build.sh,
# tools/ios_device.sh and the Python tools call is in one of these lists).
CLT_COMMANDS = ('clang', 'git', 'python3', 'objdump', 'ar', 'nm', 'lipo', 'make')
SYSTEM_COMMANDS = ('curl', 'tar', 'shasum', 'codesign', 'security', 'sips', 'iconutil', 'ditto', 'openssl',
                   'xcode-select', 'find', 'sed', 'awk', 'diff', 'unzip', 'plutil', 'cp', 'pgrep', 'sysctl', 'atos')
XCODE_COMMANDS = ('xcodebuild', 'xcrun')   # xcrun devicectl, xcrun --sdk iphoneos
# Maintainer-only (tools/build_host_deps.sh makes the prebuilt bundle): never on a user's path.
MAINTAINER_ONLY = ('meson', 'pkg-config')

# Installable tools: (id, title, command, what it's for, Homebrew formula, pinned download)
# download: (url, sha256, kind, executable inside the unpacked folder, MB)
INSTALLABLE = (
    ('xcodegen', 'XcodeGen', 'xcodegen', 'signing Sauce with your Apple ID', 'xcodegen',
     ('https://github.com/yonaskolb/XcodeGen/releases/download/2.46.0/xcodegen.zip',
      '4d9e34b62172d645eed6457cac13fc222569974098ef4ee9c3368bedf0196806', 'zip', 'xcodegen/bin/xcodegen', 4)),
    ('cmake', 'CMake', 'cmake', 'building SDL2, FreeType and gl4es for Sauce', 'cmake',
     ('https://github.com/Kitware/CMake/releases/download/v4.5.1/cmake-4.5.1-macos-universal.tar.gz',
      'e1fb288543e4eb2a410e385ecebc64323bda51f0205782f2b094ccd5d572ca2d', 'tar',
      'cmake-4.5.1-macos-universal/CMake.app/Contents/bin/cmake', 89)),
    ('ninja', 'Ninja', 'ninja', 'building SDL2, FreeType and gl4es for Sauce', 'ninja',
     ('https://github.com/ninja-build/ninja/releases/download/v1.13.2/ninja-mac.zip',
      'c99048673aa765960a99cf10c6ddb9f1fad506099ff0a0e137ad8960a88f321b', 'zip', 'ninja', 1)),
)
RUSTUP_INIT = ('https://static.rust-lang.org/rustup/archive/1.29.3/aarch64-apple-darwin/rustup-init',
               '63752a1919a3a4793173ec70f8f212621a7ce38ba6a246095984482f4de20f9f')


def run(cmd, env=None, **kw):
    try:
        return subprocess.run(cmd, capture_output=True, text=True, env=env, **kw)
    except OSError:
        return subprocess.CompletedProcess(cmd, 127, '', '')


def brew():
    """Homebrew's brew, or None (P2_NO_BREW=1: as if it weren't installed)."""
    if os.environ.get('P2_NO_BREW') == '1':
        return None
    for b in ('/opt/homebrew/bin/brew', '/usr/local/bin/brew'):
        if os.access(b, os.X_OK):
            return b
    return None


def private_rust():
    return TOOLS_DIR / 'cargo', TOOLS_DIR / 'rustup'


def path_dirs():
    """Folders to put first on PATH: the installer's tools, Homebrew's, the user's Rust."""
    dirs = [TOOLS_DIR / 'bin', TOOLS_DIR / 'xcodegen/bin', TOOLS_DIR / 'cargo/bin']
    for kind_dir in sorted(TOOLS_DIR.glob('cmake/*/CMake.app/Contents/bin')):
        dirs.append(kind_dir)
    b = brew()
    if b:
        dirs.append(Path(b).parent)
    if os.environ.get('P2_NO_USER_RUST') != '1':   # tests: as if ~/.cargo weren't there
        dirs.append(HOME / '.cargo/bin')
    return [d for d in dirs if d.is_dir()]


def environment(base=None):
    """The environment the builds run in: PATH with the tools, Rust's homes when
    the private copy is used, DEVELOPER_DIR when Xcode is installed but not selected."""
    env = dict(os.environ if base is None else base)
    env['PATH'] = os.pathsep.join([*map(str, path_dirs()), env.get('PATH') or '/usr/bin:/bin:/usr/sbin:/sbin'])
    cargo, rustup = private_rust()
    user_rust = (HOME / '.cargo/bin/cargo').exists() and os.environ.get('P2_NO_USER_RUST') != '1'
    if (cargo / 'bin/cargo').exists() and not user_rust:
        env['CARGO_HOME'], env['RUSTUP_HOME'] = str(cargo), str(rustup)
    if not env.get('DEVELOPER_DIR') and xcode_installed() and not selected_is_xcode():
        env['DEVELOPER_DIR'] = str(XCODE)
    return env


def which(cmd, env=None):
    return shutil.which(cmd, path=(env or environment())['PATH'])


# ---- checks: each (id, title, ok, problem sentence, action) ----
# action: None (fine), 'clt' (Apple's installer), 'appstore' (Xcode), 'open-xcode'
# (first launch or iOS platform), 'install' (this script does it).

def clt_ok():
    r = run(['/usr/bin/xcode-select', '-p'])
    d = r.stdout.strip()
    return r.returncode == 0 and bool(d) and any(
        os.access(f'{d}/{c}', os.X_OK) for c in ('usr/bin/clang', 'Toolchains/XcodeDefault.xctoolchain/usr/bin/clang'))


def xcode_installed():
    return (XCODE / 'usr/bin/xcodebuild').exists()


def selected_is_xcode():
    return run(['/usr/bin/xcode-select', '-p']).stdout.strip().endswith('Xcode.app/Contents/Developer')


def rust_state(env):
    """(ok, sentence): cargo and rustc >= RUST_MIN with the iOS target."""
    rustc, rustup = which('rustc', env), which('rustup', env)
    if not rustc or not rustup:
        return False, 'not installed'
    m = re.search(r'rustc (\d+)\.(\d+)', run([rustc, '--version'], env=env).stdout)
    if not m or (int(m.group(1)), int(m.group(2))) < RUST_MIN:
        return False, f'too old (needs {RUST_MIN[0]}.{RUST_MIN[1]} or newer)'
    if 'aarch64-apple-ios' not in run([rustup, 'target', 'list', '--installed'], env=env).stdout:
        return False, 'the iPhone and iPad target is missing'
    return True, ''


def checks(target='device'):
    env = environment()
    out = []

    def add(tid, title, ok, problem, action, size_mb=0):
        out.append({'id': tid, 'title': title, 'ok': bool(ok), 'problem': None if ok else problem,
                    'action': None if ok else action, 'size_mb': 0 if ok else size_mb})

    clt = clt_ok()
    add('clt', "Apple's command line tools", clt,
        "Apple's command line tools aren't installed: click Install, then Install again in the window macOS opens "
        '(about 1 GB, 5–10 minutes).', 'clt', 1000)
    if clt:
        r = run(['python3', '-c', 'import sys; print(sys.version_info >= (3, 9))'], env=env)
        add('python3', 'Python 3.9 or newer', r.stdout.strip() == 'True',
            "Python is too old: update Apple's command line tools (System Settings > General > Software Update).",
            'clt')
    if target == 'mac':
        return out
    xcode = xcode_installed()
    add('xcode', 'Xcode', xcode,
        "Xcode isn't installed. It's free: click Get Xcode, install it from the App Store (about 10 GB), "
        'open it once, then come back here.', 'appstore', 10000)
    if xcode:
        first = run(['xcodebuild', '-checkFirstLaunchStatus'], env=env).returncode == 0
        add('xcode-first-launch', "Xcode's first launch", first,
            'Xcode needs its first-launch setup: click Open Xcode, then Install when it asks (it may ask for '
            'your Mac password), then come back here.', 'open-xcode')
        sdk = run(['xcrun', '--sdk', 'iphoneos', '--show-sdk-path'], env=env).returncode == 0
        add('ios-sdk', 'iOS support in Xcode', sdk,
            "Xcode doesn't have iOS support yet: click Open Xcode, then Xcode > Settings > Components and click "
            'Get next to iOS.', 'open-xcode', 8000)
    for tid, title, cmd, purpose, _formula, dl in INSTALLABLE:
        add(tid, title, which(cmd, env), f'{title} is missing (it is needed for {purpose}): Set up tools installs it.',
            'install', dl[4])
    ok, why = rust_state(env)
    add('rust', 'Rust', ok, f'Rust is {why} (it is needed for the Metal renderer on iPad and iPhone): Set up tools installs it.',
        'install', 300)
    return out


# ---- install ----

def say(line):
    print(line, flush=True)


def fetch(url, sha, dest):
    """Downloads url to dest, SHA-256 checked."""
    h = hashlib.sha256()
    with urllib.request.urlopen(url, timeout=60) as r, open(dest, 'wb') as f:
        while True:
            chunk = r.read(1 << 20)
            if not chunk:
                break
            h.update(chunk)
            f.write(chunk)
    if h.hexdigest() != sha:
        raise RuntimeError(f'the download of {url.rsplit("/", 1)[-1]} was damaged (checksum mismatch): '
                           'check the internet connection and click Set up tools again.')


def install_download(tid, dl):
    url, sha, kind, exe, _mb = dl
    TOOLS_DIR.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=TOOLS_DIR) as tmp:
        archive = Path(tmp) / url.rsplit('/', 1)[-1]
        fetch(url, sha, archive)
        unpacked = Path(tmp) / 'out'
        if kind == 'zip':
            with zipfile.ZipFile(archive) as z:
                z.extractall(unpacked)
        else:
            with tarfile.open(archive) as t:
                # Python 3.12+: the 'data' filter (relative symlinks only, as CMake.app has).
                t.extractall(unpacked, **({'filter': 'data'} if hasattr(tarfile, 'data_filter') else {}))
        # zipfile drops the executable bit
        os.chmod(unpacked / exe, 0o755)
        if tid == 'xcodegen':   # keeps its share/ folder beside bin/ (its templates)
            dest = TOOLS_DIR / 'xcodegen'
            shutil.rmtree(dest, ignore_errors=True)
            shutil.move(str(unpacked / 'xcodegen'), str(dest))
            os.chmod(dest / 'bin/xcodegen', 0o755)
        elif tid == 'cmake':
            dest = TOOLS_DIR / 'cmake'
            shutil.rmtree(dest, ignore_errors=True)
            shutil.move(str(unpacked), str(dest))
        else:
            (TOOLS_DIR / 'bin').mkdir(exist_ok=True)
            shutil.move(str(unpacked / exe), str(TOOLS_DIR / 'bin' / Path(exe).name))


def install_rust():
    env = environment()
    rustup = which('rustup', env)
    if rustup:   # the user's own (or ours from an earlier run): bring it up to date for the build
        if not rust_state(env)[0]:
            for args in (['toolchain', 'install', 'stable', '--profile', 'minimal'], ['default', 'stable'],
                         ['target', 'add', 'aarch64-apple-ios']):
                r = run([rustup, *args], env=env)
                if r.returncode != 0:
                    raise RuntimeError(f'Rust could not be updated (rustup {" ".join(args)}): {r.stderr.strip()[-200:]}')
        return
    cargo, home = private_rust()
    TOOLS_DIR.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=TOOLS_DIR) as tmp:
        init = Path(tmp) / 'rustup-init'
        fetch(*RUSTUP_INIT, init)
        os.chmod(init, 0o755)
        env = dict(env, CARGO_HOME=str(cargo), RUSTUP_HOME=str(home))
        r = run([str(init), '-y', '--no-modify-path', '--profile', 'minimal', '--default-toolchain', RUST_TOOLCHAIN,
                 '--target', 'aarch64-apple-ios'], env=env)
        if r.returncode != 0:
            raise RuntimeError(f'Rust could not be installed: {r.stderr.strip()[-200:]}')


def install(target='device'):
    """Installs every missing tool this script can; says what's left for the user."""
    todo = [c for c in checks(target) if not c['ok'] and c['action'] == 'install']
    blocked = [c for c in checks(target) if not c['ok'] and c['action'] != 'install']
    b = brew()
    formulas = [f for tid, _t, _c, _p, f, _d in INSTALLABLE if any(c['id'] == tid for c in todo)]
    steps = len(todo) if not (b and formulas) else len(todo) - len(formulas) + 1
    done = 0
    if not todo:
        say('==> All the tools are there')
    if b and formulas:
        say(f'==> Installing {", ".join(formulas)} with Homebrew ({done + 1} of {steps})')
        r = run([b, 'install', *formulas], env=dict(environment(), HOMEBREW_NO_AUTO_UPDATE='1'))
        if r.returncode != 0:
            say(f'error: Homebrew could not install {", ".join(formulas)}: {(r.stderr or r.stdout).strip()[-300:]}')
            return 1
        done += 1
        say(f'progress {done}/{steps}')
    for c in todo:
        tid = c['id']
        if b and tid in formulas:
            continue
        say(f'==> Installing {c["title"]} ({done + 1} of {steps}, about {c["size_mb"]} MB)')
        try:
            if tid == 'rust':
                install_rust()
            else:
                install_download(tid, next(i[5] for i in INSTALLABLE if i[0] == tid))
        except (OSError, RuntimeError, urllib.error.URLError) as e:
            msg = str(e)
            if isinstance(e, urllib.error.URLError) or 'timed out' in msg:
                msg = f"{c['title']} couldn't be downloaded: check the internet connection, then click Set up tools again."
            say(f'error: {msg}')
            return 1
        done += 1
        say(f'progress {done}/{steps}')
    left = [c for c in checks(target) if not c['ok']]
    for c in left:
        say(f'warning: {c["problem"]}')
    if left and not blocked:
        say('error: some tools are still missing after setup: ' + ', '.join(c['title'] for c in left))
        return 1
    return 0 if not left else 2   # 2: only the user's clicks are left (Xcode, its first launch, the tools)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    for name in ('check', 'install'):
        p = sub.add_parser(name)
        p.add_argument('--for', dest='target', choices=('mac', 'device'), default='device')
        if name == 'check':
            p.add_argument('--json', action='store_true')
    sub.add_parser('env')
    a = ap.parse_args(argv)
    if a.cmd == 'env':
        env = environment()
        for key in ('PATH', 'CARGO_HOME', 'RUSTUP_HOME', 'DEVELOPER_DIR'):
            if key in env and env.get(key) != os.environ.get(key):
                print(f'export {key}={shlex_quote(env[key])}')
        return 0
    if a.cmd == 'check':
        rows = checks(a.target)
        if a.json:
            print(json.dumps(rows, indent=1))
        else:
            for r in rows:
                print(f"{'ok  ' if r['ok'] else 'MISSING'} {r['title']}" + ('' if r['ok'] else f": {r['problem']}"))
        return 0 if all(r['ok'] for r in rows) else 1
    return install(a.target)


def shlex_quote(s):
    import shlex
    return shlex.quote(s)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
