#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/string.h>

struct IDTDescr_struct{
   uint16_t base_low; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t type_attr; // type and attributes
   uint16_t base_mid; // offset bits 16..31
   uint32_t base_high; // offset bits 32..63
   uint32_t zero;     // reserved
}__attribute__((packed));
//typedef struct IDTDescr_struct IDTDescr;
 
struct idtr_t_struct {
  uint16_t size;
  uint64_t addr;
}__attribute__((packed));
//typedef struct idtr_t_struct idtr_t;

extern void isr8();
extern void isr14();
extern void isr32();
extern void isr33();
extern void isr128();

struct IDTDescr_struct idt_entries[256];
struct idtr_t_struct idtr;

void init_idt(){
	load_idtr();
	memset(&idt_entries, 0, sizeof(struct IDTDescr_struct) * 256);	
	set_gates();
	
	__asm("lidt (%0)": :"r" (&idtr));	
}

void load_idtr(){
	idtr.size = sizeof(struct IDTDescr_struct)*256 - 1;
	idtr.addr = (uint64_t)&idt_entries;
}

void set_gates(){
	/*
	idt_set_gate(0, (uint64_t)isr0, 0x8, 0x8E);
	idt_set_gate(1, (uint64_t)isr1, 0x8, 0x8E);
	idt_set_gate(2, (uint64_t)isr2, 0x8, 0x8E);
	idt_set_gate(3, (uint64_t)isr3, 0x8, 0x8E);
	idt_set_gate(4, (uint64_t)isr4, 0x8, 0x8E);
	idt_set_gate(5, (uint64_t)isr5, 0x8, 0x8E);
	idt_set_gate(6, (uint64_t)isr6, 0x8, 0x8E);
	idt_set_gate(7, (uint64_t)isr7, 0x8, 0x8E);
	idt_set_gate(8, (uint64_t)isr8, 0x8, 0x8E);
	idt_set_gate(9, (uint64_t)isr9, 0x8, 0x8E);
	idt_set_gate(10, (uint64_t)isr10, 0x8, 0x8E);
	idt_set_gate(11, (uint64_t)isr11, 0x8, 0x8E);
	idt_set_gate(12, (uint64_t)isr12, 0x8, 0x8E);
	idt_set_gate(13, (uint64_t)isr13, 0x8, 0x8E);
	idt_set_gate(14, (uint64_t)isr14, 0x8, 0x8E);
	idt_set_gate(15, (uint64_t)isr15, 0x8, 0x8E);
	idt_set_gate(16, (uint64_t)isr16, 0x8, 0x8E);
	idt_set_gate(17, (uint64_t)isr17, 0x8, 0x8E);
	idt_set_gate(18, (uint64_t)isr18, 0x8, 0x8E);
	idt_set_gate(19, (uint64_t)isr19, 0x8, 0x8E);
	idt_set_gate(20, (uint64_t)isr20, 0x8, 0x8E);
	idt_set_gate(21, (uint64_t)isr21, 0x8, 0x8E);
	idt_set_gate(22, (uint64_t)isr22, 0x8, 0x8E);
	idt_set_gate(23, (uint64_t)isr23, 0x8, 0x8E);
	idt_set_gate(24, (uint64_t)isr24, 0x8, 0x8E);
	idt_set_gate(25, (uint64_t)isr25, 0x8, 0x8E);
	idt_set_gate(26, (uint64_t)isr26, 0x8, 0x8E);
	idt_set_gate(27, (uint64_t)isr27, 0x8, 0x8E);
	idt_set_gate(28, (uint64_t)isr28, 0x8, 0x8E);
	idt_set_gate(29, (uint64_t)isr29, 0x8, 0x8E);
	idt_set_gate(30, (uint64_t)isr30, 0x8, 0x8E);
	idt_set_gate(31, (uint64_t)isr31, 0x8, 0x8E);
	*/
	idt_set_gate(8, (uint64_t)&isr8, 0x8, 0xEE);
	idt_set_gate(14, (uint64_t)&isr14, 0x8, 0xEE);
	idt_set_gate(32, (uint64_t)&isr32, 0x8, 0x8E);
	idt_set_gate(33, (uint64_t)&isr33, 0x8, 0x8E);
	idt_set_gate(128, (uint64_t)&isr128, 0x8, 0xEE);
}

void idt_set_gate(int num, unsigned long long base, uint16_t sel, uint8_t flags)
{
	idt_entries[num].base_low = base & 0xFFFF;
	idt_entries[num].selector = sel;
	idt_entries[num].ist = 0;
	idt_entries[num].type_attr = flags;
	idt_entries[num].base_mid = (base >> 16) & 0xFFFF;
	idt_entries[num].base_high = (base >> 32) & 0xFFFFFFFF;
	idt_entries[num].zero = 0;
} 	

