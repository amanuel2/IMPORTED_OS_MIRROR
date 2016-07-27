#ifndef __STRING_H
#define __STRING_H

#ifndef NULL
#define __need_NULL
#include <stddef.h>
#endif

#ifndef __size_t_defined
#define __size_t_defined
#define __need_size_t
#include <stddef.h>
#undef __need_size_t
#endif

int memcmp (const void*, const void*, size_t);
void* memcpy (void*, const void*, size_t);
void* memmove (void*, const void*, size_t);
void* memset (void*, int, size_t);

char* strcat (char*, const char*);
int strcmp (const char*, const char*);
char* strcpy (char*, const char*);
size_t strlen (const char*);

char* strncat (char*, const char*, size_t);
int strncmp (const char*, const char*, size_t);
char* strncpy (char*, const char*, size_t);
size_t strnlen (const char*, size_t);

size_t strlcat (char*, const char*, size_t);
size_t strlcpy (char*, const char*, size_t);

#endif
