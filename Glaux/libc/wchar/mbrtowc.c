#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

/*
 *	NOTE: It is assumed that the locale is always UTF-8.
 */

static size_t utf8_mbrtowc (wchar_t* pwc, const char* s,
                            size_t n, mbstate_t* ps)
{
	size_t i = 0;
	while (i < n)
	{
		unsigned char uc = (unsigned char) s[i];
		if (ps->index == 0)
		{
			if ((uc & 0x80) == 0x00)
			{
				ps->remaining = 0;
				ps->wchar = (wchar_t) uc & 0x7F;
			}
			else if ((uc & 0xE0) == 0xC0)
			{
				ps->remaining = 1;
				ps->wchar = (wchar_t) uc & 0x1F;
			}
			else if ((uc & 0xF0) == 0xE0)
			{
				ps->remaining = 2;
				ps->wchar = (wchar_t) uc & 0x0F;
			}
			else if ((uc & 0xF8) == 0xF0)
			{
				ps->remaining = 3;
				ps->wchar = (wchar_t) uc & 0x07;
			}
			else
			{
				return (size_t) -1;
			}
		}
		else
		{
			if ((uc & 0xC0) != 0x80)
			{
				return (size_t) -1;
			}
			ps->wchar = (ps->wchar << 6) | (uc & 0x3F);
			ps->remaining -= 1;
		}
		i += 1;
		ps->index += 1;
		if (ps->remaining == 0)
		{
			break;
		}
	}
	if (ps->remaining != 0)
	{
		return (size_t) -2;
	}
	if ((0xD800 <= ps->wchar && ps->wchar <= 0xDFFF) || 0x110000 <= ps->wchar)
	{
		return (size_t) -1;
	}
	wchar_t result = ps->wchar;
	if (pwc)
	{
		*pwc = result;
	}
	ps->index = 0;
	ps->wchar = 0;
	return result != L'\0' ? i : 0;
}

size_t mbrtowc (wchar_t* pwc, const char* s, size_t n, mbstate_t* ps)
{
	static mbstate_t static_ps;
	if (!ps)
	{
		ps = &static_ps;
	}
	if (!s)
	{
		s = "";
		n = 1;
	}
	return utf8_mbrtowc (pwc, s, n, ps);
}
