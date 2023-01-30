#include <string.h>
#include <debug.h>
#include <spinlock.h>
#include <memory.h>
#include <cpu.h>
#include <process.h>

#define current_process this_core->process
#define scheduler this_core->scheduler

struct swtch_stack {
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rbp2;
    uint64_t ret;
    struct regs r;
};

extern void isr_return();
extern void switch_stack(uint64_t*, uint64_t*);

extern uint64_t tsc_mhz;

uint64_t next_pid = 1;

LIST_DEFINE(PROCESS_LIST);
QUEUE_DEFINE(READY_QUEUE);
QUEUE_DEFINE(WAIT_QUEUE);
QUEUE_DEFINE(RETIRE_QUEUE);

spinlock_t process_list_lock;
spinlock_t ready_queue_lock;
spinlock_t wait_queue_lock;
spinlock_t retire_queue_lock;

struct process* new_process() {
    struct process* proc = (void*)calloc_page();
    proc->pid = next_pid++;
    proc->state = PROC_NEW;
    proc->p4 = alloc_page();
    proc->stack_ptr = (uint64_t)proc + PAGE_SIZE;

    memcpy((void*)proc->p4, (void*)kernel_p4, PAGE_SIZE);
    map_page(proc->p4, USERSPACE_END - PAGE_SIZE, alloc_page(), PAGE_WRITE | PAGE_USER);

    reset_process_state(proc, USERSPACE_START, USERSPACE_END);

    return proc;
}

void reset_process_state(struct process* proc, uint64_t rip, uint64_t rsp) {
    proc->stack_ptr = (uint64_t)proc + PAGE_SIZE - sizeof(struct swtch_stack);

    struct swtch_stack* stack = (struct swtch_stack*)proc->stack_ptr;
    stack->rbp = (uint64_t)&stack->rbp2;
    stack->ret = (uint64_t)&isr_return;
    stack->r.rip = rip;
    stack->r.cs = 0x10 | 3;
    stack->r.ss = 0x18 | 3;
    stack->r.rflags = 0x200;
    stack->r.rsp = rsp;
}

void yield() {
    if (current_process)
        switch_stack(&current_process->stack_ptr, &scheduler->stack_ptr);
}

void schedule_process(struct process* proc) {
    if (proc->state == PROC_NEW || proc->state == PROC_RUNNING) {
        INFO("process %x ready\n", proc->pid);
        proc->state = PROC_READY;
        acquire_lock(&ready_queue_lock);
        QUEUE_PUSH(READY_QUEUE, proc);
        release_lock(&ready_queue_lock);
    } else if (proc->state == PROC_SLEEPING) {
        INFO("process %x is sleeping\n", proc->pid);
        acquire_lock(&wait_queue_lock);
        QUEUE_PUSH(WAIT_QUEUE, proc);
        release_lock(&wait_queue_lock);
    } else if (proc->state == PROC_KILLED) {
        INFO("process %x exited\n", proc->pid);
        proc->state = PROC_RETIRED;
        acquire_lock(&retire_queue_lock);
        QUEUE_PUSH(RETIRE_QUEUE, proc);
        release_lock(&retire_queue_lock);
    }
}

static struct process* scheduler_next() {
    if (!QUEUE_EMPTY(WAIT_QUEUE)) {
        acquire_lock(&wait_queue_lock);
        struct process* proc = QUEUE_POP(WAIT_QUEUE);
        if (proc->state == PROC_SLEEPING && read_tsc() < proc->wake_time) {
            QUEUE_PUSH(WAIT_QUEUE, proc);
            proc = NULL;
        }
        release_lock(&wait_queue_lock);
        return proc;
    }
    if (!QUEUE_EMPTY(READY_QUEUE)) {
        acquire_lock(&ready_queue_lock);
        struct process* proc = QUEUE_POP(READY_QUEUE);
        release_lock(&ready_queue_lock);
        return proc;
    }
    return NULL;
}

static void scheduler_loop() {
    INFO("scheduler started on core %x\n", this_core->id);

    while (1) {
        struct process* next;
        SPIN(!(next = scheduler_next()));

        uint64_t time = read_tsc();
        next->run_time += time - next->sched_time;
        next->sched_time = time;

        current_process = next;
        current_process->state = PROC_RUNNING;
        write_cr3(current_process->p4);
        set_kernel_stack((uint64_t)next + PAGE_SIZE);

        switch_stack(&scheduler->stack_ptr, &current_process->stack_ptr);

        schedule_process(current_process);
        current_process = NULL;
    }
}

void start_scheduler() {
    struct process* sched = scheduler = (void*)calloc_page();
    sched->pid = -1;
    sched->p4 = kernel_p4;
    sched->stack_ptr = (uint64_t)sched + PAGE_SIZE - sizeof(struct swtch_stack);

    struct swtch_stack* stack = (void*)sched->stack_ptr;
    stack->rbp = (uint64_t)&stack->rbp2;
    stack->ret = (uint64_t)&scheduler_loop;

    uint64_t unused;
    switch_stack(&unused, &scheduler->stack_ptr);
}

void sys_exit() {
    current_process->state = PROC_KILLED;
    yield();
}

void sys_sleep(uint64_t secs) {
    current_process->state = PROC_SLEEPING;
    current_process->wake_time = read_tsc() + secs * 1000000 * tsc_mhz;
    yield();
}
