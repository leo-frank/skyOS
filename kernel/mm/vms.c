// Sv39: Page-Based 39-bit Virtual-Memory System
#include "vms.h"

#include "log.h"
#include "pmm.h"
#include "sched.h"
#include "type.h"

extern char kernel_start[];
extern char kernel_end[];
pg_table k_pgtable;

void flush_tlb() { asm volatile("sfence.vma"); }

static void activate_sv39(uint64 ppn) {
  uint64 v = 0;
  v = (((SV39_ADDRESSING_MODE) << 60) + ppn);
  asm volatile("csrw satp, %[val]" : : [val] "r"(v));
  flush_tlb();
}

static uint64 r_satp() {
  uint64 r;
  asm volatile("csrr %[r], satp" : [r] "=r"(r));
  return r;
}

// FIXME:
// 1. perm
// 2. lock the pg when we modify it
void map(pg_table pg, uint64 va, uint64 pa, uint64 size, uint perm) {
  int level;
  uint64 va0, pa0;
  pte *p, *a;

  // FIXME: I am not sure behaviour when va & size not aligned

  va0 = va;
  pa0 = pa;
  while (va0 < va + size) {
    log_debug("mapping va: %x to pa: %x", va0, pa0);
    level = 2;
    a = pg;
    while (level >= 0) {
      p = a + PX(level, va0);
      if (valid(*p) && level == 0) {
        // double map means pte has already been writeen with effect value
        panic("double map");
      } else if (valid(*p)) {
        a = (pte*)(pte2ppn(*p) << 12);
      } else if (level != 0) {
        a = kalloc(PGSIZE);
        log_debug("alloc new page table at: %p", a);
        // when all are zero, the PTE is a pointer to next level of page table
        *p = ((((uint64)a) >> 12) << 10) | (PTE_V | perm);
        log_debug("create new pte: %lx", *p);
      } else if (level == 0) {
        *p = ((((uint64)pa0) >> 12) << 10) |
             (PTE_X | PTE_W | PTE_R | PTE_V | perm);
      }
      level--;
    }
    va0 += PGSIZE;
    pa0 += PGSIZE;
  }
}

pte* trans(pg_table pg, uint64 va) {
  pte* p;
  int level = 2;
  pg_table a = pg;
  while (level >= 0) {
    p = a + PX(level, va);
    if (!valid(*p) || (!readable(*p) && writable(*p))) {
      log_debug("find pte: %ld", *p);
      panic("access fault");
      return NULL;
    }
    // now pte is vaild
    if (level == 0) {
      log_debug("end of trans");
      break;
    } else {
      level--;
      log_debug("find pte: %lx", *p);
      a = (pte*)(pte2ppn(*p) << 12);
      log_debug("find page: %p", a);
    }
  }
  uint64 pa;
  log_debug("find pte: %lx", *p);
  pa = ((pte2ppn(*p) << 12) + OFFSET(va));
  log_debug("va: 0x%lx -----> pa: 0x%lx", va, pa);
  return p;
}

static void vms_test() {
  int i = 5;
  int* p = &i;
  char* tmp = (char*)k_pgtable;
  log_debug("vms check kernel map: tmp = %lx, success", tmp);
  log_debug("vms check kernel map: i = %d, success", *p);
  log_debug("vms check done!");
}

static uint64 k_pgtable_init() {
  uint64 k_ppn;

  k_pgtable = kalloc(PGSIZE);
  if (!align(k_pgtable)) {
    panic("allocate a unaligned page table");
  }
  k_ppn = (uint64)k_pgtable >> 12;
  kmap(k_pgtable, (uint64)kernel_start, (uint64)kernel_start,
       (uint64)((uint64)(kernel_end + mem_size) - (uint64)kernel_start));
  return k_ppn;
}

void vms_init() {
  log_debug("read satp = %d", r_satp());
  uint64 k_ppn;

  k_ppn = k_pgtable_init();
  log_info("kernel page table address: 0x%lx", k_pgtable);

  activate_sv39(k_ppn);

  vms_test();
}
