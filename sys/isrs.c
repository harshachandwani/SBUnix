#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/generic.h>
#include <sys/keyboard.h>
#include <sys/syscalls.h>
#include <sys/process.h>
#include <sys/dirent.h>

volatile char *video1 = (volatile char*)(0xFFFFFFFF80000000 + 0xB8000);
static int timer_ticks = 0; 
static int time = 0;
int flag;
static int ctrl_pressed = 0;
static int shift_pressed = 0;

unsigned char symbols[10] = {')','!','@','#','$','%','^','&','*','('};
static volatile char buff[1000];
static volatile int len = 0;
static volatile int flg = 1;
//char addr[1000];

//extern task* currTask;

void printTimer(char* s){
	volatile char* location = video1 + 24 * 160 + 66 * 2;
	char *sec = "Sec: ";
	
	while(*sec != '\0'){
		*location++ = *sec++;
		location++;
	}

	while(*s != '\0'){
		*location++ = *s++;
		location++;
	}

}

void timer_isr(){	
 
	timer_ticks++;
	if(timer_ticks % 100 == 0){
		time++;
		char* s = convertToString(time,10);
		printTimer(s);	
	}
	pic_sendeoi(32);
}


uint64_t read_stdin(char* addr,int count){
        flg = 1;
	uint64_t ret = 0;
        __asm__ __volatile__("sti");
        //len = 0;
	while(flg);
        if(count < len)
                len = count;
	//kprintf(" \n %d \n",count);
	//kprintf(" \n %d \n",len);
	/*for(int i = 0;i < len;i++){
		//kprintf("%c",buff[i]);
	}*/
        memcpy((void*)addr,(void*)buff,len);
	//kprintf("User space : %s",addr);
	//kprintf("len = %d",len);
        for(int i = 0;i < len;i++)
		buff[i] = '\0';
	ret = len;
        len = 0;
        return ret;
}


void keyboard_isr(){

	uint8_t scancode;
	char c = 0;
	int index;
	outb(0x20,0x20);	//send EOI
	scancode = (uint8_t)inb(0x60);
	if(scancode < 0)
		return;
	c = kbdus[scancode];
	if(scancode & 0x80){
		shift_pressed = 0;
		ctrl_pressed = 0;
		return;
	}
	if(scancode == 42 || scancode == 54) //42: Left shift, 54:Right shift
		shift_pressed = 1;

	else if(scancode == 29)
		ctrl_pressed = 1;

	else if(ctrl_pressed){
		if(c >= 97 && c <= 122){
			c = c - 32;
			len++;
			print_at(63,24,'^');
			print_at(64,24,c);
		}
	}

	
	else if(c == '\n' && flg == 1){                    //Enter pressed
                kprintf("\n");
                buff[len] = '\0';
                //len = 0;
                flg = 0;
		//kprintf("Enter pressed");
        }


	else if(shift_pressed){
		if(c >= 97 && c <= 122){
			c = c - 32;
			print_at(63,24,c);
			print_at(64,24,' ');
		}

		else if(c >= 48 && c <= 57){
			c = c - 48;
			index = c;
			print_at(63,24,symbols[index]);
			print_at(64,24,' ');
		}
	}
	
	else{
		buff[len] = c;
		kprintf("%c",buff[len]);
		len++;
		print_at(63,24,c);
		print_at(64,24,' ');
	}
	pic_sendeoi(33);  
}


void double_fault_isr(){
	kprintf("Inside df isr\n");
}


void page_fault_isr(){
	uint64_t cr2_val;

	__asm__(
		"movq %%cr2,%0\n;"
		:"=g"(cr2_val)
		:
	);
	kprintf("CR3 = %p",cr2_val); 	
	kprintf("Inside page_fault_isr\n");
}



int syswrite(int fd_c, char* addr, int len){
        if (fd_c == stdout || fd_c == stderr) {
                kprintf(addr);
                return len;
        }
        return -1;
}

dir* opendir(char* path){
/*
        char tmp[100];
        strcpy(tmp,path);
        file_struct* curr = root;
        file_struct* t;
        dir* result = (dir*)kmalloc(sizeof(dir));
        char* tokens = strtok(tmp,"/");
        kprintf("path = %s \n",path);
        int i;

        while(tokens != NULL){
                t = curr;
                // Parent node, which is stored as first child.
                if(strcmp(tokens,"..") == 0)
                        curr = curr->child[1];

                // Current node
                else if(strcmp(tokens,".") == 0)
                        curr = curr->child[0];

                else{
                        //Traverse through the child array to find the directory
                        for(i = 2;i < curr->l;i++){
                                kprintf("%s \n",curr->filename);
                                if(strcmp(curr->child[i]->filename,tokens) == 0){
                                        curr = curr->child[i];
                                        break;
                                }
                        }

                        //If not found, return NULL directory
                        if(i == t->l)
                        return (dir*)NULL;
                }
                tokens = strtok(NULL,"/");
        }
        if(curr->file_type == _DIRECTORY){
                result->curr = 2;
                result->f_node = curr;
                return result;
        }*/
        return (dir*)NULL;

}


