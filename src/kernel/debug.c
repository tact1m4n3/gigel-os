#include <string.h>
#include <ports.h>
#include <spinlock.h>
#include <debug.h>

#define COM1_PORT 0x3F8

spinlock_t print_lock;

static void hex2str(char* buf, uint64_t val) {
    char syms[] = "0123456789ABCDEF";
    for (int i = 15; i >= 0; i--) {
        uint64_t offset = val % 16;
        buf[i] = syms[offset];
        val /= 16;
    }
}

static void print_char(char c) {
    while ((inb(COM1_PORT + 5) & 0x20) == 0);
    outb(COM1_PORT, c);
}

static void print_string(char* s) {
    while (*s)
        print_char(*s++);
}

static uint64_t print_with_args(char* fmt, va_list args) {
    uint64_t i = 0, j = 0, len = strlen(fmt);
    while (i < len) {
        if (fmt[i] != '%') {
            print_char(fmt[i++]);
            continue;
        }

        char buf[17];
        switch (fmt[++i]) {
            case 'c':
                print_char((char)va_arg(args, int));
                break;
            case 's':
                print_string((char*)va_arg(args, char*));
                break;
            case 'x':
                hex2str(buf, (uint64_t)va_arg(args, uint64_t));
                buf[16] = '\0';
                print_string(buf);
                break;
            case '%':
                print_char('%');
                break;
            default:
                print_char(fmt[i]);
                break;
        }

        i++;
    }
    return j;
}

void debug_init() {
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x80);
    outb(COM1_PORT + 0, 0x01);
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0x07);
    outb(COM1_PORT + 4, 0x0B);
}

void print(char* fmt, ...) {
    acquire_lock(&print_lock);
    va_list args;
    va_start(args, fmt);
    print_with_args(fmt, args);
    va_end(args);
    release_lock(&print_lock);
}
