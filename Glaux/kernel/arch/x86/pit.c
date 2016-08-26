#include <stdint.h>

#include "idle.h"
#include "pic.h"
#include "pit.h"
#include "portio.h"
#include "scheduler.h"

void pit_init (void)
{
	int reload = 1193;
	outport8 (0x43, 0x34);
	outport8 (0x40, reload >> 0 & 0xFF);
	outport8 (0x40, reload >> 8 & 0xFF);
	pic_unmask_irq (0);
}

void pit_handle (void)
{
	scheduler_time ();
}
