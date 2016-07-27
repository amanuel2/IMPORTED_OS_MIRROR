#ifndef PFF2_H
#define PFF2_H

#define UNICODE_POINTS 0x00110000

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "mod.h"

struct pff2_chix
{
	uint8_t point[4];
	uint8_t flags;
	uint8_t offset[4];
};

struct pff2_data
{
	uint8_t width[2];
	uint8_t height[2];
	int8_t xoffset[2];
	int8_t yoffset[2];
	int8_t devwidth[2];
	uint8_t bitmap[];
};

struct pff2_stat
{
	uint16_t ptsz;
	uint16_t maxw;
	uint16_t maxh;
	uint16_t asce;
	uint16_t desc;
	uint32_t chix;
	uint32_t chixlen;
};

bool pff2_scan (struct mod_ent, struct pff2_stat*);
void pff2_export (struct mod_ent, struct pff2_stat);
const struct pff2_stat* pff2_get_stat (void);
const struct pff2_data* pff2_get_data (wchar_t);

#endif
