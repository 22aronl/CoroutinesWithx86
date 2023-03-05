.text
.global save_rip
.global new_function
.extern next_ready_function

save_rip:
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    retq

new_function:
    movq %rsp, (%rdi)
    movq (%rsi), %rsp
    pushq *next_ready_function
    call *%rdx 
    retq

