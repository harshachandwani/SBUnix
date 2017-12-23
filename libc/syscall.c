#include <sys/syscalls.h>
#include <sys/defs.h>

uint64_t syscall_zero_arg(uint64_t syscall){
	uint64_t ret_val = 0;

	 __asm__ volatile(
		"pushq %%r12;\n"
		"pushq %%r13;\n"
		"pushq %%r14;\n"
		"pushq %%r15;\n"
		"movq %1, %%r12;"
		"int $0x80;"
		"movq %%r11, %0;"
		"popq %%r15;\n"
		"popq %%r14;\n"
		"popq %%r13;\n"
		"popq %%r12;\n"
        :"=g"(ret_val)
        :"r"(syscall)
    );
    return ret_val;	
}


uint64_t syscall_three_arg(uint64_t syscall,uint64_t arg1,char* arg2,uint64_t arg3){
        uint64_t ret_val = 0;

         __asm__ volatile(
                "pushq %%r8;\n"
                "pushq %%r9;\n"
                "pushq %%r10;\n"
                "pushq %%r11;\n"
                "movq %1, %%r8;"
                "movq %2, %%r9;"
                "movq %3, %%r10;"
                "movq %4, %%r11;"
                "int $0x80;"
                "movq %%r12, %0;"
                "popq %%r11;\n"
                "popq %%r10;\n"
                "popq %%r9;\n"
                "popq %%r8;\n"
         :"=g"(ret_val)
      :"r"(syscall),"r"(arg1),"r"((uint64_t)arg2),"r"(arg3)
    );
    return ret_val;
}

uint64_t syscall_one_arg(uint64_t syscall,char* arg1){
	uint64_t ret_val = 0;
	__asm__ volatile(
                "pushq %%r8;\n"
                "pushq %%r9;\n"
                "movq %1, %%r8;"
		"movq %2, %%r9;"
                "int $0x80;"
                "movq %%r12, %0;"
                "popq %%r9;\n"
                "popq %%r8;\n"
        :"=g"(ret_val)
        :"r"(syscall),"r"(arg1)
    );
	return ret_val;
}

uint64_t syscall_two_arg(uint64_t syscall,char* arg1,uint64_t arg2){
        uint64_t ret_val = 0;

         __asm__ volatile(
                "pushq %%r8;\n"
                "pushq %%r9;\n"
                "pushq %%r10;\n"
                "movq %1, %%r8;"
                "movq %2, %%r9;"
                "movq %3, %%r10;"
                "int $0x80;"
                "movq %%r12, %0;"
                "popq %%r10;\n"
                "popq %%r9;\n"
                "popq %%r8;\n"
         :"=g"(ret_val)
      :"r"(syscall),"r"(arg1),"r"(arg2)
    );
    return ret_val;
}


