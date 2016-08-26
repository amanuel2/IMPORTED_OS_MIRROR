#include <stdint.h>

#include "pic.h"
#include "portio.h"

void pic_init (void)
{
	outport8 (0x20, 0x11);
	outport8 (0xA0, 0x11);
	outport8 (0x21, 0x20);
	outport8 (0xA1, 0x28);
	outport8 (0x21, 0x04);
	outport8 (0xA1, 0x02);
	outport8 (0x21, 0x01);
	outport8 (0xA1, 0x01);
	/* IRQ2 must *not* be masked. */
	outport8 (0x21, 0xFB);
	outport8 (0xA1, 0xFF);
}

void pic_mask_irq (uint8_t irq)
{
	if (irq <= 7)
	{
		uint8_t tmp = inport8 (0x21);
		outport8 (0x21, tmp | 1 << (irq - 0));
	}
	else
	{
		uint8_t tmp = inport8 (0xA1);
		outport8 (0xA1, tmp | 1 << (irq - 8));
	}
}

void pic_unmask_irq (uint8_t irq)
{
	if (irq <= 7)
	{
		uint8_t tmp = inport8 (0x21);
		outport8 (0x21, tmp & ~(1 << (irq - 0)));
	}
	else
	{
		uint8_t tmp = inport8 (0xA1);
		outport8 (0xA1, tmp & ~(1 << (irq - 8)));
	}
}

uint16_t pic_read_irr (void)
{
	outport8 (0x20, 0x0A);
	outport8 (0xA0, 0x0A);
	return inport8 (0x20) | inport8 (0xA0) << 8;
}

uint16_t pic_read_isr (void)
{
	outport8 (0x20, 0x0B);
	outport8 (0xA0, 0x0B);
	return inport8 (0x20) | inport8 (0xA0) << 8;
}

void pic_send_eoi (uint8_t irq)
{
	if (irq == 7 && !(pic_read_isr() & 1 << 7))
	{
		return;
	}
	if (irq == 15 && !(pic_read_isr() & 1 << 15))
	{
		outport8 (0x20, 0x20);
		return;
	}
	if (irq >= 8)
	{
		outport8 (0xA0, 0x20);
	}
	outport8 (0x20, 0x20);
}
