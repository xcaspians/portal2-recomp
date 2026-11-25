# x86-64 test programs for build/x64-check (tests/x64_checks.c). Assembled by
# clang for x86_64-linux-gnu, linked into a shared object by tests/x64_link.py,
# prepared by tools/elfprep64.py and lifted by tools/lift64.py, exactly like a
# P2CE module. Expected values in x64_checks.c come from the architecture
# manuals, not from the runtime under test.
.intel_syntax noprefix
.text

# System V AMD64: the first six integer arguments in rdi, rsi, rdx, rcx, r8,
# r9, the 7th and 8th on the stack above the return address.
.globl x64_sum8
.type x64_sum8, @function
x64_sum8:
    lea rax, [rdi + rsi]
    add rax, rdx
    add rax, rcx
    add rax, r8
    add rax, r9
    add rax, qword ptr [rsp + 8]
    add rax, qword ptr [rsp + 16]
    ret

# Register widths: a 32-bit write clears bits 32-63, 8/16-bit writes keep them.
.globl x64_widths
.type x64_widths, @function
x64_widths:
    mov rax, -1
    mov eax, 5                      # rax = 5
    lea rdx, [rip + x64_out]
    mov qword ptr [rdx], rax
    mov rcx, -1
    mov cl, 0x12                    # rcx = 0xffffffffffffff12
    mov qword ptr [rdx + 8], rcx
    mov rcx, -1
    mov cx, 0x1234
    mov qword ptr [rdx + 16], rcx
    mov r11, -1
    mov r11b, 0
    mov qword ptr [rdx + 24], r11   # 0xffffffffffffff00
    mov r10, 0x1122334455667788
    mov r10d, r10d                  # 0x55667788
    mov qword ptr [rdx + 32], r10
    mov rax, 0x0000ff00
    mov ah, 0x7f                    # high byte register (no REX)
    mov qword ptr [rdx + 40], rax
    mov sil, 0x80                   # REX low byte of rsi
    movsx rsi, sil
    mov qword ptr [rdx + 48], rsi   # -128
    movsxd rax, dword ptr [rip + x64_minus2]
    mov qword ptr [rdx + 56], rax   # -2
    movzx eax, word ptr [rip + x64_minus2]
    mov qword ptr [rdx + 64], rax   # 0xfffe
    xor eax, eax
    ret

# RIP-relative loads and stores of a global, and r8..r15 as scratch.
.globl x64_counter
.type x64_counter, @function
x64_counter:
    push r15
    push r12
    mov r12, qword ptr [rip + x64_count]
    lea r15, [r12 + rdi]
    mov qword ptr [rip + x64_count], r15
    mov rax, r15
    pop r12
    pop r15
    ret

# Recursion through the guest stack: n! with imul.
.globl x64_fact
.type x64_fact, @function
x64_fact:
    cmp rdi, 1
    jbe 1f
    push rdi
    dec rdi
    call x64_fact
    pop rdi
    imul rax, rdi
    ret
1:  mov eax, 1
    ret

# Flags and conditions: unsigned/signed compares, cmov (a 32-bit cmov clears
# the upper half even when false), setcc, adc/sbb carries on 64 bits.
.globl x64_flags
.type x64_flags, @function
x64_flags:
    lea r8, [rip + x64_out]
    mov rax, -1
    add rax, 1                      # CF=1 ZF=1
    setc byte ptr [r8]
    setz byte ptr [r8 + 1]
    mov rax, 0x7fffffffffffffff
    add rax, 1                      # OF=1 SF=1
    seto byte ptr [r8 + 2]
    sets byte ptr [r8 + 3]
    mov rax, -5
    cmp rax, 3
    setl byte ptr [r8 + 4]          # signed: -5 < 3
    seta byte ptr [r8 + 5]          # unsigned: 0xff..fb > 3
    mov rcx, -1
    mov edx, 7
    cmp edx, 8
    cmovg ecx, edx                  # false, but ecx's upper half is cleared
    mov qword ptr [r8 + 8], rcx     # 0xffffffff
    mov rcx, -1
    cmovl rcx, rdx                  # true
    mov qword ptr [r8 + 16], rcx    # 7
    mov rax, -1
    mov rdx, 0
    add rax, 1
    adc rdx, 0                      # rdx = 1
    mov qword ptr [r8 + 24], rdx
    xor eax, eax
    sub rax, 1
    sbb rdx, 0                      # rdx = 0
    mov qword ptr [r8 + 32], rdx
    ret

