#include <types.h>
#include <string.h>
#include <ports.h>
#include <debug.h>
#include <multiboot.h>
#include <spinlock.h>
#include <memory.h>
#include <acpi.h>
#include <cpu.h>
#include <lapic.h>
#include <process.h>
#include <syscall.h>

void test() {
    uint64_t pid;
    asm("int $0x80" : "=a"(pid) : "a"(2)); // fork
    for (int i = 0; i < 5; i++) {
        if (pid)
            asm("int $0x80" : : "a"(4), "b"("hello from original process\n")); // print
        else
            asm("int $0x80" : : "a"(4), "b"("hello from forked process\n")); // print
        asm("int $0x80" : : "a"(3), "b"(1)); // sleep
    }
    asm("int $0x80" : : "a"(1)); // exit
}

void kernel_main(uint64_t mboot_magic, void* mboot_info) {
    debug_init();

    multiboot_init(mboot_magic, mboot_info);
    INFO("bootloader=%s,cmdline=%s\n", boot_data.bootloader, boot_data.cmdline);

    memory_init();

    struct cpu* c = (void*)calloc_page();
    c->cpu = c;
    c->id = 0;
    write_msr(KERNEL_GS_BASE, (uint64_t)c);
    asm("swapgs");

    acpi_init();
    gdt_init();
    idt_init();
    lapic_init();
    syscall_init();
    smp_init();
    asm("sti");

    struct process* proc = new_process();
    map_page(proc->p4, USERSPACE_START, alloc_page(), PAGE_WRITE | PAGE_USER);
    write_cr3(proc->p4);
    memcpy((void*)USERSPACE_START, (void*)&test, PAGE_SIZE);
    write_cr3(kernel_p4);
    schedule_process(proc);

    start_scheduler();

    PANIC("reached end of kernel main\n");
    HALT;
}
