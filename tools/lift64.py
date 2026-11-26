#!/usr/bin/env python3
"""x86-64 -> C ahead-of-time translator, first slice (P2CE, fork round 13).

The 64-bit counterpart of tools/lift.py, which stays i386-only. The input is
a module prepared by tools/elfprep64.py; LLVM's objdump decodes the machine
code (prefixes, REX, ModRM, displacements) and this file turns each decoded
instruction into C over the runtime's CPU64 (runtime/x64/rt64.h).

What changes from i386:
  * 16 general registers of 64 bits; a 32-bit write clears the upper half,
    8/16-bit writes keep it; spl/bpl/sil/dil and r8b..r15b exist.
  * RIP-relative memory: [rip + disp] is module-relative, emitted as
    `B_ + offset` where B_ is the module's load address (guest memory is host
    memory, so the module goes wherever the host maps it).
  * System V AMD64 calls: arguments in registers, the stack only for the
    return address and extra arguments; CALL/RET still go through the guest
    stack so the guest sees the layout it expects.
  * Code addresses (return addresses, jump tables, function pointers) are
    module offsets plus B_ as well.

Every guest function becomes one C function; an unsupported instruction is
replaced by a trap (and counted), so a module always compiles and the
coverage report says exactly what is missing.
"""
import argparse
import bisect
import collections
import json
import os
import re
import struct
import subprocess
import sys

import simd64
import x87_64

R64 = 'rax rcx rdx rbx rsp rbp rsi rdi'.split() + [f'r{i}' for i in range(8, 16)]
R32 = 'eax ecx edx ebx esp ebp esi edi'.split() + [f'r{i}d' for i in range(8, 16)]
R16 = 'ax cx dx bx sp bp si di'.split() + [f'r{i}w' for i in range(8, 16)]
R8 = 'al cl dl bl spl bpl sil dil'.split() + [f'r{i}b' for i in range(8, 16)]
GPR = {}
for width, names in ((64, R64), (32, R32), (16, R16), (8, R8)):
    GPR.update({name: (i, width, 0) for i, name in enumerate(names)})
GPR.update({'ah': (0, 8, 8), 'ch': (1, 8, 8), 'dh': (2, 8, 8), 'bh': (3, 8, 8)})
XMM = {f'xmm{i}': i for i in range(18)}   # 16, 17: MMX scratch (x87_64.py)
CC = dict(o='O', no='NO', b='B', c='B', nae='B', ae='AE', nb='AE', nc='AE',
          e='E', z='E', ne='NE', nz='NE', be='BE', na='BE', a='A', nbe='A',
          s='S', ns='NS', p='P', pe='P', np='NP', po='NP', l='L', nge='L',
          ge='GE', nl='GE', le='LE', ng='LE', g='G', nle='G')
WIDTHS = {'byte': 8, 'word': 16, 'dword': 32, 'qword': 64, 'xmmword': 128, 'tbyte': 80, 'ymmword': 256}
_MEMORY = re.compile(r'(?:(byte|word|dword|qword|xmmword|tbyte|ymmword) ptr )?(?:(cs|ds|es|ss|fs|gs):)?\[(.*)\]')
_LINE = re.compile(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s)+)\s*([a-z][a-z0-9]*)\s*(.*?)\s*$')


class Unsupported(ValueError):
    pass


# Flag hand-over at calls and returns. System V AMD64 does not keep the
# arithmetic flags across calls, so lift_function() drops these unless the
# function reads flags it did not set (see flag_liveness).
SAVE_F = '/*s*/c->flags = f;'
LOAD_F = '/*l*/f = c->flags;'


def disassemble(path, objdump='objdump', start=None, stop=None):
    """{pc: (next pc, mnemonic, operands)} for every executable section (or the window [start, stop))."""
    window = [f'--start-address={start:#x}', f'--stop-address={stop:#x}'] if start is not None else []
    raw = subprocess.run([objdump, '-d', '-z', '--x86-asm-syntax=intel', *window, str(path)],
                         check=True, capture_output=True, text=True).stdout
    insns = {}
    for line in raw.splitlines():
        m = _LINE.match(line.replace('\t', ' '))
        if not m:
            continue
        pc = int(m[1], 16)
        size = len(m[2].split())
        args = re.sub(r'\s*<[^>]*>', '', m[4]).split('#', 1)[0]
        insns[pc] = (pc + size, m[3], ' '.join(args.split()))
    # objdump prints LOCK on its own line; it belongs to the next instruction.
    for pc, (nxt, op, args) in list(insns.items()):
        if op == 'lock' and not args and nxt in insns:
            end, following, operands = insns.pop(nxt)
            insns[pc] = (end, 'lock_' + following, operands)
    return insns


def code(offset):
    """C expression of a code or data address of this module."""
    return f'(B_ + {offset:#x}ull)'


def address(expr, nxt):
    """Effective address of a memory operand: registers, scale, constants, RIP."""
    tokens = re.findall(r'0x[0-9a-f]+|[a-z][a-z0-9]*|[0-9]+|[-+*]', expr)
    if ''.join(tokens) != re.sub(r'\s+', '', expr):
        raise Unsupported(f'address expression {expr!r}')
    terms, constant, rip, narrow = [], 0, False, False
    for part in filter(None, re.split(r'(?=[+-])', ''.join(tokens))):
        sign = -1 if part.startswith('-') else 1
        factors = part.lstrip('+-').split('*')
        regs = [x for x in factors if x in GPR or x in ('rip', 'eiz', 'riz')]
        nums = [x for x in factors if x not in regs]
        try:
            scale = 1
            for x in nums:
                scale *= int(x, 0)
        except ValueError:
            raise Unsupported(f'address expression {expr!r}') from None
        if not regs:
            constant += sign * scale
            continue
        if len(regs) != 1:
            raise Unsupported(f'address expression {expr!r}')
        reg = regs[0]
        if reg == 'rip':
            if sign < 0 or scale != 1:
                raise Unsupported('scaled RIP')
            rip = True
        elif reg in ('eiz', 'riz'):
            continue
        else:
            index, width, shift = GPR[reg]
            if width == 32:
                narrow = True
            elif width != 64:
                raise Unsupported(f'{width}-bit address register')
            term = f'c->r[{index}]' + (f' * {scale}u' if scale != 1 else '')
            terms.append(('-' if sign < 0 else '+') + term)
    if rip:
        if terms:
            raise Unsupported('RIP with a register')
        return code(nxt + constant)
    body = ''.join(f' {t[0]} {t[1:]}' for t in terms).strip()
    body = (body[2:] if body.startswith('+ ') else '0 ' + body) if body else '0'
    if constant:
        body += f' + {constant & 0xffffffffffffffff:#x}ull'
    return f'(uint32_t)({body})' if narrow else f'(uint64_t)({body})'


PRIVILEGED = frozenset('''in out insb insw insd outsb outsw outsd sti cli iret iretd iretq hlt lgdt lidt lldt
    ltr sgdt sidt sldt str invd wbinvd invlpg rdmsr wrmsr rdpmc sysenter sysexit sysret clts lmsw smsw swapgs
    into bound aaa aas aam aad daa das les lds arpl icebp int1 retf lret ljmp lcall'''.split())


class Operand:
    def __init__(self, text, nxt):
        self.text = text = text.strip()
        self.reg = GPR.get(text)
        self.xmm = XMM.get(text)
        self.addr = self.imm = self.width = None
        if self.reg:
            self.width = self.reg[1]
            return
        if self.xmm is not None:
            self.width = 128
            return
        m = _MEMORY.fullmatch(text)
        if m:
            effective = address(m[3], nxt)
            self.effective = effective
            self.addr = f'(c->{m[2]}_base + {effective})' if m[2] in ('fs', 'gs') else effective
            self.width = WIDTHS.get(m[1]) if m[1] else None
            return
        if re.fullmatch(r'st(\(\d\))?|mm\d', text):
            return          # x87 stack or MMX register: read by x87_64.py from .text
        try:
            self.imm = int(text, 0)
        except ValueError:
            raise Unsupported(f'operand {text!r}') from None

    def read(self, width=None):
        if self.reg:
            index, width, shift = self.reg
            if width == 64:
                return f'c->r[{index}]'
            if shift:
                return f'(uint64_t)(uint8_t)(c->r[{index}] >> 8)'
            return f'(uint64_t)(uint{width}_t)c->r[{index}]'
        if self.addr is not None:
            if self.width not in (8, 16, 32, 64):
                raise Unsupported(f'memory read of width {self.width}')
            return f'R{self.width}({self.addr})'
        if self.imm is not None:
            n = width or 64
            return f'{self.imm & ((1 << n) - 1):#x}ull'
        raise Unsupported(self.text)

    def write(self, value):
        if self.reg:
            index, width, shift = self.reg
            if width == 64:
                return f'c->r[{index}] = (uint64_t)({value});'
            if width == 32:
                return f'c->r[{index}] = (uint32_t)({value});'
            mask = ((1 << width) - 1) << shift
            return (f'c->r[{index}] = (c->r[{index}] & {~mask & 0xffffffffffffffff:#x}ull) | '
                    f'((uint64_t)(uint{width}_t)({value}) << {shift});')
        if self.addr is not None:
            if self.width not in (8, 16, 32, 64):
                raise Unsupported(f'memory write of width {self.width}')
            return f'W{self.width}({self.addr}, {value});'
        raise Unsupported(f'destination {self.text!r}')


