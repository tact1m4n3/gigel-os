#pragma once

#include <types.h>

#define MAX_CPUS 16

struct acpi_info {
    uint64_t lapic_base;
    int n_cpus;
    struct {
        uint8_t id;
        uint8_t lapic_id;
    } cpus[MAX_CPUS];
};

extern struct acpi_info acpi_info;
void acpi_init();
