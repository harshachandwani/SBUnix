#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pit.h>
#include <sys/pic.h>
#include <sys/idt.h>
#include <sys/ahci_lib.h>
#include <sys/string.h>
#include <sys/paging.h>
#include <sys/process.h>
#include <sys/syscalls.h>
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/elf64.h>


#define INITIAL_STACK_SIZE 4096

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern uint64_t* kernel_cr3;
extern char kernmem, physbase;
extern void keyboard_isr();
extern void switch_to_user_mode();
extern void switch_to_user_mode_1();
void user_func();

extern void syscall_1_asm(uint64_t syscall, uint64_t fd, uint64_t string, uint64_t length);
void start(uint32_t *modulep, void *physbase, void *physfree)
{
	int i = 0;
	uint64_t limit = 0;
	uint64_t base = 0;
	
	
	struct smap_t {
	uint64_t base, length;
	uint32_t type;

	}__attribute__((packed)) *smap;
	
	while(modulep[0] != 0x9001)
		 modulep += modulep[1]+2;
  	
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    		if (smap->type == 1 /* memory */ && smap->length != 0) {
			i++;
			//kprintf("Iteration: %d\n",i);
       			//kprintf("Available Physical Memory [%p-%p]\n, len = [%p]", smap->base, smap->base + smap->length, smap->length);
			if(i == 2){
				limit = smap->base + smap->length;
				base = smap->base;
			}
    		}
  	}
	init_gdt();
 	init_idt();          
 	pic_remap(0x20,0x28);
 	__asm__("sti");       
  	init_timer(100);
	detect_ahci();     
 	
	init_phys_mem((uint64_t) physbase, (uint64_t)physfree, base, limit);	
	
	init_paging((uint64_t)physbase);
	//kprintf("Done paging");
	test_kmalloc();	
	context_switch_two_kernel_threads();
	tarfs_initialize();
	clear_screen();
	run_sbush();

	while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
 __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  while(1);
}


