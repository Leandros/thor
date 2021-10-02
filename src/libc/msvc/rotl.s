
global _rotl
global _rotl64

_rotl:
    xchg    ecx, edx
    rol     edx, cl
    mov     eax, edx
    ret

_rotl64:
    xchg    rcx, rdx
    rol     rdx, cl
    mov     rax, rdx
    ret
