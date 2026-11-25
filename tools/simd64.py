"""SSE3, SSSE3, SSE4.1/4.2 and the rest of SSE/SSE2 for tools/lift64.py.

lower(op, ops) returns the C for one instruction over the CPU64's xmm
registers (runtime/x64/rt64.h), or None when it is not a SIMD instruction
this file knows. `ops` are lift64.Operand objects (.xmm, .addr, .width,
.reg, .imm, .read(), .write()). Packed operations work lane by lane on the
XMM64 union views: b/sb (8-bit unsigned/signed), w/sw, d/i, q/s, f, g.

Also the list of instruction families the lifter leaves to a trap because
rt64_cpuid() does not report them (AVX and later, AES-NI, PCLMULQDQ, SHA,
RDRAND...): code that picks them from CPUID never reaches them.
"""
import re

LANES = {'b': 16, 'sb': 16, 'w': 8, 'sw': 8, 'd': 4, 'i': 4, 'q': 2, 's': 2, 'f': 4, 'g': 2}
CMP_PREDICATES = {'eq': '{x} == {y}', 'lt': '{x} < {y}', 'le': '{x} <= {y}', 'unord': '{x} != {x} || {y} != {y}',
                  'neq': '!({x} == {y})', 'nlt': '!({x} < {y})', 'nle': '!({x} <= {y})',
                  'ord': '{x} == {x} && {y} == {y}'}


def gated(op):
    """Instructions of CPU extensions rt64_cpuid() does not report."""
    if op.startswith('v') and op not in ('verr', 'verw'):
        return 'AVX/AVX-512'
    if re.fullmatch(r'k(mov|andn?|x?n?or|not|shift[lr]|test|ortest|unpck|add)[bwdq]{1,2}', op):
        return 'AVX-512 mask registers'
    if op.startswith('aes') or op in ('pclmulqdq', 'gf2p8affineqb', 'gf2p8affineinvqb', 'gf2p8mulb'):
        return 'AES-NI/PCLMULQDQ/GFNI'
    if op.startswith('sha1') or op.startswith('sha256'):
        return 'SHA'
    if op in ('rdrand', 'rdseed', 'xgetbv', 'xsave', 'xsavec', 'xsaveopt', 'xrstor', 'xsave64', 'xrstor64',
              'xsavec64', 'xsaveopt64', 'adcx', 'adox', 'rdpid', 'rdpkru', 'wrpkru', 'xbegin', 'xend', 'xabort',
              'xtest', 'clflushopt', 'clwb', 'movdiri', 'movdir64b', 'serialize', 'tpause', 'umonitor', 'umwait',
              'rdtscp', 'pcmpistri', 'pcmpistrm', 'pcmpestri', 'pcmpestrm'):
        return op
    return None


def src(op):
    """A 128-bit source as an XMM64 value (memory of 128, 64, 32 or 16 bits)."""
    if op.xmm is not None:
        return f'c->xmm[{op.xmm}]'
    if op.addr is not None:
        width = op.width or 128
        if width == 128:
            return f'RX({op.addr})'
        return f'rt64_xq(R{width}({op.addr}))'
    raise ValueError(f'SIMD operand {op.text!r}')


def lanes(d, s, view, expr, out=None, count=None):
    """d.out[i] = expr(x = d.view[i], y = s.view[i]) for every lane."""
    out = out or view
    n = count or LANES[view]
    body = expr.format(x=f'd_.{view}[i]', y=f's_.{view}[i]')
    return (f'{{ XMM64 s_ = {s}, d_ = c->xmm[{d}], r_ = d_; for (int i = 0; i < {n}; ++i) '
            f'r_.{out}[i] = {body}; c->xmm[{d}] = r_; }}')


def ones(view):
    return {'b': 'UINT8_MAX', 'sb': 'UINT8_MAX', 'w': 'UINT16_MAX', 'sw': 'UINT16_MAX', 'd': 'UINT32_MAX',
            'i': 'UINT32_MAX', 'q': 'UINT64_MAX', 's': 'UINT64_MAX', 'f': 'UINT32_MAX', 'g': 'UINT64_MAX'}[view]


def unsigned_view(view):
    return {'f': 'd', 'g': 'q', 'sb': 'b', 'sw': 'w', 'i': 'd', 's': 'q'}.get(view, view)


