#include <sys/defs.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/string.h>
#include <sys/kprintf.h>
#include <sys/elf64.h>
#include <sys/process.h>
#include <sys/paging.h>

typedef struct posix_header_ustar TAR_HEAD;

uint64_t get_binary_offset(char* filepath){
	uint64_t size = 0;
	if(filepath == NULL){
		return 0;
	}
	TAR_HEAD *tar_start;
	tar_start = (TAR_HEAD*)&_binary_tarfs_start;
	TAR_HEAD *tar_end;
	tar_end = (TAR_HEAD*)&_binary_tarfs_end;
	
	while(tar_start < tar_end){
		if(strcmp(tar_start->name,filepath) == 0){
			kprintf("addr = %p\n",tar_start + BLOCK_SIZ);
			return ((uint64_t)(tar_start + 1));
		}
		size = octalToDecimal(stoi(tar_start->size));
		if(size % BLOCK_SIZ != 0){
			tar_start = (TAR_HEAD*)((uint64_t)tar_start + (size/BLOCK_SIZ) * BLOCK_SIZ + BLOCK_SIZ); 
		}
		else
			tar_start = (TAR_HEAD*)((uint64_t)tar_start + size + BLOCK_SIZ);
	}
	return 0;
}


int is_elf(Elf64_Ehdr* elf){
	if ((elf->e_ident[1] == 'E') && (elf->e_ident[2] == 'L') && (elf->e_ident[3] == 'F'))
		return 1;
	else
		return 0;
}

void create_elf_proc(char* file_path, task* new_task){
	
	uint64_t file_offset = get_binary_offset(file_path);
	uint64_t current_cr3;
	uint64_t size, proc_vaddr_start, proc_vaddr_end, proc_page_start, proc_page_end, start;
	
	clear_screen();

	Elf64_Ehdr *elf = (Elf64_Ehdr *)(file_offset);
	if(is_elf(elf)){
		kprintf("\nELF is present\n");	
	}

	//uint64_t size = elf->e_phentsize;
	new_task->kernel_stack[511] = (uint64_t)elf->e_entry;
	new_task->rsp = (uint64_t)&new_task->kernel_stack[511];
	
	new_task->rip = elf->e_entry;
	current_cr3 = get_cr3();
	new_task->cr3_paddr = find_page();
	new_task->cr3_vaddr = new_task->cr3_paddr + VA_START;

	Elf64_Phdr *first_phdr, *last_phdr;
	first_phdr = (Elf64_Phdr *)((uint8_t *)elf+elf->e_phoff);	
	last_phdr =  first_phdr + elf->e_phnum;	
	
	//Copy the Kernel PML4 to User PML4, to allow user process access to kernel page tables.
	for(int i = 0; i < 512; i++){
		*((uint64_t *)(new_task->cr3_vaddr + i * 8)) = *((uint64_t *)(VA_START + current_cr3 + i * 8));
	}

	clear_screen();
	while(first_phdr < last_phdr){  
	
		if(first_phdr->p_type == 1){
			//Get the size of data to copy from the segment to the page in memory
			size = first_phdr->p_filesz;
			//Get the virtual address range for the user process from the ELF Program header
			proc_vaddr_start = first_phdr->p_vaddr;
			proc_vaddr_end = first_phdr->p_vaddr + first_phdr->p_memsz;
			proc_page_start = proc_vaddr_start & 0xFFFFFFFFFFFFF000;
			proc_page_end = proc_vaddr_end & 0xFFFFFFFFFFFFF000;
			//Offset to start the data copy
			start = (uint64_t)((elf + first_phdr->p_offset));
			uint64_t page = proc_page_start;

			while(page <= proc_page_end){
				//Page to copy the user process elf data to
				uint64_t kern_page_va = (uint64_t)(VA_START + find_page());
				int data;

				if(size < PAGE_SIZ)
					data = size;
				else
					data = PAGE_SIZ;

				int j  = 0;
				uint8_t * src, *dest;
				dest = (uint8_t *)(kern_page_va);
				src = (uint8_t *)start;
				while(data!=0){
					dest[j] = src[j]; 
					j++;
					data--;
				}

				size -= PAGE_SIZ;
				start += PAGE_SIZ;

				uint64_t kern_page_pa = kern_page_va - VA_START;
				//map_virt_to_phys((uint64_t *)(VA_START + current_cr3), page, kern_page_pa, 7);
				map_virt_to_phys((uint64_t *)(new_task->cr3_vaddr), page, kern_page_pa, 7);
				page += PAGE_SIZ;	
			}
		}	
		first_phdr++;
	}

	//Allocate HEAP and STACK to the process

	/*  
	 *	A little ineffiicient as I am allocating every VMA a page and 
	 *	also a new page for each memory region it points to
	 */

	vma_struct *stack_vma = (vma_struct*)kmalloc(sizeof(vma_struct));
	if(new_task->mm->vma_list == NULL)
		new_task->mm->vma_list = stack_vma;
	else
		new_task->mm->current = stack_vma;
	new_task->mm->current = stack_vma;
	stack_vma->mm = new_task->mm;
	stack_vma->start = USER_STACK & 0xFFFFFFFFFFFFF000;
	stack_vma->end = USER_STACK + PAGE_SIZ;
	stack_vma->type = STACK;
	//vma->flags = first_phdr->p_flags;
	stack_vma->next = NULL;
	
	uint64_t stack_pa = find_page();
	stack_pa = stack_pa & 0xFFFFFFFFFFFFF000;

	clear_screen();
	map_virt_to_phys((uint64_t *)(new_task->cr3_vaddr), USER_STACK & 0xFFFFFFFFFFFFF000, stack_pa, 7);
}
