.section .multiboot

.set MBOOT2_MAGIC, 0xE85250D6
.set MBOOT2_ARCH, 0
.set MBOOT2_LENGTH, (mboot2_header_end - mboot2_header)
.set MBOOT2_CHECKSUM, -(MBOOT2_MAGIC + MBOOT2_ARCH + MBOOT2_LENGTH)

.align 8
mboot2_header:
.long MBOOT2_MAGIC
.long MBOOT2_ARCH
.long MBOOT2_LENGTH
.long MBOOT2_CHECKSUM

.word 0
.word 0
.long 8
mboot2_header_end:

.section .bss
.align 16
.skip 4096
stack_top:

.section .bootstrap
.code32

.extern gdt_ptr
.extern boot_page_area
.global start
start:
    cli

    mov $stack_top, %esp

    pushl $0
    pushl %eax
    pushl $0
    pushl %ebx

    mov $boot_page_area, %ebx
    mov %ebx, %cr3

    mov $0x1003, %eax
    add %ebx, %eax
    mov %eax, (%ebx)

    add $0x1000, %ebx
    add $0x1000, %eax
    mov %eax, (%ebx)

    add $0x1000, %ebx
    add $0x1000, %eax
    mov %eax, (%ebx)

    add $0x1000, %ebx

    mov $512, %ecx
    mov $0x87, %edx
.set_entry:
    mov %edx, (%ebx)
    add $0x1000, %edx
    add $8, %ebx
    loop .set_entry

    mov %cr4, %eax
    or $32, %eax
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr
    or $256, %eax
    wrmsr

    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    lgdt gdt_ptr
    ljmp $0x08, $realm64

.code64

.extern kernel_main
realm64:
    mov $0x00, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    pop %rsi
    pop %rdi

    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
