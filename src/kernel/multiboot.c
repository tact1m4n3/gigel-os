#include <string.h>
#include <debug.h>
#include <multiboot.h>
#include <memory.h>

#define MBOOT2_MAGIC 0x36d76289
#define MBOOT2_CMDLINE 1
#define MBOOT2_BOOTLOADER 2
#define MBOOT2_MEMORY_MAP 6

#define NEXT_TAG(tag) ((struct mb2_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7)))

struct mb2_info {
    uint32_t size;
    uint32_t reserved;
    uint8_t tags[];
};

struct mb2_tag {
    uint32_t type;
    uint32_t size;
    uint8_t data[];
};

struct mb2_mmap {
    uint32_t entry_size;
    uint32_t entry_version;
    uint8_t entries[];
};

typedef struct mb2_mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t RESERVED;
} mb2_mmap_entry_t;

struct boot_data boot_data;

static void parse_multiboot2(struct mb2_info* info) {
    struct mb2_tag* tag = (void*)&info->tags[0];
    while (tag->type) {
        struct mb2_mmap* mmap;
        switch (tag->type) {
            case MBOOT2_BOOTLOADER:
                boot_data.bootloader = (char*)&tag->data[0];
                break;
            case MBOOT2_CMDLINE:
                boot_data.cmdline = (char*)&tag->data[0];
                break;
            case MBOOT2_MEMORY_MAP:
                mmap = (void*)&tag->data[0];
                boot_data.mmap_size = tag->size - 16;
                boot_data.mmap_len = boot_data.mmap_size / mmap->entry_size;
                boot_data.mmap = (void*)&mmap->entries[0];
                break;
            default:
                break;
        }
        tag = NEXT_TAG(tag);
    }
}

void multiboot_init(uint64_t magic, void* info) {
    INFO("parsing multiboot info\n");
    if (magic == MBOOT2_MAGIC) {
        boot_data.version = 2;
        parse_multiboot2(info);
    } else {
        FATAL("bootloader not supported!\n");
        for (;;);
    }
}

int multiboot_is_page_used(uint64_t page) {
#define overlap(st, sz) ((uint64_t)(st) < (page + PAGE_SIZE) && ((uint64_t)(st) + (sz)) > page)
    return overlap(boot_data.bootloader, strlen(boot_data.bootloader))
        && overlap(boot_data.cmdline, strlen(boot_data.cmdline))
        && overlap(boot_data.mmap, boot_data.mmap_size);
}

int multiboot_get_memory_area(uint64_t i, uint64_t* type, uint64_t* start, uint64_t* end) {
    if (i >= boot_data.mmap_len)
        return 0;

    if (boot_data.version == 2) {
        uint64_t entry_size = boot_data.mmap_size / boot_data.mmap_len;
        struct mb2_mmap_entry* entry = (void*)((uint64_t)boot_data.mmap + i * entry_size);
        *type = entry->type;
        *start = entry->base_addr;
        *end = entry->base_addr + entry->length;
    }

    return 1;
}
