#include <stdint.h>

#include "mboot.h"
#include "mod.h"
#include "paddr.h"
#include "pff2.h"

void mod_scan (void)
{
	for (uint32_t i = 0; i < mboot_info->mods_count; i++)
	{
		struct pff2_stat stat;
		if (pff2_scan (mod_ent[i], &stat))
		{
			pff2_export (mod_ent[i], stat);
		}
	}
}

bool mod_at_phys_addr (paddr_t address)
{
	for (uint32_t i = 0; i < mboot_info->mods_count; i++)
	{
		if (mod_ent[i].mod_start <= address && address <= mod_ent[i].mod_end)
		{
			return true;
		}
	}
	return false;
}
