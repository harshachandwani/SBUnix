#include<sys/elf64.h>

uint64_t get_binary_offset(char*);
int is_elf(Elf64_Ehdr*);