SATURATING = {
    'paddsb': ('sb', 'rt64_sat(({x}) + ({y}), -128, 127)'), 'paddsw': ('sw', 'rt64_sat(({x}) + ({y}), -32768, 32767)'),
    'psubsb': ('sb', 'rt64_sat(({x}) - ({y}), -128, 127)'), 'psubsw': ('sw', 'rt64_sat(({x}) - ({y}), -32768, 32767)'),
    'paddusb': ('b', 'rt64_sat(({x}) + ({y}), 0, 255)'), 'paddusw': ('w', 'rt64_sat(({x}) + ({y}), 0, 65535)'),
    'psubusb': ('b', 'rt64_sat((int)({x}) - ({y}), 0, 255)'), 'psubusw': ('w', 'rt64_sat((int)({x}) - ({y}), 0, 65535)'),
}
SIMPLE = {
    'pmullw': ('w', '(uint16_t)({x} * {y})'), 'pmulhw': ('sw', '(int16_t)(((int32_t){x} * {y}) >> 16)'),
    'pmulhuw': ('w', '(uint16_t)(((uint32_t){x} * {y}) >> 16)'), 'pmulld': ('d', '{x} * {y}'),
    'pavgb': ('b', '(uint8_t)(({x} + {y} + 1) >> 1)'), 'pavgw': ('w', '(uint16_t)(({x} + {y} + 1) >> 1)'),
    'pminub': ('b', '{x} < {y} ? {x} : {y}'), 'pmaxub': ('b', '{x} > {y} ? {x} : {y}'),
    'pminsb': ('sb', '{x} < {y} ? {x} : {y}'), 'pmaxsb': ('sb', '{x} > {y} ? {x} : {y}'),
    'pminsw': ('sw', '{x} < {y} ? {x} : {y}'), 'pmaxsw': ('sw', '{x} > {y} ? {x} : {y}'),
    'pminuw': ('w', '{x} < {y} ? {x} : {y}'), 'pmaxuw': ('w', '{x} > {y} ? {x} : {y}'),
    'pminsd': ('i', '{x} < {y} ? {x} : {y}'), 'pmaxsd': ('i', '{x} > {y} ? {x} : {y}'),
    'pminud': ('d', '{x} < {y} ? {x} : {y}'), 'pmaxud': ('d', '{x} > {y} ? {x} : {y}'),
    'maxps': ('f', '{x} > {y} ? {x} : {y}'), 'minps': ('f', '{x} < {y} ? {x} : {y}'),
    'maxpd': ('g', '{x} > {y} ? {x} : {y}'), 'minpd': ('g', '{x} < {y} ? {x} : {y}'),
    'pabsb': ('sb', '(int8_t)({y} < 0 ? -{y} : {y})'), 'pabsw': ('sw', '(int16_t)({y} < 0 ? -{y} : {y})'),
    'pabsd': ('i', '(int32_t)({y} < 0 ? -(uint32_t){y} : (uint32_t){y})'),
    'psignb': ('sb', '(int8_t)({y} < 0 ? -{x} : {y} ? {x} : 0)'), 'psignw': ('sw', '(int16_t)({y} < 0 ? -{x} : {y} ? {x} : 0)'),
    'psignd': ('i', '(int32_t)({y} < 0 ? -(uint32_t){x} : {y} ? (uint32_t){x} : 0)'),
    'sqrtps': ('f', '__builtin_sqrtf({y})'), 'sqrtpd': ('g', '__builtin_sqrt({y})'),
    'rcpps': ('f', '1.0f / {y}'), 'rsqrtps': ('f', '1.0f / __builtin_sqrtf({y})'),
    'rcpss': ('f', '1.0f / {y}', 'f', 1), 'rsqrtss': ('f', '1.0f / __builtin_sqrtf({y})', 'f', 1),
    'pcmpgtb': ('sb', '{x} > {y} ? -1 : 0'), 'pcmpgtw': ('sw', '{x} > {y} ? -1 : 0'),
    'pcmpgtd': ('i', '{x} > {y} ? -1 : 0'), 'pcmpgtq': ('s', '{x} > {y} ? -1 : 0'),
    'cvtdq2ps': ('i', '{y}', 'f'), 'cvttps2dq': ('f', '(int32_t)rt64_cvtt({y}, 32)', 'i'),
    'cvtps2dq': ('f', '(int32_t)rt64_cvtt(__builtin_nearbyint({y}), 32)', 'i'),
}


