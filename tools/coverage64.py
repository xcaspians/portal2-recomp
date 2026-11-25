#!/usr/bin/env python3
"""Instruction coverage of tools/lift64.py over x86-64 ELF modules.

A linear sweep: every instruction objdump decodes in the executable sections
(PLT stubs, int3 and nop padding excepted) is handed to lift64.translate();
the ones it refuses are counted by mnemonic and operand kinds. It streams
objdump's output, so even libv8.so or Qt (100 MB) fit in little memory.

    coverage64.py --out build/p2ce-coverage DIR_OR_FILE...    (make p2ce-coverage)
"""
import argparse
import collections
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lift64  # noqa: E402
import simd64  # noqa: E402


class _Nothing:
    def __contains__(self, item):
        return False

    def get(self, key, default=None):
        return default


class SweepModule:
    """What translate() reads, with no knowledge of the module: jumps become
    local, jump tables go through the dispatcher."""
    insns = _Nothing()
    slots = _Nothing()
    entries = frozenset()
    table_jumps = dispatch_jumps = unbounded_tables = 0

    def jump_table(self, pc, reg):
        return None

    def is_code(self, offset):
        return True


_SECTION = re.compile(r'^Disassembly of section (\S+):')


def is_elf64_x86(path):
    try:
        with open(path, 'rb') as f:
            head = f.read(20)
    except OSError:
        return False
    return head[:4] == b'\x7fELF' and head[4] == 2 and head[18:20] == b'\x3e\x00'


def sweep(path, objdump='objdump'):
    """(instructions, Counter of unsupported forms, {form: first error})."""
    process = subprocess.Popen([objdump, '-d', '--x86-asm-syntax=intel', str(path)],
                               stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1 << 20)
    module = SweepModule()
    total, missing, why = 0, collections.Counter(), {}
    traps = collections.Counter()   # handled by a trap: CPUID-gated extensions, privileged/invalid opcodes
    section, pending_lock = None, None

    def handle(pc, nxt, op, args):
        nonlocal total
        if op in ('int3', 'nop') or (section or '').startswith('.plt'):
            return
        total += 1
        base = op[5:] if op.startswith('lock_') else op
        if simd64.gated(base):
            traps['cpuid-gated: ' + simd64.gated(base)] += 1
        elif base in lift64.PRIVILEGED or re.fullmatch(r'[0-9a-f]{2}|\(bad\)', base):
            traps['privileged or invalid (data decoded as code)'] += 1
        try:
            lift64.translate(pc, (nxt, op, args), module, None, set())
        except (lift64.Unsupported, KeyError, IndexError, ValueError) as exc:
            key = lift64.form(op, args)
            missing[key] += 1
            why.setdefault(key, str(exc))

    for line in process.stdout:
        m = _SECTION.match(line)
        if m:
            section = m[1]
            continue
        m = lift64._LINE.match(line.replace('\t', ' '))
        if not m:
            continue
        pc = int(m[1], 16)
        nxt = pc + len(m[2].split())
        args = ' '.join(re.sub(r'\s*<[^>]*>', '', m[4]).split('#', 1)[0].split())
        op = m[3]
        if pending_lock is not None:
            lock_pc, lock_next = pending_lock
            pending_lock = None
            if lock_next == pc:
                handle(lock_pc, nxt, 'lock_' + op, args)
                continue
        if op == 'lock' and not args:
            pending_lock = (pc, nxt)
            continue
        handle(pc, nxt, op, args)
    process.stdout.close()
    process.wait()
    return total, missing, why, traps


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--out', required=True, help='directory for <module>.json and summary.json')
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('paths', nargs='+', help='modules or directories (searched recursively)')
    a = ap.parse_args()
    files = []
    for p in a.paths:
        if os.path.isdir(p):
            for root, _, names in os.walk(p):
                files += [os.path.join(root, n) for n in sorted(names)]
        else:
            files.append(p)
    files = sorted(f for f in files if os.path.isfile(f) and not os.path.islink(f) and is_elf64_x86(f))
    os.makedirs(a.out, exist_ok=True)
    summary, overall, all_traps = {}, collections.Counter(), collections.Counter()
    grand_total = grand_missing = 0
    for path in files:
        name = os.path.relpath(path, os.path.commonpath(a.paths) if len(a.paths) > 1 else
                               (a.paths[0] if os.path.isdir(a.paths[0]) else os.path.dirname(a.paths[0])))
        total, missing, why, traps = sweep(path, a.objdump)
        all_traps.update(traps)
        count = sum(missing.values())
        share = 100.0 * (total - count) / total if total else 100.0
        summary[name] = dict(instructions=total, unsupported=count, handled_percent=round(share, 3),
                             trapped=sum(traps.values()))
        overall.update(missing)
        grand_total += total
        grand_missing += count
        with open(os.path.join(a.out, name.replace('/', '__') + '.json'), 'w') as f:
            json.dump(dict(module=name, instructions=total, unsupported=count, handled_percent=share,
                           top_missing=missing.most_common(60),
                           why={k: why[k] for k, _ in missing.most_common(60)}), f, indent=1)
        print(f'{share:8.3f} %  {total:9}  {count:7}  {name}', flush=True)
    below = sorted(n for n, s in summary.items() if s['handled_percent'] < 99.0)
    result = dict(modules=summary, instructions=grand_total, unsupported=grand_missing,
                  handled_percent=100.0 * (grand_total - grand_missing) / max(1, grand_total),
                  below_99=below, top_missing=overall.most_common(100), trapped=dict(all_traps.most_common()))
    with open(os.path.join(a.out, 'summary.json'), 'w') as f:
        json.dump(result, f, indent=1)
    print(f'all: {grand_total} instructions, {grand_missing} not handled '
          f'({result["handled_percent"]:.3f} % handled); {len(below)} modules under 99 %: {", ".join(below) or "none"}')
    for kind, count in all_traps.most_common():
        print(f'  handled as a trap: {count:9}  {kind}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
