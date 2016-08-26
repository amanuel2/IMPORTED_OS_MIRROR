#include <string.h>

char* strncat (char* dest, const char* src, size_t len)
{
	size_t destlen = strlen (dest);
	size_t i = 0;
	while (src[i] != '\0' && i < len)
	{
		dest[destlen+1] = src[i];
		i += 1;
	}
	dest[destlen+i] = '\0';
	return dest;
}