class Module:
    """What the translator knows about the module beyond its instructions."""

    def __init__(self, layout, insns):
        self.layout = layout
        self.insns = insns
        self.order = sorted(insns)
        self.position = {pc: i for i, pc in enumerate(self.order)}
        # A linear sweep can lose step on data inside the code (a jump
        # table, padding) and swallow the start of a function: decode again
        # from each entry point it missed, until it meets known instructions.
        self.resynced = 0
        for e in layout['entries']:
            if e not in insns and self.is_code(e):
                self.resync(e)
        if self.resynced:
            self.order = sorted(insns)
            self.position = {pc: i for i, pc in enumerate(self.order)}
        self.entries = {e for e in layout['entries'] if e in insns}
        self.slots = {}
        for r in layout['relocations']:
            if r['type'] in ('64', 'GLOB_DAT', 'JUMP_SLOT') and 'value' in r:
                self.slots[r['off']] = r['value'] + (r['addend'] if r['type'] == '64' else 0)
            elif r['type'] == 'RELATIVE':
                self.slots[r['off']] = r['addend']
        # Imports reached through a GOT slot (PLT stubs, call [rip + GOT]): slot -> name.
        self.slot_imports = {r['off']: r['symbol'] for r in layout['relocations']
                             if r['type'] in ('GLOB_DAT', 'JUMP_SLOT', '64') and 'import' in r}
        self.plt_imports = {}
        for lo, hi in layout.get('plt') or []:
            for pc in self.order[bisect.bisect_left(self.order, lo):bisect.bisect_left(self.order, hi)]:
                nxt, op, args = insns[pc]
                m = re.fullmatch(r'qword ptr \[rip ([+-]) (0x[0-9a-f]+)\]', args)
                if op == 'jmp' and m:
                    slot = nxt + (1 if m[1] == '+' else -1) * int(m[2], 16)
                    name = self.slot_imports.get(slot)
                    if name:
                        # The stub may start a few instructions earlier (mov r11d, n; endbr64).
                        for start in (pc, *[p for p, _ in self.previous(pc, 2)]):
                            self.plt_imports.setdefault(start, name)
        self.landing_pads = {int(k, 16): v for k, v in (layout.get('landing_pads') or {}).items()}
        self.image = None
        if layout.get('image'):
            with open(layout['image'], 'rb') as f:
                self.image = f.read()
        self.table_jumps = self.dispatch_jumps = self.unbounded_tables = 0

    def resync(self, pc):
        """Decode from `pc` (an address the linear sweep did not start an instruction at)."""
        window = disassemble(self.layout['source'], start=pc, stop=pc + 0x400)
        at = pc
        while at in window:
            if at in self.insns and at != pc:
                break
            self.insns[at] = window[at]
            at = window[at][0]
        self.resynced += 1
        return pc in self.insns

    def call_target_import(self, insn):
        """Name of the import a CALL reaches through the PLT or the GOT, or None."""
        nxt, op, args = insn
        if op != 'call':
            return None
        try:
            return self.plt_imports.get(int(args, 0))
        except ValueError:
            pass
        m = re.fullmatch(r'qword ptr \[rip ([+-]) (0x[0-9a-f]+)\]', args)
        if m:
            return self.slot_imports.get(nxt + (1 if m[1] == '+' else -1) * int(m[2], 16))
        return None

    def is_code(self, offset):
        return any(lo <= offset < hi for lo, hi in self.layout['code'])

    def previous(self, pc, count):
        i = self.position[pc]
        return [(p, self.insns[p]) for p in reversed(self.order[max(0, i - count):i])]

    def jump_table(self, pc, reg):
        """Targets of `lea T, [rip + table]; movsxd R, dword ptr [T + 4*I];
        add R, T; jmp R` (clang/GCC PIC switch), bounded by `cmp I, N; ja`."""
        if self.image is None:
            return None
        before = self.previous(pc, 40)
        # The add that forms the target, possibly followed by a few instructions
        # that leave its register alone (GCC: `add rcx, rdx; xor ebx, ebx; jmp rcx`).
        at = None
        for k, (_, (_, op, args)) in enumerate(before[:4]):
            parts = [x.strip() for x in args.split(',')]
            if op == 'add' and len(parts) == 2 and GPR.get(parts[0], (None,))[0] == GPR[reg][0]:
                at = k
                break
            if op in ('ret', 'jmp', 'call') or any(GPR.get(x, (None,))[0] == GPR[reg][0] for x in parts[:1]):
                return None   # control leaves, or the register is written: not this pattern
        if at is None:
            return None
        table = load = index = base_reg = None
        added = [x.strip() for x in before[at][1][2].split(',')][1]
        # Unoptimised form (P2CE's engine.so, round 23): `lea rdx, [4*rax]; lea rax,
        # [rip + T]; mov eax, dword ptr [rdx + rax]; cdqe; lea rdx, [rip + T];
        # add rax, rdx; jmp rax`: the table is the rip-relative lea of the added
        # register, the load a 32-bit read of a sum of two registers.
        o0 = None
        for p, (nxt, op, args) in before[at + 1:at + 8]:
            m = re.fullmatch(r'(\w+), \[rip ([+-]) (0x[0-9a-f]+)\]', args)
            if op == 'lea' and m and o0 is None and m[1] == added:
                o0 = nxt + (1 if m[2] == '+' else -1) * int(m[3], 16)
                continue
            if o0 is not None and op == 'mov' and re.fullmatch(r'\w+, dword ptr \[\w+ \+ \w+\]', args):
                table, load = o0, p
                break
            if op in ('ret', 'jmp', 'call'):
                break
        for p, (nxt, op, args) in ([] if table is not None else before[at + 1:]):
            if op in ('ret', 'jmp', 'call') and load is None:
                return None
            m = re.fullmatch(r'(\w+), dword ptr \[(\w+) \+ 4\*(\w+)\]', args)
            if op == 'movsxd' and m and load is None:
                load, base_reg, index = p, m[2], m[3]
                continue
            m = re.fullmatch(r'(\w+), \[rip ([+-]) (0x[0-9a-f]+)\]', args)
            if op == 'lea' and m and load is not None and GPR.get(m[1], (None,))[0] == GPR[base_reg][0]:
                table = nxt + (1 if m[2] == '+' else -1) * int(m[3], 16)
                break
        if table is None:
            return None
        # Bound: the nearest `cmp X, N; ja/jae` before the load (the index
        # may have been copied to another register after the compare).
        count = None
        for p, (nxt, op, args) in self.previous(load, 12):
            if op in ('ja', 'jae'):
                for q, (_, op2, args2) in self.previous(p, 3):
                    m = re.fullmatch(r'(.+), (0x[0-9a-f]+)', args2)
                    if op2 == 'cmp' and m:
                        count = int(m[2], 16) + (1 if op == 'ja' else 0)
                        break
                break
            if op in ('ret', 'jmp', 'call'):
                break
        if count is not None and not 0 < count <= 4096:
            return None
        targets = []
        for k in range(count if count is not None else 256):
            if table + 4 * k + 4 > len(self.image):
                break
            delta, = struct.unpack_from('<i', self.image, table + 4 * k)
            target = table + delta
            if target not in self.insns or (count is None and abs(target - pc) > 0x10000):
                if count is not None:
                    return None
                break
            targets.append(target)
        if not targets:
            return None
        self.unbounded_tables += count is None
        return targets


SSE_SCALAR = {'addsd': ('g', '+'), 'subsd': ('g', '-'), 'mulsd': ('g', '*'), 'divsd': ('g', '/'),
              'addss': ('f', '+'), 'subss': ('f', '-'), 'mulss': ('f', '*'), 'divss': ('f', '/')}
SSE_PACKED = {'addpd': ('g', 2, '+'), 'subpd': ('g', 2, '-'), 'mulpd': ('g', 2, '*'), 'divpd': ('g', 2, '/'),
              'addps': ('f', 4, '+'), 'subps': ('f', 4, '-'), 'mulps': ('f', 4, '*'), 'divps': ('f', 4, '/'),
              'paddq': ('q', 2, '+'), 'psubq': ('q', 2, '-'), 'paddd': ('d', 4, '+'), 'psubd': ('d', 4, '-'),
              'paddb': ('b', 16, '+'), 'psubb': ('b', 16, '-'), 'paddw': ('w', 8, '+'), 'psubw': ('w', 8, '-')}
SSE_BITWISE = {'xorps': '^', 'xorpd': '^', 'pxor': '^', 'andps': '&', 'andpd': '&', 'pand': '&',
               'orps': '|', 'orpd': '|', 'por': '|', 'andnps': '&~', 'andnpd': '&~', 'pandn': '&~'}
SSE_MOVES = ('movaps', 'movups', 'movdqa', 'movdqu', 'movapd', 'movupd', 'lddqu')
STRING = re.compile(r'(movs|stos|lods|scas|cmps)([bwdq])')


def xmm_value(op):
    """128-bit source: register or xmmword memory."""
    if op.xmm is not None:
        return f'c->xmm[{op.xmm}]'
    if op.addr is not None and op.width in (128, None):
        return f'RX({op.addr})'
    raise Unsupported(f'128-bit operand {op.text!r}')


def scalar(op, lane):
    """Scalar float (lane 'f') or double (lane 'g') source."""
    if op.xmm is not None:
        return f'c->xmm[{op.xmm}].{lane}[0]'
    if op.addr is not None:
        return f'rt64_rd({op.addr})' if lane == 'g' else f'rt64_rf({op.addr})'
    raise Unsupported(f'scalar operand {op.text!r}')


