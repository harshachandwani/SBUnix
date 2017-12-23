#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
#include <sys/process.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/elf.h>
#include <sys/task.h>
#include <sys/syscalls.h>

extern void context_switch_asm(struct task_struct_2* prev,struct task_struct_2* curr);
extern void switch_process(struct task_struct* parent,struct task_struct* child);

struct task_struct_2* proc1;
struct task_struct_2* proc2;
int process_IDS[MAX_NUM_PROCESSES];
task* current_task;

void switch_to(struct task_struct_2* prev,struct task_struct_2* next){
	context_switch_asm(prev,next);
}

void func1(){
        int i = 0;
        while(i < 20){
              kprintf("%d \n",i);
                i+=2;
		switch_to(proc1,proc2);
        }
}

void func2(){
	int j = 1;
	while(j < 20){
		kprintf("%d \n",j);
		j+=2;
		switch_to(proc2,proc1);
	}

}


void init_kernel_process(struct task_struct_2* proc,void (*func) ()){
		proc->rsp = (uint64_t)& proc->kernel_stack[4062];
        	proc->kernel_stack[4074] = (uint64_t)func;
		proc->kernel_stack[4073] = 0;  
        	proc->kernel_stack[4072] = 0;  
        	proc->kernel_stack[4071] = 0;  
        	proc->kernel_stack[4070] = 0;  
        	proc->kernel_stack[4069] = 0;  
        	proc->kernel_stack[4068] = 0;  
        	proc->kernel_stack[4067] = 0;  
		proc->kernel_stack[4066] = 0;  
        	proc->kernel_stack[4065] = 0;  
        	proc->kernel_stack[4064] = 0;  
        	proc->kernel_stack[4063] = 0;  
        	proc->kernel_stack[4062] = 0; 
}


void init_process_IDS_Array(){
	for(int i = 0;i < MAX_NUM_PROCESSES;i++)
		process_IDS[i] = 0;
}

int get_ProcessID(){
        for(int i = 0;i < MAX_NUM_PROCESSES;i++){
                if(process_IDS[i] == 0){
                        process_IDS[i] = 1;
                        return i;
                }
        }
        return -1;
}

void create_kernel_process(struct task_struct_2** proc){
	*proc = (struct task_struct_2*) kmalloc(sizeof(struct task_struct_2));
	(*proc)->process_state = RUNNING_TASK;
	(*proc)->pid = get_ProcessID();
}


void context_switch_two_kernel_threads(){
	kprintf("in context switch\n");
	init_process_IDS_Array();
	create_kernel_process(&proc1);
	create_kernel_process(&proc2);
	init_kernel_process(proc1,&func1);
	init_kernel_process(proc2,&func2);
	func1();
}

task* task_init(char* name){
	task* new_task;
	new_task = (task*) kmalloc(sizeof(task));
	new_task->pid = get_ProcessID();
	new_task->task_name = name;
	new_task->rsp = 0; //(uint64_t)new_task->kernel_stack;
	new_task->rdi = 0;	
	memset((void*)new_task->kernel_stack,0,512*8);
	new_task->mm = (mm_struct*)kmalloc(sizeof(mm_struct));
	new_task->mm->vma_list = NULL;
	new_task->next = NULL;
	return new_task;
}

void run_sbush(){	

	uint64_t rsp_kernel;
	__asm__(
		"movq %%rsp, %0\n;"
		:"=g"(rsp_kernel)
		:
	);
	set_tss_rsp((void*)rsp_kernel);
	//switch_to_user_mode((uint64_t)&user_func) ; 
	task* new_task = (task*)task_init("sbush");
	create_elf_proc("bin/sbush",new_task);
	current_task = new_task;
//	kprintf("PID of current_task = %d", current_task->pid);
	clear_screen();
	while(1){
		__asm__ __volatile__("movq %0, %%cr3;" :: "r"(new_task->cr3_paddr));
		//kprintf("Task cr3_vaddr = %p",new_task->cr3_vaddr);
		//kprintf("Task RIP = %p", new_task->rip);
		set_tss_rsp(&(new_task->kernel_stack[KERN_STACK - 1]));
		__asm__ ("movq %0, %%rsp;" :: "r"(USER_STACK)); 
		switch_to_user_mode_1(new_task, new_task->rip);
	}
}
