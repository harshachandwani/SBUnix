#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#define VA_START 0xffffffff80000000

extern uint64_t page_address;
extern uint64_t top_VA;
uint64_t paging_en = 0;
uint64_t* kernel_cr3;
extern char kernmem;

void map_virt_to_phys(uint64_t* kernel_cr3, uint64_t virt_addr, uint64_t phys_addr, int flags){

	uint64_t pml4_entry_index = PML4_OFFSET((uint64_t)virt_addr);
	uint64_t pdpt_entry_index = PDPE_OFFSET((uint64_t)virt_addr);
	uint64_t pdt_entry_index = PDE_OFFSET((uint64_t)virt_addr);
	uint64_t pt_entry_index = PTE_OFFSET((uint64_t)virt_addr);
	
	uint64_t* pml4_entry = (kernel_cr3 + pml4_entry_index);	
	if(!(*pml4_entry & PAGE_BIT_PRESENT)){
		*pml4_entry =  find_page();
		*(kernel_cr3 + pml4_entry_index) = *pml4_entry | flags; //PAGE_BIT_PRESENT | PAGE_BIT_RW; 
//		*(kernel_cr3 + pml4_entry_index) = *pml4_entry | PAGE_BIT_PRESENT | PAGE_BIT_RW | PAGE_BIT_USER;
	}
	

//	kprintf("kernel cr3 = %p, pml4_entry_index = %p, pml4_entry = %p", kernel_cr3, pml4_entry_index, *pml4_entry); 

	uint64_t* pdp_base = (uint64_t*) (*(pml4_entry) & 0XFFFFFFFFFFFFF000);
	uint64_t* pdp_entry;
	if(paging_en)
		pdp_entry = (uint64_t *)(0xffffffff80000000 + (uint64_t)(pdp_base + pdpt_entry_index));
	else
		pdp_entry = pdp_base + pdpt_entry_index;
	if(!(*pdp_entry & PAGE_BIT_PRESENT)){
		*pdp_entry = find_page();
		*(pdp_entry) = *pdp_entry | flags; //PAGE_BIT_PRESENT | PAGE_BIT_RW;		
//		*(pdp_base + pdpt_entry_index) = *pdp_entry | PAGE_BIT_PRESENT | PAGE_BIT_RW | PAGE_BIT_USER;		
	}

//	kprintf("virt_address = %p, pdp base = %p, pdpt_entry_index = %p pdp_entry = %p", virt_addr, pdp_base, pdpt_entry_index, *pdp_entry); 



	uint64_t* pd_base = (uint64_t*) (*(pdp_entry) & 0XFFFFFFFFFFFFF000);
    uint64_t* pd_entry;// = pd_base + pdt_entry_index;
	if(paging_en)
		pd_entry = (uint64_t *)(0xffffffff80000000 + (uint64_t)(pd_base + pdt_entry_index));
	else
        pd_entry = pd_base + pdt_entry_index;

    if(!(*pd_entry & PAGE_BIT_PRESENT)){ 
    	*pd_entry = find_page();
      	*(pd_entry) = *pd_entry | flags;//PAGE_BIT_PRESENT | PAGE_BIT_RW; 
	
 //              *(pd_base + pdt_entry_index) = *pd_entry | PAGE_BIT_PRESENT | PAGE_BIT_RW | PAGE_BIT_USER; 
    }

//	kprintf("pd base = %p, pdt_entry_index = %p pdp_entry = %p", pd_base, pdt_entry_index, *pd_entry); 
	
	uint64_t* pt_base = (uint64_t*) (*(pd_entry) & 0XFFFFFFFFFFFFF000);
	uint64_t* pt_entry;
	if(paging_en)
		pt_entry = (uint64_t*)(0xFFFFFFFF80000000 + (uint64_t)(pt_base + pt_entry_index));
	else
		pt_entry = pt_base + pt_entry_index;

	*(pt_entry) = phys_addr | flags; //PAGE_BIT_PRESENT | PAGE_BIT_RW;

//	*(pt_base + pt_entry_index) = phys_addr | PAGE_BIT_PRESENT | PAGE_BIT_RW | PAGE_BIT_USER;



//	kprintf("ENtry = %p\n",*(pt_base + pt_entry_index));
//	kprintf("pt base = %p, pt_entry_index = %p pdp_entry = %p", pt_base, pt_entry_index, *(pt_base+pt_entry_index)); 

}

uint64_t get_cr3(){
	uint64_t cr3_val = 0;
	kprintf("Kernel cr3 = %p",kernel_cr3);
	__asm__ __volatile__(" movq %%cr3, %0; " :: "r" (cr3_val));
	return cr3_val;
}

void set_cr3(uint64_t new_cr3){
	
	__asm__(
    	"movq %0, %%rsp;"
    	:
    	:"r"(new_cr3)
  	);
  	
}

void init_paging(uint64_t physbase){
	kernel_cr3 = (uint64_t*)find_page();
	kprintf("kernel_cr3 = %p\n ", kernel_cr3);
	/*
	 We map everything allocated till now, kernel + freelist
	 page_address : last used page address
	 next_address : start of allocation, i.e. the kernel 
	*/
	int count = 0;
	uint64_t last_address = max_free(); //page_address;
	kprintf("Last address = %p", last_address);
	uint64_t next_address = physbase;
	while(next_address <= last_address) {
		map_virt_to_phys(kernel_cr3, VA_START + next_address, next_address, 3);
		next_address += 0x1000;
		count++;
	}
	
	map_virt_to_phys(kernel_cr3,  0xffffffff80000000 + 0xB8000, 0xB8000, 3);
	
	 __asm__ __volatile__("movq %0, %%cr3;" :: "r"(kernel_cr3));	
		
	paging_en = 1;
}