def translate_sse(op, ops, here):
    if op in SSE_MOVES and len(ops) == 2:
        dst, src = ops
        if dst.xmm is not None:
            return f'c->xmm[{dst.xmm}] = {xmm_value(src)};'
        if dst.addr is not None and src.xmm is not None:
            return f'WX({dst.addr}, c->xmm[{src.xmm}]);'
    if op in ('movsd', 'movss') and len(ops) == 2:
        lane, n = ('g', 64) if op == 'movsd' else ('f', 32)
        dst, src = ops
        if dst.xmm is not None and src.xmm is not None:
            return f'c->xmm[{dst.xmm}].{lane}[0] = c->xmm[{src.xmm}].{lane}[0];'
        if dst.xmm is not None and src.addr is not None:
            return f'{{ XMM64 v = {{0}}; v.q[0] = R{n}({src.addr}); c->xmm[{dst.xmm}] = v; }}'
        if dst.addr is not None and src.xmm is not None:
            return f'W{n}({dst.addr}, c->xmm[{src.xmm}].{"q" if n == 64 else "d"}[0]);'
    if op in ('movq', 'movd') and len(ops) == 2:
        n = 64 if op == 'movq' else 32
        dst, src = ops
        if dst.xmm is not None:
            value = f'c->xmm[{src.xmm}].q[0]' if src.xmm is not None else src.read()
            mask = '' if n == 64 else ' & 0xffffffffull'
            return f'{{ XMM64 v = {{0}}; v.q[0] = ({value}){mask}; c->xmm[{dst.xmm}] = v; }}'
        if src.xmm is not None:
            return dst.write(f'c->xmm[{src.xmm}].{"q" if n == 64 else "d"}[0]')
    if op in ('movhps', 'movhpd', 'movlps', 'movlpd') and len(ops) == 2:
        half = 1 if op[3] == 'h' else 0
        dst, src = ops
        if dst.xmm is not None and src.addr is not None:
            return f'c->xmm[{dst.xmm}].q[{half}] = R64({src.addr});'
        if dst.addr is not None and src.xmm is not None:
            return f'W64({dst.addr}, c->xmm[{src.xmm}].q[{half}]);'
    if op == 'movhlps' and len(ops) == 2 and ops[1].xmm is not None:
        return f'c->xmm[{ops[0].xmm}].q[0] = c->xmm[{ops[1].xmm}].q[1];'
    if op == 'movlhps' and len(ops) == 2 and ops[1].xmm is not None:
        return f'c->xmm[{ops[0].xmm}].q[1] = c->xmm[{ops[1].xmm}].q[0];'
    if op in SSE_BITWISE and len(ops) == 2 and ops[0].xmm is not None:
        d, s = ops[0].xmm, xmm_value(ops[1])
        operator = SSE_BITWISE[op]
        if operator == '&~':
            return (f'{{ XMM64 s = {s}; c->xmm[{d}].q[0] = ~c->xmm[{d}].q[0] & s.q[0]; '
                    f'c->xmm[{d}].q[1] = ~c->xmm[{d}].q[1] & s.q[1]; }}')
        return (f'{{ XMM64 s = {s}; c->xmm[{d}].q[0] {operator}= s.q[0]; c->xmm[{d}].q[1] {operator}= s.q[1]; }}')
    if op in SSE_SCALAR and len(ops) == 2 and ops[0].xmm is not None:
        lane, operator = SSE_SCALAR[op]
        return f'c->xmm[{ops[0].xmm}].{lane}[0] = c->xmm[{ops[0].xmm}].{lane}[0] {operator} {scalar(ops[1], lane)};'
    if op in ('minsd', 'maxsd', 'minss', 'maxss') and len(ops) == 2 and ops[0].xmm is not None:
        lane = 'g' if op.endswith('d') else 'f'
        compare = '<' if op.startswith('min') else '>'
        # x86: the second operand when either is NaN or both are zero.
        return (f'{{ __typeof__(c->xmm[0].{lane}[0]) a_ = c->xmm[{ops[0].xmm}].{lane}[0], b_ = {scalar(ops[1], lane)}; '
                f'c->xmm[{ops[0].xmm}].{lane}[0] = a_ {compare} b_ ? a_ : b_; }}')
    if op in ('sqrtsd', 'sqrtss') and len(ops) == 2 and ops[0].xmm is not None:
        lane = 'g' if op == 'sqrtsd' else 'f'
        fn = 'sqrt' if lane == 'g' else 'sqrtf'
        return f'c->xmm[{ops[0].xmm}].{lane}[0] = __builtin_{fn}({scalar(ops[1], lane)});'
    if op in SSE_PACKED and len(ops) == 2 and ops[0].xmm is not None:
        lane, count, operator = SSE_PACKED[op]
        return (f'{{ XMM64 s = {xmm_value(ops[1])}; for (int i = 0; i < {count}; ++i) '
                f'c->xmm[{ops[0].xmm}].{lane}[i] = c->xmm[{ops[0].xmm}].{lane}[i] {operator} s.{lane}[i]; }}')
    if op in ('cvtsi2sd', 'cvtsi2ss') and len(ops) == 2 and ops[0].xmm is not None:
        src = ops[1]
        n = src.width
        if n not in (32, 64):
            raise Unsupported('cvtsi2s* width')
        lane, kind = ('g', 'double') if op == 'cvtsi2sd' else ('f', 'float')
        return f'c->xmm[{ops[0].xmm}].{lane}[0] = ({kind})(int64_t)sx64({src.read()}, {n});'
    if op in ('cvttsd2si', 'cvttss2si', 'cvtsd2si', 'cvtss2si') and len(ops) == 2 and ops[0].reg:
        lane = 'g' if 'sd' in op else 'f'
        value = f'(double){scalar(ops[1], lane)}'
        if not op.startswith('cvtt'):
            value = f'__builtin_nearbyint({value})'
        return ops[0].write(f'rt64_cvtt({value}, {ops[0].width})')
    if op == 'cvtss2sd' and len(ops) == 2 and ops[0].xmm is not None:
        return f'c->xmm[{ops[0].xmm}].g[0] = (double){scalar(ops[1], "f")};'
    if op == 'cvtsd2ss' and len(ops) == 2 and ops[0].xmm is not None:
        return f'c->xmm[{ops[0].xmm}].f[0] = (float){scalar(ops[1], "g")};'
    if op in ('ucomisd', 'comisd', 'ucomiss', 'comiss') and len(ops) == 2 and ops[0].xmm is not None:
        lane = 'g' if op.endswith('d') else 'f'
        return f'f64_comi(&f, c->xmm[{ops[0].xmm}].{lane}[0], {scalar(ops[1], lane)});'
    if op in ('punpcklqdq', 'unpcklpd') and len(ops) == 2 and ops[0].xmm is not None:
        return f'{{ XMM64 s = {xmm_value(ops[1])}; c->xmm[{ops[0].xmm}].q[1] = s.q[0]; }}'
    if op in ('punpckhqdq', 'unpckhpd') and len(ops) == 2 and ops[0].xmm is not None:
        d = ops[0].xmm
        return f'{{ XMM64 s = {xmm_value(ops[1])}; c->xmm[{d}].q[0] = c->xmm[{d}].q[1]; c->xmm[{d}].q[1] = s.q[1]; }}'
    if op in ('unpcklps', 'punpckldq') and len(ops) == 2 and ops[0].xmm is not None:
        d = ops[0].xmm
        return (f'{{ XMM64 s = {xmm_value(ops[1])}, t = c->xmm[{d}]; c->xmm[{d}].d[0] = t.d[0]; '
                f'c->xmm[{d}].d[1] = s.d[0]; c->xmm[{d}].d[2] = t.d[1]; c->xmm[{d}].d[3] = s.d[1]; }}')
    if op == 'pshufd' and len(ops) == 3 and ops[0].xmm is not None and ops[2].imm is not None:
        k = ops[2].imm
        lanes = ' '.join(f'c->xmm[{ops[0].xmm}].d[{i}] = s.d[{(k >> 2 * i) & 3}];' for i in range(4))
        return f'{{ XMM64 s = {xmm_value(ops[1])}; {lanes} }}'
    if op in ('shufps', 'shufpd') and len(ops) == 3 and ops[0].xmm is not None and ops[2].imm is not None:
        k, d = ops[2].imm, ops[0].xmm
        if op == 'shufps':
            lanes = (f'c->xmm[{d}].d[0] = t.d[{k & 3}]; c->xmm[{d}].d[1] = t.d[{(k >> 2) & 3}]; '
                     f'c->xmm[{d}].d[2] = s.d[{(k >> 4) & 3}]; c->xmm[{d}].d[3] = s.d[{(k >> 6) & 3}];')
        else:
            lanes = f'c->xmm[{d}].q[0] = t.q[{k & 1}]; c->xmm[{d}].q[1] = s.q[{(k >> 1) & 1}];'
        return f'{{ XMM64 s = {xmm_value(ops[1])}, t = c->xmm[{d}]; {lanes} }}'
    if op in ('pcmpeqb', 'pcmpeqd', 'pcmpeqw', 'pcmpeqq') and len(ops) == 2 and ops[0].xmm is not None:
        lane, count = {'b': ('b', 16), 'w': ('w', 8), 'd': ('d', 4), 'q': ('q', 2)}[op[-1]]
        return (f'{{ XMM64 s = {xmm_value(ops[1])}; for (int i = 0; i < {count}; ++i) '
                f'c->xmm[{ops[0].xmm}].{lane}[i] = c->xmm[{ops[0].xmm}].{lane}[i] == s.{lane}[i] ? '
                f'(__typeof__(s.{lane}[0]))~0ull : 0; }}')
    if op in ('psrlq', 'psllq', 'psrld', 'pslld') and len(ops) == 2 and ops[1].imm is not None:
        lane, count, bits = ('q', 2, 64) if op.endswith('q') else ('d', 4, 32)
        k = ops[1].imm
        direction = '>>' if op.startswith('psr') else '<<'
        body = f'c->xmm[{ops[0].xmm}].{lane}[i] {direction}= {k}' if k < bits else f'c->xmm[{ops[0].xmm}].{lane}[i] = 0'
        return f'for (int i = 0; i < {count}; ++i) {body};'
    if op in ('psrldq', 'pslldq') and len(ops) == 2 and ops[1].imm is not None:
        k, d = min(ops[1].imm, 16), ops[0].xmm
        if op == 'psrldq':
            return f'{{ XMM64 t = c->xmm[{d}], v = {{0}}; for (int i = 0; i + {k} < 16; ++i) v.b[i] = t.b[i + {k}]; c->xmm[{d}] = v; }}'
        return f'{{ XMM64 t = c->xmm[{d}], v = {{0}}; for (int i = {k}; i < 16; ++i) v.b[i] = t.b[i - {k}]; c->xmm[{d}] = v; }}'
    if op == 'movmskpd' and len(ops) == 2 and ops[1].xmm is not None:
        s = ops[1].xmm
        return ops[0].write(f'(c->xmm[{s}].q[0] >> 63) | ((c->xmm[{s}].q[1] >> 63) << 1)')
    if op == 'movmskps' and len(ops) == 2 and ops[1].xmm is not None:
        s = ops[1].xmm
        return ops[0].write(' | '.join(f'((uint64_t)(c->xmm[{s}].d[{i}] >> 31) << {i})' for i in range(4)))
    if op == 'pmovmskb' and len(ops) == 2 and ops[1].xmm is not None:
        s = ops[1].xmm
        return f'{{ uint64_t m_ = 0; for (int i = 0; i < 16; ++i) m_ |= (uint64_t)(c->xmm[{s}].b[i] >> 7) << i; {ops[0].write("m_")} }}'
    return None


