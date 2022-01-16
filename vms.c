// Sv39: Page-Based 39-bit Virtual-Memory System
#include "vms.h"

#include "log.h"
#include "panic.h"
#include "pmm.h"
#include "type.h"

#define SV39_ADDRESSING_MODE (8L)

extern char kernel_start[];
extern char kernel_end[];  // end of .bss

pg_table pg_dir;  // root pgdir

// flush the TLB.
static inline void sfence_vma() {
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

static void Sv39_active(uint64 ppn) {
  uint64 v = 0;
  v = (((SV39_ADDRESSING_MODE) << 60) + ppn);
  asm volatile("csrw satp, %[val]" : : [val] "r"(v));
  sfence_vma();
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
  if (!align(va) || !align(pa) || !align(size)) {
    panic("not aligned");
  }
  va0 = va;
  pa0 = pa;
  while (va0 < va + size) {
    log_debug("mapping va: %x to pa: %x", va0, pa0);
    level = 2;
    a = pg;
    while (level >= 0) {
      p = a + PX(level, va0);
      if (valid(*p) && level == 0) {
        panic("double map");
      } else if (valid(*p)) {
        a = (pte*)(pte2ppn(*p) << 12);
      } else if (level != 0) {
        a = kalloc(PGSIZE);
        log_debug("alloc new page table at: %p", a);
        // when all are zero, the PTE is a pointer to next level of page table
        *p = ((((uint64)a) >> 12) << 10) | (PTE_V);
        log_debug("create new pte: %lx", *p);
      } else if (level == 0) {
        *p = ((((uint64)pa0) >> 12) << 10) | (PTE_X | PTE_W | PTE_R | PTE_V);
      }
      level--;
    }
    va0 += PGSIZE;
    pa0 += PGSIZE;
  }
}

#ifndef SV39_ADDRESSING_MODE
// emulation of hardware's memory transformation, now only for debug.
// should only be invoked before sv39 enabled. (FIXME: we will fix this later)
// 39-bit VPN is translated into a 56-bit PPN via a 3-level page table
// return 0 if no valid pte for va
// return pa if already map va
static void trans(pg_table pg, uint64 va) {
  pte* p;
  int level = 2;
  pg_table a = pg;
  while (level >= 0) {
    p = a + PX(level, va);
    if (!valid(*p) || (!readable(*p) && writable(*p))) {
      log_debug("find pte: %ld", *p);
      panic("access fault");
      return;
    }
    // now pte is vaild
    if (level == 0) {
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
  pa = ((pte2ppn(*p) << 12) + va2offset(va));
  log_debug("va: 0x%lx -----> pa: 0x%lx", va, pa);
  return;
}
#endif

static void vms_test() {
  int i = 5;
  int* p = &i;
  char* tmp = (char*)pg_dir;
  log_debug("vms check kernel map: tmp = %lx, success", tmp);
  log_debug("vms check kernel map: i = %d, success", *p);
  log_debug("vms check done!");
}
void vms_init() {
  log_debug("read satp = %d", r_satp());
  uint64 PPN;
  // FIXME:
  // pg_dir must be aligned to PG boudary.
  pg_dir = kalloc(PGSIZE);
  PPN = ((uint64)pg_dir >> 12);
  map(pg_dir, (uint64)kernel_start, (uint64)kernel_start,
      (uint64)(kernel_end - kernel_start), 0);
  log_info("kernel page table address: 0x%lx", pg_dir);
  Sv39_active(PPN);
  vms_test();
}
