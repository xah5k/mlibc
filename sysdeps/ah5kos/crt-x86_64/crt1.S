.section .text
.global _start
_start:
    xor %rbp, %rbp
    mov %rsp, %rdi      # entry_stack
    mov $main, %rsi     # main_fn
    and $-16, %rsp
    call __mlibc_entry
1:  hlt
    jmp 1b
.section .note.GNU-stack,"",%progbits