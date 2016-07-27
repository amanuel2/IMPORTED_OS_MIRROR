#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "mod.h"
#include "page.h"
#include "pff2.h"
#include "vmm.h"

static uint8_t* pff2_mod = NULL;
static uint32_t pff2_entry[UNICODE_POINTS];

static const char pff2_header[12] = {
	'F', 'I', 'L', 'E', 0x00, 0x00, 0x00, 0x04, 'P', 'F', 'F', '2',
};

static struct pff2_stat pff2_stat;

bool pff2_scan (struct mod_ent ent, struct pff2_stat* stat)
{
	uint8_t* mod = vmm_map (ent.mod_end - ent.mod_start, ent.mod_start, 1);
	if (!mod)
	{
		return false;
	}
	if (memcmp (mod, pff2_header, 12) != 0)
	{
		vmm_unmap (ent.mod_end - ent.mod_start, mod);
		return false;
	}
	uint32_t i = 0;
	uint32_t flags = 0;
	while (i < ent.mod_end - ent.mod_start)
	{
		uint32_t sectlen = mod[i+4] << 24 | mod[i+5] << 16 |
		                   mod[i+6] << 8 | mod[i+7] << 0;
		if (memcmp (mod + i, "DATA", 4) == 0)
		{
			flags |= 0x40;
			break;
		}
		if (memcmp (mod + i, "PTSZ", 4) == 0)
		{
			flags |= 0x20;
			stat->ptsz = mod[i+8] << 8 | mod[i+9] << 0;
		}
		else if (memcmp (mod + i, "MAXW", 4) == 0)
		{
			flags |= 0x10;
			stat->maxw = mod[i+8] << 8 | mod[i+9] << 0;
		}
		else if (memcmp (mod + i, "MAXH", 4) == 0)
		{
			flags |= 0x08;
			stat->maxh = mod[i+8] << 8 | mod[i+9] << 0;
		}
		else if (memcmp (mod + i, "ASCE", 4) == 0)
		{
			flags |= 0x04;
			stat->asce = mod[i+8] << 8 | mod[i+9] << 0;
		}
		else if (memcmp (mod + i, "DESC", 4) == 0)
		{
			flags |= 0x02;
			stat->desc = mod[i+8] << 8 | mod[i+9] << 0;
		}
		else if (memcmp (mod + i, "CHIX", 4) == 0)
		{
			flags |= 0x01;
			stat->chix = i;
			stat->chixlen = sectlen;
		}
		i += sectlen + 8;
	}
	vmm_unmap (ent.mod_end - ent.mod_start, mod);
	return flags == 0x7F;
}

void pff2_export (struct mod_ent ent, struct pff2_stat stat)
{
	pff2_stat = stat;
	pff2_mod = vmm_map (ent.mod_end - ent.mod_start, ent.mod_start, 1);
	memset (pff2_entry, 0, UNICODE_POINTS * sizeof (uint32_t));
	for (uint32_t i = stat.chix + 8; i < stat.chix + stat.chixlen + 8; i += 9)
	{
		struct pff2_chix* chix = (struct pff2_chix*) (uintptr_t) &pff2_mod[i];
		uint32_t point = chix->point[0] << 24 | chix->point[1] << 16 |
		                 chix->point[2] << 8 | chix->point[3] << 0;
		uint32_t offset = chix->offset[0] << 24 | chix->offset[1] << 16 |
		                  chix->offset[2] << 8 | chix->offset[3] << 0;
		if (point < UNICODE_POINTS && offset < ent.mod_end - ent.mod_start)
		{
			pff2_entry[point] = i;
		}
	}
}

const struct pff2_stat* pff2_get_stat (void)
{
	return pff2_mod ? &pff2_stat : NULL;
}

const struct pff2_data* pff2_get_data (wchar_t wc)
{
	uint32_t i = (uint32_t) wc;
	uint32_t entry = pff2_entry[i];
	if (!entry || !pff2_mod)
	{
		return NULL;
	}
	struct pff2_chix* chix = (struct pff2_chix*) (uintptr_t) &pff2_mod[entry];
	uint32_t offset = chix->offset[0] << 24 | chix->offset[1] << 16 |
	                  chix->offset[2] << 8 | chix->offset[3] << 0;
	return (struct pff2_data*) (uintptr_t) &pff2_mod[offset];
}
