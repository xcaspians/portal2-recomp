#!/usr/bin/env python3
"""A tiny linker for the x86-64 test programs: relocatable ELF64 .o -> shared object.

This Mac has clang (which assembles for x86_64-linux-gnu) but no ELF linker,
so the test fixture is linked here. The output is an ordinary ET_DYN that
tools/elfprep64.py reads like any P2CE module: one PT_LOAD (file offset =
address), a PT_DYNAMIC, .dynsym/.dynstr for exports and imports, RELA
relocations (RELATIVE, 64, GLOB_DAT, JUMP_SLOT), a PLT and a GOT, and a
.symtab that names local functions.

Supported input relocations: PC32, PLT32, 64, GOTPCREL, GOTPCRELX,
REX_GOTPCRELX, and for thread-local data TLSGD (general dynamic, through
__tls_get_addr) and GOTTPOFF (initial exec, through fs:), in .text, .rodata,
.data, .init_array, .eh_frame and .gcc_except_table (no .bss: use .zero in
.data). DT_NEEDED entries with --needed; an .eh_frame gets the .eh_frame_hdr
and PT_GNU_EH_FRAME the C++ unwinder looks for.
"""
import struct
import sys

R_64, R_PC32, R_PLT32, R_GOTPCREL, R_RELATIVE, R_GLOB_DAT, R_JUMP_SLOT = 1, 2, 4, 9, 8, 6, 7
R_GOTPCRELX, R_REX_GOTPCRELX = 41, 42
R_DTPMOD64, R_DTPOFF64, R_TPOFF64, R_TLSGD, R_GOTTPOFF = 16, 17, 18, 19, 22


def align(value, to):
    return (value + to - 1) & -to


class StringTable:
    def __init__(self):
        self.data, self.index = bytearray(b'\0'), {'': 0}

    def add(self, text):
        if text not in self.index:
            self.index[text] = len(self.data)
            self.data += text.encode() + b'\0'
        return self.index[text]


