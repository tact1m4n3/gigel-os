#pragma once

#include <types.h>

typedef uint64_t spinlock_t;
void acquire_lock(spinlock_t*);
void release_lock(spinlock_t*);

// #define acquire_lock(lock) while (__sync_lock_test_and_set(lock, 0x01))
// #define release_lock(lock) __sync_lock_release(lock)
