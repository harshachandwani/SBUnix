#ifndef _PIC_H
#define _PIC_H

#include <sys/defs.h>

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_sendeoi(unsigned char irq);
void irq_set_mask(unsigned char IRQline);
void irq_clear_mask(unsigned char IRQline);
void io_wait(void);
#endif
