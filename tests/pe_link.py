#!/usr/bin/env python3
"""A tiny linker for the PE test program: COFF x86-64 .obj files -> PE32+ DLL.

This Mac has clang, which assembles for x86_64-windows-msvc, but no PE
linker (no lld-link), so the test fixture is linked here, like
tests/x64_link.py does for ELF. The output is an ordinary PE32+ DLL that
tools/peprep64.py reads like any Windows module:

  * sections merged by name before `$` (.text, .rdata with .xdata, .data
    with .bss, .tls, .pdata), then a generated .idata (imports, exports)
    and .reloc (DIR64 base relocations);
  * relocations ADDR64, ADDR32NB (image-relative), REL32 and REL32_1..5,
    SECREL (offsets in the TLS template);
  * `__ImageBase` is offset 0; `__imp_NAME` symbols become IAT slots of the
    DLLs given with --import NAME=DLL (or NAME=DLL#ORDINAL);
  * --export NAME exports a symbol; `_tls_used` and `_load_config_used`,
    when the object defines them, become the TLS and load configuration
    directories, and .pdata the exception directory.
"""
import argparse
import struct
import sys

IMAGE_BASE = 0x140000000
SECTION_ALIGN, FILE_ALIGN = 0x1000, 0x200
ADDR64, ADDR32NB, REL32, SECTION, SECREL = 1, 3, 4, 0xA, 0xB
CODE, IDATA, UDATA, EXEC, READ, WRITE = 0x20, 0x40, 0x80, 0x20000000, 0x40000000, 0x80000000
# Output sections in order: (name, characteristics, input groups).
LAYOUT = [('.text', CODE | EXEC | READ, ('.text',)),
          ('.rdata', IDATA | READ, ('.rdata', '.xdata')),
          ('.data', IDATA | READ | WRITE, ('.data', '.bss')),
          ('.tls', IDATA | READ | WRITE, ('.tls',)),
          ('.pdata', IDATA | READ, ('.pdata',))]


def align(value, to):
    return (value + to - 1) & -to


def read_object(path):
    with open(path, 'rb') as f:
        data = f.read()
    machine, count, _, symptr, nsyms, optsize, _ = struct.unpack_from('<HHIIIHH', data, 0)
    assert machine == 0x8664, 'x86-64 COFF object expected'
    strings = symptr + 18 * nsyms
    def name_of(raw):
        if raw[:4] == b'\0\0\0\0':
            o = strings + struct.unpack_from('<I', raw, 4)[0]
            return data[o:data.index(b'\0', o)].decode()
        return raw.rstrip(b'\0').decode()
    sections = []
    for i in range(count):
        raw, vsize, _, size, ptr, relptr, _, nrel, _, chars = struct.unpack_from('<8sIIIIIIHHI', data, 20 + optsize + 40 * i)
        name = name_of(raw) if not raw.startswith(b'/') else (lambda o: data[o:data.index(b'\0', o)].decode())(
            strings + int(raw[1:].rstrip(b'\0')))
        relocs = [struct.unpack_from('<IIH', data, relptr + 10 * k) for k in range(nrel)]
        body = bytearray(data[ptr:ptr + size]) if ptr else bytearray(size)
        alignment = 1 << (((chars >> 20) & 0xf) - 1) if (chars >> 20) & 0xf else 16
        sections.append(dict(name=name, data=body, relocs=relocs, align=alignment, chars=chars))
    symbols, i = [], 0
    while i < nsyms:
        raw, value, secnum, typ, storage, aux = struct.unpack_from('<8sIhHBB', data, symptr + 18 * i)
        symbols.append(dict(name=name_of(raw), value=value, section=secnum, storage=storage))
        symbols.extend([None] * aux)
        i += 1 + aux
    return sections, symbols


def read_objects(paths):
    """Several objects as one: section numbers and symbol indices renumbered."""
    sections, symbols = [], []
    for path in paths:
        secs, syms = read_object(path)
        sec_base, sym_base = len(sections), len(symbols)
        for sec in secs:
            sec['relocs'] = [(off, index + sym_base, kind) for off, index, kind in sec['relocs']]
        for sym in syms:
            if sym and sym['section'] > 0:
                sym['section'] += sec_base
        sections += secs
        symbols += syms
    return sections, symbols


