"""x87 and MMX for tools/lift64.py.

x87: the eight-register floating-point stack, kept in doubles like the i386
lifter does (tools/float_lift.py): 80-bit memory operands are converted on
load and store (rt64_ld80/rt64_st80), so `long double` arithmetic runs at
double precision. ST(i) is c->st[(c->ftop + i) & 7]. The status word's
condition bits (C0, C2, C3) are kept for FNSTSW.

MMX: eight 64-bit registers (c->mm), separate from the x87 stack, which is
what code that runs EMMS before any x87 instruction expects.

lower(op, ops, here) returns C or None (not an x87/MMX instruction).
"""

CONST = {'fld1': '1.0', 'fldz': '0.0', 'fldpi': '3.14159265358979323846', 'fldl2e': '1.44269504088896340736',
         'fldln2': '0.693147180559945309417', 'fldl2t': '3.32192809488736234787', 'fldlg2': '0.301029995663981195214'}
ARITH = {'add': '{a} + {b}', 'sub': '{a} - {b}', 'subr': '{b} - {a}', 'mul': '{a} * {b}', 'div': '{a} / {b}',
         'divr': '{b} / {a}'}
FCMOV = {'fcmovb': 'f.cf', 'fcmove': 'f.zf', 'fcmovbe': 'f.cf || f.zf', 'fcmovu': '!parity64(f.pf)',
         'fcmovnb': '!f.cf', 'fcmovne': '!f.zf', 'fcmovnbe': '!(f.cf || f.zf)', 'fcmovnu': 'parity64(f.pf)'}


def st_index(text):
    """'st' -> 0, 'st(3)' -> 3, else None."""
    if text == 'st':
        return 0
    if text.startswith('st(') and text.endswith(')'):
        return int(text[3:-1])
    return None


def mem_value(op):
    """A memory operand read as a double (float, double, 80-bit)."""
    if op.width == 32:
        return f'(double)rt64_rf({op.addr})'
    if op.width == 64:
        return f'rt64_rd({op.addr})'
    if op.width == 80:
        return f'rt64_ld80({op.addr})'
    raise ValueError(f'x87 memory width {op.width}')


def int_value(op):
    return f'(double)(int64_t)sx64(R{op.width}({op.addr}), {op.width})'


def store(op, value):
    if op.width == 32:
        return f'{{ float v_ = (float)({value}); uint32_t b_; memcpy(&b_, &v_, 4); W32({op.addr}, b_); }}'
    if op.width == 64:
        return f'{{ double v_ = {value}; uint64_t b_; memcpy(&b_, &v_, 8); W64({op.addr}, b_); }}'
    if op.width == 80:
        return f'rt64_st80({op.addr}, {value});'
    raise ValueError(f'x87 store width {op.width}')


def compare_flags(a, b):
    # FCOMI/FUCOMI: ZF, PF, CF like COMISD.
    return f'f64_comi(&f, {a}, {b});'


def compare_status(a, b):
    # FCOM/FUCOM/FTST: C3 (bit 14), C2 (bit 10), C0 (bit 8) of the status word.
    return (f'{{ double a_ = {a}, b_ = {b}; c->fsw &= (uint16_t)~0x4500u; '
            'if (a_ != a_ || b_ != b_) c->fsw |= 0x4500u; else if (a_ < b_) c->fsw |= 0x0100u; '
            'else if (a_ == b_) c->fsw |= 0x4000u; }')