# Shifts, rotates, multiply, divide, bit scans, byte swap.
.globl x64_alu
.type x64_alu, @function
x64_alu:
    lea r9, [rip + x64_out]
    mov rax, 1
    shl rax, 63
    mov qword ptr [r9], rax         # 0x8000000000000000
    sar rax, 62
    mov qword ptr [r9 + 8], rax     # -2
    mov rax, 0x8000000000000001
    rol rax, 4
    mov qword ptr [r9 + 16], rax    # 0x18
    mov rax, 0x123456789
    mov rcx, 0x100000000
    mul rcx                         # rdx:rax = 0x1:0x2345678900000000
    mov qword ptr [r9 + 24], rax
    mov qword ptr [r9 + 32], rdx
    mov rax, -100
    cqo
    mov rcx, 7
    idiv rcx                        # -14 rem -2
    mov qword ptr [r9 + 40], rax
    mov qword ptr [r9 + 48], rdx
    mov rax, 0x10000
    bsf rcx, rax
    bsr rdx, rax
    shl rdx, 8
    or rcx, rdx
    mov qword ptr [r9 + 56], rcx    # 0x1010
    mov rax, 0x0102030405060708
    bswap rax
    mov qword ptr [r9 + 64], rax
    mov rax, 6
    imul rax, rax, -7
    mov qword ptr [r9 + 72], rax    # -42
    mov rcx, 0x40
    bts rcx, 3
    setc byte ptr [r9 + 80]         # 0
    mov qword ptr [r9 + 88], rcx    # 0x48
    ret

# A switch through a PIC jump table (clang's form) and indirect calls
# through a table of function pointers in .data.
.globl x64_switch
.type x64_switch, @function
x64_switch:
    cmp edi, 3
    ja 9f
    mov eax, edi
    lea rcx, [rip + .Ltable]
    movsxd rax, dword ptr [rcx + 4*rax]
    add rax, rcx
    jmp rax
.Lcase0: mov eax, 10
    ret
.Lcase1: mov eax, 11
    ret
.Lcase2: mov eax, 12
    ret
.Lcase3: mov eax, 13
    ret
9:  mov eax, -1
    ret

# The same with instructions between the add and the jmp (GCC, P2CE's
# launcher.so: `add rcx, rdx; xor ebx, ebx; jmp rcx`), round 23.
.globl x64_switch_gap
.type x64_switch_gap, @function
x64_switch_gap:
    push rbx
    cmp edi, 2
    ja 8f
    mov eax, edi
    lea rdx, [rip + .Ltable2]
    movsxd rcx, dword ptr [rdx + 4*rax]
    add rcx, rdx
    xor ebx, ebx
    mov esi, 5
    jmp rcx
.Lgap0: lea eax, [rbx + 20]
    pop rbx
    ret
.Lgap1: lea eax, [rsi + 16]
    pop rbx
    ret
.Lgap2: mov eax, 22
    pop rbx
    ret
8:  mov eax, -2
    pop rbx
    ret

.globl x64_callptr
.type x64_callptr, @function
x64_callptr:
    push rbx
    lea rbx, [rip + x64_ops]
    mov rax, qword ptr [rbx + 8*rdi]
    mov rdi, rsi
    call rax
    pop rbx
    ret

.type x64_double, @function
x64_double:
    lea rax, [rdi + rdi]
    ret

