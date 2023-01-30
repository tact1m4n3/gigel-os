.section .text

.macro ISR_NOERR idx
    .global _isr\idx
    _isr\idx:
        pushq $0x00
        pushq $\idx
        jmp isr_wrapper
.endm

.macro ISR_ERR idx
    .global _isr\idx
    _isr\idx:
        pushq $\idx
        jmp isr_wrapper
.endm

.macro IRQ idx
    .global _irq\idx
    _irq\idx:
        pushq $0x00
        pushq $(\idx + 0x20)
        jmp isr_wrapper
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 127
ISR_NOERR 128

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

.macro _swapgs
    cmpq $0x08, 24(%rsp) // check if we are in userspace
    je 1f
    swapgs
1:
.endm

.extern isr_handler
isr_wrapper:
    _swapgs

    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rdi
    push %rsi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rsp, %rdi
    call isr_handler
    mov %rax, %rsp

.global isr_return
isr_return:
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rsi
    pop %rdi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax

    _swapgs

    add $16, %rsp

    iretq
