#pragma once

#include <types.h>
#include <process.h>

struct regs {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

struct cpu {
    void* cpu;
    int id;
    uint8_t gdt[48];
    uint8_t tss[108];
    struct process* process;
    struct process* scheduler;
};

extern struct cpu __seg_gs* this_core;

uint64_t read_cr2();
void write_cr2(uint64_t);
uint64_t read_cr3();
void write_cr3(uint64_t);

#define KERNEL_GS_BASE 0xC0000102
uint64_t read_msr(uint64_t);
void write_msr(uint64_t, uint64_t);

uint64_t read_tsc();

void gdt_init();
void set_kernel_stack(uint64_t rsp0);

typedef struct regs* (*interrupt_handler_t)(struct regs*);

void idt_init();
void set_interrupt_handler(int num, interrupt_handler_t handler);

void smp_init();
