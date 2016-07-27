#include <string.h>

char* strncpy (char* dest, const char* src, size_t len)
{
	size_t i = 0;
	while (src[i] != '\0' && i < len)
	{
		dest[i] = src[i];
		i += 1;
	}
	while (i < len)
	{
		dest[i] = '\0';
		i += 1;
	}
	return dest;
}