def lower(op, ops, here):
    n = len(ops)
    texts = [o.text for o in ops]
    sts = [st_index(t) for t in texts]
    pop = 'FPOP(c);'
    if op in CONST and n == 0:
        return f'FPUSH(c, {CONST[op]});'
    if op == 'fld' and n == 1:
        if sts[0] is not None:
            return f'{{ double v_ = ST({sts[0]}); FPUSH(c, v_); }}'
        return f'FPUSH(c, {mem_value(ops[0])});'
    if op == 'fild' and n == 1:
        return f'FPUSH(c, {int_value(ops[0])});'
    if op in ('fst', 'fstp') and n == 1:
        tail = f' {pop}' if op == 'fstp' else ''
        if sts[0] is not None:
            return f'ST({sts[0]}) = ST(0);{tail}'
        return store(ops[0], 'ST(0)') + tail
    if op in ('fist', 'fistp', 'fisttp') and n == 1:
        value = 'ST(0)' if op == 'fisttp' else 'rt64_x87_round(c, ST(0))'
        width = ops[0].width
        limit = {16: '32768.0', 32: '2147483648.0', 64: '9223372036854775808.0'}[width]
        tail = f' {pop}' if op != 'fist' else ''
        return (f'{{ double v_ = {value}; int64_t i_ = (v_ != v_ || v_ >= {limit} || v_ < -{limit}) ? '
                f'(int64_t)(UINT64_C(1) << ({width} - 1)) : (int64_t)v_; W{width}({ops[0].addr}, (uint64_t)i_); }}{tail}')
    if op == 'fxch':
        i = sts[-1] if n else 1
        return f'{{ double t_ = ST(0); ST(0) = ST({i}); ST({i}) = t_; }}'
    if op[0] == 'f' and (op[1:] in ARITH or (op.endswith('p') and op[1:-1] in ARITH)) and n <= 2:
        kind = op[1:-1] if op.endswith('p') and op[1:] not in ARITH else op[1:]
        popping = op.endswith('p') and op[1:] not in ARITH
        expr = ARITH[kind]
        if n == 1 and ops[0].addr is not None:
            return f'ST(0) = {expr.format(a="ST(0)", b=mem_value(ops[0]))};'
        if n == 0:
            return f'ST(1) = {expr.format(a="ST(1)", b="ST(0)")}; {pop}'
        dst, src_ = sts
        return f'ST({dst}) = {expr.format(a=f"ST({dst})", b=f"ST({src_})")};' + (f' {pop}' if popping else '')
    if op[:2] == 'fi' and op[2:] in ARITH and n == 1:
        return f'ST(0) = {ARITH[op[2:]].format(a="ST(0)", b=int_value(ops[0]))};'
    if op in ('fchs', 'fabs', 'fsqrt', 'frndint', 'fsin', 'fcos', 'f2xm1') and n == 0:
        value = {'fchs': '-ST(0)', 'fabs': '__builtin_fabs(ST(0))', 'fsqrt': '__builtin_sqrt(ST(0))',
                 'frndint': 'rt64_x87_round(c, ST(0))', 'fsin': 'sin(ST(0))', 'fcos': 'cos(ST(0))',
                 'f2xm1': 'exp2(ST(0)) - 1.0'}[op]
        return f'ST(0) = {value};'
    if op == 'fsincos' and n == 0:
        return '{ double v_ = ST(0); ST(0) = sin(v_); FPUSH(c, cos(v_)); }'
    if op == 'fptan' and n == 0:
        return 'ST(0) = tan(ST(0)); FPUSH(c, 1.0);'
    if op == 'fpatan' and n == 0:
        return f'ST(1) = atan2(ST(1), ST(0)); {pop}'
    if op == 'fyl2x' and n == 0:
        return f'ST(1) = ST(1) * log2(ST(0)); {pop}'
    if op == 'fyl2xp1' and n == 0:
        return f'ST(1) = ST(1) * log2(ST(0) + 1.0); {pop}'
    if op == 'fscale' and n == 0:
        return 'ST(0) = ldexp(ST(0), (int)__builtin_trunc(ST(1)));'
    if op in ('fprem', 'fprem1') and n == 0:
        fn = 'fmod' if op == 'fprem' else 'remainder'
        return f'ST(0) = {fn}(ST(0), ST(1)); c->fsw &= (uint16_t)~0x0400u;'
    if op == 'fxtract' and n == 0:
        return '{ int e_; double m_ = frexp(ST(0), &e_); ST(0) = (double)(e_ - 1); FPUSH(c, m_ * 2.0); }'
    if op in ('fcomi', 'fucomi', 'fcomip', 'fucomip', 'fcompi', 'fucompi') and n == 2:
        return compare_flags('ST(0)', f'ST({sts[1]})') + (f' {pop}' if 'p' in op[4:] else '')
    if op in ('fcom', 'fucom', 'fcomp', 'fucomp', 'fcompp', 'fucompp') and n <= 1:
        other = 'ST(1)' if n == 0 else (f'ST({sts[0]})' if sts[0] is not None else mem_value(ops[0]))
        pops = 2 if op.endswith('pp') else 1 if op.endswith('p') else 0
        return compare_status('ST(0)', other) + (' ' + ' '.join([pop] * pops) if pops else '')
    if op == 'ftst' and n == 0:
        return compare_status('ST(0)', '0.0')
    if op == 'fxam' and n == 0:
        return ('{ double v_ = ST(0); uint16_t s_ = (uint16_t)(c->fsw & ~0x4700u); if (__builtin_signbit(v_)) s_ |= 0x0200u; '
                'if (v_ != v_) s_ |= 0x0100u; else if (__builtin_isinf(v_)) s_ |= 0x0500u; else if (v_ == 0) s_ |= 0x4000u; '
                'else s_ |= 0x0400u; c->fsw = s_; }')
    if op in ('fnstsw', 'fstsw') and n == 1:
        return ops[0].write('(uint64_t)((c->fsw & ~0x3800u) | ((c->ftop & 7u) << 11))')
    if op in ('fnstcw', 'fstcw') and n == 1:
        return ops[0].write('c->fcw')
    if op == 'fldcw' and n == 1:
        return f'c->fcw = (uint16_t){ops[0].read()};'
    if op in ('fwait', 'wait', 'fnclex', 'fclex', 'ffree', 'fdecstp', 'fincstp', 'emms', 'femms') and n <= 1:
        return {'fdecstp': 'c->ftop = (c->ftop - 1) & 7;', 'fincstp': 'c->ftop = (c->ftop + 1) & 7;'}.get(op, '')
    if op in ('fninit', 'finit') and n == 0:
        return 'c->ftop = 0; c->fcw = 0x37f; c->fsw = 0;'
    if op in FCMOV and n == 2:
        return f'if ({FCMOV[op]}) ST(0) = ST({sts[1]});'
    if op in ('fnstenv', 'fldenv', 'fnsave', 'frstor', 'fxsave', 'fxrstor', 'fxsave64', 'fxrstor64') and n == 1:
        kind = {'fnstenv': 0, 'fldenv': 1, 'fnsave': 2, 'frstor': 3, 'fxsave': 4, 'fxsave64': 4,
                'fxrstor': 5, 'fxrstor64': 5}[op]
        return f'rt64_fpu_state(c, {ops[0].addr}, {kind});'
    # MMX
    mm = [int(t[2:]) if t.startswith('mm') and t[2:].isdigit() else None for t in texts]
    if any(m is not None for m in mm):
        if op == 'movntq' and n == 2 and ops[0].addr is not None:
            return f'W64({ops[0].addr}, c->mm[{mm[1]}]);'
        if op in ('movq', 'movd') and n == 2:
            bits = 64 if op == 'movq' else 32
            if mm[0] is not None:
                if mm[1] is not None:
                    value = f'c->mm[{mm[1]}]'
                elif ops[1].xmm is not None:
                    value = f'c->xmm[{ops[1].xmm}].q[0]'
                else:
                    value = ops[1].read() if ops[1].addr is None else f'R{bits}({ops[1].addr})'
                return f'c->mm[{mm[0]}] = (uint64_t)({value}){" & 0xffffffffull" if bits == 32 else ""};'
            value = f'c->mm[{mm[1]}]' + (' & 0xffffffffull' if bits == 32 else '')
            if ops[0].xmm is not None:
                return f'c->xmm[{ops[0].xmm}] = rt64_xq({value});'
            return ops[0].write(value) if ops[0].addr is None else f'W{bits}({ops[0].addr}, {value});'
        if op in ('movq2dq',) and n == 2:
            return f'c->xmm[{ops[0].xmm}] = rt64_xq(c->mm[{mm[1]}]);'
        if op in ('movdq2q',) and n == 2:
            return f'c->mm[{mm[0]}] = c->xmm[{ops[1].xmm}].q[0];'
        if n >= 2 and mm[0] is not None:
            # Packed MMX: run the SSE2 lowering on a temporary 128-bit register.
            return ('MMX', mm[0], mm[1])
    return None
