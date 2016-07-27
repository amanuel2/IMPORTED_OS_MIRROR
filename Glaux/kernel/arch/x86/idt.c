#include <stdint.h>
#include <string.h>

#include "interrupt.h"

struct __attribute__ ((packed)) idt_entry
{
	uint16_t addr_low;
	uint16_t segment;
	uint8_t zero;
	uint8_t flags;
	uint16_t addr_high;
};

extern struct idt_entry idt_start[256];

void idt_install_entry (struct idt_entry* entry, void (*handler) (void),
                        uint8_t flags, uint16_t segment)
{
	entry->addr_low = (uintptr_t) handler >> 0 & 0xFFFF;
	entry->segment = segment;
	entry->zero = 0;
	entry->flags = flags;
	entry->addr_high = (uintptr_t) handler >> 16 & 0xFFFF;
}

void idt_init (void)
{
	memset (idt_start, 0x00, sizeof (idt_start));
	idt_install_entry (&idt_start[0], isr_0, 0x8E, 0x08);
	idt_install_entry (&idt_start[1], isr_1, 0x8E, 0x08);
	idt_install_entry (&idt_start[2], isr_2, 0x8E, 0x08);
	idt_install_entry (&idt_start[3], isr_3, 0x8E, 0x08);
	idt_install_entry (&idt_start[4], isr_4, 0x8E, 0x08);
	idt_install_entry (&idt_start[5], isr_5, 0x8E, 0x08);
	idt_install_entry (&idt_start[6], isr_6, 0x8E, 0x08);
	idt_install_entry (&idt_start[7], isr_7, 0x8E, 0x08);
	idt_install_entry (&idt_start[8], isr_8, 0x8E, 0x08);
	idt_install_entry (&idt_start[9], isr_9, 0x8E, 0x08);
	idt_install_entry (&idt_start[10], isr_10, 0x8E, 0x08);
	idt_install_entry (&idt_start[11], isr_11, 0x8E, 0x08);
	idt_install_entry (&idt_start[12], isr_12, 0x8E, 0x08);
	idt_install_entry (&idt_start[13], isr_13, 0x8E, 0x08);
	idt_install_entry (&idt_start[14], isr_14, 0x8E, 0x08);
	idt_install_entry (&idt_start[16], isr_16, 0x8E, 0x08);
	idt_install_entry (&idt_start[17], isr_17, 0x8E, 0x08);
	idt_install_entry (&idt_start[18], isr_18, 0x8E, 0x08);
	idt_install_entry (&idt_start[19], isr_19, 0x8E, 0x08);
	idt_install_entry (&idt_start[20], isr_20, 0x8E, 0x08);
	idt_install_entry (&idt_start[30], isr_30, 0x8E, 0x08);
	idt_install_entry (&idt_start[32], irq_0, 0x8E, 0x08);
	idt_install_entry (&idt_start[33], irq_1, 0x8E, 0x08);
	idt_install_entry (&idt_start[34], irq_2, 0x8E, 0x08);
	idt_install_entry (&idt_start[35], irq_3, 0x8E, 0x08);
	idt_install_entry (&idt_start[36], irq_4, 0x8E, 0x08);
	idt_install_entry (&idt_start[37], irq_5, 0x8E, 0x08);
	idt_install_entry (&idt_start[38], irq_6, 0x8E, 0x08);
	idt_install_entry (&idt_start[39], irq_7, 0x8E, 0x08);
	idt_install_entry (&idt_start[40], irq_8, 0x8E, 0x08);
	idt_install_entry (&idt_start[41], irq_9, 0x8E, 0x08);
	idt_install_entry (&idt_start[42], irq_10, 0x8E, 0x08);
	idt_install_entry (&idt_start[43], irq_11, 0x8E, 0x08);
	idt_install_entry (&idt_start[44], irq_12, 0x8E, 0x08);
	idt_install_entry (&idt_start[45], irq_13, 0x8E, 0x08);
	idt_install_entry (&idt_start[46], irq_14, 0x8E, 0x08);
	idt_install_entry (&idt_start[47], irq_15, 0x8E, 0x08);
}
