section .text
.global ah5kos_thread_init
ah5kos_thread_init:
    pop %rdi
    pop %rsi
    pop %rdx
    call ah5kos_thread_enter
.section .note.GNU-stack,"",%progbits