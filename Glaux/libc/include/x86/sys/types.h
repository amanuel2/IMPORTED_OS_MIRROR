#ifndef __TYPES_H
#define __TYPES_H

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

#endif
