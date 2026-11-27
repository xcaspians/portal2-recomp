#!/usr/bin/env python3
"""Read a PE32+ (Windows x86-64) executable or DLL for the 64-bit lifter.

The PE counterpart of tools/elfprep64.py: the same layout keys, so
tools/lift64.py and the runtime loader (runtime/x64/loader64.c) treat both
formats alike. Module offsets are RVAs (address - ImageBase).

Unlike elfprep64, no memory image is written: the runtime maps the module
section by section from the original file, read in place (the layout's
`sections`), so a game's executable is never copied. Sections named with
--exclude (a DRM stub's `.bind`, for example) are neither lifted nor mapped:
they stay zeros in memory.

What the layout records:
  * sections (RVA, virtual size, file offset and size, characteristics);
  * code ranges: executable sections, minus the excluded ones;
  * base relocations (DIR64) as RELATIVE relocations, the slot's value
    minus ImageBase as the addend;
  * imports by name and by ordinal, one import per IAT slot, named
    `dll!function` (the DLL in lower case; `dll!#ordinal` by ordinal), as
    `64` relocations of the slot; MSVC data imports (`?name@@3...`) are
    marked as objects;
  * exports by name (`exports`) and every exported RVA as an entry point;
  * the static TLS block (template, zero fill, the index slot);
  * .pdata: every RUNTIME_FUNCTION start is an entry point, and the table
    is kept (`pdata`) for the unwinder;
  * Control Flow Guard: the check and dispatch pointer slots of the load
    configuration, filled at load time by the runtime's
    `rt64!guard_check_icall` and `rt64!guard_dispatch_icall`;
  * `ImageBase` in the optional header, rewritten with the load address.
"""
import argparse
import json
import os
import struct

IMAGE_SCN_CNT_CODE, IMAGE_SCN_MEM_EXECUTE = 0x20, 0x20000000
DIR_EXPORT, DIR_IMPORT, DIR_EXCEPTION, DIR_BASERELOC, DIR_TLS, DIR_LOAD_CONFIG, DIR_DELAY_IMPORT = 0, 1, 3, 5, 9, 10, 13
REL_DIR64, REL_ABSOLUTE = 10, 0
GUARD_CHECK, GUARD_DISPATCH = 'rt64!guard_check_icall', 'rt64!guard_dispatch_icall'


