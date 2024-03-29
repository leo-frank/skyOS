/*
 * this is a temporary physical memory allocator.
 * very simple but low efficient and limit.
 * so this code will not be linked in kernel.
 */

#include "simple_allocator.h"

#include "log.h"
#include "pmm.h"
#include "strings.h"

struct spinlock pages_lock;

// pages[i].allocated = 0, means i page is free.
// define in pmm.c
extern struct Page pages[npage];
extern char *pmm_start, *pmm_end;

void simple_allocator_init() {
  memset(pages, 0, npage);
  init_lock(&pages_lock);
}

struct Page *simple_allocate() {
  acquire(&pages_lock);
  for (uint i = 0; i < npage; i++) {
    if (pages[i].allocated == 0) {
      pages[i].allocated = 1;
      release(&pages_lock);
      return &pages[i];
    }
  }
  release(&pages_lock);
  panic("kalloc: No free page\n");
  return NULL;
}

void simple_free(struct Page *p) {
  acquire(&pages_lock);
  p->allocated = 0;
  release(&pages_lock);
}

void simple_test() {
  log_debug("\n");
  log_debug("allocate pg0:\n");
  struct Page *pg0 = simple_allocate();
  log_debug("pg0 addr = %lx\n", PGADDR(pg0 - pages));
  log_debug("pg0 index = %lx\n", pg0 - pages);
  log_debug("pg0 attribute = %d\n", pages[pg0 - pages].allocated);
  log_debug("\nallocate pg1:\n");
  struct Page *pg1 = simple_allocate();
  log_debug("pg1 addr = %lx\n", PGADDR(pg1 - pages));
  log_debug("pg1 attribute = %d\n", pages[pg1 - pages].allocated);
  simple_free(pg0);
  log_debug("\nafter free pg0:\n");
  log_debug("pg0 attribute = %d\n", pages[pg0 - pages].allocated);
  log_debug("pg1 attribute = %d\n", pages[pg1 - pages].allocated);
  simple_free(pg1);
  log_debug("\nafter free pg0:\n");
  log_debug("pg0 attribute = %d\n", pages[pg0 - pages].allocated);
  log_debug("pg1 attribute = %d\n", pages[pg1 - pages].allocated);
}