#pragma once

#define NULL ((void*)0)
#define offsetof(type, member) __builtin_offsetof(type, member)

#define _CONCAT(x, y) x ## y
#define _EXPAND(x, y) _CONCAT(x, y)
#define RESERVED _EXPAND(reserved, __LINE__)

#define SPIN(x) while (x) asm("pause")
#define HALT while (1) asm("hlt")

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int64_t;
typedef unsigned long uint64_t;

typedef __builtin_va_list va_list;
#define va_start(ap,last) __builtin_va_start(ap, last)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap,type) __builtin_va_arg(ap,type)
#define va_copy(dest, src) __builtin_va_copy(dest,src)
