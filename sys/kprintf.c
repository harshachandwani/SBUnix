#include <sys/kprintf.h>
#include <sys/stdarg.h>
#include <sys/generic.h>

//volatile char *video = (volatile char*)0xB8000;
//volatile char* video = (volatile char*)(0xffffffff80000000 + 0xb8000);

char* video = (char*)(0xffffffff80000000 + 0xb8000);
static int x,y;

void clear_screen(){
	char* video1 = (char*)(0xffffffff80000000 + 0xb8000);
	int count = 0;
	while(count < (24*160)){
		*video1++ = ' ';
	 	video1++; 
		count+=2;
	}

	video = (char*)(0xffffffff80000000 + 0xb8000);
	x = 0;
	y = 0;

}		

void scroll(){
	char* video1 =(char*)(0xffffffff80000000 + 0xb8000);
	int count = 0;
	while(count != 23 * 80 * 2){
		*video = *(video1 + 80 * 2);
		video++;
		video1++;
		count++;
	}
	video = (char*)(0xffffffff80000000 + 0xb8000);	
	while(count != 24 *80 * 2){
		*(video + count) = '\0';
		count++;
	}
	video = (char*)(0xffffffff80000000 + 0xb8000);
	x = 0;
	y = 23;
}

int get_offset(){
	return ((y*80+x)*2);
}


char* convertTochar(int ch){
        static char buffer[2];
        char* ptr = &buffer[1];
        buffer[1]='\0';
        *--ptr = ch;
        return ptr;
}

char* convertToPointer(long paddress){
        static char buffer[2];
        char* ptr = &buffer[1];
        buffer[1]='\0';
        *--ptr = paddress;
        return ptr;
}


void write_char( int color, const char *string )
{
	volatile char *location = 0;
	if(y == 24)
		scroll();
	if (*string == '\n' || x == 80){
		x = 0;
		y++;
		return;
	}
	location = video + get_offset();
        *location++ = *string++;
	x++;
}

void write_string(int color, const char *string)
{
	volatile char *location = 0;
	if(y ==24)
		scroll();
        while(*string != '\0'){
		if (*string == '\n' || x == 80){
			x = 0;
			y++;
			if(*string == '\n'){
				string++;
				continue;
			}
		}

		location = video + get_offset();
		*location++ = *string++;
		x++;
        }
}

void print_at(int cursor_x, int cursor_y, int c){
	char* ch;
	volatile char* location = video + ((cursor_y * 80 + cursor_x) * 2);
	ch = convertTochar(c);
        *location++ = *ch++; 
}

	
void kprintf(const char *fmt, ...)
{

        va_list arg;
	va_start(arg,fmt);
        char* str,*numString,*ch;
        int i;
        long ptr;
	
        while(*fmt != 0){
                while(*fmt !='%' && *fmt != 0){
                        write_char(0X1F,fmt);
                        fmt++;
                }

		if(*fmt == 0)
			break;
                fmt++;
                switch(*fmt){

                        case 'c':
                                i = va_arg(arg,int);
                                ch = convertTochar(i);
                                write_string(0X1F,ch);
                        break;


                        case 's':
                                str = va_arg(arg,char*);
                                write_string(0X1F,str);
                        break;
			case 'd':
                                i = va_arg(arg,int);
                                if(i < 0){
                                        write_char(0X1F,"-");
                                        i = -i;
                                }
                                numString = convertToString(i,10);
                                write_string(0X1F,numString);
                        break;


                        case 'x':
				i = va_arg(arg,int);
                                numString = convertToString(i,16);
                                write_string(0x1F,numString);
                        break;


                        case 'p':
                                ptr = va_arg(arg,unsigned long long);
                                numString = convertToString(ptr,16);
                                write_string(0x1F,numString);
                        break;

                }
        fmt++;
        }
}
