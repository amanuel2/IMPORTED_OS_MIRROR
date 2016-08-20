#ifndef __STDIO_H
#define __STDIO_H

#ifndef __size_t_defined
#define __size_t_defined
#define __need_size_t
#include <stddef.h>
#undef __need_size_t
#endif

#ifndef __ssize_t_defined
#define __ssize_t_defined
typedef long signed int ssize_t;
#endif

#define __need___va_list
#include <stdarg.h>
#undef __need___va_list

int snprintf (char*, size_t, const char*, ...);
int cbprintf (void*, size_t (*) (void*, const char*, size_t),
              const char*, ...);

int vsnprintf (char*, size_t, const char*, va_list);
int vcbprintf (void*, size_t (*) (void*, const char*, size_t),
               const char*, va_list);

#endif
