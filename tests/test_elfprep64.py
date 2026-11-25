"""tools/elfprep64.py: ELF64 headers, segments, image and relocations."""
import json
import os
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import elfprep64  # noqa: E402


def tiny_elf64():
    """A minimal x86-64 ET_DYN: one PT_LOAD (with .bss), a PT_DYNAMIC, two
    RELATIVE relocations, a DT_INIT_ARRAY and no section headers."""
    ehsize, phsize = 64, 56
    dyn_off, rela_off, data_off, str_off = 0x100, 0x200, 0x300, 0x380
    dynamic = [(5, str_off), (10, 1), (7, rela_off), (8, 48), (9, 24),
               (25, data_off), (27, 8), (0, 0)]
    blob = bytearray(0x400)
    for i, (tag, value) in enumerate(dynamic):
        struct.pack_into('<qQ', blob, dyn_off + 16 * i, tag, value)
    struct.pack_into('<QQq', blob, rela_off, data_off, 8, 0x1234)        # init_array[0]
    struct.pack_into('<QQq', blob, rela_off + 24, data_off + 8, 8, 0x10)  # data pointer
    header = bytearray(b'\x7fELF' + bytes([2, 1, 1]) + bytes(9))
    header += struct.pack('<HHIQQQIHHHHHH', 3, 62, 1, 0, ehsize, 0, 0, ehsize, phsize, 2, 64, 0, 0)
    blob[:ehsize] = header
    struct.pack_into('<IIQQQQQQ', blob, ehsize, 1, 6, 0, 0, 0, 0x400, 0x1000, 0x1000)
    struct.pack_into('<IIQQQQQQ', blob, ehsize + phsize, 2, 6, dyn_off, dyn_off, dyn_off, 16 * len(dynamic),
                     16 * len(dynamic), 8)
    return bytes(blob)


class ElfPrep64(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        self.path = os.path.join(self.dir.name, 'libtiny.so')
        with open(self.path, 'wb') as f:
            f.write(tiny_elf64())

    def tearDown(self):
        self.dir.cleanup()

    def test_reads_headers_segments_and_relocations(self):
        elf = elfprep64.Elf64(self.path)
        self.assertEqual((elf.e_type, elf.e_machine, elf.size), (3, 62, 0x1000))
        self.assertEqual([(o, t, a) for o, t, _, a in elf.relocations()], [(0x300, 8, 0x1234), (0x308, 8, 0x10)])
        image = elf.image()
        self.assertEqual(len(image), 0x1000)               # .bss included, zero-filled
        self.assertEqual(image[:4], b'\x7fELF')
        self.assertEqual(bytes(image[0x400:]), bytes(0xc00))

    def test_prepare_writes_image_and_layout(self):
        out = os.path.join(self.dir.name, 'out')
        info = elfprep64.prepare(self.path, out)
        self.assertEqual(info['init_array'], [0x1234])     # the RELATIVE addend, not the file bytes
        self.assertEqual(info['relocation_counts'], {'RELATIVE': 2})
        self.assertEqual(os.path.getsize(os.path.join(out, 'libtiny.so.image')), 0x1000)
        with open(os.path.join(out, 'libtiny.so.layout.json')) as f:
            self.assertEqual(json.load(f)['size'], 0x1000)

    def test_rejects_elf32(self):
        with open(self.path, 'r+b') as f:
            f.seek(4)
            f.write(b'\x01')
        with self.assertRaisesRegex(ValueError, 'ELF64'):
            elfprep64.Elf64(self.path)


if __name__ == '__main__':
    unittest.main()
