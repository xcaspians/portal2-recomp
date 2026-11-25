#!/usr/bin/env python3
"""Read an x86-64 ELF64 shared object for the 64-bit lifter (tools/lift64.py).

The 64-bit counterpart of tools/elfprep.py, which stays i386-only. Guest
memory is host memory here (both sides are LP64 little-endian), so the module
is not relocated offline at a fixed address: the runtime (runtime/x64/rt64.c)
maps it wherever the host allows and applies the relocations recorded here at
load time.

Output (--out DIR): DIR/<name>.image, the module's memory image from address 0
(file bytes of every PT_LOAD, zeros for .bss), and DIR/<name>.layout.json:
segments, sections, symbols, entry points, relocations (with their symbol
names), imports, init/fini arrays and the TLS segment.
"""
import argparse
import json
import os
import struct

PT_LOAD, PT_DYNAMIC, PT_TLS = 1, 2, 7
SHF_EXECINSTR = 4
STT_FUNC, STT_TLS = 2, 6
DT_NEEDED, DT_STRTAB, DT_RELA, DT_RELASZ, DT_PLTRELSZ, DT_JMPREL = 1, 5, 7, 8, 2, 23
DT_INIT, DT_FINI, DT_INIT_ARRAY, DT_FINI_ARRAY, DT_INIT_ARRAYSZ, DT_FINI_ARRAYSZ = 12, 13, 25, 26, 27, 28
DT_SONAME = 14

R_X86_64 = {0: 'NONE', 1: '64', 2: 'PC32', 5: 'COPY', 6: 'GLOB_DAT', 7: 'JUMP_SLOT',
            8: 'RELATIVE', 16: 'DTPMOD64', 17: 'DTPOFF64', 18: 'TPOFF64', 37: 'IRELATIVE'}