def translate(pc, insn, mod, entry, callees):
    """C for one instruction: (code, local successors). Records direct and
    statically known call targets in `callees`."""
    nxt, op, args = insn
    end = f'goto L{nxt:x};'
    here, after = code(pc), code(nxt)
    if op in ('nop', 'endbr64', 'endbr32') or op.startswith('prefetch'):
        return end, [nxt]
    if op == 'pause':
        return '__atomic_signal_fence(__ATOMIC_SEQ_CST); ' + end, [nxt]
    if op in ('lfence', 'sfence', 'mfence'):
        return '__atomic_thread_fence(__ATOMIC_SEQ_CST); ' + end, [nxt]
    if op in ('int3', 'ud2', 'hlt'):
        return f'rt64_trap(c, {here}, "{op}");', []
    if op in PRIVILEGED or re.fullmatch(r'[0-9a-f]{2}|\(bad\)', op) or (op.startswith('lock_') and op[5:] not in
                                                                         ('add', 'adc', 'and', 'btc', 'btr', 'bts', 'cmpxchg', 'cmpxchg8b', 'cmpxchg16b', 'dec', 'inc', 'neg', 'not', 'or', 'sbb', 'sub', 'xadd', 'xchg', 'xor')) \
            or (op == 'lock' and args) or (op in ('int', 'into')) or 'fword ptr' in args or \
            (op in ('call', 'jmp') and args.startswith('[')):
        # Privileged, I/O or invalid in user mode: Linux would kill the
        # program with SIGSEGV/SIGILL. Mostly data decoded as code.
        return f'rt64_trap(c, {here}, "{op}: privileged or invalid instruction");', []
    extension = simd64.gated(op)
    if extension:
        # rt64_cpuid() does not report this extension: code that checks CPUID
        # first never gets here; anything else stops with a clear message.
        return f'rt64_trap(c, {here}, "{op}: {extension} not reported by CPUID");', []
    if op in ('incsspq', 'incsspd', 'rdsspq', 'rdsspd', 'saveprevssp', 'setssbsy', 'clrssbsy', 'rstorssp',
              'notrack'):
        return end, [nxt]       # CET shadow stack: no-ops while it is off, as on Linux by default
    if op == 'syscall':
        # A direct Linux system call: rax and the arguments in rdi, rsi, rdx, r10, r8, r9.
        return f'rt64_syscall(c); {end}', [nxt]
    if op in ('rep', 'repe', 'repz', 'repne', 'repnz') and args in ('ret', ''):
        if args == 'ret':
            op, args = 'ret', ''
    repeat = None
    if op in ('rep', 'repe', 'repz', 'repne', 'repnz'):
        repeat = {'repz': 'repe', 'repnz': 'repne'}.get(op, op)
        fields = args.split(None, 1)
        op, args = fields[0], fields[1] if len(fields) > 1 else ''
    m = STRING.fullmatch(op)
    if m and 'xmm' not in args:
        return string_op(m[1], {'b': 8, 'w': 16, 'd': 32, 'q': 64}[m[2]], repeat, end), [nxt]
    if repeat == 'rep' and op in ('bsf', 'tzcnt'):
        op = 'tzcnt'
    elif repeat:
        raise Unsupported(f'{repeat} {op}')

    if op == 'ret':
        adjust = int(args, 0) if args else 0
        return f'{SAVE_F} c->rip = POP64(c); c->r[RSP] += {adjust}u; return;', []
    if op in ('call', 'jmp') or (op.startswith('j') and op[1:] in CC):
        target = Operand(args, nxt)
        if target.imm is not None:
            t = target.imm
            if op == 'call':
                if t == nxt:
                    return f'PUSH64(c, {after}); {end}', [nxt]
                callees.add(t)
                return (f'PUSH64(c, {after}); {SAVE_F} fn_{t:x}(c); '
                        f'if (c->rip != {after}) goto resume; {LOAD_F} {end}'), [nxt]
            if t in mod.entries and t != entry:
                callees.add(t)
                branch, successors = f'{SAVE_F} fn_{t:x}(c); return;', []
            else:
                branch, successors = f'goto L{t:x};', [t]
            if op == 'jmp':
                return branch, successors
            return f'if (cond64(&f, CC_{CC[op[1:]]})) {{ {branch} }} {end}', successors + [nxt]
        if op not in ('call', 'jmp') or target.width not in (64, None) or target.imm is not None:
            raise Unsupported(f'{op} {args}')
        if target.addr is not None and target.addr.startswith('(B_ + '):
            slot = int(re.search(r'0x[0-9a-f]+', target.addr)[0], 16)
            known = mod.slots.get(slot)
            if known is not None and known in mod.insns:
                callees.add(known)
        if op == 'call':
            return (f'a = {target.read()}; PUSH64(c, {after}); {SAVE_F} rt64_dispatch(c, a); '
                    f'if (c->rip != {after}) goto resume; {LOAD_F} {end}'), [nxt]
        cases = mod.jump_table(pc, args) if target.reg else None
        if cases:
            mod.table_jumps += 1
            local = sorted(set(cases))
            labels = ' '.join(f'case {t:#x}: goto L{t:x};' for t in local)
            return (f'a = {target.read()}; switch (a - B_) {{ {labels} '
                    f'default: {SAVE_F} rt64_dispatch(c, a); return; }}'), local
        mod.dispatch_jumps += 1
        return f'a = {target.read()}; {SAVE_F} rt64_dispatch(c, a); return;', []
    if op in ('loop', 'loope', 'loopne', 'loopz', 'loopnz'):
        t = int(args, 0)
        test = {'loop': '', 'loope': ' && f.zf', 'loopz': ' && f.zf', 'loopne': ' && !f.zf', 'loopnz': ' && !f.zf'}[op]
        return f'if (--c->r[RCX] != 0{test}) goto L{t:x}; {end}', [t, nxt]
    if op in ('jrcxz', 'jecxz'):
        t = int(args, 0)
        test = 'c->r[RCX] == 0' if op == 'jrcxz' else '(uint32_t)c->r[RCX] == 0'
        return f'if ({test}) goto L{t:x}; {end}', [t, nxt]

    ops = [Operand(x, nxt) for x in args.split(',')] if args else []
    if op == 'lea' and len(ops) == 2 and ops[1].addr is not None and ops[1].addr.startswith('(B_ + '):
        # A code address taken with LEA (callbacks, thread entry points,
        # atexit destructors): lift the function there too.
        taken = int(re.search(r'0x[0-9a-f]+', ops[1].addr)[0], 16)
        if taken in mod.insns and getattr(mod, 'is_code', lambda _: True)(taken):
            callees.add(taken)
    sse = translate_sse(op, ops, here)
    if sse is None:
        sse = simd64.lower(op, ops)
    if sse is None:
        sse = x87_mmx(op, ops, here)
    if sse is not None:
        return sse + ' ' + end, [nxt]
    body = integer(op, ops, here)
    return body + ' ' + end, [nxt]


MMX_LANEWISE = frozenset("""paddb paddw paddd paddq psubb psubw psubd psubq paddsb paddsw paddusb paddusw psubsb
    psubsw psubusb psubusw pand pandn por pxor pcmpeqb pcmpeqw pcmpeqd pcmpgtb pcmpgtw pcmpgtd pmullw pmulhw pmulhuw
    pmaddwd pmuludq pavgb pavgw pminub pmaxub pminsw pmaxsw psadbw psllw pslld psllq psrlw psrld psrlq psraw psrad
    punpcklbw punpcklwd punpckldq pshufb pabsb pabsw pabsd psignb psignw psignd pmaddubsw pmulhrsw""".split())


MMX_SPECIAL = {
    'packssdw': 'for (int i = 0; i < 2; ++i) { r_.sw[i] = (int16_t)rt64_sat(d_.i[i], -32768, 32767); '
                'r_.sw[2 + i] = (int16_t)rt64_sat(s_.i[i], -32768, 32767); }',
    'packsswb': 'for (int i = 0; i < 4; ++i) { r_.sb[i] = (int8_t)rt64_sat(d_.sw[i], -128, 127); '
                'r_.sb[4 + i] = (int8_t)rt64_sat(s_.sw[i], -128, 127); }',
    'packuswb': 'for (int i = 0; i < 4; ++i) { r_.b[i] = (uint8_t)rt64_sat(d_.sw[i], 0, 255); '
                'r_.b[4 + i] = (uint8_t)rt64_sat(s_.sw[i], 0, 255); }',
    'punpckhbw': 'for (int i = 0; i < 4; ++i) { r_.b[2 * i] = d_.b[4 + i]; r_.b[2 * i + 1] = s_.b[4 + i]; }',
    'punpckhwd': 'for (int i = 0; i < 2; ++i) { r_.w[2 * i] = d_.w[2 + i]; r_.w[2 * i + 1] = s_.w[2 + i]; }',
    'punpckhdq': 'r_.d[0] = d_.d[1]; r_.d[1] = s_.d[1];',
    'pshufw': 'r_.w[0] = s_.w[{k0}]; r_.w[1] = s_.w[{k1}]; r_.w[2] = s_.w[{k2}]; r_.w[3] = s_.w[{k3}];',
}


def x87_mmx(op, ops, here):
    """x87 and MMX (tools/x87_64.py). A packed MMX instruction that works
    lane by lane runs as its SSE2 form on the scratch registers 16 and 17."""
    result = x87_64.lower(op, ops, here)
    if not isinstance(result, tuple):
        return result
    _, dst, source = result
    special = MMX_SPECIAL.get(op)
    if special is not None:
        if source is None and ops[1].addr is None:
            raise Unsupported(f'MMX {op} operand')
        value = f'c->mm[{source}]' if source is not None else f'R64({ops[1].addr})'
        k = ops[2].imm if len(ops) > 2 and ops[2].imm is not None else 0
        body = special.replace("{k0}", str(k & 3)).replace("{k1}", str(k >> 2 & 3)).replace("{k2}", str(k >> 4 & 3)).replace("{k3}", str(k >> 6 & 3))
        return (f'{{ XMM64 d_ = rt64_xq(c->mm[{dst}]), s_ = rt64_xq({value}), r_ = rt64_xq(0); (void)d_; {body} '
                f'c->mm[{dst}] = r_.q[0]; }}')
    if op not in MMX_LANEWISE:
        raise Unsupported(f'MMX {op}')
    load = f'c->xmm[17] = rt64_xq(c->mm[{source}]); ' if source is not None else ''
    if source is None and ops[1].imm is None:
        if ops[1].addr is None:
            raise Unsupported(f'MMX {op} operand')
        load = f'c->xmm[17] = rt64_xq(R64({ops[1].addr})); '
    second = Operand('xmm17', 0) if ops[1].imm is None else ops[1]
    inner = translate_sse(op, [Operand('xmm16', 0), second], here) or simd64.lower(op, [Operand('xmm16', 0), second])
    if inner is None:
        raise Unsupported(f'MMX {op}')
    return f'c->xmm[16] = rt64_xq(c->mm[{dst}]); {load}{inner} c->mm[{dst}] = c->xmm[16].q[0];'


