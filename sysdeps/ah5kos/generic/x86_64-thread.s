.section .text
.global __ah5kos_thread_init
.hidden __ah5kos_thread_init
.type   __ah5kos_thread_init, @function
__ah5kos_thread_init:
    pop %rdi
    pop %rsi
    pop %rdx
    call ah5kos_thread_enter
.size   __ah5kos_thread_init, . - __ah5kos_thread_init
.section .note.GNU-stack,"",%progbits