#pragma once

#include <types.h>
#include <list.h>
#include <queue.h>

#define PROC_READY 1
#define PROC_RUNNING 2
#define PROC_SLEEPING 3
#define PROC_KILLED 4

#define PROCESS_LIST process_list, struct process, process_list_prev, process_list_next
LIST_DECLARE(PROCESS_LIST);

#define RUN_QUEUE run_queue, struct process, run_queue_next
QUEUE_DECLARE(RUN_QUEUE);

struct process {
    uint64_t pid;
    uint64_t state;
    uint64_t p4;
    uint64_t stack_ptr;

    uint64_t sched_time;
    uint64_t run_time;
    uint64_t wake_time;

    LIST_SPOT(PROCESS_LIST);
    QUEUE_SPOT(RUN_QUEUE);
};

struct process* new_process();
void schedule_process(struct process* proc);
void yield();
void start_scheduler();
