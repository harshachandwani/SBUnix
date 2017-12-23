#ifndef _IDT_H
#define _IDT_H

void init_idt(void);
void load_idtr();	
void set_gates();
void idt_set_gate(int num, unsigned long long base, uint16_t sel, uint8_t flags);
#endif
