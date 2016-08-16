;
; ProjectKetchup:
; ---------------
;
; Copyright (c) 2008 Damien Chavarria
;

section .text
align 4

%imacro cglobalfunc 1
%ifdef __PIC__
global %1:function
%else
%ifdef  __NOU__
global %1
%else
global _%1
%define %1 _%1
%endif
%endif
%endmacro

cglobalfunc isCpuX86

isCpuX86:
    push      ebp
    mov       ebp, esp
    sub       esp, 8
    pushf
    pop       eax
    mov       [ebp-8], eax
    mov       eax, [ebp-8]
    xor       eax, 2097152
    mov       [ebp-8], eax
    mov       eax, [ebp-8]
    push      eax
    popf
    pushf
    pop       eax
    mov       [ebp-4], eax
    mov       eax, [ebp-8]
    xor       eax, [ebp-4]
    test      eax, 2097152
    je          .is_x86
    xor       eax, eax
    mov       esp, ebp
    pop       ebp
    ret
.is_x86:
    mov       eax, 1
    mov       esp, ebp
    pop       ebp
    ret


cglobalfunc doCpuId 

doCpuId:
    push      ebp
    mov       ebp, esp
    sub       esp, 8
    mov       [ebp-4], ebx
    mov       eax, [ebp+8]
    cpuid
    push      ebp
    mov       ebp, [ebp+12]
    mov       [ebp+ 0], eax
    mov       [ebp+ 4], ebx
    mov       [ebp+ 8], ecx
    mov       [ebp+12], edx
    pop       ebp
    mov       ebx, [ebp-4]
    mov       esp, ebp
    pop       ebp
    ret
