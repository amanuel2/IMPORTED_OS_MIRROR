#include <string.h>

char* strcpy (char* dest, const char* src)
{
	if (src == dest)
	{
		return dest;
	}
	size_t i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i += 1;
	}
	dest[i] = '\0';
	return dest;
}
