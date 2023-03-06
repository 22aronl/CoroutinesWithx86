.text
.global save_rip
.global new_function
.extern next_ready_function
.extern me
.extern send

save_rip:
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx
    retq

new_function:
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    call *%rdx
    pushq %rax
    infi:
	call me
	movq %rax, %rdi
	popq %rsi
	pushq %rsi
	call send
	jmp infi 
    retq