dirent* readdir(struct dir* dir){
  /*      if(dir->curr > 0)
        {
                if(dir->f_node->l >= 3 && dir->curr < dir->f_node->l){
                        dir->curr+=1;
                        strcpy(dir->dir_curr.d_name,dir->f_node->child[dir->curr]->filename);
                        return &dir->dir_curr;
                }
        }*/
        return NULL;

}

int closedir(dir* dirr){
  /*      if(dirr->curr > 1 && dirr->f_node->file_type == _DIRECTORY ) {
                dirr->curr = 0;
                dirr->f_node =(file_struct*)NULL;
                return 0;
        }
        return -1;
*/
return 1;
}

int sysread(int fd_c,char* addr,uint64_t len){
        //uint64_t start=0;
        //uint64_t end =0;
        uint64_t c;

        if(fd_c == stdin){
                c = read_stdin(addr,len);
                return c;
        }
/*
        if((currTask->fd_buff[fd_c] != NULL) && (currTask->fd_buff[fd_c]->permission != WRONLY)) {
                start = currTask->fd_buff[fd_c]->current;
                end  = currTask->fd_buff[fd_c]->f_node->l;
                if(len > (end - start))
                        len = end - start;
                currTask->fd_buff[fd_c]->current +=len;
                memcpy((void *)addr,(void *)start,len);
                return len;
        }*/
        return -1;
}

int sysclose(int fd_c){/*
        if(currTask->fd_buff[fd_c] != NULL){
                currTask->fd_buff[fd_c] = NULL;
                return 0;
        }*/
        return -1;
}


int sysopen(char* path,int perm){
	/*char tmp[100];
        strcpy(tmp,path);
        file_struct* curr = root;
        file_struct* t;
	char* tokens = strtok(tmp,"/");
        kprintf("path = %s \n",path);
        int i = 0;
	int c = 2;
	fd* fd_d = (struct fd*)kmalloc(sizeof(struct fd));

	if(tokens == NULL)
		return -1;

	if(strcmp(tokens,"rootfs") == 0){
		while(tokens != NULL){
			t = curr;
			if(strcmp(tokens,".") == 0)
				curr = curr->child[0];
			else if(strcmp(tokens,"..") == 0)
				curr = curr->child[1];
			else{
				for(i = 2;i < curr->l;i++){
					if(strcmp(tokens,curr->child[i]->filename) == 0){
						curr = (file_struct*)curr->child[i];
						break;
					}

				}

				if(i >= t->l)
					return -1;

				tokens = strtok(NULL,"/");
			}
		}

		if(curr->file_type == _DIRECTORY)
			return -1;
		
		fd_d->f_node = curr;
		fd_d->current = curr->f;
		fd_d->permission = perm;

	//	currTask->fd_buff[++c] = fd_d;
		return c;
	}	
	return -1;
*/
return 1;
}


uint64_t getpid(){
        //return currTask->pid;
        return 1;
}

uint64_t getppid(){
        //return currTask->parent->pid;
        return 1;
}


char *getcwd(char *buf, uint64_t size){
/*	if(size < strlen(currTask->curr_dir))
		return NULL;
	else
		strcpy(buf,currTask->curr_dir);
	return buf;*/
	return (char*)1;
}

void syscall_isr(){

	//Read the syscall parameters into the registers and pass it to the syscall Handler
	struct regs reg;
	__asm volatile("movq %%r8, %0" : "=r" (reg.rax));		
	__asm volatile("movq %%r9, %0" : "=r" (reg.rbx));		
	__asm volatile("movq %%r10, %0" : "=r" (reg.rcx));		
	__asm volatile("movq %%r11, %0" : "=r" (reg.rdx));
	/*__asm volatile("movq %%r15, %0" : "=r" (reg.rdx));
	*/
	switch(reg.rax){
		case READ_SYSCALL:
			sysread(0, (char*)reg.rbx,(uint64_t)reg.rcx);
		break;
		case WRITE_SYSCALL:		
			syswrite(1,(char*)reg.rcx,(int)reg.rdx);
		break;	
		case FORK_SYSCALL:
		//	fork_syscall();
		break;
	/*	case OPENDIR_SYSCALL:
				opendir("/rootfs/bin/");
		break;
		case READDIR_SYSCALL:
				
		break;
		case OPEN_SYSCALL:

		break;
		case READ_SYSCALL:

		break;
		case CLOSE_SYSCALL:

		break;
		case GETCWD_SYSCALL:

		break;
		case GETPID_SYSCALL:

		break;
		case GETPPID_SYSCALL:

		break;*/
	}
	//char* str = (char*)(reg.rcx);		
	//kprintf(str);			

}	
