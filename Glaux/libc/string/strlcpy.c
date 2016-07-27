#include <string.h>

size_t strlcpy (char* dest, const char* src, size_t len)
{
	if (len == 0)
	{
		return strlen (src);
	}
	size_t i = 0;
	while (src[i] != '\0' && i < len - 1)
	{
		dest[i] = src[i];
		i += 1;
	}
	dest[i] = '\0';
	return strlen (src);
}
