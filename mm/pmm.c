#include "pmm.h"

#include "lock.h"
#include "log.h"
#include "panic.h"
#include "strings.h"
#include "type.h"

// TODO: explain char[] matters, char * not.
extern char kernel_start[];
extern char kernel_end[];   // end of .bss
char *pmm_start, *pmm_end;  // free memory region

struct Page pages[npage];

void pmm_init() {
  pmm_start = kernel_end;
  pmm_end = pmm_start + mem_size;
  log_info("kernel mem: [0x%x, 0x%x]", kernel_start, kernel_end);
  log_info("avliable mem: [0x%x, 0x%x]", pmm_start, pmm_end);

  buddy_init();
}

void* kalloc(uint64 size) {
  struct Page* p;
  if (size >= PGSIZE) {
    p = buddy_allocate(size);
  } else {
    p = buddy_allocate(PGSIZE);
  }
  return (void*)PGADDR(PGINDEX(p));
}

/* free continous pages */
void kfree(struct Page* p) { buddy_free_page(p); }
