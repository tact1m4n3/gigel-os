#pragma once

#include <types.h>

#define MMAP_FREE 1

struct boot_data {
    int version;
    char* bootloader;
    char* cmdline;
    uint64_t mmap_size;
    uint64_t mmap_len;
    void* mmap;
};

extern struct boot_data boot_data;

void multiboot_init(uint64_t magic, void* info);
int multiboot_is_page_used(uint64_t page);
int multiboot_get_memory_area(uint64_t i, uint64_t* type, uint64_t* start, uint64_t* end);
