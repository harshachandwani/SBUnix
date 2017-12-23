#include <sys/defs.h>

#define PML_SHIFT 39
#define PDPE_SHIFT 30
#define PDE_SHIFT 21
#define PTE_SHIFT 12

#define PML4_OFFSET(va) ((va >> PML_SHIFT)& 0x1ff)
#define PDPE_OFFSET(va) ((va >> PDPE_SHIFT)& 0x1ff)
#define PDE_OFFSET(va)  ((va >> PDE_SHIFT)& 0x1ff)
#define PTE_OFFSET(va)  ((va >> PTE_SHIFT)& 0x1ff)

#define PAGE_BIT_PRESENT 0x1
#define PAGE_BIT_RW 	 0x2
#define PAGE_BIT_USER	 0x4

#define PAGE_SIZE 0x1000
#define VA_START 0xffffffff80000000
#define MASK_LAST_12 0xFFFFFFFFFFFFF000
#define PAGE_ENTRIES 512

void init_phys_mem(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t size); 
void init_paging(uint64_t physbase);
uint64_t find_page();
uint64_t find_vpage();
uint64_t find_free_page_index();
void free_page(uint64_t page_addr);
void map_virt_to_phys(uint64_t* kernel_cr3,uint64_t virt_addr, uint64_t phys_addr, int flags);
void user_map_virt_to_phys(uint64_t* kernel_cr3,uint64_t virt_addr, uint64_t phys_addr);
void test_kmalloc();
uint64_t max_free();
void* kmalloc(uint64_t size);
uint64_t get_cr3();
void set_cr3(uint64_t new_cr3);
uint64_t create_entry(int offset, uint64_t* address, int permission);
void copy_kernel_entry(uint64_t* c_cr3, uint64_t* p_cr3, int permission);
void create_child_page_tables(uint64_t *child_vir_CR3, uint64_t *parent_vir_CR3);

