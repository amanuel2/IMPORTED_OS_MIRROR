#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ansicode.h"
#include "lfb.h"
#include "pff2.h"
#include "vbe.h"
#include "vmm.h"

static uint32_t* tempbuf = NULL;
static uint8_t* framebuf = NULL;

static size_t pixel_x = 0;
static size_t pixel_y = 0;

static uint32_t color_bg = NORMAL_BLACK;
static uint32_t color_fg = NORMAL_WHITE;

static bool bold = false;
static bool inverse = false;

static mbstate_t ps = {0, 0, 0};

static bool inside_ansi_code = false;
static struct ansicode_state ansicode_state = {
	0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0,},
};

struct lfb_vprintf_info
{
	size_t index;
};

static size_t callback (void* info_ptr, const char* str, size_t size)
{
	struct lfb_vprintf_info* info = (struct lfb_vprintf_info*) info_ptr;
	size_t result = lfb_write (str, size);
	info->index += result;
	return result;
}

bool lfb_init (void)
{
	bool vbe_3 = vbe_ctrl_info->version >= 0x0300;
	uint16_t x_res = vbe_mode_info->x_res;
	uint16_t y_res = vbe_mode_info->y_res;
	uint32_t pitch = vbe_3 ? vbe_mode_info->lin_pitch : vbe_mode_info->pitch;
	framebuf = vmm_map (pitch * y_res, vbe_mode_info->framebuffer_addr, 3);
	if (!framebuf)
	{
		return false;
	}
	tempbuf = vmm_alloc (x_res * y_res * sizeof (uint32_t), 3);
	if (!tempbuf)
	{
		vmm_unmap (pitch * y_res, framebuf);
		framebuf = NULL;
		return false;
	}
	return true;
}

void lfb_update (void)
{
	if (!framebuf || !tempbuf)
	{
		return;
	}
	uint32_t bpp = vbe_mode_info->bpp;
	uint32_t width = vbe_mode_info->x_res;
	uint32_t height = vbe_mode_info->y_res;
	bool vbe_3 = vbe_ctrl_info->version >= 0x0300;
	uint32_t pitch = vbe_3 ? vbe_mode_info->lin_pitch :
	                         vbe_mode_info->pitch;
	uint8_t red_pos = vbe_3 ? vbe_mode_info->lin_red_field_pos :
	                          vbe_mode_info->red_field_pos;
	uint8_t red_mask_size = vbe_3 ? vbe_mode_info->lin_red_mask_size :
	                                vbe_mode_info->red_mask_size;
	uint8_t green_pos = vbe_3 ? vbe_mode_info->lin_green_field_pos :
	                            vbe_mode_info->green_field_pos;
	uint8_t green_mask_size = vbe_3 ? vbe_mode_info->lin_green_mask_size :
	                                  vbe_mode_info->green_mask_size;
	uint8_t blue_pos = vbe_3 ? vbe_mode_info->lin_blue_field_pos :
	                           vbe_mode_info->blue_field_pos;
	uint8_t blue_mask_size = vbe_3 ? vbe_mode_info->lin_blue_mask_size :
	                                 vbe_mode_info->blue_mask_size;
	uint32_t red_mask = (1 << red_mask_size) - 1;
	uint32_t green_mask = (1 << green_mask_size) - 1;
	uint32_t blue_mask = (1 << blue_mask_size) - 1;
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t index = y * width + x;
			uint32_t data = tempbuf[index];
			uint32_t red = (data >> (24 - red_mask_size)) & red_mask;
			uint32_t green = (data >> (16 - green_mask_size)) & green_mask;
			uint32_t blue = (data >> (8 - blue_mask_size)) & blue_mask;
			size_t newindex = y * pitch + x * ((bpp + 7) / 8);
			uint32_t newdata = red << red_pos |
			                   green << green_pos |
			                   blue << blue_pos;
			if (bpp == 15 || bpp == 16)
			{
				framebuf[newindex+0] = newdata >> 0;
				framebuf[newindex+1] = newdata >> 8;
			}
			else if (bpp == 24)
			{
				framebuf[newindex+0] = newdata >> 0;
				framebuf[newindex+1] = newdata >> 8;
				framebuf[newindex+2] = newdata >> 16;
			}
			else if (bpp == 32)
			{
				framebuf[newindex+0] = newdata >> 0;
				framebuf[newindex+1] = newdata >> 8;
				framebuf[newindex+2] = newdata >> 16;
				framebuf[newindex+3] = newdata >> 24;
			}
		}
	}
}

