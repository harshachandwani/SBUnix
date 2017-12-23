.text

.global isr32
isr32:
#	pushq $0
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi
	pushq %rbp
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
#	movq %rsp,%rdi
	call timer_isr
	popq %r15 
	popq %r14 
	popq %r13 
	popq %r12 
	popq %r11 
	popq %r10 
	popq %r9 
	popq %r8 
	popq %rbp 
	popq %rdi 
	popq %rsi 
	popq %rdx 
	popq %rcx 
	popq %rbx 
	popq %rax
#	addq $8,%rsp
	iretq
.endm

.global isr33
isr33:
	pushq $0
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi
	pushq %rbp
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	movq %rsp,%rdi
	call keyboard_isr
	popq %r15 
	popq %r14 
	popq %r13 
	popq %r12 
	popq %r11 
	popq %r10 
	popq %r9 
	popq %r8 
	popq %rbp 
	popq %rdi 
	popq %rsi 
	popq %rdx 
	popq %rcx 
	popq %rbx 
	popq %rax
	addq $8,%rsp
	iretq
.endm

.global isr8
isr8:
        pushq $0
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        movq %rsp,%rdi
        call double_fault_isr
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rbp
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        addq $8,%rsp
        iretq
.endm

.global isr14
isr14:
        pushq $0
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        movq %rsp,%rdi
        call page_fault_isr
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rbp
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        addq $8,%rsp
        iretq
.endm


.global isr128
isr128:
        pushq $0
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        #pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        movq %rsp,%rdi
        call syscall_isr
        popq %r15
        popq %r14
        popq %r13
        #popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rbp
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        addq $8,%rsp
        iretq
.endm

