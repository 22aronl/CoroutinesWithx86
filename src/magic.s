.text
.global save_rip

save_rip:
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    retq

new_function:
    movq %rsp, (%rdi)
    movq (%rsi), %rsp
    call *%rdx
    retq

jmp_function_loc:
    movq %rdi, (%rip)
    retq
