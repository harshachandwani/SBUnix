#include <sys/defs.h>
#include <dirent.h>
#include <sys/tarfs.h>
#include <sys/paging.h>
#include <sys/generic.h>
#include <sys/string.h>
#include <sys/kprintf.h>
#include <sys/string.h>


#define FORK_SYSCALL 57
#define WRITE_SYSCALL 1
#define OPENDIR_SYSCALL 58
#define READ_SYSCALL 0

struct regs{
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbp;
	uint64_t rsi;
}__attribute__((packed));

uint64_t syscall_1(uint64_t syscall, uint64_t fd, uint64_t string, uint64_t length);
uint64_t syscall_three_arg(uint64_t syscall,uint64_t arg1,char* arg2,uint64_t arg3);
uint64_t syscall_one_arg(uint64_t syscall,char* arg1);
uint64_t syscall_two_arg(uint64_t syscall,char* arg1,uint64_t len);
uint64_t syscall_zero_arg(uint64_t syscall);