class Elf64:
    """An ELF64 x86-64 file (ET_DYN or ET_EXEC) read into memory."""

    def __init__(self, path):
        self.path = path
        self.name = os.path.basename(path)
        with open(path, 'rb') as f:
            self.data = bytes(f.read())
        d = self.data
        if d[:4] != b'\x7fELF':
            raise ValueError(f'{path}: not an ELF file')
        if d[4] != 2 or d[5] != 1:
            raise ValueError(f'{path}: not a little-endian ELF64 file (i386 modules go through tools/elfprep.py)')
        (self.e_type, self.e_machine, _, self.e_entry, self.e_phoff, self.e_shoff, _, _,
         self.e_phentsize, self.e_phnum, self.e_shentsize, self.e_shnum, self.e_shstrndx) = \
            struct.unpack_from('<HHIQQQIHHHHHH', d, 16)
        if self.e_machine != 62:
            raise ValueError(f'{path}: machine {self.e_machine}, expected x86-64 (62)')
        self.phdrs = [dict(zip(('type', 'flags', 'off', 'vaddr', 'paddr', 'filesz', 'memsz', 'align'),
                               struct.unpack_from('<IIQQQQQQ', d, self.e_phoff + i * self.e_phentsize)))
                      for i in range(self.e_phnum)]
        self.shdrs = [dict(zip(('name', 'type', 'flags', 'addr', 'off', 'size', 'link', 'info', 'align', 'entsize'),
                               struct.unpack_from('<IIQQQQIIQQ', d, self.e_shoff + i * self.e_shentsize)))
                      for i in range(self.e_shnum)] if self.e_shoff else []
        self.secs = {}
        if self.shdrs:
            names = self.shdrs[self.e_shstrndx]['off']
            for s in self.shdrs:
                s['name'] = self.cstr(names + s['name'])
                self.secs.setdefault(s['name'], s)
        self.loads = [p for p in self.phdrs if p['type'] == PT_LOAD]
        self.tls = next((p for p in self.phdrs if p['type'] == PT_TLS), None)
        self.size = max(p['vaddr'] + p['memsz'] for p in self.loads)
        self._parse_dynamic()

    def cstr(self, off):
        return self.data[off:self.data.index(b'\0', off)].decode('utf-8', 'replace')

    def v2o(self, va):
        for p in self.loads:
            if p['vaddr'] <= va < p['vaddr'] + p['filesz']:
                return p['off'] + va - p['vaddr']
        return None

    def u64(self, va):
        o = self.v2o(va)
        return struct.unpack_from('<Q', self.data, o)[0] if o is not None else 0

    def _symbols(self, section):
        if section is None:
            return []
        strings = self.shdrs[section['link']]['off']
        out = []
        for i in range(section['size'] // 24):
            nm, info, other, shndx, value, size = struct.unpack_from('<IBBHQQ', self.data, section['off'] + 24 * i)
            out.append(dict(name=self.cstr(strings + nm), value=value, size=size, bind=info >> 4,
                            type=info & 15, shndx=shndx, visibility=other & 3))
        return out

    def _parse_dynamic(self):
        self.dyn, self.needed = {}, []
        dynamic = next((p for p in self.phdrs if p['type'] == PT_DYNAMIC), None)
        if dynamic:
            o = dynamic['off']
            while True:
                tag, val = struct.unpack_from('<qQ', self.data, o)
                o += 16
                if tag == 0:
                    break
                if tag == DT_NEEDED:
                    self.needed.append(val)
                else:
                    self.dyn.setdefault(tag, val)
            strtab = self.v2o(self.dyn[DT_STRTAB])
            self.needed = [self.cstr(strtab + n) for n in self.needed]
            if DT_SONAME in self.dyn:
                self.soname = self.cstr(strtab + self.dyn[DT_SONAME])
        self.dynsyms = self._symbols(self.secs.get('.dynsym'))
        self.symtab = self._symbols(self.secs.get('.symtab'))

    def relocations(self):
        """(offset, type, symbol or None, addend) of DT_RELA then DT_JMPREL."""
        out = []
        for start, size in ((DT_RELA, DT_RELASZ), (DT_JMPREL, DT_PLTRELSZ)):
            if start not in self.dyn:
                continue
            o = self.v2o(self.dyn[start])
            for i in range(self.dyn[size] // 24):
                off, info, addend = struct.unpack_from('<QQq', self.data, o + 24 * i)
                index = info >> 32
                out.append((off, info & 0xffffffff, self.dynsyms[index] if index else None, addend))
        return out

    def exports(self):
        return {s['name']: s for s in self.dynsyms
                if s['shndx'] != 0 and s['name'] and s['bind'] in (1, 2, 10) and s['visibility'] in (0, 3)}

    def image(self):
        """Memory image from address 0: file bytes, zeros past filesz."""
        out = bytearray(self.size)
        for p in self.loads:
            out[p['vaddr']:p['vaddr'] + p['filesz']] = self.data[p['off']:p['off'] + p['filesz']]
        return out

    def code_ranges(self):
        """Executable sections (by name order), as [start, end) module offsets."""
        return sorted((s['addr'], s['addr'] + s['size']) for s in self.shdrs
                      if s['flags'] & SHF_EXECINSTR and s['size'] and s['addr'])


def layout(elf, image_path=None):
    """Everything the lifter and the runtime loader need, as a JSON-ready dict."""
    exports = elf.exports()
    code = elf.code_ranges()
    in_code = lambda a: any(lo <= a < hi for lo, hi in code)
    relocs, imports, counts = [], [], {}
    import_index = {}
    for off, typ, sym, addend in elf.relocations():
        kind = R_X86_64.get(typ, str(typ))
        counts[kind] = counts.get(kind, 0) + 1
        if kind == 'NONE':
            continue
        entry = dict(off=off, type=kind, addend=addend)
        if sym is not None:
            entry['symbol'] = sym['name']
            local = sym['shndx'] != 0 and (sym['bind'] == 0 or sym['visibility'] in (1, 2, 3) or sym['name'] in exports)
            if local:
                entry['value'] = sym['value']   # defined in this module: base + value
            elif kind not in ('DTPMOD64',):
                if sym['name'] not in import_index:
                    import_index[sym['name']] = len(imports)
                    imports.append(dict(name=sym['name'], weak=sym['bind'] == 2, object=sym['type'] == 1))
                entry['import'] = import_index[sym['name']]
        relocs.append(entry)

    names, entries = {}, set()
    for s in elf.symtab + elf.dynsyms:
        if s['type'] == STT_FUNC and s['shndx'] != 0 and s['value'] and in_code(s['value']):
            names.setdefault(s['value'], s['name'])
            if not s['name'].endswith('.cold') and '.cold.' not in s['name']:
                entries.add(s['value'])
    for tag in (DT_INIT, DT_FINI):
        if tag in elf.dyn:
            entries.add(elf.dyn[tag])

    # Code pointers stored in data (vtables, callback tables): RELATIVE and
    # 64 relocations whose target is code and whose slot is not.
    def slot_value(r):
        if r['type'] == 'RELATIVE':
            return r['addend']
        if r['type'] in ('64', 'GLOB_DAT', 'JUMP_SLOT') and 'value' in r:
            return r['value'] + (r['addend'] if r['type'] == '64' else 0)
        return None
    slots = {r['off']: slot_value(r) for r in relocs}
    data_ptrs = sorted({v for off, v in slots.items() if v is not None and in_code(v) and not in_code(off)})

    def array(tag, size_tag):
        if tag not in elf.dyn:
            return []
        start = elf.dyn[tag]
        return [slots.get(start + 8 * i) if slots.get(start + 8 * i) is not None else elf.u64(start + 8 * i)
                for i in range(elf.dyn[size_tag] // 8)]
    init_array, fini_array = array(DT_INIT_ARRAY, DT_INIT_ARRAYSZ), array(DT_FINI_ARRAY, DT_FINI_ARRAYSZ)
    entries.update(x for x in init_array + fini_array if x and x != 0xffffffffffffffff)
    entries.update(data_ptrs)
    plt = [elf.secs[n] for n in ('.plt', '.plt.sec', '.plt.got') if n in elf.secs]
    return dict(
        name=getattr(elf, 'soname', elf.name), source=os.path.abspath(elf.path),
        image=os.path.abspath(image_path) if image_path else None, size=elf.size, type=elf.e_type,
        segments=[dict(vaddr=p['vaddr'], filesz=p['filesz'], memsz=p['memsz'], flags=p['flags']) for p in elf.loads],
        code=[list(r) for r in code],
        plt=[[s['addr'], s['addr'] + s['size']] for s in plt],
        tls=dict(vaddr=elf.tls['vaddr'], filesz=elf.tls['filesz'], memsz=elf.tls['memsz'],
                 align=elf.tls['align']) if elf.tls else None,
        init=elf.dyn.get(DT_INIT, 0), fini=elf.dyn.get(DT_FINI, 0),
        init_array=init_array, fini_array=fini_array,
        entries=sorted(entries), data_ptrs=data_ptrs,
        names={hex(k): v for k, v in sorted(names.items())},
        exports={n: s['value'] for n, s in sorted(exports.items())},
        export_types={n: s['type'] for n, s in sorted(exports.items())},
        needed=elf.needed, imports=imports, relocations=relocs, relocation_counts=counts)


def prepare(path, out_dir):
    elf = Elf64(path)
    os.makedirs(out_dir, exist_ok=True)
    name = getattr(elf, 'soname', elf.name)
    image_path = os.path.join(out_dir, name + '.image')
    with open(image_path, 'wb') as f:
        f.write(elf.image())
    info = layout(elf, image_path)
    with open(os.path.join(out_dir, name + '.layout.json'), 'w') as f:
        json.dump(info, f, indent=1)
    return info


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--out', required=True, help='output directory (image and layout)')
    ap.add_argument('module', help='x86-64 ELF64 .so')
    a = ap.parse_args()
    info = prepare(a.module, a.out)
    print(f'{info["name"]}: {info["size"]:#x} bytes, {len(info["entries"])} entries, '
          f'{len(info["data_ptrs"])} code pointers in data, {len(info["imports"])} imports, '
          f'relocations {info["relocation_counts"]}')


if __name__ == '__main__':
    main()
