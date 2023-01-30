#include <string.h>
#include <ports.h>
#include <debug.h>
#include <memory.h>
#include <acpi.h>
#include <cpu.h>
#include <lapic.h>

extern void* ap_trampoline_start;
extern void* ap_trampoline_end;

struct cpu __seg_gs* this_core;

int ap_started;
int ap_id;
uint64_t ap_stack;

void smp_init() {
    if (acpi_info.n_cpus <= 1) return;

    memcpy((void*)0x8000, &ap_trampoline_start, (uint64_t)&ap_trampoline_end - (uint64_t)&ap_trampoline_start);

    INFO("starting other cpu cores(n_cpus=%x)\n", acpi_info.n_cpus);
    for (int i = 1; i < acpi_info.n_cpus; i++) {
        ap_started = 0;
        ap_id = acpi_info.cpus[i].id;
        ap_stack = alloc_page();

        lapic_wake(acpi_info.cpus[i].lapic_id, 0x8000);
        SPIN(!ap_started);
    }
}

void ap_main() {
    struct cpu* c = (void*)calloc_page();
    c->cpu = c;
    c->id = ap_id;
    write_msr(KERNEL_GS_BASE, (uint64_t)c);
    asm("swapgs");

    ap_started = 1;

    INFO("started core %x\n", this_core->id);

    gdt_init();
    idt_init();
    lapic_init();
    asm("sti");

    start_scheduler();

    PANIC("reached end of ap main\n");
    HALT;
}
