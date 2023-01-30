#include <string.h>
#include <debug.h>
#include <cpu.h>

#define GDT_CODE (0x18 << 8)
#define GDT_DATA (0x12 << 8)
#define GDT_TSS (0x09 << 8)
#define GDT_DPL(lvl) ((lvl) << 13)
#define GDT_PRESENT (1 << 15)
#define GDT_LONG (1 << 21)

struct gdt_entry {
    uint32_t addr;
    uint32_t flags;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct tss {
    uint32_t RESERVED;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t RESERVED;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t RESERVED;
    uint16_t RESERVED;
    uint16_t iopb;
} __attribute__((packed));

extern void load_gdt(uint64_t);

struct gdt_entry boot_gdt[] = {
    {0, 0},
    {0, GDT_PRESENT | GDT_DPL(0) | GDT_CODE | GDT_LONG},
    {0, GDT_PRESENT | GDT_DPL(3) | GDT_CODE | GDT_LONG},
    {0, GDT_PRESENT | GDT_DPL(3) | GDT_DATA},
    {0, 0},
    {0, 0},
};

struct gdt_ptr gdt_ptr = {2*8-1, (uint64_t)&boot_gdt};

void gdt_init() {
    INFO("initializing gdt on core %x\n", this_core->id);

    struct gdt_entry* gdt = (void*)&this_core->gdt;
    memcpy(gdt, boot_gdt, sizeof(boot_gdt));

    struct tss* tss = (void*)&this_core->tss;
    tss->iopb = sizeof(tss);

    uint32_t tss_limit = sizeof(struct tss)-1;
    uint64_t tss_base = (uint64_t)tss;
    gdt[4].flags = GDT_PRESENT | GDT_TSS;
    gdt[4].flags |= ((tss_base >> 24) & 0xFF) << 24;
    gdt[4].flags |= (tss_base >> 16) & 0xFF;
    gdt[4].flags |= ((tss_limit >> 16) & 0xF) << 16;
    gdt[4].addr = ((tss_base & 0xFFFF) << 16) | (tss_limit & 0xFFFF);
    gdt[5].addr = (tss_base >> 32) & 0xFFFFFFFF;

    gdt_ptr.limit = 6*8-1;
    gdt_ptr.base = (uint64_t)gdt;

    load_gdt((uint64_t)&gdt_ptr);
}

void set_kernel_stack(uint64_t rsp0) {
    struct tss* tss = (void*)&this_core->tss;
    tss->rsp0 = rsp0;
}