def string_op(kind, n, repeat, end):
    size = n // 8
    acc = Operand({8: 'al', 16: 'ax', 32: 'eax', 64: 'rax'}[n], 0)
    step = f'(c->df ? (0ull - {size}) : {size}ull)'
    one = {
        'movs': f'W{n}(c->r[RDI], R{n}(c->r[RSI])); c->r[RSI] += {step}; c->r[RDI] += {step};',
        'stos': f'W{n}(c->r[RDI], c->r[RAX]); c->r[RDI] += {step};',
        'lods': acc.write(f'R{n}(c->r[RSI])') + f' c->r[RSI] += {step};',
        'scas': f'f64_sub(&f, c->r[RAX], R{n}(c->r[RDI]), {n}); c->r[RDI] += {step};',
        'cmps': f'f64_sub(&f, R{n}(c->r[RSI]), R{n}(c->r[RDI]), {n}); c->r[RSI] += {step}; c->r[RDI] += {step};',
    }[kind]
    if not repeat:
        return one + ' ' + end
    stop = ''
    if kind in ('scas', 'cmps'):
        stop = ' if (!f.zf) break;' if repeat == 'repe' else ' if (f.zf) break;'
    return f'while (c->r[RCX]) {{ {one} c->r[RCX]--;{stop} }} {end}'


ATOMIC_ALU = {'add': ('fetch_add', 'f64_add'), 'sub': ('fetch_sub', 'f64_sub'), 'and': ('fetch_and', '&'),
              'or': ('fetch_or', '|'), 'xor': ('fetch_xor', '^'), 'inc': ('fetch_add', 'f64_inc'),
              'dec': ('fetch_sub', 'f64_dec')}


def pointer(op):
    return f'(uint{op.width}_t *)(uintptr_t){op.addr}'