size_t lfb_write (const char* str, size_t size)
{
	size_t i = 0;
	while (i < size)
	{
		wchar_t wc;
		size_t amount = mbrtowc (&wc, str + i, size - i, &ps);
		if (amount == (size_t) -2)
		{
			i = size;
			continue;
		}
		if (amount == (size_t) -1)
		{
			memset (&ps, 0, sizeof (ps));
			lfb_blitc (L'�', bold ? color_fg | 0x555555 : color_fg, color_bg);
			i += 1;
			continue;
		}
		if (amount == 0)
		{
			i += 1;
			continue;
		}
		if (!inside_ansi_code)
		{
			if (wc == L'\e')
			{
				inside_ansi_code = true;
				i += amount;
			}
			else
			{
				if (inverse)
				{
					lfb_blitc (wc, color_bg,
					           bold ? color_fg | 0x555555 : color_fg);
				}
				else
				{
					lfb_blitc (wc, bold ? color_fg | 0x555555 : color_fg,
					           color_bg);
				}
				i += amount;
			}
		}
		else
		{
			int result = ansicode_addc (wc, &ansicode_state);
			if (result == -1)
			{
				inside_ansi_code = false;
				ansicode_reset (&ansicode_state);
				i += amount;
			}
			if (result == -2)
			{
				i += amount;
			}
			else
			{
				if (ansicode_state.cmd == L'm')
				{
					if (ansicode_state.arg_num == 0)
					{
						bold = false;
						inverse = false;
						color_fg = NORMAL_WHITE;
						color_bg = NORMAL_BLACK;
					}
					else
					{
						for (size_t k = 0; k < ansicode_state.arg_num; k++)
						{
							if (ansicode_state.arg[k] == 0)
							{
								bold = false;
								inverse = false;
								color_fg = NORMAL_WHITE;
								color_bg = NORMAL_BLACK;
							}
							if (ansicode_state.arg[k] == 1)
							{
								bold = true;
							}
							if (ansicode_state.arg[k] == 7)
							{
								inverse = true;
							}
							if (ansicode_state.arg[k] == 22)
							{
								bold = false;
							}
							if (ansicode_state.arg[k] == 27)
							{
								inverse = false;
							}
							if (ansicode_state.arg[k] == 30)
							{
								color_fg = NORMAL_BLACK;
							}
							if (ansicode_state.arg[k] == 31)
							{
								color_fg = NORMAL_RED;
							}
							if (ansicode_state.arg[k] == 32)
							{
								color_fg = NORMAL_GREEN;
							}
							if (ansicode_state.arg[k] == 33)
							{
								color_fg = NORMAL_YELLOW;
							}
							if (ansicode_state.arg[k] == 34)
							{
								color_fg = NORMAL_BLUE;
							}
							if (ansicode_state.arg[k] == 35)
							{
								color_fg = NORMAL_MAGENTA;
							}
							if (ansicode_state.arg[k] == 36)
							{
								color_fg = NORMAL_CYAN;
							}
							if (ansicode_state.arg[k] == 37)
							{
								color_fg = NORMAL_WHITE;
							}
							if (ansicode_state.arg[k] == 39)
							{
								color_fg = NORMAL_WHITE;
							}
							if (ansicode_state.arg[k] == 40)
							{
								color_bg = NORMAL_BLACK;
							}
							if (ansicode_state.arg[k] == 41)
							{
								color_bg = NORMAL_RED;
							}
							if (ansicode_state.arg[k] == 42)
							{
								color_bg = NORMAL_GREEN;
							}
							if (ansicode_state.arg[k] == 43)
							{
								color_bg = NORMAL_YELLOW;
							}
							if (ansicode_state.arg[k] == 44)
							{
								color_bg = NORMAL_BLUE;
							}
							if (ansicode_state.arg[k] == 45)
							{
								color_bg = NORMAL_MAGENTA;
							}
							if (ansicode_state.arg[k] == 46)
							{
								color_bg = NORMAL_CYAN;
							}
							if (ansicode_state.arg[k] == 47)
							{
								color_bg = NORMAL_WHITE;
							}
							if (ansicode_state.arg[k] == 49)
							{
								color_bg = NORMAL_BLACK;
							}
						}
					}
				}
				inside_ansi_code = false;
				ansicode_reset (&ansicode_state);
				i += amount;
			}
		}
	}
	return i;
}

