#include <debug.h>
#include <ports.h>
#include <spinlock.h>
#include <memory.h>
#include <acpi.h>
#include <cpu.h>
#include <lapic.h>
#include <process.h>

#define LAPIC_WRITE(addr, value) *(uint32_t*)(acpi_info.lapic_base + addr) = value;
#define LAPIC_READ(addr) *(uint32_t*)(acpi_info.lapic_base + addr)

uint64_t tsc_mhz;

static uint64_t measure_tsc_mhz() {
    outb(0x61, (inb(0x61) & 0xDD) | 0x01);

    uint16_t freq = 1193180/100;
    outb(0x43, 0xB2);
    outb(0x42, freq & 0xFF);
    outb(0x42, (freq >> 8) & 0xFF);

    outb(0x61, inb(0x61) & 0xDE);
    outb(0x61, inb(0x61) | 0x01);

    uint64_t stsc = read_tsc();
    if (inb(0x61) & 0x20)
        while (inb(0x61) & 0x20);
    else
        while (!(inb(0x61) & 0x20));
    uint64_t etsc = read_tsc();

    return (etsc - stsc) / 10000;
}

static void tsc_delay(uint64_t msecs) {
    uint64_t start = read_tsc();
    while (read_tsc() < start + msecs * tsc_mhz);
}

static struct regs* lapic_callback(struct regs* r) {
    // INFO("APIC TICK %x!\n", this_core->id);
    LAPIC_WRITE(0x0B0, 0x0);
    yield();
    return r;
}

void lapic_init() {
    if (this_core->id == 0) {
        set_interrupt_handler(0x7F, &lapic_callback);
        map_page(kernel_p4, acpi_info.lapic_base, acpi_info.lapic_base, PAGE_WRITE);
        tsc_mhz = measure_tsc_mhz();
    }

    LAPIC_WRITE(0x0F0, 0x127);
    LAPIC_WRITE(0x320, 0x7F);
    LAPIC_WRITE(0x3E0, 0x3);
    LAPIC_WRITE(0x380, 0xFFFFFFFF);
    tsc_delay(100000);
    LAPIC_WRITE(0x320, 0x10000);

    uint64_t lapic_freq = 0xFFFFFFFF - LAPIC_READ(0x390);

    LAPIC_WRITE(0x320, 0x7F | 0x20000);
    LAPIC_WRITE(0x3E0, 0x3);
    LAPIC_WRITE(0x380, lapic_freq);
}

void lapic_wake(uint8_t id, uint64_t addr) {
    LAPIC_WRITE(0x310, id << 24);
    LAPIC_WRITE(0x300, 0xC500);
    tsc_delay(200);
    LAPIC_WRITE(0x310, id << 24);
    LAPIC_WRITE(0x300, 0x8500);
    tsc_delay(10000);
    for (int i = 0; i < 2; i++) {
        LAPIC_WRITE(0x310, id << 24);
        LAPIC_WRITE(0x300, 0x600 | (addr >> 12));
        tsc_delay(200);
    }
}

void lapic_send_ipi(int num) {
    LAPIC_WRITE(0x300, 0xC0000 | num);
    while (LAPIC_READ(0x300) & (1 << 12));
}
