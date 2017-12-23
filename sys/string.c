#include <sys/defs.h>

void memset(void* str,int val,int len){
	unsigned char* ptr = str;
	while(len > 0){
		*ptr = val;
		len--;
		ptr++;
	}
}

int strlen(char* str){
	int length = 0;
	while(*str){
		length++;
		str++;
	}
	return length;
}

char* strtok(char* str,const char* delim){
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

int stoi(char* str){
	int result = 0;
	while(*str && (*str >= '0' && *str <= '9')){
		result = result * 10 + (*str - '0');
		str++;
	}
	return result;
}

char* strcpy(char* dest_str, char* src_str){
	char* head = dest_str;
	while( *src_str){
	 	*dest_str = *src_str;
		dest_str++;
		src_str++;
	}

	*dest_str = '\0';
	return head;

	//char* head = dest_str;
	//while((*dest_str++ = *src_str++) != '\0');
	//return head;
}
