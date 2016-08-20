#ifndef LFB_H
#define LFB_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#include "paddr.h"

#define NORMAL_BLACK   0x000000
#define NORMAL_RED     0xAA0000
#define NORMAL_GREEN   0x00AA00
#define NORMAL_YELLOW  0xAAAA00
#define NORMAL_BLUE    0x0000AA
#define NORMAL_MAGENTA 0xAA00AA
#define NORMAL_CYAN    0x00AAAA
#define NORMAL_WHITE   0xAAAAAA
#define BRIGHT_BLACK   0x555555
#define BRIGHT_RED     0xFF5555
#define BRIGHT_GREEN   0x55FF55
#define BRIGHT_YELLOW  0xFFFF55
#define BRIGHT_BLUE    0x5555FF
#define BRIGHT_MAGENTA 0xFF55FF
#define BRIGHT_CYAN    0x55FFFF
#define BRIGHT_WHITE   0xFFFFFF

bool lfb_init (void);
void lfb_update (void);

size_t lfb_write (const char*, size_t);
int lfb_printf (const char*, ...);
int lfb_vprintf (const char*, va_list);

bool lfb_newscreen (uint32_t);
wchar_t lfb_newline (uint32_t);
wchar_t lfb_blitc (wchar_t, uint32_t, uint32_t);

#endif