def integer(op, ops, here):
    n = ops[0].width if ops else None
    if op == 'mov' or op == 'movabs':
        dst, src = ops
        width = dst.width or src.width
        return dst.write(src.read(width))
    if op == 'movzx':
        return ops[0].write(ops[1].read())
    if op in ('movsx', 'movsxd'):
        dst, src = ops
        if not src.width:
            raise Unsupported('movsx width')
        return dst.write(f'sx64({src.read()}, {src.width})')
    if op == 'lea' and ops[1].addr is not None:
        return ops[0].write(ops[1].effective)
    if op == 'push' and len(ops) == 1:
        if ops[0].width == 16:
            raise Unsupported('16-bit push')
        return f'a = {ops[0].read(64) if ops[0].imm is None else hex(ops[0].imm & (2**64 - 1)) + "ull"}; PUSH64(c, a);'
    if op == 'pop' and len(ops) == 1 and ops[0].width in (64, None):
        return f'a = POP64(c); {ops[0].write("a")}'
    if op == 'leave':
        return 'c->r[RSP] = c->r[RBP]; c->r[RBP] = POP64(c);'
    if op in ('add', 'adc', 'sub', 'sbb', 'cmp', 'and', 'or', 'xor', 'test') and len(ops) == 2:
        dst, src = ops
        n = dst.width or src.width
        if op == 'xor' and dst.reg and dst.text == src.text:
            return f'f64_logic(&f, 0, {n}); ' + dst.write('0')
        code = f'a = {dst.read()}; b = {src.read(n)}; '
        if op in ('and', 'or', 'xor', 'test'):
            value = f'f64_logic(&f, a {dict(test="&", **{"and": "&", "or": "|", "xor": "^"})[op]} b, {n})'
        else:
            value = f'f64_{"sub" if op == "cmp" else op}(&f, a, b, {n})'
        code += f'r = {value};'
        if op not in ('cmp', 'test'):
            code += ' ' + dst.write('r')
        return code
    if op in ('shl', 'sal', 'shr', 'sar', 'rol', 'ror', 'rcl', 'rcr') and len(ops) in (1, 2):
        dst = ops[0]
        count = ops[1].read(8) if len(ops) == 2 else '1ull'
        if op in ('rol', 'ror', 'rcl', 'rcr'):
            kind = ('rol', 'ror', 'rcl', 'rcr').index(op)
            value = f'f64_rotate(&f, a, b, {n}, {kind})'
        else:
            value = f'f64_{"shl" if op == "sal" else op}(&f, a, b, {n})'
        return f'a = {dst.read()}; b = {count}; r = {value}; ' + dst.write('r')
    if op in ('not', 'neg', 'inc', 'dec') and len(ops) == 1:
        value = '~a' if op == 'not' else f'f64_{op}(&f, a, {n})'
        return f'a = {ops[0].read()}; r = {value}; ' + ops[0].write('r')
    if op == 'imul' and len(ops) in (2, 3):
        lhs, rhs = (ops[0], ops[1]) if len(ops) == 2 else (ops[1], ops[2])
        return f'a = {lhs.read()}; b = {rhs.read(n)}; r = f64_imul(&f, a, b, {n}); ' + ops[0].write('r')
    if op in ('mul', 'imul') and len(ops) == 1:
        src = ops[0].read()
        if n == 64:
            kind = 'unsigned __int128' if op == 'mul' else '__int128'
            cast = '' if op == 'mul' else '(int64_t)'
            check = 'c->r[RDX] != 0' if op == 'mul' else 'p_ != (__int128)(int64_t)c->r[RAX]'
            return (f'{{ {kind} p_ = ({kind}){cast}c->r[RAX] * ({kind}){cast}{src}; c->r[RAX] = (uint64_t)p_; '
                    f'c->r[RDX] = (uint64_t)((unsigned __int128)p_ >> 64); f.cf = f.of = {check}; }}')
        if op == 'mul':
            product = f'(uint64_t)(c->r[RAX] & {(1 << n) - 1:#x}ull) * {src}'
            fits = f'(p_ >> {n}) == 0'
        else:
            product = f'(uint64_t)((int64_t)sx64(c->r[RAX], {n}) * (int64_t)sx64({src}, {n}))'
            fits = f'p_ == (uint64_t)(int64_t)sx64(p_, {n})'
        if n == 8:
            store = Operand('ax', 0).write('p_')
        else:
            store = Operand(R32[0] if n == 32 else 'ax', 0).write('p_') + ' ' + \
                Operand(R32[2] if n == 32 else 'dx', 0).write(f'p_ >> {n}')
        return f'{{ uint64_t p_ = {product}; {store} f.cf = f.of = !({fits}); }}'
    if op in ('div', 'idiv') and len(ops) == 1:
        return f'rt64_div(c, {ops[0].read()}, {n}, {int(op == "idiv")}, {here});'
    if op == 'cdqe':
        return 'c->r[RAX] = sx64(c->r[RAX], 32);'
    if op == 'cwde':
        return 'c->r[RAX] = (uint32_t)sx64(c->r[RAX], 16);'
    if op == 'cbw':
        return Operand('ax', 0).write('sx64(c->r[RAX], 8)')
    if op == 'cqo':
        return 'c->r[RDX] = (int64_t)c->r[RAX] < 0 ? ~0ull : 0;'
    if op == 'cdq':
        return 'c->r[RDX] = (int32_t)c->r[RAX] < 0 ? 0xffffffffull : 0;'
    if op == 'cwd':
        return Operand('dx', 0).write('(c->r[RAX] & 0x8000) ? 0xffff : 0')
    if op.startswith('cmov') and op[4:] in CC and len(ops) == 2:
        dst, src = ops
        otherwise = f' else {{ {dst.write(dst.read())} }}' if dst.width == 32 else ''
        return f'a = {src.read()}; if (cond64(&f, CC_{CC[op[4:]]})) {{ {dst.write("a")} }}{otherwise}'
    if op.startswith('set') and op[3:] in CC and len(ops) == 1:
        return ops[0].write(f'cond64(&f, CC_{CC[op[3:]]})')
    if op in ('xchg', 'lock_xchg') and len(ops) == 2:
        dst, src = ops
        if dst.reg and src.reg:
            return f'a = {dst.read()}; b = {src.read()}; ' + dst.write('b') + ' ' + src.write('a')
        mem, reg = (dst, src) if dst.addr is not None else (src, dst)
        return f'a = __atomic_exchange_n({pointer(mem)}, ({reg.read()}), __ATOMIC_SEQ_CST); ' + reg.write('a')
    if op.startswith('lock_') and op[5:] in ATOMIC_ALU and ops[0].addr is not None:
        operation = op[5:]
        builtin, flags = ATOMIC_ALU[operation]
        value = '1ull' if operation in ('inc', 'dec') else ops[1].read(n)
        code = f'b = {value}; a = __atomic_{builtin}({pointer(ops[0])}, b, __ATOMIC_SEQ_CST); '
        if flags in '&|^':
            return code + f'f64_logic(&f, a {flags} b, {n});'
        if operation in ('inc', 'dec'):
            return code + f'{flags}(&f, a, {n});'
        return code + f'{flags}(&f, a, b, {n});'
    if op in ('xadd', 'lock_xadd') and len(ops) == 2:
        dst, src = ops
        if dst.addr is not None:
            return (f'b = {src.read()}; a = __atomic_fetch_add({pointer(dst)}, b, __ATOMIC_SEQ_CST); '
                    f'f64_add(&f, a, b, {n}); ' + src.write('a'))
        return f'a = {dst.read()}; b = {src.read()}; r = f64_add(&f, a, b, {n}); {src.write("a")} {dst.write("r")}'
    if op in ('cmpxchg', 'lock_cmpxchg') and len(ops) == 2 and ops[0].addr is not None:
        dst, src = ops
        acc = Operand({8: 'al', 16: 'ax', 32: 'eax', 64: 'rax'}[n], 0)
        return (f'{{ uint{n}_t expected_ = (uint{n}_t)c->r[RAX]; a = expected_; '
                f'int ok_ = __atomic_compare_exchange_n({pointer(dst)}, &expected_, (uint{n}_t){src.read()}, 0, '
                f'__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); f64_sub(&f, a, expected_, {n}); '
                f'if (!ok_) {{ {acc.write("expected_")} }} }}')
    if op in ('cmpxchg16b', 'lock_cmpxchg16b') and ops[0].addr is not None:
        return ('{ unsigned __int128 expected_ = ((unsigned __int128)c->r[RDX] << 64) | c->r[RAX]; '
                'unsigned __int128 desired_ = ((unsigned __int128)c->r[RCX] << 64) | c->r[RBX]; '
                f'f.zf = __atomic_compare_exchange_n((unsigned __int128 *)(uintptr_t){ops[0].addr}, &expected_, '
                'desired_, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); '
                'if (!f.zf) { c->r[RAX] = (uint64_t)expected_; c->r[RDX] = (uint64_t)(expected_ >> 64); } }')
    if op in ('bt', 'bts', 'btr', 'btc') and len(ops) == 2:
        base, index = ops
        update = {'bt': None, 'bts': 'a | m_', 'btr': 'a & ~m_', 'btc': 'a ^ m_'}[op]
        if base.addr is not None and index.reg:
            offset = f'(uint64_t)((int64_t)sx64({index.read()}, {index.width}) >> {6 if n == 64 else 5 if n == 32 else 4}) * {n // 8}u'
            store = f'W{n}(p_, {update});' if update else ''
            return (f'{{ uint64_t p_ = {base.addr} + {offset}; uint64_t m_ = 1ull << ({index.read()} & {n - 1}); '
                    f'a = R{n}(p_); f.cf = (a & m_) != 0; {store} }}')
        return (f'{{ uint64_t m_ = 1ull << ({index.read()} & {n - 1}); a = {base.read()}; f.cf = (a & m_) != 0; '
                f'{base.write(update) if update else ""} }}')
    if op in ('bsf', 'bsr', 'tzcnt', 'lzcnt') and len(ops) == 2:
        dst, src = ops
        if op == 'bsf':
            return f'a = {src.read()}; f.zf = a == 0; if (a) {{ {dst.write("__builtin_ctzll(a)")} }}'
        if op == 'bsr':
            return f'a = {src.read()}; f.zf = a == 0; if (a) {{ {dst.write("63 - __builtin_clzll(a)")} }}'
        if op == 'tzcnt':
            return f'a = {src.read()}; f.cf = a == 0; r = a ? (uint64_t)__builtin_ctzll(a) : {n}; f.zf = r == 0; ' + dst.write('r')
        return f'a = {src.read()}; f.cf = a == 0; r = a ? (uint64_t)__builtin_clzll(a) - {64 - n} : {n}; f.zf = r == 0; ' + dst.write('r')
    if op == 'bswap' and len(ops) == 1:
        return ops[0].write(f'__builtin_bswap{n}({ops[0].read()})')
    if op in ('shld', 'shrd') and len(ops) == 3:
        dst, src, count = ops
        value = f'(a << k_) | (b >> ({n} - k_))' if op == 'shld' else f'(a >> k_) | (b << ({n} - k_))'
        carry = f'(a >> ({n} - k_)) & 1' if op == 'shld' else '(a >> (k_ - 1)) & 1'
        return (f'{{ a = {dst.read()}; b = {src.read()}; unsigned k_ = (unsigned)({count.read(8)} & {63 if n == 64 else 31}); '
                f'if (k_) {{ f.cf = (uint32_t)({carry}); r = ({value}) & MSK64({n}); f.of = (uint32_t)(((a ^ r) >> ({n} - 1)) & 1); '
                f'szp64(&f, r, {n}); {dst.write("r")} }} }}')
    if op == 'popcnt' and len(ops) == 2:
        return (f'a = {ops[1].read()}; f.zf = a == 0; f.cf = f.of = f.sf = f.af = 0; f.pf = 1; '
                + ops[0].write('(uint64_t)__builtin_popcountll(a)'))
    if op in ('andn', 'bextr', 'bzhi', 'sarx', 'shlx', 'shrx', 'pdep', 'pext') and len(ops) == 3:
        dst, x, y = ops
        mask = f'MSK64({n})'
        if op == 'andn':
            return (f'r = ~{x.read()} & {y.read()} & {mask}; f64_logic(&f, r, {n}); ' + dst.write('r'))
        if op == 'bextr':
            return (f'{{ uint64_t s_ = {x.read()}, k_ = {y.read()}; unsigned start_ = k_ & 0xff, len_ = (k_ >> 8) & 0xff; '
                    f'r = start_ >= {n} ? 0 : s_ >> start_; if (len_ < 64) r &= (UINT64_C(1) << len_) - 1; '
                    f'f64_logic(&f, r, {n}); {dst.write("r")} }}')
        if op == 'bzhi':
            return (f'{{ uint64_t s_ = {x.read()}; unsigned k_ = (unsigned)({y.read()} & 0xff); '
                    f'r = k_ >= {n} ? s_ : s_ & ((UINT64_C(1) << k_) - 1); f64_logic(&f, r, {n}); f.cf = k_ > {n - 1}; '
                    f'{dst.write("r")} }}')
        if op in ('sarx', 'shlx', 'shrx'):
            k = f'({y.read()} & {n - 1})'
            value = {'shlx': f'({x.read()} << {k})', 'shrx': f'({x.read()} >> {k})',
                     'sarx': f'(uint64_t)((int64_t)sx64({x.read()}, {n}) >> {k})'}[op]
            return dst.write(f'{value} & {mask}')
        return dst.write(f'rt64_{op}({x.read()}, {y.read()}, {n})')
    if op in ('blsi', 'blsmsk', 'blsr') and len(ops) == 2:
        value = {'blsi': 'a & (0 - a)', 'blsmsk': 'a ^ (a - 1)', 'blsr': 'a & (a - 1)'}[op]
        return (f'a = {ops[1].read()}; r = ({value}) & MSK64({n}); f64_logic(&f, r, {n}); '
                f'f.cf = {"a == 0" if op == "blsr" or op == "blsmsk" else "a != 0"}; ' + ops[0].write('r'))
    if op == 'rorx' and len(ops) == 3:
        k = ops[2].imm & (n - 1)
        value = f'(({ops[1].read()} >> {k}) | ({ops[1].read()} << ({n} - {k}))) & MSK64({n})' if k else ops[1].read()
        return ops[0].write(value)
    if op == 'mulx' and len(ops) == 3:
        hi, lo, src = ops
        if n == 64:
            return (f'{{ unsigned __int128 p_ = (unsigned __int128)c->r[RDX] * {src.read()}; '
                    f'{lo.write("(uint64_t)p_")} {hi.write("(uint64_t)(p_ >> 64)")} }}')
        return (f'{{ uint64_t p_ = (uint64_t)(uint32_t)c->r[RDX] * {src.read()}; '
                f'{lo.write("p_")} {hi.write("p_ >> 32")} }}')
    if op == 'movbe' and len(ops) == 2:
        width = ops[0].width or ops[1].width
        swap = {16: '__builtin_bswap16', 32: '__builtin_bswap32', 64: '__builtin_bswap64'}[width]
        return ops[0].write(f'{swap}({ops[1].read()})')
    if op in ('cmpxchg', 'lock_cmpxchg') and len(ops) == 2 and ops[0].reg:
        dst, src = ops
        acc = Operand({8: 'al', 16: 'ax', 32: 'eax', 64: 'rax'}[n], 0)
        return (f'a = {acc.read()}; b = {dst.read()}; f64_sub(&f, a, b, {n}); '
                f'if (f.zf) {{ {dst.write(src.read())} }} else {{ {acc.write("b")} }}')
    if op == 'cpuid':
        return 'rt64_cpuid(c);'
    if op == 'rdtsc':
        return '{ uint64_t t_ = rt64_rdtsc(); c->r[RAX] = (uint32_t)t_; c->r[RDX] = t_ >> 32; }'
    if op in ('pushfq', 'pushf'):
        return 'PUSH64(c, rt64_get_rflags(c, &f));'
    if op in ('popfq', 'popf'):
        return 'a = POP64(c); rt64_set_rflags(c, &f, a);'
    if op == 'xlatb':
        return 'c->r[RAX] = (c->r[RAX] & ~0xffull) | R8(c->r[RBX] + (c->r[RAX] & 0xff));'
    if op in ('clc', 'stc', 'cmc'):
        return {'clc': 'f.cf = 0;', 'stc': 'f.cf = 1;', 'cmc': 'f.cf ^= 1;'}[op]
    if op == 'enter' and len(ops) == 2 and ops[1].imm == 0:
        return f'PUSH64(c, c->r[RBP]); c->r[RBP] = c->r[RSP]; c->r[RSP] -= {ops[0].imm & 0xffff}u;'
    if op in ('cld', 'std'):
        return f'c->df = {int(op == "std")};'
    if op == 'sahf':
        return ('a = c->r[RAX] >> 8; f.cf = a & 1; f.pf = (a >> 2) & 1 ? 0 : 1; f.af = (a >> 4) & 1; '
                'f.zf = (a >> 6) & 1; f.sf = (a >> 7) & 1;')
    if op == 'lahf':
        return ('c->r[RAX] = (c->r[RAX] & ~0xff00ull) | ((uint64_t)(f.cf | (parity64(f.pf) << 2) | (f.af << 4) | '
                '(f.zf << 6) | (f.sf << 7) | 2) << 8);')
    if op == 'stmxcsr' and len(ops) == 1:
        return ops[0].write('c->mxcsr')
    if op == 'ldmxcsr' and len(ops) == 1:
        return f'c->mxcsr = (uint32_t){ops[0].read()};'
    raise Unsupported(f'{op} {", ".join(o.text for o in ops)}'.strip())


def form(op, args):
    """Coverage key: mnemonic and operand kinds (r64, m32, imm, xmm...)."""
    kinds = []
    for text in [x.strip() for x in args.split(',')] if args else []:
        if text in GPR:
            kinds.append(f'r{GPR[text][1]}')
        elif text in XMM:
            kinds.append('xmm')
        elif '[' in text:
            m = _MEMORY.fullmatch(text)
            kinds.append(f'm{WIDTHS.get(m[1], "") if m and m[1] else ""}')
        elif re.fullmatch(r'-?(0x[0-9a-f]+|[0-9]+)', text):
            kinds.append('imm')
        else:
            kinds.append(text)
    return f'{op} {",".join(kinds)}'.strip()


