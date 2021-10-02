; 64 Bit: rdi: ptr to old struct | rsi: ptr to new struct
; The new struct contains the function ptr, the argument and stack ptr.
;
; Caller saved: RAX, RDI, RSI, RDX, RCX, R8, R9, R10, R11
; Callee saved: RBX, RSP, RBP, R12, R13, R14, R15

; struct sys_fiber, equal to the struct defined in sys_fibers.h
%define fib_rbx     0x0
%define fib_rsp     0x8
%define fib_rbp     0x10
%define fib_r12     0x18
%define fib_r13     0x20
%define fib_r14     0x28
%define fib_r15     0x30
%define fib_ret     0x38
%define fib_xmm8    0x40
%define fib_xmm9    0x50
%define fib_xmm10   0x60
%define fib_xmm11   0x70
%define fib_xmm12   0x80
%define fib_xmm13   0x90
%define fib_xmm14   0xA0
%define fib_xmm15   0xB0
%define fib_pc      0xC0
%define fib_data    0xC8
%define fib_ssize   0xD0
%define fib_stack   0xD8
%define fib_flags   0xE0

global sys_fiber_save
global sys_fiber_restore
global sys_fiber_switch

section .text

; void sys_fiber_switch(struct sys_fiber *rdi, struct sys_fiber *rsi)
sys_fiber_switch:
    call    sys_fiber_save
    cmp     qword [rdi + 424], 1
    je      .restored

    ; Check if sys_fiber.fp is not NULL, and if execute it.
    cmp     qword [rsi + 392], 0
    je      .null

    ; Setup our new stack
    mov     rcx, [rsi + 408]        ; sys_fiber.stacksize
    mov     rsp, [rsi + 416]             ; sys_fiber.stack
    mov     rbp, [rsi + 416]
    add     rsp, rcx
    add     rbp, rcx
    push    rdi

    ; Execute the function assigned with our new context.
    mov     rcx, [rsi + 392]        ; sys_fiber.fp
    mov     rdi, [rsi + 400]          ; sys_fiber.data

    ; Align the stack to make macOS happy.
    sub     rsp, 0x8
    call    rcx
    add     rsp, 0x8

    ; Restore our old context
    pop     rsi

.null:
    mov     rdi, rsi
    call    sys_fiber_restore

.restored:
    ret


; void sys_fiber_save(struct sys_fiber *rdi)
sys_fiber_save:
    ; Save all registers, except eax, which contains the address to our struct.
    mov     [rdi],      rax
    mov     [rdi + 8],  rbx
    mov     [rdi + 16], rcx
    mov     [rdi + 24], rdx
    mov     [rdi + 32], rsi
    ; Do not save rdi itself, this place is used for our return function.
    ; mov     [rdi + 40], rdi
    ; mov     [rdi + 48], rsp
    mov     [rdi + 56], rbp
    mov     [rdi + 64], r8
    mov     [rdi + 72], r9
    mov     [rdi + 80], r10
    mov     [rdi + 88], r11
    mov     [rdi + 96], r12
    mov     [rdi + 104],r13
    mov     [rdi + 112],r14
    mov     [rdi + 120],r15

    ; Save all xmm registers, even though not all have to, but better safe
    ; than sorry.
    movdqu  [rdi + 136],xmm0
    movdqu  [rdi + 152],xmm1
    movdqu  [rdi + 168],xmm2
    movdqu  [rdi + 184],xmm3
    movdqu  [rdi + 200],xmm4
    movdqu  [rdi + 216],xmm5
    movdqu  [rdi + 232],xmm6
    movdqu  [rdi + 248],xmm7
    movdqu  [rdi + 264],xmm8
    movdqu  [rdi + 280],xmm9
    movdqu  [rdi + 296],xmm10
    movdqu  [rdi + 312],xmm11
    movdqu  [rdi + 328],xmm12
    movdqu  [rdi + 344],xmm13
    movdqu  [rdi + 360],xmm14
    movdqu  [rdi + 376],xmm15

    ; Set flags to zero.
    mov     qword [rdi + 424], 0

    ; Save the position we need to return to.
    pop     rcx
    mov     [rdi + 40], rcx
    mov     [rdi + 48], rsp

    ; Getting the PC in x64 is amazingly simple, compared to good ol' x86.
    lea     rcx, [rip]
    mov     [rdi + 128], rcx

    ; Return back to caller, both after jumping here by using fiber_restore
    ; and by normally executing.
    ; It's saved into the struct simply because something else might've stomped
    ; the stack with another return address.
    mov     rcx, [rdi + 40]
    jmp     rcx


; void sys_fiber_restore(struct sys_fiber *rdi)
sys_fiber_restore:
    ; Restore all registers, except rdi, which contains the address to our struct.
    mov     rax,  [rdi]
    mov     rbx,  [rdi + 8]
    mov     rcx,  [rdi + 16]
    mov     rdx,  [rdi + 24]
    mov     rsi,  [rdi + 32]
    ; Reserved space.
    ; mov     rdi,  [rdi + 40]
    mov     rsp,  [rdi + 48]
    mov     rbp,  [rdi + 56]
    mov     r8,   [rdi + 64]
    mov     r9,   [rdi + 72]
    mov     r10,  [rdi + 80]
    mov     r11,  [rdi + 88]
    mov     r12,  [rdi + 96]
    mov     r13,  [rdi + 104]
    mov     r14,  [rdi + 112]
    mov     r15,  [rdi + 120]
    movdqu  xmm0, [rdi + 136]
    movdqu  xmm1, [rdi + 152]
    movdqu  xmm2, [rdi + 168]
    movdqu  xmm3, [rdi + 184]
    movdqu  xmm4, [rdi + 200]
    movdqu  xmm5, [rdi + 216]
    movdqu  xmm6, [rdi + 232]
    movdqu  xmm7, [rdi + 248]
    movdqu  xmm8, [rdi + 264]
    movdqu  xmm9, [rdi + 280]
    movdqu  xmm10,[rdi + 296]
    movdqu  xmm11,[rdi + 312]
    movdqu  xmm12,[rdi + 328]
    movdqu  xmm13,[rdi + 344]
    movdqu  xmm14,[rdi + 360]
    movdqu  xmm15,[rdi + 376]

    ; Set flags to 1, to indicate we restored our context.
    mov     qword [rdi + 424], 1

    ; Jump to formerly saved pc
    mov     rcx, [rdi + 128]
    jmp     rcx

