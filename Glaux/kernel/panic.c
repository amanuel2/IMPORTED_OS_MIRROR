#include <stdarg.h>

#include "halt.h"
#include "log.h"

void panic (const char* format, ...)
{
	va_list args;
	va_start (args, format);
	log_vprintf (format, args);
	va_end (args);
	halt ();
}
