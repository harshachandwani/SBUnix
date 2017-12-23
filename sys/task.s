.text

.global switch_to_user_mode


switch_to_user_mode:
	movq $0x23,%rax
	movq %rax,%ds
	movq %rax,%es
	movq %rax,%fs
	movq %rax,%gs

	movq %rsp,%rax
	pushq $0x23 
	pushq %rax 
	pushfq
	popq %rax
	or $0x200, %rax
	pushq %rax	
	pushq $0x2B
	pushq %rdi
	iretq
	

.global switch_to_user_mode_1

switch_to_user_mode_1:
	movq $0x23,%rax
	movq %rax,%ds
	movq %rax,%es
	movq %rax,%fs
	movq %rax,%gs

	movq %rsp,%rax
	pushq $0x23 
	pushq %rax 
	pushfq
	popq %rax
	orq $0x200, %rax
	pushq %rax	
	pushq $0x2B
	pushq %rsi
	iretq
	