int lfb_printf (const char* format, ...)
{
	va_list args;
	va_start (args, format);
	int result = lfb_vprintf (format, args);
	va_end (args);
	return result;
}

int lfb_vprintf (const char* format, va_list args)
{
	struct lfb_vprintf_info info;
	info.index = 0;
	return vcbprintf (&info, callback, format, args);
}

bool lfb_newscreen (uint32_t bg)
{
	if (!tempbuf)
	{
		return false;
	}
	uint32_t width = vbe_mode_info->x_res;
	uint32_t height = vbe_mode_info->y_res;
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t index = y * width + x;
			tempbuf[index] = bg;
		}
	}
	pixel_x = 0;
	pixel_y = 0;
	return true;
}

wchar_t lfb_newline (uint32_t bg)
{
	if (!tempbuf)
	{
		return (wchar_t) -1;
	}
	const struct pff2_stat* stat = pff2_get_stat ();
	if (!stat)
	{
		return (wchar_t) -1;
	}
	uint32_t width = vbe_mode_info->x_res;
	uint32_t height = vbe_mode_info->y_res;
	for (size_t y = pixel_y; y < pixel_y + stat->maxh; y++)
	{
		for (size_t x = pixel_x; x < width; x++)
		{
			size_t index = y * width + x;
			tempbuf[index] = bg;
		}
	}
	pixel_x = 0;
	if (pixel_y + 2 * stat->maxh > height)
	{
		memmove (tempbuf, tempbuf + width * stat->maxh,
		         width * pixel_y * sizeof (tempbuf[0]));
		for (size_t y = pixel_y; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				size_t index = y * width + x;
				tempbuf[index] = bg;
			}
		}
	}
	else
	{
		pixel_y += stat->maxh;
	}
	return L'\n';
}

wchar_t lfb_blitc (wchar_t c, uint32_t fg, uint32_t bg)
{
	if (!tempbuf)
	{
		return (wchar_t) -1;
	}
	if (c == L'\n')
	{
		return lfb_newline (bg);
	}
	const struct pff2_stat* stat = pff2_get_stat ();
	const struct pff2_data* data = pff2_get_data (c);
	if (!stat || !data)
	{
		return (wchar_t) -1;
	}
	uint32_t width = vbe_mode_info->x_res;
	uint16_t cwidth = data->width[0] << 8 | data->width[1] << 0;
	uint16_t cheight = data->height[0] << 8 | data->height[1] << 0;
	int16_t cxoffset = data->xoffset[0] << 8 | data->xoffset[1] << 0;
	int16_t cyoffset = data->yoffset[0] << 8 | data->yoffset[1] << 0;
	int16_t cdevwidth = data->devwidth[0] << 8 | data->devwidth[1] << 0;
	if (pixel_x + cdevwidth > width)
	{
		lfb_newline (bg);
	}
	for (size_t y = pixel_y; y < pixel_y + stat->maxh; y++)
	{
		for (size_t x = pixel_x; x < pixel_x + cdevwidth; x++)
		{
			size_t index = y * width + x;
			tempbuf[index] = bg;
		}
	}
	size_t i = 0;
	uint8_t b = 0;
	for (size_t y = pixel_y - cyoffset + stat->asce - cheight;
	     y < pixel_y - cyoffset + stat->asce; y++)
	{
		for (size_t x = pixel_x + cxoffset;
		     x < pixel_x + cxoffset + cwidth; x++)
		{
			if (b > 7)
			{
				b = 0;
				i += 1;
			}
			uint8_t bitmap = data->bitmap[i];
			size_t index = y * width + x;
			tempbuf[index] = bitmap & (1 << (7 - b)) ? fg : bg;
			b += 1;
		}
	}
	pixel_x += cdevwidth;
	return c;
}
