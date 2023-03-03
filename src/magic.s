
save_rip:
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    retq

jmp_function_loc:
    movq %rdi, (%rip)
    retq