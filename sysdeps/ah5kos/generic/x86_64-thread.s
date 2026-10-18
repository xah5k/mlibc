.section .text
.global __ah5kos_thread_init
__ah5kos_thread_init:
    pop %rdi
    pop %rsi
    pop %rdx
    call ah5kos_thread_enter
.section .note.GNU-stack,"",%progbits