.type x64_square, @function
x64_square:
    mov rax, rdi
    imul rax, rdi
    ret

# Tail call to another function.
.globl x64_tail
.type x64_tail, @function
x64_tail:
    add rdi, 1
    jmp x64_square

# Imports through the PLT (memset, strlen) and through the GOT (memcpy).
.globl x64_imports
.type x64_imports, @function
x64_imports:
    push rbx
    mov rbx, rdi                    # buffer of 32 bytes
    mov esi, 0x41
    mov edx, 7
    call memset@PLT
    mov byte ptr [rbx + 7], 0
    mov rdi, rbx
    call strlen@PLT
    mov rdx, rax
    lea rdi, [rbx + 16]
    mov rsi, rbx
    add rdx, 1
    call qword ptr [rip + memcpy@GOTPCREL]
    lea rdi, [rbx + 16]
    call strlen@PLT
    pop rbx
    ret

# String instructions and atomics.
.globl x64_strings
.type x64_strings, @function
x64_strings:
    lea rdi, [rip + x64_out]
    mov rax, 0x0101010101010101
    mov ecx, 4
    cld
    rep stosq
    lea rsi, [rip + x64_out]
    lea rdi, [rip + x64_out + 40]
    mov ecx, 3
    rep movsb
    lea rdi, [rip + x64_count]
    mov qword ptr [rdi], 5
    mov rax, 5
    mov rcx, 9
    lock cmpxchg qword ptr [rdi], rcx
    setz byte ptr [rip + x64_out + 48]        # swapped: 1
    mov rax, 5
    lock cmpxchg qword ptr [rdi], rcx
    setz byte ptr [rip + x64_out + 49]        # not swapped: 0, rax = 9
    mov edx, 3
    lock xadd qword ptr [rdi], rdx            # rdx = 9, memory = 12
    add rax, rdx
    add rax, qword ptr [rdi]                  # 9 + 9 + 12 = 30
    ret

# Scalar SSE2 doubles: (a * 3 + 0.5) / 2, then truncation to an integer.
.globl x64_sse
.type x64_sse, @function
x64_sse:
    cvtsi2sd xmm1, rdi
    mulsd xmm1, qword ptr [rip + x64_three]
    addsd xmm1, qword ptr [rip + x64_half]
    divsd xmm1, qword ptr [rip + x64_two]
    movsd qword ptr [rip + x64_out], xmm1
    cvttsd2si rax, xmm1
    xorpd xmm0, xmm0
    ucomisd xmm1, xmm0
    seta byte ptr [rip + x64_out + 8]
    movapd xmm0, xmm1
    ret

