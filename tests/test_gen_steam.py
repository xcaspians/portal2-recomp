"""tools/gen_steam.py: struct_sizes() gives the pack(4) size of the plain Steam structures
copied as is between the guest (i386) and Steam (arm64). Synthetic Steamworks.NET-style
sources, so that no SDK download is needed."""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import gen_steam  # noqa: E402

AUTOGEN = 'Assets/com.rlabrecque.steamworks.net/Runtime/autogen/'
CONSTANTS = '\t\tpublic const int k_cchTitle = 128 + 1;\n'
STRUCTS = '''
\tpublic struct SteamUGCDetails_t {
\t\tpublic ulong m_nPublishedFileId;
\t\tpublic EResult m_eResult;
\t\t[MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.k_cchTitle)]
\t\tprivate byte[] m_rgchTitle_;
\t\tpublic ulong m_ulSteamIDOwner;\t\t// aligned on 4, not 8, under pack(4)
\t\t[MarshalAs(UnmanagedType.I1)]
\t\tpublic bool m_bBanned;
\t\tpublic CSteamID m_steamID;
\t\tpublic float m_flScore;
\t\t[MarshalAs(UnmanagedType.I1)]
\t\tpublic bool m_bLast;
\t}
'''


class StructSizes(unittest.TestCase):
    def test_pack4_layout(self):
        with tempfile.TemporaryDirectory() as tmp:
            files = {}
            for name, text in (('SteamConstants.cs', CONSTANTS), ('SteamStructs.cs', STRUCTS)):
                path = Path(tmp) / name
                path.write_text(text)
                files[AUTOGEN + name] = path
            sizes = gen_steam.struct_sizes(files, {'EResult': 'int'})
        # 8 + 4 + 129 = 141 -> 144, + 8 = 152, + 1 = 153 -> 156, + 8 + 4 + 1 = 169, rounded to 172.
        self.assertEqual(sizes, {'SteamUGCDetails_t': 172})


class DisassembleI386(unittest.TestCase):
    def test_slots_and_stack_words(self):
        import subprocess
        from unittest import mock
        # Real objdump -d shapes from SDK 1.40 i386 flat thunks (esp-relative, ebp-frame, 8-byte load).
        disasm = '\n'.join([
            '00010000 <SteamAPI_ISteamUser_BLoggedOn>:',
            '   10000: 8b 44 24 04\tmovl\t0x4(%esp), %eax',
            '   10004: 8b 08\tmovl\t(%eax), %ecx',
            '   10006: ff 61 04\tjmpl\t*0x4(%ecx)',
            '00010010 <SteamAPI_ISteamGameServer_EnableHeartbeats>:',
            '   10010: 55\tpushl\t%ebp',
            '   10011: 89 e5\tmovl\t%esp, %ebp',
            '   10013: 8b 45 08\tmovl\t0x8(%ebp), %eax',
            '   10016: 0f b6 55 0c\tmovzbl\t0xc(%ebp), %edx',
            '   1001a: 8b 08\tmovl\t(%eax), %ecx',
            '   1001c: ff 91 9c 00 00 00\tcalll\t*0x9c(%ecx)',
            '00010030 <SteamAPI_ISteamUserStats_SetStat0>:',
            '   10030: 83 ec 10\tsubl\t$0x10, %esp',
            '   10033: 8b 44 24 14\tmovl\t0x14(%esp), %eax',
            '   10037: f2 0f 10 44 24 1c\tmovsd\t0x1c(%esp), %xmm0',
            '   1003d: 8b 08\tmovl\t(%eax), %ecx',
            '   1003f: ff 51 10\tcalll\t*0x10(%ecx)',
        ]) + '\n'
        fake = subprocess.CompletedProcess([], 0, disasm, '')
        with mock.patch.object(gen_steam.subprocess, 'run', return_value=fake):
            got = gen_steam.disassemble_i386(Path('fake.so'))
        self.assertEqual(got['SteamAPI_ISteamUser_BLoggedOn'], (1, 1))
        self.assertEqual(got['SteamAPI_ISteamGameServer_EnableHeartbeats'], (39, 2))
        self.assertEqual(got['SteamAPI_ISteamUserStats_SetStat0'], (4, 4))


if __name__ == '__main__':
    unittest.main()
