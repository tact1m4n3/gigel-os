#include <string.h>
#include <debug.h>
#include <spinlock.h>
#include <memory.h>
#include <cpu.h>
#include <process.h>

#define current_process this_cpu->process
#define scheduler this_cpu->scheduler

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
QUEUE_DEFINE(RUN_QUEUE);

spinlock_t pid_lock;
spinlock_t process_list_lock;
spinlock_t run_queue_lock;

static uint64_t alloc_pid() {
    acquire_lock(&pid_lock);
    uint64_t pid = next_pid++;
    release_lock(&pid_lock);
    return pid;
}

static struct process* lookup_process(uint64_t pid) {
    LIST_FOREACH(PROCESS_LIST)
        if (item->pid == pid)
            return item;
    return NULL;
}

static void init_process_stack(struct process* proc, uint64_t rip, uint64_t rsp) {
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

static struct process* scheduler_next() {
    if (QUEUE_EMPTY(RUN_QUEUE))
        return NULL;

    acquire_lock(&run_queue_lock);
    struct process* proc = QUEUE_NEXT(RUN_QUEUE);
    QUEUE_POP(RUN_QUEUE);

    if (proc->state == PROC_SLEEPING && read_tsc() < proc->wake_time) {
        QUEUE_PUSH(RUN_QUEUE, proc);
        release_lock(&run_queue_lock);
        return NULL;
    }

    release_lock(&run_queue_lock);
    return proc;
}

static struct regs* pagefault_callback(struct regs* r) {
    PANIC("pagefault at %x\n", read_cr2());
    return r;
}

struct process* new_process() {
    if (next_pid == 1)
        set_interrupt_handler(0x0E, &pagefault_callback);

    struct process* proc = (void*)calloc_page();
    proc->pid = alloc_pid();
    proc->state = PROC_READY;
    proc->p4 = alloc_page();
    proc->stack_ptr = (uint64_t)proc + PAGE_SIZE;

    memcpy((void*)proc->p4, (void*)kernel_p4, PAGE_SIZE);
    map_page(proc->p4, USERSPACE_END - PAGE_SIZE, alloc_page(), PAGE_WRITE | PAGE_USER);

    init_process_stack(proc, USERSPACE_START, USERSPACE_END);

    acquire_lock(&process_list_lock);
    LIST_APPEND(PROCESS_LIST, proc);
    release_lock(&process_list_lock);

    return proc;
}

void schedule_process(struct process* proc) {
    acquire_lock(&run_queue_lock);
    QUEUE_PUSH(RUN_QUEUE, proc);
    release_lock(&run_queue_lock);
}

void yield() {
    if (current_process)
        switch_stack(&current_process->stack_ptr, &scheduler->stack_ptr);
}

static void scheduler_loop() {
    INFO("scheduler started on cpu %x\n", this_cpu->id);

    while (1) {
        struct process* next;
        while (!(next = scheduler_next()));

        uint64_t time = read_tsc();
        next->run_time += time - next->sched_time;
        next->sched_time = time;

        current_process = next;
        current_process->state = PROC_RUNNING;
        write_cr3(current_process->p4);
        set_kernel_stack((uint64_t)next + PAGE_SIZE);

        switch_stack(&scheduler->stack_ptr, &current_process->stack_ptr);

        if (current_process->state == PROC_RUNNING)
            current_process->state = PROC_READY;

        if (current_process->state != PROC_KILLED)
            schedule_process(current_process);
        else
            INFO("process %x killed\n", current_process->pid);

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

int sys_fork(struct regs* r) {
    // INFO("forking process %x\n", current_process->pid);

    struct process* new = new_process();

    struct swtch_stack* stack = (void*)new->stack_ptr;
    memcpy((void*)&stack->r, (void*)r, sizeof(struct regs));
    stack->r.rax = 0;

    uint64_t i = USERSPACE_START;
    while (is_page(current_process->p4, i)) {
        clone_page(new->p4, current_process->p4, i);
        i += PAGE_SIZE;
    }

    i = USERSPACE_END - PAGE_SIZE;
    while (is_page(current_process->p4, i)) {
        clone_page(new->p4, current_process->p4, i);
        i -= PAGE_SIZE;
    }

    schedule_process(new);

    return new->pid;
}

void sys_sleep(uint64_t secs) {
    current_process->state = PROC_SLEEPING;
    current_process->wake_time = read_tsc() + secs * 1000000 * tsc_mhz;
    yield();
}
