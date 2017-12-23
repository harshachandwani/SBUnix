#include <sys/defs.h>
#include <sys/tarfs.h>
#include <dirent.h>


#define NEW_TASK                1
#define READY_TASK		        1
#define RUNNING_TASK	    	2
#define INTERRUPTIBLE_TASK  	3
#define UNINTERRUPTIBLE_TASK	4
#define STOPPED_TASK		    5
#define ZOMBIE_TASK		        6
#define WAITING_TASK		    7

#define KERN_STK_SIZE_2         4096
#define KERN_STACK              512
#define MAX_NUM_PROCESSES       100
#define USER_STACK              0x00000006ffffffff

#define STACK 1
#define HEAP  2
#define OTHER 3

#define PAGE_SIZ 4096

typedef struct task_struct task;
typedef struct vm_struct vma_struct;
typedef struct mm_struct mm_struct;

extern task* currTask;

struct task_struct{	
	uint64_t kernel_stack[KERN_STACK]; 
	uint64_t rsp; 
        uint64_t rip;         
        uint64_t pid; 
   	uint64_t rdi;
        uint64_t process_state;
        uint64_t cr3_paddr;
        uint64_t cr3_vaddr;
        task* parent;
        char* task_name;
        fd* fd_buff[200];
	char curr_dir[100];
	file_struct* curr;
        task* next;
        struct mm_struct* mm;			// pointer to mm_struct
};

struct mm_struct{
	struct vm_struct *vma_list, *current;
	uint64_t start_code;
	uint64_t end_code;
	uint64_t start_data;
	uint64_t end_data;
};

struct vm_struct{
	struct mm_struct *mm;
	uint64_t start;
	uint64_t end;
	uint64_t flags;
	uint64_t type;
	struct vm_struct* next;
};


//Ronak's CS
struct task_struct_2{	
	uint64_t pid;
    uint64_t rsp;         
   	uint64_t rdi;
   	uint64_t rip;
    uint64_t kernel_stack[KERN_STK_SIZE_2];                  
    uint64_t process_state;
    uint64_t cr3_paddr;
    uint64_t cr3_vaddr;
    struct mm_struct* mm;			// pointer to mm_struct
};

void init_process_IDS_Array();
int getProcessID(task* proc);
task* task_init(char* task_name);
void create_kernel_process(struct task_struct_2** proc);
void create_elf_proc(char*,task*);
void context_switch_two_kernel_threads();
void func1();
void func2();
void run_sbush();
task* copy_pcb(task*);
uint64_t copy_context(task* parent, task* child);
void fork_syscall();

