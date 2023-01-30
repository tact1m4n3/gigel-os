.section .text

.global switch_stack
switch_stack:
    push %rbp
    mov %rsp, %rbp
    push %r15
    push %r14
    push %r13
    push %r12
    push %rbx
    push %rbp

    mov %rsp, (%rdi)
    mov (%rsi), %rsp

    pop %rbp
    pop %rbx
    pop %r12
    pop %r13
    pop %r14
    pop %r15
    mov %rbp, %rsp
    pop %rbp
    ret
