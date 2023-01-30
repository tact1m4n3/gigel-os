#pragma once

#include <types.h>

void* memcpy(void* dst, void* src, uint64_t n);
void* memset(void* dst, uint8_t val, uint64_t n);
int memcmp(void* buf1, void* buf2, uint64_t n);
uint64_t strlen(char* str);
char* strcpy(char* dst, char* src);
int strcmp(char* str1, char* str2);
