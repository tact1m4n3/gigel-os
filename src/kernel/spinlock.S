.global acquire_lock
acquire_lock:
    lock btsq $1, (%rdi)
    jc .spin
    ret

.spin:
    pause
    testq $1, (%rdi)
    jz .spin
    jmp acquire_lock

.global release_lock
release_lock:
    movq $0, (%rdi)
    ret