CF, PF, AF, ZF, SF, OF = 1, 2, 4, 8, 16, 32
ALL_FLAGS = 63
CC_READS = dict(O=OF, NO=OF, B=CF, AE=CF, E=ZF, NE=ZF, BE=CF | ZF, A=CF | ZF, S=SF, NS=SF, P=PF, NP=PF,
                L=SF | OF, GE=SF | OF, LE=ZF | SF | OF, G=ZF | SF | OF)
_FULL_WRITERS = frozenset('add sub cmp neg and or xor test adc sbb xadd cmpxchg ucomisd comisd ucomiss comiss '
                          'scasb scasw scasd scasq cmpsb cmpsw cmpsd cmpsq popcnt ptest andn bextr bzhi blsi blsmsk '
                          'blsr fcomi fucomi fcomip fucomip fcompi fucompi'.split())
_FCMOV_READS = dict(fcmovb=1, fcmovnb=1, fcmove=8, fcmovne=8, fcmovbe=9, fcmovnbe=9, fcmovu=2, fcmovnu=2)
_SHIFTS = frozenset('shl sal shr sar'.split())


def _count(args):
    """Constant shift count of `x, imm` (or the implicit 1), else None."""
    parts = [p.strip() for p in args.split(',')] if args else []
    if len(parts) == 1:
        return 1
    try:
        return int(parts[-1], 0)
    except ValueError:
        return None


def flag_effect(op, args):
    """(flags read, flags always written) of one instruction, as CF..OF bits.
    An instruction that may leave a flag unchanged does not count as writing
    it; an instruction this table does not know is assumed to read them all
    when its C mentions the flags (conservative)."""
    if op.startswith('lock_'):
        op = op[5:]
    if op in ('rep', 'repe', 'repz', 'repne', 'repnz'):
        return 0, 0              # may run zero times: transparent
    if op.startswith('j') and op[1:] in CC:
        return CC_READS[CC[op[1:]]], 0
    if op.startswith('set') and op[3:] in CC:
        return CC_READS[CC[op[3:]]], 0
    if op.startswith('cmov') and op[4:] in CC:
        return CC_READS[CC[op[4:]]], 0
    if op in _FULL_WRITERS:
        return (CF if op in ('adc', 'sbb') else 0), ALL_FLAGS
    if op == 'imul':
        parts = args.split(',') if args else []
        return 0, ALL_FLAGS if len(parts) > 1 else CF | OF
    if op == 'mul':
        return 0, CF | OF
    if op in ('inc', 'dec'):
        return 0, ALL_FLAGS & ~CF
    if op in _SHIFTS or op in ('shld', 'shrd'):
        count = _count(args)
        return (0, CF | PF | ZF | SF | OF) if count and count & 63 else (0, 0)
    if op in ('rol', 'ror', 'rcl', 'rcr'):
        count = _count(args)
        reads = CF if op in ('rcl', 'rcr') else 0
        return reads, (CF if count and count & 63 else 0)
    if op in ('bt', 'bts', 'btr', 'btc'):
        return 0, CF
    if op in ('bsf', 'bsr'):
        return 0, ZF
    if op in ('tzcnt', 'lzcnt'):
        return 0, CF | ZF
    if op == 'cmpxchg16b':
        return 0, ZF
    if op == 'sahf':
        return 0, CF | PF | AF | ZF | SF
    if op in ('popfq', 'popf'):
        return 0, ALL_FLAGS
    if op in ('lahf', 'pushfq', 'pushf'):
        return CF | PF | AF | ZF | SF | OF, 0
    if op in ('clc', 'stc'):
        return 0, CF
    if op == 'cmc':
        return CF, CF
    if op in ('loope', 'loopz', 'loopne', 'loopnz'):
        return ZF, 0
    if op in _FCMOV_READS:
        return _FCMOV_READS[op], 0
    if op in ('call', 'ret', 'jmp', 'loop'):
        return 0, 0              # handled by the caller of flag_liveness
    return None                  # decided from the generated C


def flag_liveness(order, flow, insns, bodies):
    """Flags live on entry to each instruction (backward data flow). A call
    clobbers every flag (System V: not preserved), so nothing live after a
    call reaches back across it."""
    effect = {}
    for pc in order:
        insn = insns.get(pc)
        e = flag_effect(insn[1], insn[2]) if insn else (0, 0)
        if e is None:
            code = bodies[pc]
            e = (ALL_FLAGS, 0) if ('&f' in code or 'f.' in code) else (0, 0)
        if insn and insn[1] == 'call':
            e = (0, ALL_FLAGS)
        effect[pc] = e
    live_in = dict.fromkeys(order, 0)
    predecessors = {}
    for pc in order:
        for s in flow.get(pc, ()):
            predecessors.setdefault(s, []).append(pc)
    # Worklist: an instruction is revisited only when a successor changed.
    pending, queued = list(order), set(order)
    while pending:
        pc = pending.pop()
        queued.discard(pc)
        out = 0
        for s in flow.get(pc, ()):
            out |= live_in.get(s, 0)
        reads, writes = effect[pc]
        value = reads | (out & ~writes)
        if value != live_in[pc]:
            live_in[pc] = value
            for p in predecessors.get(pc, ()):
                if p not in queued:
                    queued.add(p)
                    pending.append(p)
    return live_in


def lift_function(entry, mod, stats=None, keep_flags=None):
    """C body of the function at `entry` and its callees."""
    pending, done, flow, callees = [entry], {}, {}, set()
    pad_table, pads = getattr(mod, 'landing_pads', {}), set()
    while pending:
        pc = pending.pop()
        if pc in done:
            continue
        insn = mod.insns.get(pc)
        if insn is None and hasattr(mod, 'resync') and mod.is_code(pc) and mod.resync(pc):
            insn = mod.insns.get(pc)
        if insn is None:
            done[pc] = f'rt64_trap(c, {code(pc)}, "no decoded instruction here");'
            if stats is not None:
                stats['missing'][f'(no instruction at {pc:#x})'] += 1
            continue
        try:
            body, successors = translate(pc, insn, mod, entry, callees)
        except (Unsupported, KeyError, IndexError, ValueError) as exc:
            text = f'{insn[1]} {insn[2]}'.strip().replace('\\', '\\\\').replace('"', "'")
            body, successors = f'rt64_trap(c, {code(pc)}, "not lifted: {text}");', []
            if stats is not None:
                stats['missing'][form(insn[1], insn[2])] += 1
                stats['unsupported_pcs'].add(pc)
                stats['why'].setdefault(form(insn[1], insn[2]), str(exc))
            # Keep exploring past it, for the coverage count only.
            if insn[1] not in ('jmp', 'ret') and stats is not None:
                pending.append(insn[0])
                stats['shadow'].add(insn[0])
        done[pc] = body
        flow[pc] = successors
        pending.extend(successors)
        if pc in pad_table:
            # Start of an FDE with landing pads (exception handlers, cleanups):
            # reached by the unwinder, they are resume points of this function.
            pads.update(pad_table[pc])
            pending.extend(pad_table[pc])
    if stats is not None:
        stats['pcs'].update(p for p in done if p in mod.insns)
    if keep_flags is None:
        keep_flags = bool(os.environ.get('P2_LIFT64_KEEP_FLAGS'))
    entry_live = 0
    if not keep_flags:
        order = sorted(done)
        live = flag_liveness(order, flow, mod.insns, done)
        entry_live = live[entry]
        for pc in order:
            insn = mod.insns.get(pc)
            if insn and insn[1] == 'call' and any(live.get(s, 0) for s in flow.get(pc, ())):
                if stats is not None:
                    stats['read_after_call'].append(pc)
                continue        # flags read after the call: keep the hand-over
            done[pc] = done[pc].replace(SAVE_F, '').replace(LOAD_F, '')
        if entry_live and stats is not None:
            stats['entry_live'].append(entry)
    init = 'c->flags' if keep_flags or entry_live else '(FL64){0, 0, 0, 0, 0, 0}'
    # A call that returned elsewhere (longjmp, a C++ exception reaching a
    # landing pad): continue here when the target is one of this function's
    # resume points, else leave this host frame too.
    find_import = getattr(mod, 'call_target_import', lambda insn: None)
    resume = sorted(mod.insns[pc][0] for pc in done if pc in mod.insns and find_import(mod.insns[pc]) in SETJMP)
    resume = [r for r in resume if r in done]
    resume += sorted(t for t in pads if t in done and t not in resume)
    lines = [f'void fn_{entry:x}(CPU64 *restrict c) {{',
             f'    FL64 f = {init}; uint64_t a, b, r; (void)f; (void)a; (void)b; (void)r;',
             f'    goto L{entry:x};']
    lines += [f'L{pc:x}: {body}' for pc, body in sorted(done.items())]
    if resume:
        cases = ' '.join(f'case {r:#x}: goto L{r:x};' for r in resume)
        lines.append(f'resume: f = c->flags; switch (c->rip - B_) {{ {cases} default: return; }}')
    else:
        lines.append('resume: return;')
    lines.append('}')
    return '\n'.join(lines), callees


def new_stats():
    return dict(missing=collections.Counter(), unsupported_pcs=set(), pcs=set(), why={}, shadow=set(),
                read_after_call=[], entry_live=[])


SETJMP = frozenset(('_setjmp', 'setjmp', '__sigsetjmp', 'sigsetjmp'))
IGNORED_WEAK = ('_ITM_', '_ZGTt', '__gmon_start__')


def relocation_rows(layout):
    """(offset, kind, index, addend) for runtime/x64/loader64.c."""
    rows = []
    for r in layout['relocations']:
        kind, addend = r['type'], r['addend']
        if kind == 'RELATIVE':
            rows.append((r['off'], 'RT64_REL_BASE', 0, addend))
        elif kind in ('64', 'GLOB_DAT', 'JUMP_SLOT'):
            extra = addend if kind == '64' else 0
            if 'value' in r:
                rows.append((r['off'], 'RT64_REL_BASE', 0, r['value'] + extra))
            elif r['symbol'].startswith(IGNORED_WEAK) and layout['imports'][r['import']]['weak']:
                rows.append((r['off'], 'RT64_REL_CONST', 0, 0))
            else:
                rows.append((r['off'], 'RT64_REL_IMPORT', r['import'], extra))
        elif kind == 'DTPMOD64':
            if 'import' in r:
                rows.append((r['off'], 'RT64_REL_IMPORT_TLS', r['import'], 0))
            else:
                rows.append((r['off'], 'RT64_REL_MODID', 0, 0))
        elif kind == 'DTPOFF64':
            if 'import' in r:
                rows.append((r['off'], 'RT64_REL_IMPORT_TLS', r['import'], 2))   # offset in its block
            else:
                rows.append((r['off'], 'RT64_REL_CONST', 0, r.get('value', 0) + addend))
        elif kind == 'TPOFF64':
            if 'import' in r:
                rows.append((r['off'], 'RT64_REL_IMPORT_TLS', r['import'], 1))
            else:
                rows.append((r['off'], 'RT64_REL_TPOFF', 0, r.get('value', 0) + addend))
    return rows