def shuffle(d, s, assignments):
    """r_ = d (destination), then the given lane assignments from d_ and s_."""
    return f"{{ XMM64 s_ = {s}, d_ = c->xmm[{d}], r_ = d_; (void)d_; {chr(32).join(assignments)} c->xmm[{d}] = r_; }}"


def interleave(op, d, s):
    if op.startswith('punpck'):
        high, view = op[6] == 'h', {'bw': 'b', 'wd': 'w', 'dq': 'd', 'qdq': 'q'}[op[7:]]
    else:
        high, view = op.startswith('unpckh'), 'd' if op.endswith('ps') else 'q'
    n = LANES[view]
    base = n // 2 if high else 0
    return shuffle(d, s, [f'r_.{view}[{2 * i}] = d_.{view}[{base + i}]; r_.{view}[{2 * i + 1}] = s_.{view}[{base + i}];'
                          for i in range(n // 2)])


def pack(op, d, s):
    kind = {'packsswb': ('sw', 'b', -128, 127), 'packuswb': ('sw', 'b', 0, 255),
            'packssdw': ('i', 'w', -32768, 32767), 'packusdw': ('i', 'w', 0, 65535)}[op]
    view, out, lo, hi = kind
    n = LANES[view]
    parts = [f'r_.{out}[{i}] = rt64_sat(d_.{view}[{i}], {lo}, {hi});' for i in range(n)]
    parts += [f'r_.{out}[{n + i}] = rt64_sat(s_.{view}[{i}], {lo}, {hi});' for i in range(n)]
    return shuffle(d, s, parts)


def shift(op, d, count):
    kind, size = op[1:4], op[4]          # sll/srl/sra, w/d/q
    view = {'w': 'w', 'd': 'd', 'q': 'q'}[size]
    signed = {'w': 'sw', 'd': 'i', 'q': 's'}[size]
    bits = {'w': 16, 'd': 32, 'q': 64}[size]
    if kind == 'sll':
        expr = f'n_ >= {bits} ? 0 : d_.{view}[i] << n_'
    elif kind == 'srl':
        expr = f'n_ >= {bits} ? 0 : d_.{view}[i] >> n_'
    else:
        expr = f'd_.{signed}[i] >> (n_ >= {bits} ? {bits - 1} : n_)'
        view = signed
    return (f'{{ uint64_t n_ = {count}; XMM64 d_ = c->xmm[{d}]; for (int i = 0; i < {LANES[view]}; ++i) '
            f'd_.{view}[i] = {expr}; c->xmm[{d}] = d_; }}')


def lower(op, ops):
    n = len(ops)
    d = ops[0].xmm if n and ops[0].xmm is not None else None
    if op in SATURATING and n == 2 and d is not None:
        view, expr = SATURATING[op]
        return lanes(d, src(ops[1]), view, expr)
    if op in SIMPLE and n == 2 and d is not None:
        view, expr, *rest = SIMPLE[op]
        return lanes(d, src(ops[1]), view, expr, rest[0] if rest else None, rest[1] if len(rest) > 1 else None)
    if (op.startswith('punpck') or op in ('unpckhps', 'unpckhpd')) and n == 2 and d is not None:
        if op in ('punpckldq',):
            return None   # already in lift64 (unpcklps form)
        return interleave(op, d, src(ops[1]))
    if op in ('packsswb', 'packuswb', 'packssdw', 'packusdw') and n == 2 and d is not None:
        return pack(op, d, src(ops[1]))
    if op[:5] in ('psllw', 'pslld', 'psllq', 'psrlw', 'psrld', 'psrlq', 'psraw', 'psrad') and len(op) == 5 \
            and n == 2 and d is not None:
        count = f'{ops[1].imm & 0xff}ull' if ops[1].imm is not None else f'({src(ops[1])}).q[0]'
        return shift(op, d, count)
    if op[:3] == 'cmp' and op[-2:] in ('ps', 'pd', 'ss', 'sd') and op[3:-2] in CMP_PREDICATES and n == 2 and d is not None:
        view = 'f' if op[-1] == 's' else 'g'
        test = CMP_PREDICATES[op[3:-2]].format(x='x_', y='y_')
        mask = unsigned_view(view)
        count = 1 if op[-2] == 's' else LANES[view]
        return (f'{{ XMM64 s_ = {src(ops[1])}, r_ = c->xmm[{d}]; for (int i = 0; i < {count}; ++i) {{ '
                f'__typeof__(r_.{view}[0]) x_ = r_.{view}[i], y_ = s_.{view}[i]; '
                f'r_.{mask}[i] = ({test}) ? {ones(view)} : 0; }} c->xmm[{d}] = r_; }}')
    if op in ('pmuludq', 'pmuldq') and n == 2 and d is not None:
        cast = 'uint64_t)(uint32_t' if op == 'pmuludq' else 'int64_t)(int32_t'
        return shuffle(d, src(ops[1]), [f'r_.q[{i}] = (uint64_t)(({cast})d_.d[{2 * i}] * ({cast})s_.d[{2 * i}]);'
                                        for i in range(2)])
    if op == 'pmaddwd' and n == 2 and d is not None:
        return shuffle(d, src(ops[1]), [f'r_.d[{i}] = (uint32_t)((int32_t)d_.sw[{2 * i}] * s_.sw[{2 * i}] + '
                                        f'(int32_t)d_.sw[{2 * i + 1}] * s_.sw[{2 * i + 1}]);' for i in range(4)])
    if op == 'pmaddubsw' and n == 2 and d is not None:
        return shuffle(d, src(ops[1]), [f'r_.sw[{i}] = (int16_t)rt64_sat((int32_t)d_.b[{2 * i}] * s_.sb[{2 * i}] + '
                                        f'(int32_t)d_.b[{2 * i + 1}] * s_.sb[{2 * i + 1}], -32768, 32767);'
                                        for i in range(8)])
    if op == 'pmulhrsw' and n == 2 and d is not None:
        return lanes(d, src(ops[1]), 'sw', '(int16_t)((((int32_t){x} * {y}) >> 14) + 1 >> 1)')
    if op == 'psadbw' and n == 2 and d is not None:
        return shuffle(d, src(ops[1]), [
            f'r_.q[{h}] = ' + ' + '.join(f'(uint64_t)(d_.b[{8 * h + i}] > s_.b[{8 * h + i}] ? '
                                         f'd_.b[{8 * h + i}] - s_.b[{8 * h + i}] : s_.b[{8 * h + i}] - d_.b[{8 * h + i}])'
                                         for i in range(8)) + ';' for h in range(2)])
    if op in ('phaddw', 'phaddd', 'phsubw', 'phsubd', 'haddps', 'haddpd', 'hsubps', 'hsubpd') and n == 2 and d is not None:
        view = {'phaddw': 'w', 'phaddd': 'd', 'phsubw': 'w', 'phsubd': 'd',
                'haddps': 'f', 'haddpd': 'g', 'hsubps': 'f', 'hsubpd': 'g'}[op]
        sign = '-' if 'sub' in op else '+'
        k = LANES[view] // 2
        parts = [f'r_.{view}[{i}] = d_.{view}[{2 * i}] {sign} d_.{view}[{2 * i + 1}];' for i in range(k)]
        parts += [f'r_.{view}[{k + i}] = s_.{view}[{2 * i}] {sign} s_.{view}[{2 * i + 1}];' for i in range(k)]
        return shuffle(d, src(ops[1]), parts)
    if op in ('addsubps', 'addsubpd') and n == 2 and d is not None:
        view = 'f' if op.endswith('ps') else 'g'
        return shuffle(d, src(ops[1]), [f'r_.{view}[{i}] = d_.{view}[{i}] {"-" if i % 2 == 0 else "+"} s_.{view}[{i}];'
                                        for i in range(LANES[view])])
    if op in ('movsldup', 'movshdup') and n == 2 and d is not None:
        k = 0 if op == 'movsldup' else 1
        return shuffle(d, src(ops[1]), [f'r_.d[{i}] = s_.d[{(i & 2) + k}];' for i in range(4)])
    if op == 'movddup' and n == 2 and d is not None:
        return shuffle(d, src(ops[1]), ['r_.q[0] = s_.q[0]; r_.q[1] = s_.q[0];'])
    if op in ('pshuflw', 'pshufhw') and n == 3 and d is not None and ops[2].imm is not None:
        k, base = ops[2].imm, 0 if op == 'pshuflw' else 4
        return shuffle(d, src(ops[1]), ['r_ = s_;'] + [f'r_.w[{base + i}] = s_.w[{base + ((k >> 2 * i) & 3)}];'
                                                       for i in range(4)])
    if op == 'pshufb' and n == 2 and d is not None:
        return ('{ XMM64 s_ = %s, d_ = c->xmm[%d], r_; for (int i = 0; i < 16; ++i) '
                'r_.b[i] = (s_.b[i] & 0x80) ? 0 : d_.b[s_.b[i] & 15]; c->xmm[%d] = r_; }' % (src(ops[1]), d, d))
    if op == 'palignr' and n == 3 and d is not None and ops[2].imm is not None:
        k = ops[2].imm
        return ('{ XMM64 s_ = %s, d_ = c->xmm[%d], r_; for (int i = 0; i < 16; ++i) { int j_ = i + %d; '
                'r_.b[i] = j_ < 16 ? s_.b[j_] : j_ < 32 ? d_.b[j_ - 16] : 0; } c->xmm[%d] = r_; }' % (src(ops[1]), d, k, d))
    if op in ('blendps', 'blendpd', 'pblendw') and n == 3 and d is not None and ops[2].imm is not None:
        view = {'blendps': 'd', 'blendpd': 'q', 'pblendw': 'w'}[op]
        k = ops[2].imm
        return shuffle(d, src(ops[1]), [f'r_.{view}[{i}] = s_.{view}[{i}];' for i in range(LANES[view]) if k >> i & 1])
    if op in ('blendvps', 'blendvpd', 'pblendvb') and n in (2, 3) and d is not None:
        view, signed = {'blendvps': ('d', 'i'), 'blendvpd': ('q', 's'), 'pblendvb': ('b', 'sb')}[op]
        return (f'{{ XMM64 s_ = {src(ops[1])}, m_ = c->xmm[0], r_ = c->xmm[{d}]; for (int i = 0; i < {LANES[view]}; ++i) '
                f'if (m_.{signed}[i] < 0) r_.{view}[i] = s_.{view}[i]; c->xmm[{d}] = r_; }}')
    if op == 'insertps' and n == 3 and d is not None and ops[2].imm is not None:
        k = ops[2].imm
        value = f'c->xmm[{ops[1].xmm}].d[{k >> 6 & 3}]' if ops[1].xmm is not None else f'(uint32_t)R32({ops[1].addr})'
        parts = [f'r_.d[{k >> 4 & 3}] = v_;'] + [f'r_.d[{i}] = 0;' for i in range(4) if k >> i & 1]
        return f'{{ uint32_t v_ = {value}; XMM64 r_ = c->xmm[{d}]; {" ".join(parts)} c->xmm[{d}] = r_; }}'
    if op in ('extractps', 'pextrb', 'pextrw', 'pextrd', 'pextrq') and n == 3 and ops[1].xmm is not None \
            and ops[2].imm is not None:
        view = {'extractps': 'd', 'pextrb': 'b', 'pextrw': 'w', 'pextrd': 'd', 'pextrq': 'q'}[op]
        value = f'c->xmm[{ops[1].xmm}].{view}[{ops[2].imm & (LANES[view] - 1)}]'
        return ops[0].write(value)
    if op in ('pinsrb', 'pinsrw', 'pinsrd', 'pinsrq') and n == 3 and d is not None and ops[2].imm is not None:
        view = {'pinsrb': 'b', 'pinsrw': 'w', 'pinsrd': 'd', 'pinsrq': 'q'}[op]
        source = ops[1]
        bits = {'b': 8, 'w': 16, 'd': 32, 'q': 64}[view]
        value = source.read() if source.addr is None else f'R{bits}({source.addr})'
        return f'c->xmm[{d}].{view}[{ops[2].imm & (LANES[view] - 1)}] = {value};'
    if op[:5] in ('pmovz', 'pmovs') and len(op) == 8 and n == 2 and d is not None:
        signed = op[4] == 's'
        frm, to = op[6], op[7]
        frm_view = {'b': 'sb' if signed else 'b', 'w': 'sw' if signed else 'w', 'd': 'i' if signed else 'd'}[frm]
        to_view = {'w': 'w', 'd': 'd', 'q': 'q'}[to]
        count = LANES[to_view]
        source = ops[1]
        if source.addr is not None:
            source.width = count * {'b': 8, 'w': 16, 'd': 32}[frm]
        cast = {'w': 'uint16_t', 'd': 'uint32_t', 'q': 'uint64_t'}[to]
        return (f'{{ XMM64 s_ = {src(source)}, r_; for (int i = 0; i < {count}; ++i) '
                f'r_.{to_view}[i] = ({cast})s_.{frm_view}[i]; c->xmm[{d}] = r_; }}')
    if op == 'phminposuw' and n == 2 and d is not None:
        return (f'{{ XMM64 s_ = {src(ops[1])}, r_; r_.q[0] = r_.q[1] = 0; unsigned k_ = 0; '
                'for (unsigned i = 1; i < 8; ++i) if (s_.w[i] < s_.w[k_]) k_ = i; '
                f'r_.w[0] = s_.w[k_]; r_.w[1] = (uint16_t)k_; c->xmm[{d}] = r_; }}')
    if op == 'ptest' and n == 2 and d is not None:
        return (f'{{ XMM64 s_ = {src(ops[1])}, d_ = c->xmm[{d}]; f.zf = ((d_.q[0] & s_.q[0]) | (d_.q[1] & s_.q[1])) == 0; '
                f'f.cf = ((~d_.q[0] & s_.q[0]) | (~d_.q[1] & s_.q[1])) == 0; f.of = f.sf = f.af = 0; f.pf = 1; }}')
    if op in ('roundss', 'roundsd', 'roundps', 'roundpd') and n == 3 and d is not None and ops[2].imm is not None:
        view = 'f' if op in ('roundss', 'roundps') else 'g'
        k = ops[2].imm
        fn = 'nearbyint' if k & 4 else ('nearbyint', 'floor', 'ceil', 'trunc')[k & 3]
        if view == 'f':
            fn += 'f'
        count = 1 if op[-2] == 's' else LANES[view]
        return (f'{{ XMM64 s_ = {src(ops[1])}, r_ = c->xmm[{d}]; for (int i = 0; i < {count}; ++i) '
                f'r_.{view}[i] = __builtin_{fn}(s_.{view}[i]); c->xmm[{d}] = r_; }}')
    if op in ('dpps', 'dppd') and n == 3 and d is not None and ops[2].imm is not None:
        view = 'f' if op == 'dpps' else 'g'
        k, count = ops[2].imm, LANES['f' if op == 'dpps' else 'g']
        terms = ' + '.join(f'd_.{view}[{i}] * s_.{view}[{i}]' for i in range(count) if k >> (4 + i) & 1) or '0'
        parts = [f'r_.{view}[{i}] = {"t_" if k >> i & 1 else "0"};' for i in range(count)]
        return (f'{{ XMM64 s_ = {src(ops[1])}, d_ = c->xmm[{d}], r_; __typeof__(d_.{view}[0]) t_ = {terms}; '
                f'{" ".join(parts)} c->xmm[{d}] = r_; }}')
    if op in ('cvtps2pd', 'cvtdq2pd') and n == 2 and d is not None:
        view = 'f' if op == 'cvtps2pd' else 'i'
        return shuffle(d, src(ops[1]), ['r_.g[0] = s_.%s[0]; r_.g[1] = s_.%s[1];' % (view, view)])
    if op in ('cvtpd2ps', 'cvttpd2dq', 'cvtpd2dq') and n == 2 and d is not None:
        if op == 'cvtpd2ps':
            convert = '(float)s_.g[{i}]'
            out = 'f'
        else:
            value = 's_.g[{i}]' if op == 'cvttpd2dq' else '__builtin_nearbyint(s_.g[{i}])'
            convert = f'(int32_t)rt64_cvtt({value}, 32)'
            out = 'i'
        parts = [f'r_.{out}[{i}] = {convert.format(i=i)};' for i in range(2)] + ['r_.q[1] = 0;']
        return shuffle(d, src(ops[1]), parts)
    if op in ('movntdq', 'movntps', 'movntpd', 'movntdqa') and n == 2:
        if ops[0].xmm is not None:
            return f'c->xmm[{ops[0].xmm}] = {src(ops[1])};'
        return f'WX({ops[0].addr}, c->xmm[{ops[1].xmm}]);'
    if op == 'movnti' and n == 2:
        return ops[0].write(ops[1].read())
    if op == 'crc32' and n == 2 and ops[0].reg:
        width = ops[1].width
        fn = {8: '__crc32cb', 16: '__crc32ch', 32: '__crc32cw', 64: '__crc32cd'}[width]
        return ops[0].write(f'{fn}((uint32_t){ops[0].read()}, {ops[1].read()})')
    return None
