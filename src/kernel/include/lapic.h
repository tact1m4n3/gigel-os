#pragma once

#include <types.h>

void lapic_init();
void lapic_wake(uint8_t id, uint64_t addr);
void lapic_send_ipi(int num);

