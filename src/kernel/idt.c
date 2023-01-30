#include <ports.h>
#include <debug.h>
#include <cpu.h>

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

#define PIC_WAIT() outb(0x80, 0);

#define IDT_INTERRUPT 0xE
#define IDT_DPL(lvl) ((lvl) << 5) // TODO: check this
#define IDT_PRESENT 0x80

struct idt_entry {
    uint16_t base_low;
    uint16_t cs;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t RESERVED;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

#include "isr.S.h"

extern void load_idt(uint64_t);

struct idt_entry idt[256];
struct idt_ptr idt_ptr;
interrupt_handler_t int_handlers[256];

static void set_idt_entry(int id, uint64_t base, uint16_t cs, uint16_t flags) {
    idt[id].base_low = base & 0xFFFF;
    idt[id].cs = cs;
    idt[id].ist = 0;
    idt[id].flags = flags;
    idt[id].base_mid = (base >> 16) & 0xFFFF;
    idt[id].base_high = (base >> 32) & 0xFFFFFFFF;
}

static void irq_remap() {
    outb(PIC1_CMD_PORT, 0x11); PIC_WAIT();
    outb(PIC2_CMD_PORT, 0x11); PIC_WAIT();

    outb(PIC1_DATA_PORT, 0x20); PIC_WAIT();
    outb(PIC2_DATA_PORT, 0x20 + 8); PIC_WAIT();

    outb(PIC1_DATA_PORT, 0x04); PIC_WAIT();
    outb(PIC2_DATA_PORT, 0x02); PIC_WAIT();

    outb(PIC1_DATA_PORT, 0x01); PIC_WAIT();
    outb(PIC2_DATA_PORT, 0x01); PIC_WAIT();
}

void idt_init() {
    INFO("initializing idt on core %x\n", this_core->id);

    if (this_core->id == 0) {
        set_idt_entry(0, (uint64_t)&_isr0, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(1, (uint64_t)&_isr1, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(2, (uint64_t)&_isr2, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(3, (uint64_t)&_isr3, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(4, (uint64_t)&_isr4, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(5, (uint64_t)&_isr5, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(6, (uint64_t)&_isr6, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(7, (uint64_t)&_isr7, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(8, (uint64_t)&_isr8, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(9, (uint64_t)&_isr9, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(10, (uint64_t)&_isr10, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(11, (uint64_t)&_isr11, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(12, (uint64_t)&_isr12, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(13, (uint64_t)&_isr13, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(14, (uint64_t)&_isr14, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(15, (uint64_t)&_isr15, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(16, (uint64_t)&_isr16, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(17, (uint64_t)&_isr17, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(18, (uint64_t)&_isr18, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(19, (uint64_t)&_isr19, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(20, (uint64_t)&_isr20, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(21, (uint64_t)&_isr21, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(22, (uint64_t)&_isr22, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(23, (uint64_t)&_isr23, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(24, (uint64_t)&_isr24, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(25, (uint64_t)&_isr25, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(26, (uint64_t)&_isr26, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(27, (uint64_t)&_isr27, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(28, (uint64_t)&_isr28, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(29, (uint64_t)&_isr29, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(30, (uint64_t)&_isr30, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(31, (uint64_t)&_isr31, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(127, (uint64_t)&_isr127, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(128, (uint64_t)&_isr128, 0x08, IDT_INTERRUPT | IDT_DPL(3) | IDT_PRESENT);

        set_idt_entry(32, (uint64_t)&_irq0, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(33, (uint64_t)&_irq1, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(34, (uint64_t)&_irq2, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(35, (uint64_t)&_irq3, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(36, (uint64_t)&_irq4, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(37, (uint64_t)&_irq5, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(38, (uint64_t)&_irq6, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(39, (uint64_t)&_irq7, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(40, (uint64_t)&_irq8, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(41, (uint64_t)&_irq9, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(42, (uint64_t)&_irq10, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(43, (uint64_t)&_irq11, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(44, (uint64_t)&_irq12, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(45, (uint64_t)&_irq13, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(46, (uint64_t)&_irq14, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);
        set_idt_entry(47, (uint64_t)&_irq15, 0x08, IDT_INTERRUPT | IDT_DPL(0) | IDT_PRESENT);

        irq_remap();
    }

    idt_ptr.limit = 256*16-1;
    idt_ptr.base = (uint64_t)&idt;

    load_idt((uint64_t)&idt_ptr);
}

void set_interrupt_handler(int num, interrupt_handler_t handler) {
    int_handlers[num] = handler;
}

struct regs* isr_handler(struct regs* r) {
    interrupt_handler_t handler;
    if ((handler = int_handlers[r->int_no]) != NULL)
        r = handler(r);
    else if (r->int_no == 0x0E)
        PANIC("unhandled exception %x at %x on core %x\n", r->int_no, read_cr2(), this_core->id);
    else if (r->int_no < 0x20)
        PANIC("unhandled exception %x on core %x\n", r->int_no, this_core->id);
    return r;
}