# SSSE3/SSE4.1 and the rest of SSE2 (round B of docs/fork/P2CE_PLAN.md).
.globl x64_simd
.type x64_simd, @function
x64_simd:
    lea rdi, [rip + x64_out]
    movdqa xmm0, xmmword ptr [rip + x64_v1]
    movdqa xmm1, xmmword ptr [rip + x64_shuf]
    pshufb xmm0, xmm1                         # 0, 0, 1, ..., 14
    movdqu xmmword ptr [rdi], xmm0
    movdqa xmm0, xmmword ptr [rip + x64_v1]
    movdqa xmm2, xmmword ptr [rip + x64_v2]
    palignr xmm2, xmm0, 4                     # 4, ..., 15, 16, ..., 19
    movdqu xmmword ptr [rdi + 16], xmm2
    pmovzxbd xmm3, dword ptr [rip + x64_v1 + 12]   # 12, 13, 14, 15 as dwords
    movdqu xmmword ptr [rdi + 32], xmm3
    movdqa xmm4, xmmword ptr [rip + x64_i32]
    packssdw xmm4, xmm4                       # -5, 7, 32767, -2, twice
    movdqu xmmword ptr [rdi + 48], xmm4
    movdqa xmm5, xmmword ptr [rip + x64_i32]
    psrad xmm5, 1                             # -3, 3, 50000, -1
    movdqu xmmword ptr [rdi + 64], xmm5
    movdqa xmm6, xmmword ptr [rip + x64_i32]
    pxor xmm1, xmm1
    pminsd xmm6, xmm1                         # -5, 0, 0, -2
    movdqu xmmword ptr [rdi + 80], xmm6
    movaps xmm7, xmmword ptr [rip + x64_f4]
    movaps xmm8, xmmword ptr [rip + x64_f4b]
    cmpltps xmm7, xmm8                        # all ones, 0, 0, 0
    movdqu xmmword ptr [rdi + 96], xmm7
    movaps xmm9, xmmword ptr [rip + x64_f4]
    insertps xmm9, xmm8, 0x58                 # 1.5, -3.0, 3.0, 0
    movups xmmword ptr [rdi + 112], xmm9
    movaps xmm10, xmmword ptr [rip + x64_f4]
    blendps xmm10, xmm8, 0x5                  # 2.0, -2.5, 3.0, 4.0
    movups xmmword ptr [rdi + 128], xmm10
    movsd xmm11, qword ptr [rip + x64_d25]
    roundsd xmm11, xmm11, 0                   # to nearest even: 2.0
    movsd qword ptr [rdi + 144], xmm11
    movsd xmm12, qword ptr [rip + x64_d25]
    roundsd xmm12, xmm12, 2                   # up: 3.0
    movsd qword ptr [rdi + 152], xmm12
    movdqa xmm13, xmmword ptr [rip + x64_v1]
    ptest xmm13, xmm13
    setz byte ptr [rdi + 160]                 # 0
    pxor xmm14, xmm14
    ptest xmm14, xmm13
    setz byte ptr [rdi + 161]                 # 1
    setc byte ptr [rdi + 162]                 # 0
    movdqa xmm15, xmmword ptr [rip + x64_i16]
    pmaddwd xmm15, xmm15                      # 5, 25, 61, 113
    movdqu xmmword ptr [rdi + 176], xmm15
    movdqa xmm0, xmmword ptr [rip + x64_i16]
    punpckhwd xmm0, xmmword ptr [rip + x64_i16]   # 5, 5, 6, 6, 7, 7, 8, 8
    movdqu xmmword ptr [rdi + 192], xmm0
    ret

# x87 (doubles, 80-bit memory) and MMX.
.globl x64_x87
.type x64_x87, @function
x64_x87:
    lea rdi, [rip + x64_out]
    fld qword ptr [rip + x64_d25]
    fild dword ptr [rip + x64_int3]
    fmulp st(1), st                           # 7.5
    fld st(0)
    fadd st, st(1)                            # 15.0
    fstp qword ptr [rdi]
    fistp dword ptr [rdi + 8]                 # 7.5 to nearest even: 8
    fld qword ptr [rip + x64_d25]
    fstp tbyte ptr [rdi + 16]
    fld tbyte ptr [rdi + 16]
    fld1
    fcomip st, st(1)                          # 1.0 < 2.5: CF = 1
    setb byte ptr [rdi + 32]
    fstp st(0)
    fnstcw word ptr [rdi + 34]                # 0x37f
    movq mm0, qword ptr [rip + x64_i16]
    movq mm1, qword ptr [rip + x64_i16 + 8]
    paddw mm0, mm1                            # 6, 8, 10, 12
    movq qword ptr [rdi + 40], mm0
    packuswb mm0, mm1                         # 6, 8, 10, 12, 5, 6, 7, 8
    movq qword ptr [rdi + 48], mm0
    pshufw mm2, mm1, 0x1b                     # 8, 7, 6, 5
    movq qword ptr [rdi + 56], mm2
    emms
    ret

