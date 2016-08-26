#ifndef VMM_H
#define VMM_H

#include "paddr.h"

void* vmm_map_initial (size_t, paddr_t, size_t);
void* vmm_map (size_t, paddr_t, size_t);
void* vmm_alloc (size_t, size_t);
void vmm_unmap (size_t, void*);
void vmm_free (size_t, void*);

#endif
