# Second x86-64 test module (round C of docs/fork/P2CE_PLAN.md): linked by
# tests/x64_link.py with DT_NEEDED libx64fixture.so and opened through the
# runtime's loader by build/x64-check. It imports a function of the first
# module, has a constructor, thread-local variables reached both ways a
# shared object uses (general dynamic through __tls_get_addr, initial exec
# through fs:), and a setjmp/longjmp pair.
.intel_syntax noprefix
.text

.globl x64b_add_via_first
.type x64b_add_via_first, @function
x64b_add_via_first:                       # x64_sum8(1..6, 7, 8) of the first module
    sub rsp, 8
    push 8
    push 7
    mov edi, 1
    mov esi, 2
    mov edx, 3
    mov ecx, 4
    mov r8d, 5
    mov r9d, 6
    call x64_sum8@PLT
    add rsp, 24
    ret

.globl x64b_ctor_value
.type x64b_ctor_value, @function
x64b_ctor_value:
    mov eax, dword ptr [rip + x64b_ctor_ran]
    ret

.type x64b_ctor, @function
x64b_ctor:
    mov dword ptr [rip + x64b_ctor_ran], 1234
    ret

.globl x64b_tls_gd
.type x64b_tls_gd, @function
x64b_tls_gd:                              # ++x64b_tls_a, through __tls_get_addr
    sub rsp, 8
    lea rdi, [rip + x64b_tls_a@tlsgd]
    call __tls_get_addr@PLT
    add dword ptr [rax], 1
    mov eax, dword ptr [rax]
    add rsp, 8
    ret

.globl x64b_tls_ie
.type x64b_tls_ie, @function
x64b_tls_ie:                              # x64b_tls_b, through its offset from fs
    mov rax, qword ptr [rip + x64b_tls_b@gottpoff]
    mov eax, dword ptr fs:[rax]
    ret

.globl x64b_setjmp
.type x64b_setjmp, @function
x64b_setjmp:                              # 35 from longjmp + 7 kept in rbx = 42
    push rbx
    sub rsp, 208
    mov rbx, 7
    mov rdi, rsp
    call _setjmp@PLT
    test eax, eax
    jnz 1f
    mov rdi, rsp
    call x64b_jumper
    mov eax, -1
    jmp 2f
1:  add eax, ebx
2:  add rsp, 208
    pop rbx
    ret

.type x64b_jumper, @function
x64b_jumper:
    push rbx
    mov rbx, 99                           # longjmp must bring back the 7 saved by setjmp
    mov esi, 35
    call longjmp@PLT
    ud2

# Threads: four guest threads add to a counter with LOCK XADD, then join.
.globl x64b_threads
.type x64b_threads, @function
x64b_threads:
    push rbx
    push r12
    sub rsp, 40                           # 4 pthread_t, aligned
    xor ebx, ebx
3:  lea rdi, [rsp + 8*rbx]
    xor esi, esi
    lea rdx, [rip + x64b_worker]
    mov rcx, rbx
    call pthread_create@PLT
    test eax, eax
    jnz 5f
    inc rbx
    cmp rbx, 4
    jb 3b
    xor ebx, ebx
4:  mov rdi, qword ptr [rsp + 8*rbx]
    xor esi, esi
    call pthread_join@PLT
    inc rbx
    cmp rbx, 4
    jb 4b
    mov rax, qword ptr [rip + x64b_total]
5:  add rsp, 40
    pop r12
    pop rbx
    ret

.type x64b_worker, @function
x64b_worker:                              # adds 1000 times (argument + 1), returns 0
    lea rcx, [rdi + 1]
    mov edx, 1000
6:  mov rax, rcx
    lock xadd qword ptr [rip + x64b_total], rax
    dec edx
    jnz 6b
    xor eax, eax
    ret

.section .init_array, "aw"
.p2align 3
.quad x64b_ctor

.section .tdata, "awT", @progbits
.p2align 2
.type x64b_tls_a, @tls_object
x64b_tls_a: .long 10
.type x64b_tls_b, @tls_object
x64b_tls_b: .long 77

.data
.p2align 2
x64b_ctor_ran: .long 0
.p2align 3
x64b_total: .quad 0