# BMI1/BMI2, POPCNT, LZCNT/TZCNT, LOOP.
.globl x64_bmi
.type x64_bmi, @function
x64_bmi:
    lea rdi, [rip + x64_out]
    mov rax, 0xf0
    mov ecx, 4
    shrx rdx, rax, rcx                        # 0xf
    mov qword ptr [rdi], rdx
    mov rbx, 0xff
    andn rdx, rax, rbx                        # 0x0f
    mov qword ptr [rdi + 8], rdx
    blsr rdx, rax                             # 0xe0
    mov qword ptr [rdi + 16], rdx
    mov rsi, 0xff00
    pdep rdx, rax, rsi                        # 0xf000
    mov qword ptr [rdi + 24], rdx
    popcnt rdx, rax                           # 4
    mov qword ptr [rdi + 32], rdx
    mov ecx, 5
    xor eax, eax
2:  add eax, ecx
    loop 2b                                   # 15
    mov qword ptr [rdi + 40], rax
    tzcnt rdx, rax                            # 0
    mov qword ptr [rdi + 48], rdx
    lzcnt edx, eax                            # 28
    mov qword ptr [rdi + 56], rdx
    ret

# PUSHFQ/POPFQ, XLATB, SYSCALL (getpid), MASKMOVDQU, MOVNTQ.
.globl x64_misc
.type x64_misc, @function
x64_misc:
    lea rdi, [rip + x64_out]
    stc
    pushfq
    pop rax
    and eax, 1
    mov qword ptr [rdi], rax                  # CF seen through RFLAGS: 1
    push 0x40                                 # ZF only
    popfq
    setz byte ptr [rdi + 8]                   # 1
    setc byte ptr [rdi + 9]                   # 0
    lea rbx, [rip + x64_v2]
    mov eax, 5
    xlatb                                     # al = x64_v2[5] = 21
    mov byte ptr [rdi + 10], al
    push rdi
    mov eax, 39                               # getpid
    syscall
    pop rdi
    mov qword ptr [rdi + 16], rax
    movdqa xmm0, xmmword ptr [rip + x64_v1]
    movdqa xmm1, xmmword ptr [rip + x64_shuf] # byte 0 has bit 7: only byte 0 stored
    lea rdi, [rip + x64_out + 32]
    maskmovdqu xmm0, xmm1
    lea rdi, [rip + x64_out]
    movq mm0, qword ptr [rip + x64_i16]
    movntq qword ptr [rdi + 48], mm0
    emms
    ret

# A stray byte in .text (data, as compilers and hand-written code leave):
# a linear disassembly reads 0xe8 as a CALL and swallows the start of the
# next function; the lifter decodes again from the function's own address.
    .byte 0xe8
.globl x64_after_data
.type x64_after_data, @function
x64_after_data:
    mov eax, 0x5a5a
    ret

.section .rodata
.p2align 2
.Ltable:
    .long .Lcase0 - .Ltable
    .long .Lcase1 - .Ltable
    .long .Lcase2 - .Ltable
    .long .Lcase3 - .Ltable
.p2align 3
x64_minus2: .quad -2
x64_three: .double 3.0
x64_half: .double 0.5
x64_two: .double 2.0
x64_d25: .double 2.5
x64_int3: .long 3
.p2align 4
x64_v1: .byte 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
x64_v2: .byte 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
x64_shuf: .byte 0x80, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
x64_i32: .long -5, 7, 100000, -2
x64_i16: .short 1, 2, 3, 4, 5, 6, 7, 8
x64_f4: .float 1.5, -2.5, 3.0, 4.0
x64_f4b: .float 2.0, -3.0, 3.0, 1.0
.Ltable2:
    .long .Lgap0 - .Ltable2
    .long .Lgap1 - .Ltable2
    .long .Lgap2 - .Ltable2

.data
.p2align 3
.globl x64_count
x64_count: .quad 0
x64_ops:
    .quad x64_double
    .quad x64_square
.globl x64_out
.p2align 4
x64_out: .zero 256
