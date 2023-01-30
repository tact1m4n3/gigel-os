.code16
.org 0x0

.global ap_trampoline_start
.extern boot_page_area
.extern gdt_ptr
ap_trampoline_start:
    cli

    mov $0xA0, %eax
    mov %eax, %cr4

    mov $boot_page_area, %edx
    mov %edx, %cr3

    mov $0xC0000080, %ecx
    rdmsr
    or $256, %eax
    wrmsr

    mov $0x80000011, %eax
    mov %eax, %cr0

    addr32 lgdt %cs:ap_gdt_ptr-ap_trampoline_start
    data32 jmp $0x08, $ap_realm64

.extern boot_gdt
ap_gdt_ptr:
.word 2*8-1
.quad boot_gdt

.code64
.extern ap_stack
.extern ap_main
ap_realm64:
    mov $0x00, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    mov ap_stack, %rsp

    call ap_main

.halt:
    cli
    hlt
    jmp .halt

.global ap_trampoline_end
ap_trampoline_end:
