#include <string.h>

void* memcpy(void* dst, void* src, uint64_t n) {
    uint8_t* p1 = (uint8_t*)dst;
    uint8_t* p2 = (uint8_t*)src;
    while (n--) {
        *p1++ = *p2++;
    }
    return dst;
}

void* memset(void* dst, uint8_t val, uint64_t n) {
    uint8_t* p = (uint8_t*)dst;
    while (n--) {
        *p++ = val;
    }
    return dst;
}

int memcmp(void* buf1, void* buf2, uint64_t n) {
    uint8_t* p1 = (uint8_t*)buf1;
    uint8_t* p2 = (uint8_t*)buf2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++, p2++;
    }
    return 0;
}

uint64_t strlen(char* str) {
    uint64_t len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

char* strcpy(char* dst, char* src) {
    return memcpy(dst, src, strlen(src) + 1);
}

int strcmp(char* str1, char* str2) {
    return memcmp(str1, str2, strlen(str1) + 1);
}
