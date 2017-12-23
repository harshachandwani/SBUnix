#include <sys/paging.h>
#include <sys/defs.h>
#include <sys/kprintf.h> 
#include <sys/string.h>

#define MMAP_SIZE 256
#define PAGE_SIZE	0x1000
uint64_t* freelist;
uint64_t gphysfree; 
uint64_t avail_mem;
uint64_t page_address;
uint64_t pages_start_address;
uint64_t free_page_index = 0;
uint64_t used_page = 0;
uint64_t num_pages = 0;
extern uint64_t paging_en;
uint64_t top_VA = 0;
uint64_t size;


void init_phys_mem(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t limit){
	uint64_t avail_mem;
	uint64_t avail_mem_pages;
	uint64_t num_bytes;
	uint64_t rem;

	//gphysfree = physfree;
	kprintf("\nPhysbase = %p Physfree = %p Limit = %p", physbase, physfree, limit);
		
	avail_mem = limit - (physfree-physbase);
	avail_mem_pages = avail_mem >> 12;
	num_bytes = avail_mem_pages / 8 + 1;
//	kprintf("No. of Pages= %d, avail_mem = %x, num_bytes = %d ", avail_mem_pages, avail_mem, num_bytes); 		
	
	//Recalculate the available memory, excluding the freelist space.
	avail_mem = avail_mem - num_bytes;  		
	num_pages = avail_mem >> 12;
//	kprintf("\navail mem = %p Num pages now = %d", num_pages, avail_mem);		
	rem = num_bytes % 4096;


	if (rem){
		pages_start_address = physfree + (num_bytes / 4096) * 4096 + 4096;
	}
	else{
		pages_start_address = physfree + num_bytes;
	}

	memset((void *)physfree, 0x0, avail_mem);
	gphysfree = physfree;
	freelist = (uint64_t*)physfree;
	size = limit;
	return ;
}

uint64_t find_page(){
	uint64_t free_page_index = 0;
	if(num_pages == used_page)
		return 0;
	int i = 0;

	free_page_index = find_free_page_index();

	page_address = pages_start_address + (free_page_index *  4096);
	if(paging_en)
		memset((uint64_t *)(0xFFFFFFFF80000000 + page_address), 0x0, 4096);
	else
		memset((uint64_t *)(page_address), 0x0, 4096);
	i = free_page_index / 64;
	
	//Mark the page as accessed int by setting the corresponding bit in the freelist
	freelist[i] = freelist[i] | (1 << (free_page_index % 64));
	used_page++;
	top_VA = VA_START + page_address + PAGE_SIZE;
	return page_address;	
}

void free_page(uint64_t page_addr){
	uint64_t page_num;
	int i = 0;
	page_num = (page_addr - pages_start_address)  >> 12;
	i = page_num / 64;
	freelist[i] = freelist[i] & ~(1 << (free_page_index % 64));
}

uint64_t find_free_page_index(){
        uint64_t index = 0;
        uint64_t freelist_val;
        uint64_t check_val = 1;
       	if(paging_en)
		//freelist = (uint64_t*)gphysfree + 0xffffffff80000000;
		freelist = (uint64_t*)(0xffffffff80000000 + gphysfree);
	else
		freelist = (uint64_t*)gphysfree;

	for(uint64_t i = 0; i <= (num_pages/64); i++){
                freelist_val = freelist[i];
                if(freelist_val == 0xffffffffffffffff){
                        index += 64;
                        continue;
                }
                else{
                        for(i = 0; i < 64 ; i++){
                                check_val = 1 << i;
                                if(freelist_val & check_val){
                                        index++;
                                        continue;
                                }
                                else{
                                        free_page_index = index;
                                        break;
                                }
                        }
                        break;
                }

        }
        return free_page_index;
}

uint64_t max_free(){
	return size;
}




		
