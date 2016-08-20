#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

int log_printf (const char*, ...);
int log_vprintf (const char*, va_list);

#endif
