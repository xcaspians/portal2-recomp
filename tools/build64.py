#!/usr/bin/env python3
"""Prepare, lift and compile x86-64 modules (P2CE) into static libraries.

    build64.py --out build/p2ce/lib libtier0.so libvstdlib.so ...

For each module: tools/elfprep64.py (image and layout), tools/lift64.py
(every known entry point, split into C files of about --shard-size bytes),
then clang on each file with an object cache keyed by the file's contents
and the flags (an unchanged file is never recompiled), --jobs at a time,
and `ar` into <out>/<symbol>/lib<symbol>.a. Before each compile it waits
while the Mac's used swap is above --max-swap-gb (heavy builds may be
running next to it). The 64-bit counterpart of tools/build_game.py's
splitting and caching; it builds nothing of the i386 games.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
import re
import subprocess
import sys
import time

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)
import elfprep64  # noqa: E402
import lift64  # noqa: E402


def symbol_for(name):
    """C identifier of a module: libtier0.so -> tier0, libQt6Core.so.6 -> qt6core."""
    base = name.split('.so')[0]
    if base.startswith('lib') and len(base) > 3:
        base = base[3:]
    ident = re.sub(r'[^0-9a-zA-Z_]', '_', base).lower()
    return 'm_' + ident


def used_swap_gb():
    try:
        out = subprocess.run(['sysctl', '-n', 'vm.swapusage'], capture_output=True, text=True).stdout
        m = re.search(r'used = ([0-9.]+)([MG])', out)
        return float(m[1]) / (1024 if m[2] == 'M' else 1) if m else 0.0
    except OSError:
        return 0.0


def wait_for_memory(limit_gb, log):
    while limit_gb and used_swap_gb() > limit_gb:
        log(f'swap {used_swap_gb():.1f} GB > {limit_gb} GB: waiting 60 s')
        time.sleep(60)


def compile_one(source, obj_dir, flags, limit_gb, log):
    with open(source, 'rb') as f:
        text = f.read()
    header = os.path.join(os.path.dirname(source), os.path.basename(source).rsplit('_', 1)[0] + '.h')
    digest = hashlib.sha256(text + ' '.join(flags).encode())
    if os.path.exists(header) and not source.endswith('.h'):
        with open(header, 'rb') as f:
            digest.update(f.read())
    with open(os.path.join(ROOT, 'runtime', 'x64', 'rt64.h'), 'rb') as f:
        digest.update(f.read())
    obj = os.path.join(obj_dir, os.path.basename(source)[:-2] + '-' + digest.hexdigest()[:16] + '.o')
    if os.path.exists(obj):
        return obj, False
    wait_for_memory(limit_gb, log)
    started = time.time()
    subprocess.run(['clang', *flags, '-c', source, '-o', obj + '.tmp'], check=True)
    os.replace(obj + '.tmp', obj)
    log(f'compiled {os.path.basename(source)} in {time.time() - started:.0f} s')
    return obj, True


def build_module(path, out, jobs, shard_size, opt, limit_gb, roots=None, log=print):
    elf_name = os.path.basename(path)
    symbol = symbol_for(elf_name)
    work = os.path.join(out, symbol)
    src, obj_dir = os.path.join(work, 'src'), os.path.join(work, 'obj')
    os.makedirs(src, exist_ok=True)
    os.makedirs(obj_dir, exist_ok=True)
    started = time.time()
    layout = elfprep64.prepare(path, work)
    with open(os.path.join(work, 'layout.path'), 'w') as f:
        f.write(os.path.join(work, layout['name'] + '.layout.json'))
    bodies, report = lift64.lift_module(layout, lift64.resolve_roots(layout, roots or []), every=not roots)
    size = sum(len(b) for b in bodies.values())
    shards = max(2, -(-size // shard_size))
    files = lift64.emit(layout, bodies, symbol, shards)
    for name in os.listdir(src):
        if name not in files:
            os.remove(os.path.join(src, name))
    for name, text in files.items():
        target = os.path.join(src, name)
        try:
            with open(target) as f:
                if f.read() == text:
                    continue
        except OSError:
            pass
        with open(target, 'w') as f:
            f.write(text)
    log(f'{elf_name}: lifted {report["functions"]} functions, {report["instructions"]} instructions '
        f'({report["unsupported"]} traps) into {len(files) - 1} C files, {size / 1e6:.1f} MB, '
        f'in {time.time() - started:.0f} s')
    flags = [f'-O{opt}', '-g0', '-w', '-I' + os.path.join(ROOT, 'runtime'), '-fno-strict-aliasing']
    sources = sorted(os.path.join(src, n) for n in files if n.endswith('.c'))
    objects, compiled = [], 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        for obj, fresh in pool.map(lambda s: compile_one(s, obj_dir, flags, limit_gb, log), sources):
            objects.append(obj)
            compiled += fresh
    for name in os.listdir(obj_dir):
        if os.path.join(obj_dir, name) not in objects:
            os.remove(os.path.join(obj_dir, name))
    library = os.path.join(work, f'lib{symbol}.a')
    if os.path.exists(library):
        os.remove(library)
    subprocess.run(['ar', 'rcs', library, *objects], check=True)
    summary = dict(module=elf_name, symbol=symbol, library=library, functions=report['functions'],
                   instructions=report['instructions'], traps=report['unsupported'], c_bytes=size,
                   c_files=len(sources), compiled=compiled, seconds=round(time.time() - started),
                   flags_read_after_call=report['flags_read_after_call'],
                   flags_live_at_entry=report['flags_live_at_entry'])
    with open(os.path.join(work, 'build.json'), 'w') as f:
        json.dump(summary, f, indent=1)
    log(f'{elf_name}: {library} ({compiled} of {len(sources)} files compiled, {summary["seconds"]} s)')
    return summary


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--out', required=True, help='output directory (one folder per module)')
    ap.add_argument('--jobs', type=int, default=2, help='parallel compiles (default 2)')
    ap.add_argument('--shard-size', type=int, default=1_500_000, help='bytes of C per file (default 1.5 MB)')
    ap.add_argument('--opt', default='1', help='clang optimisation level (default 1)')
    ap.add_argument('--max-swap-gb', type=float, default=6.0, help='wait while used swap is above this (0: never)')
    ap.add_argument('--roots', nargs='*', help='lift only these exports and what they call')
    ap.add_argument('modules', nargs='+')
    a = ap.parse_args()
    results = [build_module(m, a.out, a.jobs, a.shard_size, a.opt, a.max_swap_gb, a.roots) for m in a.modules]
    with open(os.path.join(a.out, 'summary.json'), 'w') as f:
        json.dump(results, f, indent=1)
    return 0


if __name__ == '__main__':
    sys.exit(main())