def link(objs, out, dll_name, imports=(), exports=()):
    sections, symbols = read_objects([objs] if isinstance(objs, str) else objs)
    # Place the input sections.
    placed, rva, outputs = {}, SECTION_ALIGN, []
    for out_name, chars, groups in LAYOUT:
        members = sorted((s for s in sections if s['name'].split('$')[0] in groups and not s['chars'] & 0x800),
                         key=lambda s: (groups.index(s['name'].split('$')[0]), s['name']))
        if not members:
            continue
        start, body = rva, bytearray()
        for s in members:
            body += bytes(align(len(body), s['align']) - len(body))
            placed[id(s)] = start + len(body)
            body += s['data']
        outputs.append(dict(name=out_name, rva=start, data=body, chars=chars))
        rva = align(start + max(len(body), 1), SECTION_ALIGN)
    section_rva = {i + 1: placed.get(id(s)) for i, s in enumerate(sections)}
    tls_output = next((o for o in outputs if o['name'] == '.tls'), None)

    def symbol_rva(sym):
        if sym['name'] == '__ImageBase':
            return 0
        if sym['section'] > 0 and section_rva.get(sym['section']) is not None:
            return section_rva[sym['section']] + sym['value']
        return None

    defined = {s['name']: symbol_rva(s) for s in symbols if s and symbol_rva(s) is not None}
    defined['__ImageBase'] = 0

    # Imports: one IAT slot per __imp_ symbol, grouped by DLL.
    wanted = {}
    for spec in imports:
        name, dll = spec.split('=')
        ordinal = None
        if '#' in dll:
            dll, ordinal = dll.split('#')
            ordinal = int(ordinal)
        wanted[name] = (dll, ordinal)
    used = sorted({s['name'][6:] for s in symbols if s and s['name'].startswith('__imp_') and s['section'] == 0})
    missing = [n for n in used if n not in wanted]
    if missing:
        raise SystemExit(f'pe_link: no --import for {", ".join(missing)}')
    by_dll = {}
    for n in used:
        by_dll.setdefault(wanted[n][0], []).append(n)

    idata_rva = rva
    idata = bytearray()
    def put(blob, alignment=8):
        nonlocal idata
        idata += bytes(align(len(idata), alignment) - len(idata))
        at = idata_rva + len(idata)
        idata += blob
        return at
    descriptors_at = put(bytes(20 * (len(by_dll) + 1)), 4)
    descriptor_rows, iat_slot = [], {}
    for dll, names in by_dll.items():
        name_at = put(dll.encode() + b'\0', 2)
        entries = []
        for n in names:
            ordinal = wanted[n][1]
            entries.append((1 << 63) | ordinal if ordinal is not None else put(struct.pack('<H', 0) + n.encode() + b'\0', 2))
        ilt = put(b''.join(struct.pack('<Q', e) for e in entries) + bytes(8))
        iat = put(b''.join(struct.pack('<Q', e) for e in entries) + bytes(8))
        for k, n in enumerate(names):
            iat_slot[n] = iat + 8 * k
        descriptor_rows.append(struct.pack('<IIIII', ilt, 0, 0, name_at, iat))
    for k, row in enumerate(descriptor_rows):
        idata[descriptors_at - idata_rva + 20 * k:descriptors_at - idata_rva + 20 * k + 20] = row
    for n, slot in iat_slot.items():
        defined['__imp_' + n] = slot

    export_dir = None
    if exports:
        names = sorted(exports)
        dll_at = put(dll_name.encode() + b'\0', 2)
        name_rvas = [put(n.encode() + b'\0', 2) for n in names]
        functions = put(b''.join(struct.pack('<I', defined[n]) for n in names), 4)
        name_table = put(b''.join(struct.pack('<I', a) for a in name_rvas), 4)
        ordinals = put(b''.join(struct.pack('<H', k) for k in range(len(names))), 2)
        export_dir = put(struct.pack('<IIHHIIIIIII', 0, 0, 0, 0, dll_at, 1, len(names), len(names),
                                     functions, name_table, ordinals), 4)
    outputs.append(dict(name='.idata', rva=idata_rva, data=idata, chars=IDATA | READ | WRITE))
    rva = align(idata_rva + len(idata), SECTION_ALIGN)

    # Apply the relocations.
    base_relocs = []
    for index, s in enumerate(sections):
        at = placed.get(id(s))
        if at is None or not s['relocs']:
            continue
        out_sec = next(o for o in outputs if o['rva'] <= at < o['rva'] + max(len(o['data']), 1))
        body = out_sec['data']
        for offset, symindex, kind in s['relocs']:
            sym = symbols[symindex]
            target = defined.get(sym['name']) if sym['storage'] == 2 or sym['section'] <= 0 else symbol_rva(sym)
            if target is None:
                raise SystemExit(f'pe_link: undefined symbol {sym["name"]}')
            site = at + offset
            o = site - out_sec['rva']
            if kind == ADDR64:
                addend = struct.unpack_from('<q', body, o)[0]
                struct.pack_into('<Q', body, o, IMAGE_BASE + target + addend)
                base_relocs.append(site)
            elif kind == ADDR32NB:
                addend = struct.unpack_from('<i', body, o)[0]
                struct.pack_into('<I', body, o, target + addend)
            elif REL32 <= kind <= REL32 + 5:
                addend = struct.unpack_from('<i', body, o)[0]
                struct.pack_into('<i', body, o, target + addend - (site + 4 + (kind - REL32)))
            elif kind == SECREL:
                addend = struct.unpack_from('<i', body, o)[0]
                struct.pack_into('<I', body, o, target + addend - (tls_output['rva'] if tls_output else 0))
            elif kind == SECTION:
                pass
            else:
                raise SystemExit(f'pe_link: relocation type {kind:#x}')

    # .reloc: DIR64 entries in 4 KB pages.
    reloc = bytearray()
    pages = {}
    for site in sorted(base_relocs):
        pages.setdefault(site & ~0xfff, []).append(site & 0xfff)
    for page, offsets in sorted(pages.items()):
        entries = [(10 << 12) | o for o in offsets]
        if len(entries) % 2:
            entries.append(0)
        reloc += struct.pack('<II', page, 8 + 2 * len(entries)) + b''.join(struct.pack('<H', e) for e in entries)
    if reloc:
        outputs.append(dict(name='.reloc', rva=rva, data=reloc, chars=IDATA | READ))
        rva = align(rva + len(reloc), SECTION_ALIGN)
    image_size = rva

    dirs = [(0, 0)] * 16
    if export_dir:
        dirs[0] = (export_dir, 40)
    if by_dll:
        dirs[1] = (descriptors_at, 20 * (len(by_dll) + 1))
    pdata = next((o for o in outputs if o['name'] == '.pdata'), None)
    if pdata:
        dirs[3] = (pdata['rva'], len(pdata['data']))
    if reloc:
        dirs[5] = (outputs[-1]['rva'], len(reloc))
    if '_tls_used' in defined:
        dirs[9] = (defined['_tls_used'], 40)
    if '_load_config_used' in defined:
        dirs[10] = (defined['_load_config_used'], 0x94)

    # Headers and file.
    headers_size = align(0x40 + 4 + 20 + 240 + 40 * len(outputs), FILE_ALIGN)
    file_at, rows = headers_size, []
    for o in outputs:
        raw = align(len(o['data']), FILE_ALIGN)
        rows.append((o, file_at, raw))
        file_at += raw
    dos = bytearray(0x40)
    dos[:2] = b'MZ'
    struct.pack_into('<I', dos, 0x3c, 0x40)
    coff = struct.pack('<HHIIIHH', 0x8664, len(outputs), 0, 0, 0, 240, 0x2022)
    entry = 0
    code_size = sum(len(o['data']) for o in outputs if o['chars'] & CODE)
    opt = struct.pack('<HBBIIIII', 0x20b, 14, 0, code_size, 0, 0, entry, SECTION_ALIGN)
    opt += struct.pack('<QIIHHHHHHIIIIHHQQQQII', IMAGE_BASE, SECTION_ALIGN, FILE_ALIGN, 6, 0, 0, 0, 6, 0, 0,
                       image_size, headers_size, 0, 2, 0x0160, 0x100000, 0x1000, 0x100000, 0x1000, 0, 16)
    opt += b''.join(struct.pack('<II', a, b) for a, b in dirs)
    table = b''
    for o, at, raw in rows:
        table += struct.pack('<8sIIIIIIHHI', o['name'].encode(), len(o['data']), o['rva'], raw, at, 0, 0, 0, 0, o['chars'])
    head = bytes(dos) + b'PE\0\0' + coff + opt + table
    image = bytearray(head + bytes(headers_size - len(head)))
    for o, at, raw in rows:
        image += o['data'] + bytes(raw - len(o['data']))
    with open(out, 'wb') as f:
        f.write(image)
    return defined


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--name', required=True, help="the DLL's own name (export directory)")
    ap.add_argument('--import', dest='imports', action='append', default=[], help='NAME=DLL or NAME=DLL#ORDINAL')
    ap.add_argument('--export', dest='exports', action='append', default=[])
    ap.add_argument('objs', nargs='+', metavar='obj')
    ap.add_argument('out')
    a = ap.parse_args(argv)
    link(a.objs, a.out, a.name, a.imports, a.exports)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
