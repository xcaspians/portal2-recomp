#!/usr/bin/env python3
"""A tiny linker for the x86-64 test programs: relocatable ELF64 .o -> shared object.

This Mac has clang (which assembles for x86_64-linux-gnu) but no ELF linker,
so the test fixture is linked here. The output is an ordinary ET_DYN that
tools/elfprep64.py reads like any P2CE module: one PT_LOAD (file offset =
address), a PT_DYNAMIC, .dynsym/.dynstr for exports and imports, RELA
relocations (RELATIVE, 64, GLOB_DAT, JUMP_SLOT), a PLT and a GOT, and a
.symtab that names local functions.

Supported input relocations: PC32, PLT32, 64, GOTPCREL, GOTPCRELX and
REX_GOTPCRELX, in .text, .rodata and .data (no .bss: use .zero in .data).
"""
import struct
import sys

R_64, R_PC32, R_PLT32, R_GOTPCREL, R_RELATIVE, R_GLOB_DAT, R_JUMP_SLOT = 1, 2, 4, 9, 8, 6, 7
R_GOTPCRELX, R_REX_GOTPCRELX = 41, 42
SECTIONS = ('.text', '.rodata', '.data')


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


def link(obj_path, out_path, soname):
    data, sections, symbols = read_object(obj_path)
    by_name = {s['name']: i for i, s in enumerate(sections)}
    raw_relocs = []
    for s in sections:
        if s['type'] == 4 and sections[s['info']]['name'] in SECTIONS:    # SHT_RELA
            for i in range(s['size'] // 24):
                off, info, addend = struct.unpack_from('<QQq', data, s['off'] + 24 * i)
                raw_relocs.append((s['info'], off, info & 0xffffffff, symbols[info >> 32], addend))
    plt_names = sorted({sym['name'] for _, _, typ, sym, _ in raw_relocs if typ == R_PLT32 and sym['shndx'] == 0})
    imports = sorted({s['name'] for s in symbols if s['shndx'] == 0 and s['name'] and s['bind'] != 0})

    # Layout: headers, .text, the PLT (16 bytes per stub), .rodata, .data, the GOT.
    image = bytearray(0x1000)
    base = {}
    for name in SECTIONS:
        if name not in by_name:
            continue
        s = sections[by_name[name]]
        start = align(len(image), max(s['align'], 16))
        image += bytes(start - len(image))
        base[by_name[name]] = start
        image += data[s['off']:s['off'] + s['size']]
        if name == '.text':
            plt_start = align(len(image), 16)
            plt_size = 16 * len(plt_names)
            image += b"\xcc" * (plt_start + plt_size - len(image))   # int3 padding: objdump stays in step
            data_start = align(len(image), 0x1000)
            image += bytes(data_start - len(image))

    def address(sym):
        return base[sym['shndx']] + sym['value'] if sym['shndx'] in base else None

    relocs_in = [(base[section] + off, typ, sym, addend) for section, off, typ, sym, addend in raw_relocs]
    got_names = sorted({sym['name'] if sym['shndx'] == 0 else f'@{address(sym)}'
                        for _, typ, sym, _ in relocs_in if typ in (R_GOTPCREL, R_GOTPCRELX, R_REX_GOTPCRELX)})
    got_start = align(len(image), 16)
    image += bytes(got_start - len(image) + 8 * (len(plt_names) + len(got_names)))
    got = {name: got_start + 8 * i for i, name in enumerate(plt_names + got_names)}
    plt = {name: plt_start + 16 * i for i, name in enumerate(plt_names)}

    dyn_relocs, plt_relocs = [], []
    dynsyms = [dict(name='', value=0, info=0, shndx=0, size=0)]
    dyn_index = {}

    def dynsym(name, defined_at=None, kind=2, size=0):
        if name not in dyn_index:
            dyn_index[name] = len(dynsyms)
            dynsyms.append(dict(name=name, value=defined_at or 0, info=(1 << 4) | (kind if defined_at else 0),
                                shndx=1 if defined_at else 0, size=size))
        return dyn_index[name]

    for s in symbols:   # exports: every global defined symbol
        if s['bind'] == 1 and s['shndx'] in base:
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
        elif typ == R_64:
            if target is None:
                dyn_relocs.append((off, R_64, dyn_index[sym['name']], addend))
            else:
                dyn_relocs.append((off, R_RELATIVE, 0, target + addend))
        else:
            raise SystemExit(f'relocation type {typ} not supported by the test linker')

    # Dynamic metadata after the data: .dynsym, .dynstr, .rela.dyn, .rela.plt, .dynamic.
    dynstr = StringTable()
    soname_index = dynstr.add(soname)
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
    dynamic = [(14, soname_index), (5, dynstr_off), (10, len(dynstr.data)), (6, dynsym_off), (11, 24),
               (7, rela_off), (8, 24 * len(dyn_relocs)), (9, 24), (23, jmprel_off), (2, 24 * len(plt_relocs)),
               (20, 7), (0, 0)]
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
    shstr_names = ['.text', '.data', '.dynsym', '.dynstr', '.symtab', '.strtab', '.shstrtab', '.dynamic']
    for n in shstr_names:
        shstr.add(n)
    shstr_off = place(bytes(shstr.data), 1)
    total = align(len(image), 8)
    image.extend(bytes(total - len(image)))
    shdrs = [(0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
             (shstr.index['.text'], 1, 6, 0x1000, 0x1000, plt_start + plt_size - 0x1000, 0, 0, 16, 0),
             (shstr.index['.data'], 1, 3, data_start, data_start, dynsym_off - data_start, 0, 0, 16, 0),
             (shstr.index['.dynsym'], 11, 2, dynsym_off, dynsym_off, 24 * len(dynsyms), 4, 1, 8, 24),
             (shstr.index['.dynstr'], 3, 2, dynstr_off, dynstr_off, len(dynstr.data), 0, 0, 1, 0),
             (shstr.index['.symtab'], 2, 0, 0, symtab_off, 24 * len(symtab), 6, 1, 8, 24),
             (shstr.index['.strtab'], 3, 0, 0, strtab_off, len(strtab.data), 0, 0, 1, 0),
             (shstr.index['.shstrtab'], 3, 0, 0, shstr_off, len(shstr.data), 0, 0, 1, 0),
             (shstr.index['.dynamic'], 6, 3, dynamic_off, dynamic_off, dynamic_size, 4, 0, 8, 16)]
    shoff = len(image)
    for h in shdrs:
        image += struct.pack('<IIQQQQIIQQ', *h)
    header = b'\x7fELF' + bytes([2, 1, 1]) + bytes(9)
    header += struct.pack('<HHIQQQIHHHHHH', 3, 62, 1, 0, 64, shoff, 0, 64, 56, 2, 64, len(shdrs), 7)
    image[:64] = header
    struct.pack_into('<IIQQQQQQ', image, 64, 1, 7, 0, 0, 0, total, total, 0x1000)
    struct.pack_into('<IIQQQQQQ', image, 64 + 56, 2, 6, dynamic_off, dynamic_off, dynamic_off,
                     dynamic_size, dynamic_size, 8)
    with open(out_path, 'wb') as f:
        f.write(image)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        raise SystemExit('usage: x64_link.py input.o output.so')
    import os
    link(sys.argv[1], sys.argv[2], os.path.basename(sys.argv[2]))
