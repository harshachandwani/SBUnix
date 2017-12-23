#include <stdio.h>
#include <sys/defs.h>
#include <sys/process.h>
#include <unistd.h>


char* strtok(char* str,char* delim){
        static int p;
        static char* s;
        int start;

        start = p;

        if(str!=NULL)
                s = str;

        int j;
        while(s[p]!='\0'){
                j = 0;
                while(delim[j]!='\0'){
                        if(s[p]==delim[j]){
                                s[p] = '\0';
                                p = p + 1;

                                if(s[start] != '\0')
                                        return &s[start];
                                else{
                                        start = p;
                                        p--;
                                        break;
                                }
                        }
                j++;
                }
        p++;
        }
        s[p] = '\0';
        if(s[start]=='\0'){
                p = 0;
                return NULL;
        }
        else
                return &s[start];
}


int strcmp(char* str1,char* str2){
        while(*str1  && *str2 ){
                if(*str1 != *str2)
                        break;
                str1++;
                str2++;
        }
        return *str1 - *str2;
}


char buf[100];
int main(int argc, char *argv[], char *envp[]) {
 // puts("sbush> ");

	char *str = "sbush>";
	uint64_t length = 6;
	uint64_t syscall_number = 1;
	uint64_t fd = 1;
	 __asm__ volatile(

		"movq %0,%%r12;\n"
		"movq %1,%%r13;\n"
		"movq %2,%%r14;\n"
		"movq %3,%%r15;\n"
		"int $0x80;"
                ::"r"(syscall_number),"r"(fd),"r"((uint64_t)str),"r"(length)
                :
                );

	
	/*int pid  = fork();

	if(pid == 0){

		__asm__ volatile(
		"movq %0,%%r12;\n"
		"movq %1,%%r13;\n"
		"movq %2,%%r14;\n"
		"movq %3,%%r15;\n"
		"int $0x80;"
                ::"r"(syscall_number),"r"(fd),"r"((uint64_t)"child"),"r"(length)
                :
                );
	}
	else{
		__asm__ volatile(
		"movq %0,%%r12;\n"
		"movq %1,%%r13;\n"
		"movq %2,%%r14;\n"
		"movq %3,%%r15;\n"
		"int $0x80;"
                ::"r"(syscall_number),"r"(fd),"r"((uint64_t)"parent"),"r"(length)
                :
                );

	}*/
	while(1){
		write(1," \n sbush>",8);
	//write(1,"In Shell\n",5);
		for(int i = 0;i < 100;i++){
			buf[i] = '\0';
		}
		read(0,buf,50);
		char* tokens = strtok(buf," ");
		while(tokens != NULL){
			write(1," ",3);
			if(strcmp(tokens,"echo") == 0)
				tokens = strtok(NULL," ");
			write(1,tokens,15);
			tokens = strtok(NULL," ");
		}
		//write(1,buf,len);
	}		
	while(1);
  return 0;
}