def read_object(path):
    with open(path, 'rb') as f:
        data = f.read()
    assert data[:4] == b'\x7fELF' and data[4] == 2, 'ELF64 relocatable expected'
    e_type, machine = struct.unpack_from('<HH', data, 16)
    assert e_type == 1 and machine == 62, 'x86-64 ET_REL expected'
    shoff, = struct.unpack_from('<Q', data, 40)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 58)
    sections = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, alignment, entsize = \
            struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * shentsize)
        sections.append(dict(name=name, type=typ, flags=flags, off=off, size=size, link=link, info=info,
                             align=max(alignment, 1)))
    names = sections[shstrndx]['off']
    cstr = lambda o: data[o:data.index(b'\0', o)].decode()
    for s in sections:
        s['name'] = cstr(names + s['name'])
    symtab = next(s for s in sections if s['type'] == 2)
    strings = sections[symtab['link']]['off']
    symbols = []
    for i in range(symtab['size'] // 24):
        nm, info, other, shndx, value, size = struct.unpack_from('<IBBHQQ', data, symtab['off'] + 24 * i)
        symbols.append(dict(name=cstr(strings + nm), bind=info >> 4, type=info & 15, shndx=shndx,
                            value=value, size=size))
    return data, sections, symbols


def link(obj_path, out_path, soname, needed=()):
    data, sections, symbols = read_object(obj_path)
    by_name = {s['name']: i for i, s in enumerate(sections)}
    # Allocated sections by kind (comdat .text.*/.data.rel.ro.* included):
    # code, then data, then thread-local initialised and zeroed data.
    alloc = [i for i, s in enumerate(sections) if s['flags'] & 2 and s['size']]
    code_sections = [i for i in alloc if sections[i]['flags'] & 4]
    tls = [i for i in alloc if sections[i]['flags'] & 0x400]
    data_sections = [i for i in alloc if i not in code_sections and i not in tls]
    tls = [i for i in tls if sections[i]['type'] != 8] + [i for i in tls if sections[i]['type'] == 8]
    linked = set(alloc)
    raw_relocs = []
    for s in sections:
        if s['type'] == 4 and s['info'] in linked:    # SHT_RELA
            for i in range(s['size'] // 24):
                off, info, addend = struct.unpack_from('<QQq', data, s['off'] + 24 * i)
                raw_relocs.append((s['info'], off, info & 0xffffffff, symbols[info >> 32], addend))
    plt_names = sorted({sym['name'] for _, _, typ, sym, _ in raw_relocs if typ == R_PLT32 and sym['shndx'] == 0})
    imports = sorted({s['name'] for s in symbols if s['shndx'] == 0 and s['name'] and s['bind'] != 0})

    # Layout: headers, .text, the PLT (16 bytes per stub), then the other
    # sections (TLS ones together), the GOT.
    image = bytearray(0x1000)
    base = {}
    tls_range = None
    for index in code_sections + [None] + data_sections + tls:
        if index is not None:
            s = sections[index]
            start = align(len(image), max(s['align'], 16 if index not in tls else s['align']))
            image += (b'\xcc' if index in code_sections else b'\0') * (start - len(image))
            base[index] = start
            image += bytes(s['size']) if s['type'] == 8 else data[s['off']:s['off'] + s['size']]   # SHT_NOBITS
            if index in tls:
                tls_range = (tls_range[0] if tls_range else start, len(image))
        else:
            plt_start = align(len(image), 16)
            plt_size = 16 * len(plt_names)
            image += b"\xcc" * (plt_start + plt_size - len(image))   # int3 padding: objdump stays in step
            data_start = align(len(image), 0x1000)
            image += bytes(data_start - len(image))

    def address(sym):
        return base[sym['shndx']] + sym['value'] if sym['shndx'] in base else None

    def tls_offset(sym):
        return address(sym) - tls_range[0]

    relocs_in = [(base[section] + off, typ, sym, addend) for section, off, typ, sym, addend in raw_relocs]
    got_names = sorted({sym['name'] if sym['shndx'] == 0 else f'@{address(sym)}'
                        for _, typ, sym, _ in relocs_in if typ in (R_GOTPCREL, R_GOTPCRELX, R_REX_GOTPCRELX)})
    tlsgd_names = sorted({sym['name'] for _, typ, sym, _ in relocs_in if typ == R_TLSGD})
    gottpoff_names = sorted({sym['name'] for _, typ, sym, _ in relocs_in if typ == R_GOTTPOFF})
    got_start = align(len(image), 16)
    slots = plt_names + got_names + [f'tpoff:{n}' for n in gottpoff_names]
    got = {name: got_start + 8 * i for i, name in enumerate(slots)}
    for i, name in enumerate(tlsgd_names):       # two slots: module id, offset
        got[f'gd:{name}'] = got_start + 8 * len(slots) + 16 * i
    image += bytes(got_start - len(image) + 8 * len(slots) + 16 * len(tlsgd_names))
    plt = {name: plt_start + 16 * i for i, name in enumerate(plt_names)}
    tls_symbols = {s['name']: s for s in symbols if s['type'] == 6 and s['shndx'] in base}

    dyn_relocs, plt_relocs = [], []
    dynsyms = [dict(name='', value=0, info=0, shndx=0, size=0)]
    dyn_index = {}

    def dynsym(name, defined_at=None, kind=2, size=0):
        if name not in dyn_index:
            dyn_index[name] = len(dynsyms)
            dynsyms.append(dict(name=name, value=defined_at or 0, info=(1 << 4) | (kind if defined_at else 0),
                                shndx=1 if defined_at else 0, size=size))
        return dyn_index[name]

    for s in symbols:   # exports: every global defined symbol (not TLS ones, kept local here)
        if s['bind'] == 1 and s['shndx'] in base and s['type'] != 6:
            dynsym(s['name'], address(s), s['type'] or 1, s['size'])
    for name in imports:
        dynsym(name)
    for i, name in enumerate(plt_names):
        stub = plt[name]
        image[stub:stub + 16] = b'\xff\x25' + struct.pack('<i', got[name] - (stub + 6)) + b'\xcc' * 10
        plt_relocs.append((got[name], R_JUMP_SLOT, dyn_index[name], 0))
    for name in got_names:
        if name.startswith('@'):
            dyn_relocs.append((got[name], R_RELATIVE, 0, int(name[1:])))
        else:
            dyn_relocs.append((got[name], R_GLOB_DAT, dyn_index[name], 0))
    for name in tlsgd_names:
        dyn_relocs.append((got[f'gd:{name}'], R_DTPMOD64, 0, 0))
        dyn_relocs.append((got[f'gd:{name}'] + 8, R_DTPOFF64, 0, tls_offset(tls_symbols[name])))
    for name in gottpoff_names:
        dyn_relocs.append((got[f'tpoff:{name}'], R_TPOFF64, 0, tls_offset(tls_symbols[name])))
    for off, typ, sym, addend in relocs_in:
        target = address(sym)
        if typ in (R_PC32, R_PLT32):
            if target is None:
                if typ != R_PLT32:
                    raise SystemExit(f'PC32 to undefined {sym["name"]}: use @PLT or @GOTPCREL')
                target = plt[sym['name']]
            struct.pack_into('<i', image, off, target + addend - off)
        elif typ in (R_GOTPCREL, R_GOTPCRELX, R_REX_GOTPCRELX):
            slot = got[sym['name'] if sym['shndx'] == 0 else f'@{target}']
            struct.pack_into('<i', image, off, slot + addend - off)
        elif typ == R_TLSGD:
            struct.pack_into('<i', image, off, got[f'gd:{sym["name"]}'] + addend - off)
        elif typ == R_GOTTPOFF:
            struct.pack_into('<i', image, off, got[f'tpoff:{sym["name"]}'] + addend - off)
        elif typ == R_64:
            if target is None:
                dyn_relocs.append((off, R_64, dyn_index[sym['name']], addend))
            else:
                dyn_relocs.append((off, R_RELATIVE, 0, target + addend))
        else:
            raise SystemExit(f'relocation type {typ} not supported by the test linker')

    # .eh_frame_hdr: the sorted FDE table the unwinder finds through PT_GNU_EH_FRAME.
    eh_hdr = None
    if '.eh_frame' in by_name:
        frame, frame_size = base[by_name['.eh_frame']], sections[by_name['.eh_frame']]['size']
        fdes = eh_frame_fdes(image, frame, frame_size)
        hdr_start = align(len(image), 4)
        image.extend(bytes(hdr_start - len(image)))
        table = b''.join(struct.pack('<ii', start - hdr_start, fde - hdr_start) for start, fde in sorted(fdes))
        image.extend(bytes([1, 0x1b, 0x03, 0x3b]) + struct.pack('<iI', frame - (hdr_start + 4), len(fdes)) + table)
        eh_hdr = (hdr_start, len(image) - hdr_start)

    # Dynamic metadata after the data: .dynsym, .dynstr, .rela.dyn, .rela.plt, .dynamic.
    dynstr = StringTable()
    soname_index = dynstr.add(soname)
    needed_index = [dynstr.add(n) for n in needed]
    for s in dynsyms:
        s['name_index'] = dynstr.add(s['name'])

    def place(blob, alignment=8):
        start = align(len(image), alignment)
        image.extend(bytes(start - len(image)))
        image.extend(blob)
        return start
    dynsym_off = place(b''.join(struct.pack('<IBBHQQ', s['name_index'], s['info'], 0, s['shndx'], s['value'], s['size'])
                                for s in dynsyms))
    dynstr_off = place(bytes(dynstr.data), 1)
    pack_rela = lambda rows: b''.join(struct.pack('<QQq', o, (i << 32) | t, a) for o, t, i, a in rows)
    rela_off = place(pack_rela(dyn_relocs))
    jmprel_off = place(pack_rela(plt_relocs))
    dynamic = [(1, i) for i in needed_index]
    dynamic += [(14, soname_index), (5, dynstr_off), (10, len(dynstr.data)), (6, dynsym_off), (11, 24),
                (7, rela_off), (8, 24 * len(dyn_relocs)), (9, 24), (23, jmprel_off), (2, 24 * len(plt_relocs)),
                (20, 7)]
    if '.init_array' in by_name:
        dynamic += [(25, base[by_name['.init_array']]), (27, sections[by_name['.init_array']]['size'])]
    dynamic.append((0, 0))
    dynamic_off = place(b''.join(struct.pack('<qQ', t, v) for t, v in dynamic))
    dynamic_size = 16 * len(dynamic)

    # Local and global function names for the lifter (.symtab).
    strtab = StringTable()
    symtab = [struct.pack('<IBBHQQ', 0, 0, 0, 0, 0, 0)]
    for s in symbols:
        if s['type'] == 2 and s['shndx'] in base:
            symtab.append(struct.pack('<IBBHQQ', strtab.add(s['name']), (s['bind'] << 4) | 2, 0, 1, address(s), s['size']))
    symtab_off = place(b''.join(symtab))
    strtab_off = place(bytes(strtab.data), 1)
    shstr = StringTable()
    shstr_names = ['.text', '.plt', '.data', '.dynsym', '.dynstr', '.symtab', '.strtab', '.shstrtab', '.dynamic', '.eh_frame']
    for n in shstr_names:
        shstr.add(n)
    shstr_off = place(bytes(shstr.data), 1)
    total = align(len(image), 8)
    image.extend(bytes(total - len(image)))
    shdrs = [(0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
             (shstr.index['.text'], 1, 6, 0x1000, 0x1000, plt_start - 0x1000, 0, 0, 16, 0),
             (shstr.index['.plt'], 1, 6, plt_start, plt_start, plt_size, 0, 0, 16, 16),
             (shstr.index['.data'], 1, 3, data_start, data_start, dynsym_off - data_start, 0, 0, 16, 0),
             (shstr.index['.dynsym'], 11, 2, dynsym_off, dynsym_off, 24 * len(dynsyms), 5, 1, 8, 24),
             (shstr.index['.dynstr'], 3, 2, dynstr_off, dynstr_off, len(dynstr.data), 0, 0, 1, 0),
             (shstr.index['.symtab'], 2, 0, 0, symtab_off, 24 * len(symtab), 7, 1, 8, 24),
             (shstr.index['.strtab'], 3, 0, 0, strtab_off, len(strtab.data), 0, 0, 1, 0),
             (shstr.index['.shstrtab'], 3, 0, 0, shstr_off, len(shstr.data), 0, 0, 1, 0),
             (shstr.index['.dynamic'], 6, 3, dynamic_off, dynamic_off, dynamic_size, 5, 0, 8, 16)]
    if '.eh_frame' in by_name:
        frame = base[by_name['.eh_frame']]
        shdrs.append((shstr.index['.eh_frame'], 1, 2, frame, frame, sections[by_name['.eh_frame']]['size'], 0, 0, 8, 0))
    phdrs = [(1, 7, 0, 0, 0, total, total, 0x1000),
             (2, 6, dynamic_off, dynamic_off, dynamic_off, dynamic_size, dynamic_size, 8)]
    if tls_range:
        initialised = [i for i in tls if sections[i]['type'] != 8]
        filesz = (base[initialised[-1]] + sections[initialised[-1]]['size'] - tls_range[0]) if initialised else 0
        phdrs.append((7, 4, tls_range[0], tls_range[0], tls_range[0], filesz, tls_range[1] - tls_range[0], 16))
    if eh_hdr:
        phdrs.append((0x6474e550, 4, eh_hdr[0], eh_hdr[0], eh_hdr[0], eh_hdr[1], eh_hdr[1], 4))
    shoff = len(image)
    for h in shdrs:
        image += struct.pack('<IIQQQQIIQQ', *h)
    header = b'\x7fELF' + bytes([2, 1, 1]) + bytes(9)
    header += struct.pack('<HHIQQQIHHHHHH', 3, 62, 1, 0, 64, shoff, 0, 64, 56, len(phdrs), 64, len(shdrs), 8)
    image[:64] = header
    for i, p in enumerate(phdrs):
        struct.pack_into('<IIQQQQQQ', image, 64 + 56 * i, *p)
    with open(out_path, 'wb') as f:
        f.write(image)


def eh_frame_fdes(image, start, size):
    """(function start, FDE address) of every FDE of an .eh_frame (pc-relative sdata4 pointers, as clang emits)."""
    out, at, cies = [], start, {}
    while at < start + size:
        length = struct.unpack_from('<I', image, at)[0]
        if not length:
            break
        record = at + 4
        cie_id = struct.unpack_from('<I', image, record)[0]
        if cie_id == 0:
            cies[at] = True
        else:
            pc_begin = record + 4
            out.append((pc_begin + struct.unpack_from('<i', image, pc_begin)[0], at))
        at = record + length
    return out


if __name__ == '__main__':
    import argparse
    import os
    ap = argparse.ArgumentParser(description='link a relocatable x86-64 object into a shared object (tests)')
    ap.add_argument('object')
    ap.add_argument('output')
    ap.add_argument('--needed', action='append', default=[], help='DT_NEEDED entry (repeatable)')
    a = ap.parse_args()
    link(a.object, a.output, os.path.basename(a.output), a.needed)
