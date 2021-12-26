#include "type.h"
void pmm_init();
void page_init();
void* kalloc();
void kfree(void *pgaddr);
void pmm_test();