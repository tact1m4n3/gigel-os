.section .text

.global load_gdt
load_gdt:
    lgdt (%rdi)

    mov $0x00, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    pushq $0x08
    movabs $.reload_cs, %rax
    push %rax
    retfq

.reload_cs:
    mov $0x20, %ax
    ltr %ax
    ret

.global load_idt
load_idt:
    lidt (%rdi)
    ret

.global read_cr2
read_cr2:
    mov %cr2, %rax
    ret

.global write_cr2
write_cr2:
    mov %rdi, %cr2
    ret

.global read_cr3
read_cr3:
    mov %cr3, %rax
    ret

.global write_cr3
write_cr3:
    mov %rdi, %cr3
    ret

.global read_msr
read_msr:
    mov %rdi, %rcx
    rdmsr
    shl $32, %rdx
    or %rdx, %rax
    ret

.global write_msr
write_msr:
    mov %rdi, %rcx
    mov %rsi, %rax
    mov %rsi, %rdx
    shr $32, %rdx
    wrmsr
    ret

.global read_tsc
read_tsc:
    rdtsc
    shl $32, %rdx
    or %rdx, %rax
    ret
