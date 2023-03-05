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
    call *%rdx
    call next_ready_function #stack alightment TODO:  
    retq

jmp_function_loc:
    movq %rdi, (%rip)
    retq
