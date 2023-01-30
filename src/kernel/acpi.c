#include <string.h>
#include <debug.h>
#include <acpi.h>

#define RSDP_SIGNATURE "RSD PTR"
#define MADT_SIGNATURE "APIC"

#define MADT_CPU 0

struct rsdp {
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt;
    uint32_t len;
    uint64_t xsdt;
    uint8_t checksum2;
    uint8_t RESERVED[3];
} __attribute__((packed));

struct sdt {
    uint8_t signature[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t table_id[8];
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
    uint8_t data[];
} __attribute__((packed));

struct madt {
  uint32_t lapic_base;
  uint32_t flags;
  uint8_t data[];
} __attribute__((packed));

struct madt_entry {
    uint8_t type;
    uint8_t len;
    union {
        struct {
            uint8_t id;
            uint8_t lapic_id;
            uint32_t flags;
        } __attribute__((packed)) cpu;
    };
} __attribute__((packed));

struct acpi_info acpi_info;

static struct rsdp* find_rsdp() {
    for (uint64_t i = 0xE0000; i < 0x100000; i += 16)
        if (!memcmp((void*)i, RSDP_SIGNATURE, 7))
            return (void*)i;
    return NULL;
}

static void parse_madt(struct madt* madt, uint64_t len) {
    acpi_info.lapic_base = madt->lapic_base;

    uint64_t e = (uint64_t)&madt->data[0], end = (uint64_t)madt + len;
    while (e < end) {
        int i;
        struct madt_entry* entry = (void*)e;
        switch (entry->type) {
        case MADT_CPU:
            i = acpi_info.n_cpus;
            acpi_info.cpus[i].id = entry->cpu.id;
            acpi_info.cpus[i].lapic_id = entry->cpu.lapic_id;
            acpi_info.n_cpus++;
            break;
        }
        e += entry->len;
    }
}

static void parse_rsdt(struct sdt* rsdt) {
    uint32_t entries = (rsdt->len - sizeof(struct sdt)) / 4;
    for (uint32_t i = 0; i < entries; i++) {
        struct sdt* table = (void*)((uint64_t)((uint32_t*)rsdt->data)[i]);
        if (!memcmp(table->signature, MADT_SIGNATURE, 4))
            parse_madt((void*)table->data, table->len);
    }
}

void acpi_init() {
    INFO("parsing acpi tables\n");
    struct rsdp* rsdp = find_rsdp();
    parse_rsdt((void*)((uint64_t)rsdp->rsdt));
}