class Pe64:
    """A PE32+ x86-64 file read into memory (never written)."""

    def __init__(self, path, exclude=()):
        self.path = path
        self.name = os.path.basename(path).lower()
        with open(path, 'rb') as f:
            self.data = bytes(f.read())
        d = self.data
        if d[:2] != b'MZ':
            raise ValueError(f'{path}: not a PE file (no MZ header)')
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        if d[pe:pe + 4] != b'PE\0\0':
            raise ValueError(f'{path}: no PE signature')
        (self.machine, count, self.timestamp, _, _, opt_size, self.characteristics) = \
            struct.unpack_from('<HHIIIHH', d, pe + 4)
        if self.machine != 0x8664:
            raise ValueError(f'{path}: machine {self.machine:#x}, expected x86-64 (0x8664)')
        opt = pe + 24
        if struct.unpack_from('<H', d, opt)[0] != 0x20b:
            raise ValueError(f'{path}: not PE32+')
        self.entry = struct.unpack_from('<I', d, opt + 16)[0]
        self.image_base = struct.unpack_from('<Q', d, opt + 24)[0]
        self.image_base_field = opt + 24            # file offset = RVA inside the headers
        self.size = struct.unpack_from('<I', d, opt + 56)[0]
        self.header_size = struct.unpack_from('<I', d, opt + 60)[0]
        self.subsystem, self.dll_characteristics = struct.unpack_from('<HH', d, opt + 68)
        ndirs = struct.unpack_from('<I', d, opt + 108)[0]
        self.dirs = [struct.unpack_from('<II', d, opt + 112 + 8 * i) for i in range(min(ndirs, 16))]
        self.dirs += [(0, 0)] * (16 - len(self.dirs))
        self.sections = []
        at = opt + opt_size
        for i in range(count):
            raw_name, vsize, rva, raw_size, raw_off, _, _, _, _, chars = struct.unpack_from('<8sIIIIIIHHI', d, at + 40 * i)
            self.sections.append(dict(name=raw_name.rstrip(b'\0').decode('latin-1'), rva=rva, vsize=vsize,
                                      raw_size=raw_size, raw_off=raw_off, characteristics=chars,
                                      excluded=raw_name.rstrip(b'\0').decode('latin-1') in exclude))

    def section_of(self, rva):
        for s in self.sections:
            if s['rva'] <= rva < s['rva'] + max(s['vsize'], s['raw_size']):
                return s
        return None

    def r2o(self, rva):
        """File offset of an RVA, or None (headers map 1:1, past raw data reads as zeros)."""
        if rva < self.header_size:
            return rva
        s = self.section_of(rva)
        if s is None or rva - s['rva'] >= s['raw_size']:
            return None
        return s['raw_off'] + rva - s['rva']

    def u16(self, rva):
        o = self.r2o(rva)
        return struct.unpack_from('<H', self.data, o)[0] if o is not None else 0

    def u32(self, rva):
        o = self.r2o(rva)
        return struct.unpack_from('<I', self.data, o)[0] if o is not None else 0

    def u64(self, rva):
        o = self.r2o(rva)
        return struct.unpack_from('<Q', self.data, o)[0] if o is not None else 0

    def cstr(self, rva):
        o = self.r2o(rva)
        if o is None:
            return ''
        return self.data[o:self.data.index(b'\0', o)].decode('latin-1')

    def image(self):
        """The module as Windows maps it (unrelocated), excluded sections zeroed. In memory only."""
        out = bytearray(self.size)
        out[:self.header_size] = self.data[:self.header_size]
        for s in self.sections:
            if s['excluded']:
                continue
            n = min(s['raw_size'], s['vsize'] or s['raw_size'])
            out[s['rva']:s['rva'] + n] = self.data[s['raw_off']:s['raw_off'] + n]
        return out

    def code_ranges(self):
        return sorted((s['rva'], s['rva'] + (s['vsize'] or s['raw_size'])) for s in self.sections
                      if s['characteristics'] & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE) and not s['excluded'])

    def base_relocations(self):
        """RVAs of the DIR64 base relocations."""
        rva, size = self.dirs[DIR_BASERELOC]
        out, end = [], rva + size
        while rva + 8 <= end:
            page, block = self.u32(rva), self.u32(rva + 4)
            if block < 8:
                break
            for i in range((block - 8) // 2):
                entry = self.u16(rva + 8 + 2 * i)
                kind, offset = entry >> 12, entry & 0xfff
                if kind == REL_DIR64:
                    out.append(page + offset)
                elif kind != REL_ABSOLUTE:
                    raise ValueError(f'{self.path}: base relocation type {kind} at {page + offset:#x}')
            rva += block
        return out

    def imports(self):
        """[(IAT slot RVA, dll, name or None, ordinal or None)]."""
        rva, size = self.dirs[DIR_IMPORT]
        out = []
        while rva and size:
            lookup, _, _, name_rva, iat = (self.u32(rva + 4 * k) for k in range(5))
            if not (lookup or name_rva or iat):
                break
            dll = self.cstr(name_rva).lower()
            table = lookup or iat
            for k in range(1 << 20):
                entry = self.u64(table + 8 * k)
                if not entry:
                    break
                if entry >> 63:
                    out.append((iat + 8 * k, dll, None, entry & 0xffff))
                else:
                    out.append((iat + 8 * k, dll, self.cstr((entry & 0x7fffffff) + 2), None))
            rva += 20
        return out

    def delay_imports(self):
        rva, size = self.dirs[DIR_DELAY_IMPORT]
        return [] if not (rva and size) else [rva]

    def exports(self):
        """({name: RVA}, [every exported RVA], {ordinal: RVA}); forwarders are skipped."""
        rva, size = self.dirs[DIR_EXPORT]
        if not (rva and size):
            return {}, [], {}
        base, nfun, nnames, funcs, names, ordinals = (self.u32(rva + 16), self.u32(rva + 20), self.u32(rva + 24),
                                                      self.u32(rva + 28), self.u32(rva + 32), self.u32(rva + 36))
        addresses = [self.u32(funcs + 4 * i) for i in range(nfun)]
        forwarder = lambda a: rva <= a < rva + size
        named = {}
        for i in range(nnames):
            index = self.u16(ordinals + 2 * i)
            if index < nfun and addresses[index] and not forwarder(addresses[index]):
                named[self.cstr(self.u32(names + 4 * i))] = addresses[index]
        by_ordinal = {base + i: a for i, a in enumerate(addresses) if a and not forwarder(a)}
        return named, sorted(set(by_ordinal.values())), by_ordinal

    def pdata(self):
        """[(begin, end, unwind info RVA)] of the exception directory."""
        rva, size = self.dirs[DIR_EXCEPTION]
        out = []
        for i in range(size // 12):
            begin, end, unwind = self.u32(rva + 12 * i), self.u32(rva + 12 * i + 4), self.u32(rva + 12 * i + 8)
            if begin:
                out.append((begin, end, unwind))
        return out

    def tls(self):
        rva, size = self.dirs[DIR_TLS]
        if not (rva and size):
            return None
        start, end, index, callbacks = (self.u64(rva + 8 * k) for k in range(4))
        zero_fill, characteristics = self.u32(rva + 32), self.u32(rva + 36)
        align = 1 << (((characteristics >> 20) & 0xf) - 1) if (characteristics >> 20) & 0xf else 16
        callback_list = []
        if callbacks:
            at = callbacks - self.image_base
            while self.u64(at):
                callback_list.append(self.u64(at) - self.image_base)
                at += 8
        filesz = end - start if end >= start else 0
        return dict(vaddr=start - self.image_base if start else 0, filesz=filesz, memsz=filesz + zero_fill,
                    align=align, index=index - self.image_base if index else 0, callbacks=callback_list)

    def load_config(self):
        """{'check': slot RVA, 'dispatch': slot RVA, 'cookie': RVA, 'guard_flags': flags} (zeros if absent)."""
        rva, size = self.dirs[DIR_LOAD_CONFIG]
        out = dict(check=0, dispatch=0, cookie=0, guard_flags=0)
        if not rva:
            return out
        length = self.u32(rva)
        va = lambda at: (self.u64(rva + at) - self.image_base) if length >= at + 8 and self.u64(rva + at) else 0
        out.update(cookie=va(0x58), check=va(0x70), dispatch=va(0x78))
        if length >= 0x94:
            out['guard_flags'] = self.u32(rva + 0x90)
        return out


def unwind_handler(pe, unwind_rva):
    """RVA of the language-specific handler of an UNWIND_INFO, or 0 (chained or none)."""
    o = pe.r2o(unwind_rva)
    if o is None:
        return 0
    version_flags, _, count, _ = struct.unpack_from('<BBBB', pe.data, o)
    flags = version_flags >> 3
    if not flags & 3:          # UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER
        return 0
    at = unwind_rva + 4 + 2 * ((count + 1) & ~1)
    return pe.u32(at)


def is_data_import(name):
    """MSVC-mangled variables (`?cout@std@@3V...`, `?_Fac_tidy_reg@std@@3U...`) are data, not code."""
    return bool(name) and name.startswith('?') and '@@3' in name


def layout(pe, real_entry=None):
    """Everything the lifter and the runtime loader need, as a JSON-ready dict (elfprep64's keys)."""
    code = pe.code_ranges()
    in_code = lambda a: any(lo <= a < hi for lo, hi in code)
    relocs, imports, counts = [], [], {}
    for at in pe.base_relocations():
        value = pe.u64(at)
        relocs.append(dict(off=at, type='RELATIVE', addend=value - pe.image_base))
    counts['DIR64'] = len(relocs)
    for slot, dll, name, ordinal in pe.imports():
        full = f'{dll}!{name}' if name is not None else f'{dll}!#{ordinal}'
        imports.append(dict(name=full, weak=False, object=is_data_import(name)))
        relocs.append(dict(off=slot, type='64', addend=0, symbol=full, import_=len(imports) - 1))
    counts['IMPORT'] = len(imports)
    guard = pe.load_config()
    for key, name in (('check', GUARD_CHECK), ('dispatch', GUARD_DISPATCH)):
        if guard[key]:
            # The slot holds a pointer the loader fills: replace any base relocation of it.
            relocs = [r for r in relocs if r['off'] != guard[key]]
            imports.append(dict(name=name, weak=False, object=False))
            relocs.append(dict(off=guard[key], type='64', addend=0, symbol=name, import_=len(imports) - 1))
    for r in relocs:
        if 'import_' in r:
            r['import'] = r.pop('import_')
    # The optional header's ImageBase holds the real load address, as Windows' loader leaves it.
    relocs.append(dict(off=pe.image_base_field, type='RELATIVE', addend=0))
    relocs.sort(key=lambda r: r['off'])

    plain, exported, by_ordinal = pe.exports()
    # Exports carry the module's name, like its imports in other modules (`dll!name`, `dll!#ordinal`).
    named = {f'{pe.name}!{n}': a for n, a in plain.items()}
    named.update({f'{pe.name}!#{o}': a for o, a in by_ordinal.items()})
    pdata = pe.pdata()
    entries = {b for b, _, _ in pdata if in_code(b)}
    handlers = set()
    for _, _, unwind in pdata:
        h = unwind_handler(pe, unwind)
        if h and in_code(h):
            handlers.add(h)
    entries |= handlers
    entries |= {a for a in exported if in_code(a)}
    entry = real_entry if real_entry is not None else pe.entry
    if entry and in_code(entry):
        entries.add(entry)
    tls = pe.tls()
    if tls:
        entries.update(cb for cb in tls['callbacks'] if in_code(cb))
    data_ptrs = sorted({r['addend'] for r in relocs if r['type'] == 'RELATIVE' and r['addend'] and in_code(r['addend'])
                        and not in_code(r['off'])})
    entries.update(data_ptrs)
    names = {a: n for n, a in named.items()}
    return dict(
        format='pe', name=pe.name, source=os.path.abspath(pe.path), image=None, image_base=pe.image_base,
        size=pe.size, type=0, subsystem=pe.subsystem,
        sections=[dict(name=s['name'], rva=s['rva'], vsize=s['vsize'], raw_off=s['raw_off'], raw_size=s['raw_size'],
                       characteristics=s['characteristics'], excluded=s['excluded']) for s in pe.sections],
        header_size=pe.header_size,
        segments=[], code=[list(r) for r in code], plt=[],
        tls=dict(vaddr=tls['vaddr'], filesz=tls['filesz'], memsz=tls['memsz'], align=tls['align'],
                 index=tls['index']) if tls else None,
        tls_callbacks=tls['callbacks'] if tls else [],
        init=0, fini=0, entry=entry, original_entry=pe.entry,
        init_array=[], fini_array=[],
        entries=sorted(entries), data_ptrs=data_ptrs,
        names={hex(k): v for k, v in sorted(names.items())},
        exports=dict(sorted(named.items())), export_types={n: 2 for n in named},
        needed=sorted({i['name'].split('!')[0] for i in imports if not i['name'].startswith('rt64!')}),
        imports=imports, relocations=relocs, relocation_counts=counts,
        landing_pads={}, pdata=[list(p) for p in pdata], handlers=sorted(handlers),
        guard=guard, delay_imports=len(pe.delay_imports()))


def prepare(path, out_dir, exclude=(), real_entry=None):
    pe = Pe64(path, exclude)
    os.makedirs(out_dir, exist_ok=True)
    info = layout(pe, real_entry)
    with open(os.path.join(out_dir, pe.name + '.layout.json'), 'w') as f:
        json.dump(info, f, indent=1)
    return info


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--out', required=True, help='output directory (layout only: the module is read in place)')
    ap.add_argument('--exclude', action='append', default=[], help='section never lifted nor mapped (repeatable)')
    ap.add_argument('--entry', type=lambda s: int(s, 0), help='entry point RVA to use instead of the header\'s')
    ap.add_argument('module', help='PE32+ x86-64 .exe or .dll')
    a = ap.parse_args()
    info = prepare(a.module, a.out, a.exclude, a.entry)
    print(f'{info["name"]}: {info["size"]:#x} bytes at {info["image_base"]:#x}, {len(info["entries"])} entries '
          f'({len(info["pdata"])} in .pdata), {len(info["data_ptrs"])} code pointers in data, '
          f'{len(info["imports"])} imports from {len(info["needed"])} DLLs, relocations {info["relocation_counts"]}')


if __name__ == '__main__':
    main()
