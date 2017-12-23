#include <sys/paging.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/string.h>

extern uint64_t top_VA;
extern uint64_t* kernel_cr3;

void test_kmalloc();
void* kmalloc(uint64_t size){
	int num_pages_req = 0;
	int temp = 0, i = 0;
	uint64_t page_virt_addr;
	void* ret_addr = NULL;
	temp = size % PAGE_SIZE;
		
	if(temp == 0)
		num_pages_req = size / PAGE_SIZE;
	else
		num_pages_req = (size / PAGE_SIZE) + 1;
	
	ret_addr = (void*)top_VA;
	for(i = 0; i < num_pages_req; i++){
		page_virt_addr = 0xffffffff80000000 + (uint64_t) find_page();			
		temp  = page_virt_addr;
	}
	return ret_addr;
}

void test_kmalloc(){
#if 0
    char *ptr = (char*) kmalloc(4096);
    *ptr = 'S';
    *(ptr+1) = '\0';	


    kprintf("\tmain1 %p\n", ptr);

    ptr = (char*) kmalloc(8006);
    *ptr = 'A';
    *(ptr+1) = '\0';

    kprintf("\tmain2 %p\t%s\n", ptr, ptr);

    ptr = (char*) kmalloc(4096);
    *ptr = 'H';
    *(ptr+1) = '\0';

    kprintf("\tmain3 %p\t%s\n", ptr, ptr);
	return;
#endif
}

