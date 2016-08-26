#ifndef MEMMAP_H
#define MEMMAP_H

#include <stdint.h>

struct __attribute__ ((packed)) mmap_ent
{
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
};

extern struct mmap_ent* mmap_ent;

#endif
