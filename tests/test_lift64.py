"""tools/lift64.py: operands, register widths, RIP-relative addresses and
control flow of the x86-64 translator (the C it emits is run by
build/x64-check)."""
import collections
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import lift64  # noqa: E402


class FakeModule:
    """Just what translate() reads from a Module."""
    def __init__(self, insns=None, entries=(), slots=None):
        self.insns = insns or {}
        self.entries = set(entries)
        self.slots = slots or {}
        self.table_jumps = self.dispatch_jumps = 0

    def jump_table(self, pc, reg):
        return None


def tr(op, args, pc=0x1000, size=4, module=None, entry=0x1000):
    callees = set()
    code, successors = lift64.translate(pc, (pc + size, op, args), module or FakeModule(), entry, callees)
    return code, successors, callees


class Operands(unittest.TestCase):
    def test_register_widths(self):
        self.assertEqual(lift64.Operand('r9d', 0).write('x'), 'c->r[9] = (uint32_t)(x);')   # clears bits 32-63
        self.assertEqual(lift64.Operand('rax', 0).write('x'), 'c->r[0] = (uint64_t)(x);')
        self.assertIn('0xffffffffffffff00ull', lift64.Operand('sil', 0).write('x'))        # keeps the rest
        self.assertIn('<< 8', lift64.Operand('ah', 0).write('x'))
        self.assertEqual(lift64.Operand('r15w', 0).reg, (15, 16, 0))
        self.assertEqual(lift64.Operand('spl', 0).reg, (4, 8, 0))

    def test_rip_relative_is_module_relative(self):
        operand = lift64.Operand('qword ptr [rip + 0x10]', 0x2000)
        self.assertEqual(operand.addr, '(B_ + 0x2010ull)')
        self.assertEqual(lift64.Operand('byte ptr [rip - 0x8]', 0x2000).addr, '(B_ + 0x1ff8ull)')

    def test_scaled_index_and_negative_displacement(self):
        addr = lift64.Operand('dword ptr [rdi + 4*r12 - 0x10]', 0).addr
        self.assertEqual(addr, '(uint64_t)(c->r[7] + c->r[12] * 4u + 0xfffffffffffffff0ull)')

    def test_immediates_are_masked_to_the_operand_width(self):
        code, _, _ = tr('and', 'rsp, -0x10')
        self.assertIn('b = 0xfffffffffffffff0ull', code)
        code, _, _ = tr('add', 'eax, -0x66')
        self.assertIn('b = 0xffffff9aull', code)

    def test_fs_segment(self):
        self.assertEqual(lift64.Operand('qword ptr fs:[0x28]', 0).addr, '(c->fs_base + (uint64_t)(0 + 0x28ull))')


