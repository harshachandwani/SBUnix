#include <sys/defs.h>
#include <sys/kprintf.h>

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outb(uint16_t port, uint8_t val)
{
    __asm( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outl(uint16_t port,uint32_t val)
{
        __asm( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

char* convertToString(unsigned long long num,int b)
{
        static char arr[] = "0123456789ABCDEF";
        static char buffer[100];
        char* ptr = &buffer[99];
        buffer[99] = '\0';
        do{
		*--ptr = arr[num%b];
                num = num/b;
        }while(num != 0);

        return ptr;
}

uint64_t pow(uint64_t num,int exp){
	
	uint64_t res = 1;
	while(exp >= 0){
		if(exp == 0)
			break;
		else
			res *= num;	
		exp--;
	}
	return res;
}

uint64_t octalToDecimal(uint64_t octal){
	uint64_t res = 0;
	uint64_t tmp = octal;
	int b = 1;
	while(tmp){
		res += (tmp%10)*b;
		tmp = tmp/10;
		b *= 8;
	}
	return res;
}

void* memcpy(void* dest,void* src,int size){
	char* s = (char*)src;
	char* d = (char*)dest;
	//kprintf("In memcpy");
	//kprintf("%s",(char*)src);
	while(size > 0){
		*d++ = *s++;
		size--;
	}
	return dest;
}
