#include <sys/defs.h>
#include <sys/generic.h>

void init_timer(uint32_t freq){
	uint32_t divisor = 1193180 / freq;
	
	outb(0x43, 0x36);
	outb(0x40, (uint16_t)(divisor & 0XFF));
	outb(0x40, (uint16_t)(divisor >> 8));
}
