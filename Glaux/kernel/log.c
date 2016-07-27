#include <stdarg.h>

#include "log.h"
#include "lfb.h"

int log_vprintf (const char* format, va_list args)
{
	int result = lfb_vprintf (format, args);
	lfb_update ();
	return result;
}

int log_printf (const char* format, ...)
{
	va_list args;
	va_start (args, format);
	int result = lfb_vprintf (format, args);
	lfb_update ();
	va_end (args);
	return result;
}
