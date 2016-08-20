#include <string.h>

size_t strnlen (const char* s, size_t len)
{
	size_t i = 0;
	while (s[i] != '\0' && i < len)
	{
		i += 1;
	}
	return i;
}
