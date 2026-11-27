"""tools/build_tools.py: its tool list matches what the build scripts call, and it
installs tools with checksums, without the network (file:// downloads)."""
import hashlib
import io
import os
import re
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import build_tools  # noqa: E402

INSTALLABLE = {cmd for _id, _t, cmd, _p, _f, _d in build_tools.INSTALLABLE} | {'cargo', 'rustup', 'rustc'}
KNOWN = (set(build_tools.CLT_COMMANDS) | set(build_tools.SYSTEM_COMMANDS) | set(build_tools.XCODE_COMMANDS)
         | INSTALLABLE)
# What a user's build runs (tools/build_host_deps.sh is the maintainer's, not here).
USER_SCRIPTS = ('setup.sh', 'ios/deps/build.sh', 'ios/deps/patch_state.sh', 'tools/ios_device.sh')


def called_commands():
    """Every external command the user's build path calls, as the scripts spell them."""
    found = set()
    setup = (ROOT / 'setup.sh').read_text()
    found |= set(re.findall(r'^\s*need (\w[\w.-]*) ', setup, re.M))
    found |= {w for loop in re.findall(r'for t in ([\w ]+); do\n\s+need "\$t"', setup) for w in loop.split()}
    for name in ('cargo', 'rustup', 'xcodegen', 'xcodebuild', 'xcrun', 'openssl', 'security', 'objdump', 'ar', 'git',
                 'curl', 'shasum', 'tar'):
        if re.search(rf'(^|[\s(;|&`]){re.escape(name)} ', setup, re.M):
            found.add(name)
    build = (ROOT / 'ios/deps/build.sh').read_text()
    found |= set(re.findall(r'^\s*(cmake|git|curl|tar|patch|make|ninja)\b', build, re.M))
    if '-G Ninja' in build:
        found.add('ninja')   # CMake's generator runs ninja
    found |= set(re.findall(r'^\s*(git|diff|patch)\b', (ROOT / 'ios/deps/patch_state.sh').read_text(), re.M))
    for p in [*(ROOT / 'tools').glob('*.py'), *(ROOT / 'ios').rglob('*.py')]:
        if p.name in ('build_tools.py', 'build_host_deps.py'):
            continue
        found |= set(re.findall(r"""\[\s*['"]([a-z][\w.-]*)['"]\s*,""", p.read_text()))
    device = (ROOT / 'tools/ios_device.sh').read_text()
    found |= set(re.findall(r'\b(xcrun|python3|plutil|ditto)\b', device))
    return found


class ToolListTests(unittest.TestCase):
    def test_every_command_the_builds_call_is_listed(self):
        found = called_commands()
        # Words a Python list can start with that aren't commands (flags, keys, our own scripts).
        noise = {w for w in found if '.' in w or w in {'data', 'install', 'saves', 'manifest', 'list', 'plan', 'status',
                                                       'pick', 'mel', 'workshop', 'push', 'device', 'game'}}
        missing = sorted(w for w in found - noise - KNOWN
                         if w in {'cmake', 'ninja', 'meson', 'pkg-config', 'xcodegen', 'autoconf', 'automake',
                                  'libtool', 'brew', 'cargo', 'rustup', 'scons', 'swift', 'clang', 'objdump', 'ar',
                                  'nm', 'lipo', 'git', 'curl', 'codesign', 'sips', 'iconutil', 'security', 'xcrun',
                                  'xcodebuild', 'openssl', 'python3', 'patch', 'diff', 'make'})
        self.assertEqual(missing, [], 'called by a build script but not in tools/build_tools.py')
        for cmd in ('xcodegen', 'cmake', 'ninja', 'cargo', 'xcodebuild', 'xcrun', 'clang', 'objdump'):
            self.assertIn(cmd, found, f'{cmd}: listed but no build script calls it any more')

    def test_maintainer_tools_stay_off_the_users_path(self):
        for script in USER_SCRIPTS:
            text = (ROOT / script).read_text()
            for tool in build_tools.MAINTAINER_ONLY:
                self.assertNotRegex(text, rf'(^|\s){re.escape(tool)}\s', f'{script} calls {tool}')

    def test_pins_are_exact(self):
        for _id, _t, _c, _p, formula, (url, sha, kind, exe, mb) in build_tools.INSTALLABLE:
            self.assertRegex(url, r'^https://github\.com/.+/releases/download/v?\d')
            self.assertRegex(sha, r'^[0-9a-f]{64}$')
            self.assertIn(kind, ('zip', 'tar'))
        self.assertRegex(build_tools.RUSTUP_INIT[0], r'/rustup/archive/\d+\.\d+\.\d+/aarch64-apple-darwin/')
        self.assertGreaterEqual(tuple(map(int, build_tools.RUST_TOOLCHAIN.split('.')[:2])), build_tools.RUST_MIN)
        cargo = (ROOT / 'deps/mtld3d-native/native/Cargo.toml')
        if cargo.is_file():   # the Rust mtld3d asks for (setup.sh's pinned checkout)
            m = re.search(r'rust-version = "(\d+)\.(\d+)"', cargo.read_text())
            if m:
                self.assertEqual(build_tools.RUST_MIN, (int(m.group(1)), int(m.group(2))))

    def test_setup_sh_sets_everything_up_at_once(self):
        text = (ROOT / 'setup.sh').read_text()
        self.assertIn('tools/build_tools.py" install --for device', text)
        self.assertIn('eval "$(python3 "$REPO/tools/build_tools.py" env)"', text)
        self.assertNotIn('"brew install xcodegen"', text)   # no more one missing tool per failed build
        self.assertNotIn('"brew install ninja"', text)


class InstallTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.dir = Path(self.tmp.name)
        self.old = build_tools.TOOLS_DIR
        build_tools.TOOLS_DIR = self.dir / 'tools'
        self.addCleanup(setattr, build_tools, 'TOOLS_DIR', self.old)

    def zip_with(self, name, data):
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, 'w') as z:
            z.writestr(name, data)
        path = self.dir / 'ninja-mac.zip'
        path.write_bytes(buf.getvalue())
        return path, hashlib.sha256(buf.getvalue()).hexdigest()

    def test_download_is_checked_then_installed_executable(self):
        archive, sha = self.zip_with('ninja', b'#!/bin/sh\necho 1.13.2\n')
        with self.assertRaisesRegex(RuntimeError, 'damaged'):
            build_tools.install_download('ninja', (archive.as_uri(), '0' * 64, 'zip', 'ninja', 1))
        self.assertFalse((build_tools.TOOLS_DIR / 'bin/ninja').exists())
        build_tools.install_download('ninja', (archive.as_uri(), sha, 'zip', 'ninja', 1))
        ninja = build_tools.TOOLS_DIR / 'bin/ninja'
        self.assertTrue(os.access(ninja, os.X_OK))
        self.assertIn(build_tools.TOOLS_DIR / 'bin', build_tools.path_dirs())

    def test_a_bare_mac_lists_what_to_install_and_what_to_click(self):
        env = {'PATH': '/usr/bin:/bin', 'P2_NO_BREW': '1', 'HOME': str(self.dir)}
        old_env = dict(os.environ)
        os.environ.clear(); os.environ.update(env)
        old_home = build_tools.HOME
        build_tools.HOME = self.dir
        try:
            rows = {r['id']: r for r in build_tools.checks('device')}
        finally:
            os.environ.clear(); os.environ.update(old_env)
            build_tools.HOME = old_home
        for tid in ('xcodegen', 'cmake', 'ninja', 'rust'):
            self.assertFalse(rows[tid]['ok'], tid)
            self.assertEqual(rows[tid]['action'], 'install', tid)
            self.assertIn('Set up tools installs it', rows[tid]['problem'])
        mac = {r['id'] for r in build_tools.checks('mac')}
        self.assertNotIn('xcodegen', mac)   # the Mac build needs Apple's command line tools only


class WindowTests(unittest.TestCase):
    def test_setting_up_tools_is_the_first_step(self):
        swift = (ROOT / 'installer/Devices.swift').read_text()
        self.assertIn('StepRow(number: 1, title: "Setting up tools"', swift)
        self.assertIn('if !(await setUpTools())', swift)   # Install sets the tools up first, all at once
        self.assertIn('Button("Get Xcode")', swift)
        self.assertIn('Button("Open Xcode")', swift)
        self.assertIn('tools.installTools()', swift)       # xcode-select --install
        self.assertIn('env["DEVELOPER_DIR"] = xcode', swift)


if __name__ == '__main__':
    unittest.main()
