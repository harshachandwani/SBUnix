#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/generic.h>
#include <sys/syscalls.h>
#include <dirent.h>


void printTimer(char* s);
void timer_isr();
uint64_t read_stdin(void* addr,int count);
void keyboard_isr();
void double_fault_isr();
void page_fault_isr();
void syscall_isr();
