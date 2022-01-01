#include "type.h"

#define PGSIZE (1 << 12)
#define mem_size = (1 << 26); 
#define npage (1 << 26) / PGSIZE // # of avliable page



void pmm_init();
void page_init();
void* kalloc();
void kfree(void *pgaddr);
void pmm_test();