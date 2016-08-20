#ifndef MOD_H
#define MOD_H

#include <stdbool.h>
#include <stdint.h>

#include "paddr.h"

struct mod_ent
{
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t cmdline;
	uint32_t pad;
};

extern struct mod_ent* mod_ent;

bool mod_at_phys_addr (paddr_t);
void mod_scan (void);

#endif
