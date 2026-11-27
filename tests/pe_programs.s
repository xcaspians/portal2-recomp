# The PE test program (make check): small Windows x64 functions with what
# MSVC-built code relies on, assembled with clang -target x86_64-windows-msvc,
# linked by tests/pe_link.py into pefix.dll, prepared by tools/peprep64.py,
# lifted by tools/lift64.py and run by tests/pe_checks.c.
.intel_syntax noprefix

.text
# Five arguments: rcx, rdx, r8, r9, then the stack above the shadow area.
.globl pe_add5
pe_add5:
    lea rax, [rcx + rdx]
    add rax, r8
    add rax, r9
    add rax, qword ptr [rsp + 0x28]
    ret

# The thread block's self pointer (gs:[0x30]) and the stack it records.
.globl pe_teb_self
pe_teb_self:
    mov rax, qword ptr gs:[0x30]
    ret

# Static TLS: _tls_index, gs:[0x58], the block's offset (SECREL).
.globl pe_tls_bump
pe_tls_bump:
    mov eax, dword ptr [rip + _tls_index]
    mov rcx, qword ptr gs:[0x58]
    mov rcx, qword ptr [rcx + 8*rax]
    mov eax, dword ptr [rcx + tls_counter@SECREL32]
    inc eax
    mov dword ptr [rcx + tls_counter@SECREL32], eax
    ret

# MSVC's switch: lea of __ImageBase, a table of RVAs, add, jmp.
.globl pe_switch
pe_switch:
    mov eax, ecx
    cmp rax, 3
    ja .Ldefault
    lea rdx, [rip + __ImageBase]
    mov eax, dword ptr [rdx + 4*rax + .Ltable@IMGREL]
    add rax, rdx
    jmp rax
.Lcase0:
    mov eax, 100
    ret
.Lcase1:
    mov eax, 111
    ret
.Lcase2:
    mov eax, 122
    ret
.Lcase3:
    mov eax, 133
    ret
.Ldefault:
    mov eax, -1
    ret
    .p2align 2
.Ltable:
    .long .Lcase0@IMGREL
    .long .Lcase1@IMGREL
    .long .Lcase2@IMGREL
    .long .Lcase3@IMGREL

# The two-level form: a byte table of case numbers, then the RVA table.
.globl pe_switch2
pe_switch2:
    mov eax, ecx
    cmp eax, 5
    ja .L2default
    lea r8, [rip + __ImageBase]
    movzx eax, byte ptr [r8 + rax + .L2bytes@IMGREL]
    mov ecx, dword ptr [r8 + 4*rax + .L2table@IMGREL]
    add rcx, r8
    jmp rcx
.L2even:
    mov eax, 2
    ret
.L2odd:
    mov eax, 1
    ret
.L2default:
    xor eax, eax
    ret
    .p2align 2
.L2table:
    .long .L2even@IMGREL
    .long .L2odd@IMGREL
.L2bytes:
    .byte 0, 1, 0, 1, 0, 1

# The two-level form as MSVC also writes it: the byte table read with the
# base register second, the RVA table through one register holding the base
# and the base added from another (a second lea).
.globl pe_switch3
pe_switch3:
    lea r9, [rip + __ImageBase]
    cmp ecx, 3
    ja .L3default
    movsxd rax, ecx
    movzx eax, byte ptr [rax + r9 + .L3bytes@IMGREL]
    mov ecx, dword ptr [r9 + 4*rax + .L3table@IMGREL]
    lea rax, [rip + __ImageBase]
    add rcx, rax
    jmp rcx
.L3low:
    mov eax, 30
    ret
.L3high:
    mov eax, 31
    ret
.L3default:
    mov eax, 39
    ret
    .p2align 2
.L3table:
    .long .L3low@IMGREL
    .long .L3high@IMGREL
.L3bytes:
    .byte 0, 0, 1, 1

# MPX's BND prefix (F2) on branches, as in MSVC's CRT: no effect.
.globl pe_bnd
pe_bnd:
    xor eax, eax
    test ecx, ecx
    .byte 0xf2
    jne .Lbnd_one
    .byte 0xf2
    ret
.Lbnd_one:
    mov eax, 1
    .byte 0xf2
    ret

# Imports through the IAT: by name, with five arguments, by ordinal.
.globl pe_import_twice
pe_import_twice:
    sub rsp, 40
    call qword ptr [rip + __imp_twice]
    add rsp, 40
    ret

.globl pe_import_sum5
pe_import_sum5:
    sub rsp, 56
    mov qword ptr [rsp + 0x20], 5
    mov ecx, 1
    mov edx, 2
    mov r8d, 3
    mov r9d, 4
    call qword ptr [rip + __imp_sum5]
    add rsp, 56
    ret

.globl pe_import_ordinal
pe_import_ordinal:
    sub rsp, 40
    call qword ptr [rip + __imp_seven]
    add rsp, 40
    ret

# Control Flow Guard: the check pointer with the target in rcx, then the
# dispatch pointer with it in rax; the target is read from a relocated
# table (a DIR64 base relocation).
.globl pe_guard_call
pe_guard_call:
    push rbx
    sub rsp, 32
    lea rax, [rip + pe_targets]
    mov rbx, qword ptr [rax + 8*rcx]
    mov rcx, rbx
    call qword ptr [rip + __guard_check_icall_fptr]
    mov rax, rbx
    mov ecx, 7
    call qword ptr [rip + __guard_dispatch_icall_fptr]
    add rsp, 32
    pop rbx
    ret

pe_times3:
    lea eax, [rcx + 2*rcx]
    ret

pe_plus1000:
    lea eax, [rcx + 1000]
    ret

# Known only from .pdata: reached through an RVA computed at run time.
.globl pe_hidden_call
pe_hidden_call:
    sub rsp, 40
    lea rdx, [rip + __ImageBase]
    mov eax, dword ptr [rip + hidden_rva]
    add rax, rdx
    call rax
    add rsp, 40
    ret

.def pe_hidden; .scl 3; .type 32; .endef
.seh_proc pe_hidden
pe_hidden:
    sub rsp, 8
    .seh_stackalloc 8
    .seh_endprologue
    mov eax, 4242
    add rsp, 8
    ret
.seh_endproc

.data
    .p2align 3
pe_targets:
    .quad pe_times3
    .quad pe_plus1000
hidden_rva:
    .long pe_hidden@IMGREL
    .p2align 3
.globl _tls_index
_tls_index:
    .long 0

.section .rdata,"dr"
    .p2align 3
.globl __guard_check_icall_fptr
__guard_check_icall_fptr:
    .quad 0
.globl __guard_dispatch_icall_fptr
__guard_dispatch_icall_fptr:
    .quad 0
# IMAGE_TLS_DIRECTORY64: template start and end, index slot, callbacks,
# zero fill, characteristics (16-byte alignment).
.globl _tls_used
_tls_used:
    .quad tls_start
    .quad tls_end
    .quad _tls_index
    .quad 0
    .long 8
    .long 0x00500000
# IMAGE_LOAD_CONFIG_DIRECTORY64, up to GuardFlags (0x94 bytes).
.globl _load_config_used
_load_config_used:
    .long 0x94
    .zero 0x6c
    .quad __guard_check_icall_fptr
    .quad __guard_dispatch_icall_fptr
    .quad 0
    .quad 0
    .long 0x100
    .zero 4

.section .tls$,"dw"
tls_start:
    .long 0x12345678
tls_counter:
    .long 41
.section .tls$ZZZ,"dw"
tls_end:
    .byte 0
