"""tools/peprep64.py and lift64's PE support on the PE test program
(tests/pe_programs.s linked by tests/pe_link.py): what the layout records and
that MSVC's jump tables are recognised. The run itself is build/pe-check."""
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'tests'))
import lift64  # noqa: E402
import pe_link  # noqa: E402
import peprep64  # noqa: E402


@unittest.skipUnless(shutil.which('clang') and shutil.which('objdump'), 'needs clang and objdump')
class PeFixture(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.dir = tempfile.TemporaryDirectory()
        obj = os.path.join(cls.dir.name, 'pe_programs.obj')
        subprocess.run(['clang', '-target', 'x86_64-windows-msvc', '-c', str(ROOT / 'tests/pe_programs.s'), '-o', obj],
                       check=True)
        cls.dll = os.path.join(cls.dir.name, 'pefix.dll')
        pe_link.link(obj, cls.dll, 'pefix.dll', ['twice=pehost.dll', 'sum5=pehost.dll', 'seven=pehost.dll#7'],
                     ['pe_add5', 'pe_switch', 'pe_hidden_call'])
        cls.layout = peprep64.prepare(cls.dll, cls.dir.name)

    @classmethod
    def tearDownClass(cls):
        cls.dir.cleanup()

    def test_read_in_place(self):
        self.assertEqual(self.layout['format'], 'pe')
        self.assertIsNone(self.layout['image'])          # no copy of the module is written
        self.assertEqual(sorted(os.listdir(self.dir.name)), ['pe_programs.obj', 'pefix.dll', 'pefix.dll.layout.json'])

    def test_imports_exports(self):
        names = [i['name'] for i in self.layout['imports']]
        self.assertIn('pehost.dll!twice', names)
        self.assertIn('pehost.dll!#7', names)
        self.assertIn('rt64!guard_dispatch_icall', names)
        self.assertIn('pefix.dll!pe_add5', self.layout['exports'])
        self.assertIn('pefix.dll!#1', self.layout['exports'])

    def test_tls_guard_pdata(self):
        self.assertEqual(self.layout['tls']['filesz'], 8)
        self.assertTrue(self.layout['tls']['index'])
        self.assertTrue(self.layout['guard']['dispatch'])
        hidden = self.layout['pdata'][0][0]
        self.assertIn(hidden, self.layout['entries'])     # known only from .pdata
        self.assertNotIn(hidden, self.layout['exports'].values())

    def test_excluded_section_is_not_code(self):
        with tempfile.TemporaryDirectory() as out:
            layout = peprep64.prepare(self.dll, out, exclude=('.text',))
        self.assertEqual(layout['code'], [])
        self.assertTrue(next(s for s in layout['sections'] if s['name'] == '.text')['excluded'])

    def test_msvc_jump_table(self):
        bodies, report = lift64.lift_module(self.layout, [], every=True, sweep=False)
        self.assertEqual(report['unsupported'], 0)
        self.assertGreaterEqual(report['table_jumps'], 1)
        self.assertEqual(report['dispatch_jumps'], 0)


if __name__ == '__main__':
    unittest.main()
