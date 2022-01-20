#include "sched.h"

#include "isa.h"
#include "log.h"
#include "panic.h"
#include "pmm.h"
#include "sbi.h"
#include "strings.h"
#include "trap.h"
#include "vms.h"

uint64 volatile jiffies = 0;
task_struct *task[NR_TASK];

extern pg_table k_pgtable;
extern void sreturn();
extern void flush_tlb();

// allocate user pagetable
pg_table alloc_pgtable() {
  pg_table t = kalloc(PGSIZE);
  if (!align(t)) {
    panic("allocate a unaligned page table");
  }
  memset(t, 0, PGSIZE);
  // map: [kernel_start, pmm_end] -> [kernel_start, pmm_end] (without PTE_U)
  // useful when privilege changed from u to s
  memcpy(t, k_pgtable, PGSIZE);
  log_debug("flush_tlb addr:%lx", flush_tlb);
  return t;
}

// until now, this is a simple test userspace handler
void move_to_user_mode() {
  char *idle;
  pg_table user_pgt;

  // set status.spp = 0, then return to user mode
  sstatus_set(sstatus_get() & ~SSTATUS_SPP);
  // set sepc = 0, then return to userspace addr 0
  sepc_set((uint64)0);

  user_pgt = alloc_pgtable();

  // primary idle userspace process, do while()
  idle = kalloc(PGSIZE);
  idle[0] = 0x01, idle[1] = 0xa0;
  // map: [0, PGSIZE] -> [idle, idle + PGSIZE]
  umap(user_pgt, (uint64)0, (uint64)idle, PGSIZE);

  // set satp.PPN with userspace pagetable
  satp_set((SV39_ADDRESSING_MODE << 60) + ((uint64)(user_pgt) >> 12));

  // FIXME: flush_tlb();

  sreturn();
}

void do_timer() {
  uint64 status = sstatus_get();
  char status_spp = (status & SSTATUS_SPP) >> 8;
  if (status_spp == 0) {
    // trap originated from user mode
    log_debug("timer interrupt: trap originated from user mode");
  } else if (status_spp == 1) {
    // trap originated from supervisor mode
    log_debug("timer interrupt: trap originated from supervisor mode");
  } else {
    panic("unexpected status.spp");
  }
  jiffies++;
  sbi_set_timer(mtime_get() + TIMER_CLK_RATE);
}

// void schedule() {
//   uint i, next;
//   uint64 maxc = 0;  // find max counter
//   task_struct *p;
// start:
//   // find task with max counter
//   for (i = 0; i < NR_TASK; i++) {
//     p = task[i];
//     if (p) {
//       if (p->state == TASK_RUNNING && p->counter > maxc) {
//         maxc = p->counter, next = i;
//       }
//     } else {
//       continue;
//     }
//   }
//   if (maxc != 0) {
//     switch_to(next);
//   } else {
//     for (i = 0; i < NR_TASK; i++) {
//       p = task[i];
//       if (p) {
//         p->counter = p->priority;
//       }
//     }
//     goto start;
//   }
// }
