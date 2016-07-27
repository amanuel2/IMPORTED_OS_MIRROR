#include <stddef.h>

#include "paddr.h"
#include "page.h"
#include "vmm.h"

void* vmm_map_initial (size_t bytes, paddr_t phys, size_t flags)
{
	size_t offset = phys & 0x00000FFF;
	size_t base = phys & 0xFFFFF000;
	size_t pages = (bytes + offset + PAGESIZE - 1) / PAGESIZE;
	void* result = page_virt_alloc_initial (pages);
	if (!result)
	{
		return NULL;
	}
	page_map (pages, base, result, flags);
	return (void*) ((uintptr_t) result + offset);
}

void* vmm_map (size_t bytes, paddr_t phys, size_t flags)
{
	size_t offset = phys & 0x00000FFF;
	size_t base = phys & 0xFFFFF000;
	size_t pages = (bytes + offset + PAGESIZE - 1) / PAGESIZE;
	void* result = page_virt_alloc (pages);
	if (!result)
	{
		return NULL;
	}
	page_map (pages, base, result, flags);
	return (void*) ((uintptr_t) result + offset);
}

void* vmm_alloc_initial (size_t bytes, size_t flags)
{
	size_t pages = (bytes + PAGESIZE - 1) / PAGESIZE;
	void* result = page_virt_alloc_initial (pages);
	if (!result)
	{
		return NULL;
	}
	for (size_t i = 0; i < pages; i++)
	{
		paddr_t phys_tmp = page_phys_alloc ();
		if (phys_tmp == (paddr_t) -1)
		{
			page_unmap (pages, result);
			return NULL;
		}
		page_map (1, phys_tmp, result + i * PAGESIZE, flags);
	}
	return (void*) result;
}

void* vmm_alloc (size_t bytes, size_t flags)
{
	size_t pages = (bytes + PAGESIZE - 1) / PAGESIZE;
	void* result = page_virt_alloc (pages);
	if (!result)
	{
		return NULL;
	}
	for (size_t i = 0; i < pages; i++)
	{
		paddr_t phys_tmp = page_phys_alloc ();
		if (phys_tmp == (paddr_t) -1)
		{
			page_unmap (pages, result);
			return NULL;
		}
		page_map (1, phys_tmp, result + i * PAGESIZE, flags);
	}
	return (void*) result;
}

void vmm_unmap (size_t bytes, void* virt)
{
	size_t offset = (uintptr_t) virt & 0x00000FFF;
	size_t base = (uintptr_t) virt & 0xFFFFF000;
	size_t pages = (bytes + offset + PAGESIZE - 1) / PAGESIZE;
	page_unmap (pages, (void*) base);
}

void vmm_free (size_t bytes, void* virt)
{
	size_t offset = (uintptr_t) virt & 0x00000FFF;
	size_t base = (uintptr_t) virt & 0xFFFFF000;
	size_t pages = (bytes + offset + PAGESIZE - 1) / PAGESIZE;
	for (size_t i = 0; i < pages; i++)
	{
		page_phys_free (page_get_phys_addr ((void*) (base + i * PAGESIZE)));
	}
	page_unmap (pages, (void*) base);
}

paddr_t vmm_get_phys_addr (void* virt_addr)
{
	return page_get_phys_addr (virt_addr);
}
