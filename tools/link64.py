#!/usr/bin/env python3
"""Link lifted x86-64 modules (P2CE) as one dylib each, like the real engine's .so files.

    link64.py --out DIR --exe FILE --main SOURCE [--main ...] [--registry FILE] MODULE.a...

One executable holding every lifted module stopped linking in round 28:
arm64 branches reach +/-128 MB, and past about 2 GB of code the linker's
branch islands run out ("B/BL out of range"). Each module is therefore its
own dylib, so no image is bigger than one module (engine.so, the largest,
already linked inside a bigger executable):

- <out>/librt64.dylib: the runtime (runtime/x64/*.c), once, shared by all;
  its state (the module table, threads, TLS) exists once.
- <out>/lib<symbol>.dylib for each module archive (tools/build64.py's
  lib<symbol>.a), every object forced in. A module refers only to the
  runtime (rt64_dispatch and friends) and libc: calls between modules go
  through the runtime's module table by guest address, never by symbol, so
  no dylib links another. Its constructor registers it (rt64_register).
- the executable: --main's sources (src/main64.c, a test's main) and the
  registry, linked against all of them, found through @executable_path.

A dylib is relinked only when its archive (or the runtime dylib's exports)
is newer, so changing one module relinks one module. The i386 games never
go through this tool.
"""
import argparse
import concurrent.futures
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CFLAGS = ['-O1', '-g', '-Wall', '-Wextra', '-Werror']
LIBS = ['-liconv']


def symbol_of(archive):
    """lib<symbol>.a -> <symbol> (m_engine)."""
    name = os.path.basename(archive)
    if not (name.startswith('lib') and name.endswith('.a')):
        raise ValueError(f'{archive}: expected lib<symbol>.a')
    return name[3:-2]


def stale(target, sources):
    """True when target is missing or older than any source."""
    try:
        built = os.path.getmtime(target)
    except OSError:
        return True
    return any(os.path.getmtime(s) > built for s in sources)


def runtime_command(out, cflags=CFLAGS):
    sources = sorted(os.path.join(ROOT, 'runtime/x64', n) for n in os.listdir(os.path.join(ROOT, 'runtime/x64'))
                     if n.endswith('.c'))
    target = os.path.join(out, 'librt64.dylib')
    cmd = ['clang', '-dynamiclib', *cflags, '-I' + os.path.join(ROOT, 'runtime'), *sources, *LIBS,
           '-install_name', '@rpath/librt64.dylib', '-o', target]
    return target, sources + [os.path.join(ROOT, 'runtime/x64/rt64.h')], cmd


def module_command(out, archive, runtime):
    target = os.path.join(out, f'lib{symbol_of(archive)}.dylib')
    cmd = ['clang', '-dynamiclib', f'-Wl,-force_load,{archive}', runtime, *LIBS,
           '-install_name', f'@rpath/{os.path.basename(target)}', '-o', target]
    return target, [archive], cmd


def main_command(out, executable, mains, registry, runtime, dylibs, cflags=CFLAGS):
    rpath = os.path.relpath(out, os.path.dirname(os.path.abspath(executable)))
    cmd = ['clang', *cflags, '-I' + os.path.join(ROOT, 'runtime'), *mains, *([registry] if registry else []),
           runtime, *dylibs, *LIBS, f'-Wl,-rpath,@executable_path/{rpath}', '-o', executable]
    return cmd


def plan(out, executable, mains, registry, archives, cflags=CFLAGS):
    """[(target, sources, command)] in order: the runtime, the modules, the executable (always last)."""
    runtime = runtime_command(out, cflags)
    modules = [module_command(out, a, runtime[0]) for a in archives]
    dylibs = [t for t, _, _ in modules]
    exe = (executable, [runtime[0], *dylibs, *mains, *([registry] if registry else [])],
           main_command(out, executable, mains, registry, runtime[0], dylibs, cflags))
    return [runtime, *modules, exe]


def run(steps, jobs, log=print):
    runtime, *modules, exe = steps
    if stale(runtime[0], runtime[1]):
        log('linking librt64.dylib')
        subprocess.run(runtime[2], check=True)
    todo = [m for m in modules if stale(m[0], m[1])]

    def link(step):
        subprocess.run(step[2], check=True)
        return os.path.basename(step[0])
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        for name in pool.map(link, todo):
            log(f'linked {name}')
    log(f'{len(todo)} of {len(modules)} module dylibs relinked')
    subprocess.run(exe[2], check=True)
    log(f'linked {exe[0]}')


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--out', required=True, help='folder of the dylibs')
    ap.add_argument('--exe', required=True, help='the executable to link')
    ap.add_argument('--main', action='append', required=True, help='its own C source (main); repeat for more')
    ap.add_argument('--registry', help="tools/build64.py's registry.c")
    ap.add_argument('--jobs', type=int, default=4)
    ap.add_argument('--cflags', help='replaces the default flags of the runtime and the executable')
    ap.add_argument('archives', nargs='+')
    a = ap.parse_args(argv)
    os.makedirs(a.out, exist_ok=True)
    flags = a.cflags.split() if a.cflags else CFLAGS
    run(plan(a.out, a.exe, a.main, a.registry, a.archives, flags), a.jobs)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
