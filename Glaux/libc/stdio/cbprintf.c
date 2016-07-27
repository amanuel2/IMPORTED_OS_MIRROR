#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int cbprintf (void* info, size_t (*callback) (void*, const char*, size_t),
              const char* format, ...)
{
	va_list args;
	va_start (args, format);
	int result = vcbprintf (info, callback, format, args);
	va_end (args);
	return result;
}