def lift_module(layout, roots, objdump='objdump', every=False, sweep=True):
    """Lift `roots` (module offsets) and everything they call directly, or
    every known entry point. Returns ({entry: body}, report)."""
    insns = disassemble(layout['source'], objdump)
    mod = Module(layout, insns)
    stats = new_stats()
    pending = sorted(mod.entries) if every else list(roots)
    bodies = {}
    while pending:
        entry = pending.pop()
        if entry in bodies:
            continue
        body, callees = lift_function(entry, mod, stats)
        bodies[entry] = body
        pending.extend(t for t in callees if t not in bodies)
    handled = stats['pcs'] - stats['unsupported_pcs']
    jumps = (mod.table_jumps, mod.unbounded_tables, mod.dispatch_jumps)
    # Linear sweep: every decoded instruction of the code sections, reached
    # or not (code only reached through exception tables, for example).
    sweep_count, sweep_missing = 0, collections.Counter()
    plt = layout.get('plt') or []
    for pc, insn in (insns.items() if sweep else ()):
        if insn[1] in ('int3', 'nop') or any(lo <= pc < hi for lo, hi in plt) or not mod.is_code(pc):
            continue
        sweep_count += 1
        try:
            translate(pc, insn, mod, None, set())
        except (Unsupported, KeyError, IndexError, ValueError):
            sweep_missing[form(insn[1], insn[2])] += 1
    report = dict(functions=len(bodies),
                  functions_complete=sum('not lifted:' not in b for b in bodies.values()),
                  instructions=len(stats['pcs']), handled=len(handled),
                  unsupported=len(stats['unsupported_pcs']),
                  sweep_instructions=sweep_count, sweep_unsupported=sum(sweep_missing.values()),
                  sweep_top_missing=sweep_missing.most_common(40),
                  table_jumps=jumps[0], unbounded_tables=jumps[1], dispatch_jumps=jumps[2],
                  flags_read_after_call=len(stats['read_after_call']),
                  flags_live_at_entry=len(stats['entry_live']),
                  top_missing=stats['missing'].most_common(40),
                  why={k: stats['why'].get(k, '') for k, _ in stats['missing'].most_common(40)})
    return bodies, report


_FN = re.compile(r'\bfn_([0-9a-f]+)\b')


def emit(layout, bodies, symbol, shards=1):
    """C files of a lifted module: {name: text}. One file when shards == 1;
    otherwise <symbol>.h (prototypes, the base address), <symbol>_NNN.c (the
    functions, grouped by address, about the same size each) and
    <symbol>_tables.c (dispatch table, relocations, imports, exports)."""
    def rename(text):
        return _FN.sub(lambda m: f'{symbol}_f{m[1]}', text)
    header = ['// Generated by tools/lift64.py: do not edit.', '#include "x64/rt64.h"', '#include <math.h>',
              '#include <arm_acle.h>',
              '#pragma clang diagnostic ignored "-Wunused-label"',
              f'extern uint64_t {symbol}_base;', f'#define B_ {symbol}_base']
    header += [f'void {symbol}_f{e:x}(CPU64 *);' for e in sorted(bodies)]
    rows = relocation_rows(layout)
    tables = [f'uint64_t {symbol}_base;', 'static const RT64Function functions_[] = {']
    tables += [f'    {{{e:#x}, {symbol}_f{e:x}}},' for e in sorted(bodies)] or ['    {0, 0},']
    tables.append('};')
    tables.append('static const RT64Reloc relocs_[] = {')
    tables += [f'    {{{o:#x}, {k}, {i}, {a}ll}},' for o, k, i, a in rows] or ['    {0, 0, 0, 0},']
    tables.append('};')
    tables.append('static const RT64Import imports_[] = {')
    tables += [f'    {{{json.dumps(i["name"])}, {int(i["weak"]) | 2 * int(i.get("object", False))}}},'
               for i in layout['imports']] or ['    {"", 0},']
    tables.append('};')
    def offsets(name, values):
        tables.append(f'static const uint64_t {name}[] = {{' + ', '.join(f'{v:#x}' for v in values or [0]) + '};')
    offsets('init_array_', layout.get('init_array'))
    offsets('fini_array_', layout.get('fini_array'))
    tables.append('static const char *const needed_[] = {' +
                  ', '.join(json.dumps(n) for n in (layout.get('needed') or [''])) + '};')
    tls = layout.get('tls') or dict(vaddr=0, filesz=0, memsz=0, align=0)
    tables.append('static const RT64Export exports_[] = {')
    tables += [f'    {{{json.dumps(n)}, {v:#x}}},' for n, v in layout['exports'].items()] or ['    {"", 0},']
    tables.append('};')
    tables.append(f'const RT64Module {symbol} = {{{json.dumps(layout["name"])}, {json.dumps(layout["image"] or "")}, '
                  f'{layout["size"]:#x}, &{symbol}_base, functions_, {len(bodies)}, relocs_, {len(rows)}, '
                  f'imports_, {len(layout["imports"])}, exports_, {len(layout["exports"])}, '
                  f'{layout.get("init", 0):#x}, {layout.get("fini", 0):#x}, '
                  f'init_array_, {len(layout.get("init_array") or [])}, fini_array_, {len(layout.get("fini_array") or [])}, '
                  f'{tls["vaddr"]:#x}, {tls["filesz"]:#x}, {tls["memsz"]:#x}, {tls["align"]:#x}, '
                  f'needed_, {len(layout.get("needed") or [])}, {layout.get("entry", 0):#x}}};')
    tables.append(f'__attribute__((constructor)) static void register_(void) {{ rt64_register(&{symbol}); }}')
    ordered = [rename(bodies[e]) for e in sorted(bodies)]
    if shards <= 1:
        return {f'{symbol}.c': '\n'.join(header + ordered + tables) + '\n'}
    files = {f'{symbol}.h': '#pragma once\n' + '\n'.join(header) + '\n'}
    include = f'#include "{symbol}.h"'
    target = sum(len(b) for b in ordered) / shards + 1
    group, size, index = [], 0, 0
    for body in ordered:
        group.append(body)
        size += len(body)
        if size >= target:
            files[f'{symbol}_{index:03}.c'] = include + '\n' + '\n'.join(group) + '\n'
            group, size, index = [], 0, index + 1
    if group:
        files[f'{symbol}_{index:03}.c'] = include + '\n' + '\n'.join(group) + '\n'
    files[f'{symbol}_tables.c'] = include + '\n' + '\n'.join(tables) + '\n'
    return files


def generate(layout, roots, symbol, objdump='objdump', every=False):
    """One C file for `roots` and their callees (or every entry point).
    Returns (source, report)."""
    bodies, report = lift_module(layout, roots, objdump, every)
    return emit(layout, bodies, symbol)[f'{symbol}.c'], report


def resolve_roots(layout, specs):
    roots = []
    for spec in specs:
        if spec in layout['exports']:
            roots.append(layout['exports'][spec])
        else:
            roots.append(int(spec, 0))
    return roots


def main():
    ap = argparse.ArgumentParser(description='x86-64 -> C translator (first slice)')
    ap.add_argument('--layout', required=True, help='<name>.layout.json from tools/elfprep64.py')
    ap.add_argument('--out', help='generated C file (one file)')
    ap.add_argument('--out-dir', help='directory for a split module (with --shards)')
    ap.add_argument('--shards', type=int, default=1, help='number of C files for the functions (--out-dir)')
    ap.add_argument('--symbol', default='rt64_module', help='name of the RT64Module in the generated C')
    ap.add_argument('--all', action='store_true', help='every known entry point, not only the roots')
    ap.add_argument('--report', help='coverage report (JSON)')
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('roots', nargs='*', help='exported names or module offsets')
    a = ap.parse_args()
    with open(a.layout) as f:
        layout = json.load(f)
    bodies, report = lift_module(layout, resolve_roots(layout, a.roots), a.objdump, a.all, sweep=bool(a.report))
    if a.out:
        with open(a.out, 'w') as f:
            f.write(emit(layout, bodies, a.symbol)[f'{a.symbol}.c'])
    if a.out_dir:
        os.makedirs(a.out_dir, exist_ok=True)
        for name, text in emit(layout, bodies, a.symbol, max(2, a.shards)).items():
            path = os.path.join(a.out_dir, name)
            # Unchanged files keep their date, so object caches stay valid.
            try:
                with open(path) as f:
                    if f.read() == text:
                        continue
            except OSError:
                pass
            with open(path, 'w') as f:
                f.write(text)
    if a.report:
        with open(a.report, 'w') as f:
            json.dump(report, f, indent=1)
    share = 100.0 * report['handled'] / max(1, report['instructions'])
    print(f'{layout["name"]}: {report["functions"]} functions ({report["functions_complete"]} complete), '
          f'{report["instructions"]} instructions, {report["handled"]} handled ({share:.2f} %), '
          f'{report["table_jumps"]} jump tables, {report["dispatch_jumps"]} indirect jumps', file=sys.stderr)
    if a.report:
        sweep_share = 100.0 * (report['sweep_instructions'] - report['sweep_unsupported']) / max(1, report['sweep_instructions'])
        print(f'  linear sweep of the code: {report["sweep_instructions"]} instructions, '
              f'{report["sweep_unsupported"]} not handled ({sweep_share:.2f} % handled)', file=sys.stderr)
    print(f'  flags read after a call: {report["flags_read_after_call"]} sites, '
          f'live at entry: {report["flags_live_at_entry"]} functions', file=sys.stderr)
    for name, count in report['top_missing'][:15]:
        print(f'  {count:6}  {name}', file=sys.stderr)


if __name__ == '__main__':
    main()
