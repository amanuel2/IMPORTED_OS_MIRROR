#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct vsnprintf_info
{
	char* buf;
	size_t size;
	size_t index;
};

static size_t callback (void* info_ptr, const char* str, size_t len)
{
	struct vsnprintf_info* info = (struct vsnprintf_info*) info_ptr;
	if (info->size && info->index < info->size)
	{
		size_t available = info->size - info->index;
		size_t possible = len < available ? len : available;
		memcpy (info->buf + info->index, str, possible);
		info->index += possible;
	}
	return len;
}

int vsnprintf (char* buf, size_t size, const char* format, va_list args)
{
	struct vsnprintf_info info;
	info.buf = buf;
	info.size = size ? size - 1 : 0;
	info.index = 0;
	int result = vcbprintf (&info, callback, format, args);
	if (size)
	{
		info.buf[info.index] = '\0';
	}
	return result;
}
