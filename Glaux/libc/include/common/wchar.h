#ifndef __WCHAR_H
#define __WCHAR_H

#ifndef __size_t_defined
#define __size_t_defined
#define __need_size_t
#include <stddef.h>
#undef __need_size_t
#endif

#ifndef __wchar_t_defined
#define __wchar_t_defined
#define __need_wchar_t
#include <stddef.h>
#undef __need_wchar_t
#endif

#ifndef __wint_t_defined
#define __wint_t_defined
#define __need_wint_t
#include <stddef.h>
#undef __need_wint_t
#endif

typedef struct
{
	uint8_t remaining;
	uint8_t index;
	wint_t wchar;
} mbstate_t;

size_t mbrtowc (wchar_t*, const char*, size_t, mbstate_t*);

#endif
