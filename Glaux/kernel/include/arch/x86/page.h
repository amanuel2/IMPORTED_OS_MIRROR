#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>

#include "paddr.h"

#define PAGESIZE 4096

void page_phys_list (void);
paddr_t page_phys_alloc (void);
void page_phys_free (paddr_t);
void* page_virt_alloc_initial (size_t);
void* page_virt_alloc (size_t);
void page_map (size_t, paddr_t, void*, size_t);
void page_unmap (size_t, void*);
paddr_t page_get_phys_addr (void*);
size_t page_get_pagedir_index (void*);
size_t page_get_pagetab_index (void*);
void* page_get_pagedir_base (size_t);
void* page_get_pagetab_base (size_t);

#endif
