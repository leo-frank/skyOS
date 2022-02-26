#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H

#include "buddy.h"
#include "type.h"

#define PGSIZE (1 << 12)             // 4k page size
#define mem_size (1 << 26)           // 64 MB memory
#define npage ((mem_size) / PGSIZE)  // # of avliable page

// given index in pages, return addr of it's towarding page
#define PGADDR(i) (pmm_start + (i)*PGSIZE)

// given addr of struct Page, return index in pages.
#define PGINDEX(pgaddr) (pgaddr - pages)

void pmm_init();
void page_init();
void* kalloc(uint64 size);
void kfree(struct Page* p);
void pmm_test();

#endif /* PHYSICAL_MEMORY_MANAGER_H */