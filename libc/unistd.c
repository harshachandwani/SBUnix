#include <unistd.h>
#include <sys/syscalls.h>
int fork(){
	uint64_t pid = syscall_zero_arg(FORK_SYSCALL);
	return pid;
}

uint64_t write(int fd_c, char* addr, int len){
	uint64_t r = syscall_three_arg(WRITE_SYSCALL,fd_c,addr,len);
	return r;
}

uint64_t read(int fd_c,char* addr,int len){
	uint64_t r = syscall_two_arg(fd_c,addr,len);
	return r;
}





