#pragma once

#include <types.h>
#include <list.h>
#include <queue.h>

#define PROC_NEW 0
#define PROC_READY 1
#define PROC_RUNNING 2
#define PROC_SLEEPING 3
#define PROC_KILLED 4
#define PROC_RETIRED 5

#define PROCESS_LIST process_list, struct process, process_list_prev, process_list_next
LIST_DECLARE(PROCESS_LIST);

#define READY_QUEUE ready_queue, struct process, ready_queue_next
QUEUE_DECLARE(READY_QUEUE);

#define WAIT_QUEUE wait_queue, struct process, wait_queue_next
QUEUE_DECLARE(WAIT_QUEUE);

#define RETIRE_QUEUE retire_queue, struct process, retired_queue_next
QUEUE_DECLARE(RETIRE_QUEUE);

struct process {
    uint64_t pid;
    uint64_t state;
    uint64_t p4;
    uint64_t stack_ptr;

    uint64_t sched_time;
    uint64_t run_time;
    uint64_t wake_time;

    LIST_SPOT(PROCESS_LIST);
    QUEUE_SPOT(READY_QUEUE);
    QUEUE_SPOT(WAIT_QUEUE);
    QUEUE_SPOT(RETIRE_QUEUE);
};

struct process* new_process();
void reset_process_state(struct process* proc, uint64_t entry, uint64_t stack);
void yield();
void schedule_process(struct process* proc);
void start_scheduler();
