#include <debug.h>
#include <cpu.h>
#include <process.h>
#include <syscall.h>

extern void sys_exit();
extern int sys_fork(struct regs* r);
extern void sys_sleep(uint64_t secs);

static struct regs* syscall_callback(struct regs* r) {
    switch (r->rax) {
    case 0x01:
        sys_exit();
        break;
    case 0x02:
        r->rax = sys_fork(r);
        break;
    case 0x03:
        sys_sleep(r->rbx);
        break;
    case 0x04:
        print("%s", r->rbx);
        break;
    }
    return r;
}

void syscall_init() {
    set_interrupt_handler(0x80, &syscall_callback);
}
