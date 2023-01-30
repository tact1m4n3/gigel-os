#pragma once

#include <types.h>

#define PAGE_SIZE 0x1000

#define USERSPACE_START 0x50000000000
#define USERSPACE_END   0xFFFFF00000000000

#define MMIO_START 0xFFFFFF8000000000

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

extern uint64_t kernel_p4;

void memory_init();
uint64_t alloc_page();
uint64_t calloc_page();
void free_page(uint64_t page);
uint64_t get_page(uint64_t p4, uint64_t addr);
int map_page(uint64_t p4, uint64_t addr, uint64_t page, uint16_t flags);
