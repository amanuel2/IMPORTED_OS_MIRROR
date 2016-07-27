#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void pic_init (void);
void pic_mask_irq (uint8_t);
void pic_unmask_irq (uint8_t);
void pic_send_eoi (uint8_t);

uint16_t pic_read_irr (void);
uint16_t pic_read_isr (void);

#endif
