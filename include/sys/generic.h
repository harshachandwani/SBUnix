#include <sys/defs.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
char* convertToString(unsigned long long num,int b);
uint32_t inl(uint16_t port);
void outl(uint16_t port,uint32_t val);
uint64_t pow(uint64_t num,int exp);
uint64_t octalToDecimal(uint64_t octal);
void* memcpy(void* dest,void* src,int size);
