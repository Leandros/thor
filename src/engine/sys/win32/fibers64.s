; Win64 calling convention:
; rax foo(rcx, rdx, r8, r9)
;
; Caller saved: RAX, RCX, RDX, R8, R9, R10, R11
; Callee saved: RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15

; struct sys_fiber, equal to the struct defined in sys_fibers.h
%define fib_rbx     0x0
%define fib_rbp     0x8
%define fib_rdi     0x10
%define fib_rsi     0x18
%define fib_rsp     0x20
%define fib_r12     0x28
%define fib_r13     0x30
%define fib_r14     0x38
%define fib_r15     0x40
%define fib_ret     0x48
%define fib_xmm6    0x50
%define fib_xmm7    0x60
%define fib_xmm8    0x70
%define fib_xmm9    0x80
%define fib_xmm10   0x90
%define fib_xmm11   0xA0
%define fib_xmm12   0xB0
%define fib_xmm13   0xC0
%define fib_xmm14   0xD0
%define fib_xmm15   0xE0
%define fib_pc      0xF0
%define fib_fp      0xF8
%define fib_data    0x100
%define fib_ssize   0x108
%define fib_stack   0x110
%define fib_flags   0x118

global sys_fiber_save
global sys_fiber_restore
global sys_fiber_switch

section .text

; void sys_fiber_switch(struct sys_fiber *rcx, struct sys_fiber *rdx)
sys_fiber_switch:
    ; Prolog, as wished by Windows.
    mov     [rsp + 16], rdx
    mov     [rsp + 8], rcx
    sub     rsp, 16

    call    sys_fiber_save
    cmp     qword [rcx + fib_flags], 1
    je      .restored

    ; Check if sys_fiber.fp is not NULL, and if execute it.
    cmp     qword [rdx + fib_fp], 0
    je      .null

    ; Setup our new stack
    mov     r8,  [rdx + fib_ssize]
    mov     rsp, [rdx + fib_stack]
    mov     rbp, [rdx + fib_stack]
    add     rsp, r8
    add     rbp, r8
    push    rcx

    ; Execute the function assigned with our new context.
    mov     r8,  [rdx + fib_fp]
    mov     rcx, [rdx + fib_data]

    ; Align the stack to make macOS happy.
    sub     rsp, 0x8
    call    r8
    add     rsp, 0x8

    ; Restore our old context
    pop     rdx

.null:
    mov     rcx, rdx
    call    sys_fiber_restore

.restored:
    ; Epilog, as wished by Windows.
    ; Not required, since no registers are saved.
    add     rsp, 16
    ret


; void sys_fiber_save(struct sys_fiber *rcx)
sys_fiber_save:
    ; Prolog, as wished by Windows.
    mov     [rsp + 8], rcx

    ; Save non-volatile registers.
    mov     [rcx + fib_rbx], rbx
    mov     [rcx + fib_rbp], rbp
    mov     [rcx + fib_rdi], rdi
    mov     [rcx + fib_rsi], rsi
    mov     [rcx + fib_rsp], rsp
    mov     [rcx + fib_r12], r12
    mov     [rcx + fib_r13], r13
    mov     [rcx + fib_r14], r14
    mov     [rcx + fib_r15], r15
    movdqu  [rcx + fib_xmm6], xmm6
    movdqu  [rcx + fib_xmm7], xmm7
    movdqu  [rcx + fib_xmm8], xmm8
    movdqu  [rcx + fib_xmm9], xmm9
    movdqu  [rcx + fib_xmm10],xmm10
    movdqu  [rcx + fib_xmm11],xmm11
    movdqu  [rcx + fib_xmm12],xmm12
    movdqu  [rcx + fib_xmm13],xmm13
    movdqu  [rcx + fib_xmm14],xmm14
    movdqu  [rcx + fib_xmm15],xmm15

    ; Set flags to zero.
    mov     qword [rcx + fib_flags], 0

    ; Save the position we need to return to.
    pop     r8
    mov     [rcx + fib_ret], r8
    mov     [rcx + fib_rsp], rsp

    ; Getting the PC in x64 is amazingly simple, compared to good ol' x86.
    lea     r8, [rip]
    mov     [rcx + fib_pc], r8

    ; Return back to caller, both after jumping here by using fiber_restore
    ; and by normally executing.
    ; It's saved into the struct simply because something else might've stomped
    ; the stack with another return address.
    mov     r8, [rcx + fib_ret]
    jmp     r8


; void sys_fiber_restore(struct sys_fiber *rcx)
sys_fiber_restore:
    ; Prolog, as wished by Windows.
    mov     [rsp + 8], rcx

    ; Restore non-volatile registers.
    mov     rbx, [rcx + fib_rbx]
    mov     rbp, [rcx + fib_rbp]
    mov     rdi, [rcx + fib_rdi]
    mov     rsi, [rcx + fib_rsi]
    mov     rsp, [rcx + fib_rsp]
    mov     r12, [rcx + fib_r12]
    mov     r13, [rcx + fib_r13]
    mov     r14, [rcx + fib_r14]
    mov     r15, [rcx + fib_r15]
    movdqu xmm6 ,[rcx + fib_xmm6]
    movdqu xmm7 ,[rcx + fib_xmm7]
    movdqu xmm8 ,[rcx + fib_xmm8]
    movdqu xmm9 ,[rcx + fib_xmm9]
    movdqu xmm10,[rcx + fib_xmm10]
    movdqu xmm11,[rcx + fib_xmm11]
    movdqu xmm12,[rcx + fib_xmm12]
    movdqu xmm13,[rcx + fib_xmm13]
    movdqu xmm14,[rcx + fib_xmm14]
    movdqu xmm15,[rcx + fib_xmm15]

    ; Set flags to 1, to indicate we restored our context.
    mov     qword [rcx + fib_flags], 1

    ; Jump to formerly saved pc
    mov     r8, [rcx + fib_pc]
    jmp     r8

