#pragma once

#include <types.h>

#define INFO(fmt, ...) print("[info]  "), print(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) print("[warn]  "), print(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) print("[error] "), print(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) print("[fatal] "), print(fmt, ##__VA_ARGS__)

#define PANIC(fmt, ...) do { FATAL(fmt, ##__VA_ARGS__); HALT; } while (0)

void debug_init();
void print(char* fmt, ...);
