#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernelsym.h"
#include "mboot.h"
#include "mmap.h"
#include "mod.h"
#include "page.h"
#include "paging.h"
#include "vbe.h"

#define PAGETAB_MAPPING ((uint32_t*) 0xFFC00000)
#define PAGEDIR_MAPPING ((uint32_t*) 0xFFFFF000)

paddr_t first_free_page_addr = (paddr_t) -1;
paddr_t last_free_page_addr = (paddr_t) -1;

void page_phys_list (void)
{
	void* kernel_start = (void*) &_kernel_start;
	void* kernel_end = (void*) &_kernel_end;
	for (struct mmap_ent* ent = mmap_ent;
	     ent < (struct mmap_ent*) ((uintptr_t)
	           mmap_ent + mboot_info->mmap_length);
	     ent = (struct mmap_ent*) ((uintptr_t)
	           ent + ent->size + sizeof (ent->size)))
	{
		if (ent->type != 1)
		{
			continue;
		}
		for (paddr_t addr = ((paddr_t) ent->base_addr + 4095) & 0xFFFFF000;
		     addr < (((paddr_t) ent->base_addr + ent->length) & 0xFFFFF000);
		     addr += 4096)
		{
			if (page_get_phys_addr (kernel_start) <= addr &&
			    addr < page_get_phys_addr (kernel_end))
			{
				continue;
			}
			if (mod_at_phys_addr (addr) ||
			    addr == page_get_phys_addr (mboot_info) ||
			    addr == (paddr_t) mboot_info->vbe_ctrl_info ||
			    addr == (paddr_t) mboot_info->vbe_mode_info)
			{
				continue;
			}
			if (mboot_info->mmap_addr <= addr &&
			    addr < mboot_info->mmap_addr + mboot_info->mmap_length)
			{
				continue;
			}
			if (mboot_info->mods_addr <= addr &&
				addr < mboot_info->mods_addr +
				       mboot_info->mods_count * sizeof (struct mod_ent))
			{
				continue;
			}
			if (first_free_page_addr == (paddr_t) -1)
			{
				first_free_page_addr = addr;
			}
			else
			{
				paddr_t* next_addr = page_virt_alloc_initial (1);
				page_map (1, last_free_page_addr, next_addr, 3);
				*next_addr = addr;
				page_unmap (1, next_addr);
			}
			last_free_page_addr = addr;
		}
	}
	paddr_t* next_addr = page_virt_alloc_initial (1);
	page_map (1, last_free_page_addr, next_addr, 3);
	*next_addr = (paddr_t) -1;
	page_unmap (1, next_addr);
}

paddr_t page_phys_alloc (void)
{
	if (first_free_page_addr == (paddr_t) -1)
	{
		return (paddr_t) -1;
	}
	paddr_t result = first_free_page_addr;
	paddr_t* next_addr = page_virt_alloc_initial (1);
	page_map (1, result, next_addr, 3);
	first_free_page_addr = *next_addr;
	memset (next_addr, 0, 4096);
	page_unmap (1, next_addr);
	return result;
}

void page_phys_free (paddr_t phys)
{
	paddr_t* next_addr = page_virt_alloc_initial (1);
	page_map (1, phys, next_addr, 3);
	memset (next_addr, 0, 4096);
	*next_addr = first_free_page_addr;
	first_free_page_addr = phys;
	page_unmap (1, next_addr);
}

void* page_virt_alloc_initial (size_t count)
{
	uintptr_t i = 0;
	size_t k = 0;
	for (i = 0xE0000000; i < 0xE1000000; i += 4096)
	{
		size_t pagetab_index = page_get_pagetab_index ((void*) i);
		if (PAGETAB_MAPPING[pagetab_index] & 1)
		{
			k = 0;
		}
		else
		{
			if (k == count)
			{
				for (size_t n = 0; n < count; n++)
				{
					size_t index = pagetab_index - k + n;
					PAGETAB_MAPPING[index] = 1;
				}
				paging_flush_tlb ();
				return page_get_pagetab_base (pagetab_index - k);
			}
			k += 1;
		}
	}
	return NULL;
}

void* page_virt_alloc (size_t count)
{
	uintptr_t i = 0;
	size_t k = 0;
	for (i = 0xE1000000; i < 0xFFC00000; i += 4096)
	{
		size_t pagedir_index = page_get_pagedir_index ((void*) i);
		size_t pagetab_index = page_get_pagetab_index ((void*) i);
		if (!(PAGEDIR_MAPPING[pagedir_index] & 1))
		{
			paddr_t phys_tmp = page_phys_alloc ();
			if (phys_tmp == (paddr_t) -1)
			{
				for (size_t n = 0; n < count; n++)
				{
					size_t index = pagetab_index - k + n;
					PAGETAB_MAPPING[index] = 0;
				}
				paging_flush_tlb ();
				return NULL;
			}
			PAGEDIR_MAPPING[pagedir_index] = phys_tmp | 3;
			paging_flush_tlb ();
		}
		if (PAGETAB_MAPPING[pagetab_index] & 1)
		{
			k = 0;
		}
		else
		{
			if (k == count)
			{
				for (size_t n = 0; n < count; n++)
				{
					size_t index = pagetab_index - k + n;
					PAGETAB_MAPPING[index] = 1;
				}
				paging_flush_tlb ();
				return page_get_pagetab_base (pagetab_index - k);
			}
			k += 1;
		}
	}
	return NULL;
}

void page_map (size_t count, paddr_t phys, void* virt, size_t flags)
{
	for (uintptr_t i = 0; i < count * 4096; i += 4096)
	{
		size_t pagetab_index = page_get_pagetab_index (virt + i);
		PAGETAB_MAPPING[pagetab_index] = (phys + i) | flags;
	}
	paging_flush_tlb ();
}

void page_unmap (size_t count, void* virt)
{
	for (size_t i = 0; i < count * 4096; i += 4096)
	{
		size_t pagetab_index = page_get_pagetab_index (virt + i);
		PAGETAB_MAPPING[pagetab_index] = 0;
	}
	paging_flush_tlb ();
}

paddr_t page_get_phys_addr (void* virt_addr)
{
	uintptr_t offset = (uintptr_t) virt_addr & 0x00000FFF;
	uint32_t pagetab_index = page_get_pagetab_index (virt_addr);
	uint32_t pagedir_index = page_get_pagedir_index (virt_addr);
	if (!(PAGEDIR_MAPPING[pagedir_index] & 1))
	{
		return (paddr_t) -1;
	}
	if (!(PAGETAB_MAPPING[pagetab_index] & 1))
	{
		return (paddr_t) -1;
	}
	return (PAGETAB_MAPPING[pagetab_index] & 0xFFFFF000) + offset;
}

size_t page_get_pagedir_index (void* virt_ptr)
{
	return ((uintptr_t) virt_ptr) >> 22;
}

size_t page_get_pagetab_index (void* virt_ptr)
{
	return ((uintptr_t) virt_ptr) >> 12;
}

void* page_get_pagedir_base (size_t index)
{
	return (void*) (index << 22);
}

void* page_get_pagetab_base (size_t index)
{
	return (void*) (index << 12);
}