class ControlFlow(unittest.TestCase):
    def test_direct_call_pushes_the_module_relative_return_address(self):
        code, successors, callees = tr('call', '0x3000', size=5)
        self.assertIn('PUSH64(c, (B_ + 0x1005ull))', code)
        self.assertIn('fn_3000(c)', code)
        self.assertEqual((successors, callees), ([0x1005], {0x3000}))

    def test_jump_to_another_entry_is_a_tail_call(self):
        code, successors, callees = tr('jmp', '0x3000', module=FakeModule(entries=[0x3000]))
        self.assertEqual((code, successors), (lift64.SAVE_F + ' fn_3000(c); return;', []))
        code, successors, _ = tr('jne', '0x1010')
        self.assertEqual(successors, [0x1010, 0x1004])

    def test_got_call_records_the_known_target(self):
        module = FakeModule(insns={0x5000: (0x5001, 'ret', '')}, slots={0x9000: 0x5000})
        code, _, callees = tr('call', 'qword ptr [rip + 0x7ffa]', size=6, module=module)
        self.assertIn('rt64_dispatch(c, a)', code)
        self.assertEqual(callees, {0x5000})

    def test_ret_pops_rip(self):
        code, successors, _ = tr('ret', '')
        self.assertEqual(successors, [])
        self.assertIn('c->rip = POP64(c)', code)

    def test_cmov32_always_writes(self):
        code, _, _ = tr('cmovg', 'ecx, edx')
        self.assertIn('else { c->r[1] = (uint32_t)', code)

    def test_unknown_instruction_is_reported(self):
        with self.assertRaises(lift64.Unsupported):
            tr('fbstp', 'tbyte ptr [rax]')

    def test_privileged_and_invalid_opcodes_fault_like_on_linux(self):
        for op, args in (('out', 'dx, al'), ('sti', ''), ('(bad)', ''), ('ff', '')):
            code, successors, _ = tr(op, args)
            self.assertIn('rt64_trap', code)
            self.assertEqual(successors, [])

    def test_loop(self):
        code, successors, _ = tr('loopne', '0x1000', size=2)
        self.assertIn('--c->r[RCX]', code)
        self.assertEqual(successors, [0x1000, 0x1002])

    def test_extensions_cpuid_does_not_report_trap(self):
        code, successors, _ = tr('vfmadd231ps', 'ymm0, ymm1, ymm2')
        self.assertIn('not reported by CPUID', code)
        self.assertEqual(successors, [])


class Coverage(unittest.TestCase):
    def test_unsupported_instruction_becomes_a_trap_and_is_counted(self):
        insns = {0x10: (0x14, 'syscall', ''), 0x14: (0x15, 'ret', '')}
        module = FakeModule(insns=insns)
        stats = lift64.new_stats()
        body, _ = lift64.lift_function(0x10, module, stats)
        self.assertIn('not lifted: syscall', body)
        self.assertEqual(stats['missing'], collections.Counter({'syscall': 1}))
        self.assertEqual(stats['pcs'], {0x10, 0x14})

    def test_form(self):
        self.assertEqual(lift64.form('movsd', 'xmm0, qword ptr [rip + 0x10]'), 'movsd xmm,m64')
        self.assertEqual(lift64.form('shl', 'rax, 0x3'), 'shl r64,imm')


class Flags(unittest.TestCase):
    def test_effects(self):
        self.assertEqual(lift64.flag_effect('jne', '0x10'), (lift64.ZF, 0))
        self.assertEqual(lift64.flag_effect('adc', 'rax, rbx'), (lift64.CF, lift64.ALL_FLAGS))
        self.assertEqual(lift64.flag_effect('inc', 'eax')[1] & lift64.CF, 0)     # INC keeps CF
        self.assertEqual(lift64.flag_effect('shl', 'rax, cl'), (0, 0))           # count may be 0
        self.assertEqual(lift64.flag_effect('shl', 'rax, 0x3')[1] & lift64.CF, lift64.CF)

    def test_hand_over_dropped_unless_flags_are_read_after_the_call(self):
        insns = {0x10: (0x15, 'call', '0x100'), 0x15: (0x18, 'cmp', 'eax, 0x1'), 0x18: (0x1a, 'je', '0x10'),
                 0x1a: (0x1b, 'ret', '')}
        body, _ = lift64.lift_function(0x10, FakeModule(insns=insns), keep_flags=False)
        self.assertNotIn('c->flags', body)
        self.assertIn('FL64 f = (FL64)', body)                                   # nothing live at entry
        insns[0x15] = (0x18, 'mov', 'ecx, 0x1')                                  # JE now reads the callee's flags
        stats = lift64.new_stats()
        body, _ = lift64.lift_function(0x10, FakeModule(insns=insns), stats, keep_flags=False)
        self.assertIn(lift64.LOAD_F, body)
        self.assertEqual(stats['read_after_call'], [0x10])
        body, _ = lift64.lift_function(0x10, FakeModule(insns=insns), keep_flags=True)
        self.assertIn('FL64 f = c->flags', body)


if __name__ == '__main__':
    unittest.main()
