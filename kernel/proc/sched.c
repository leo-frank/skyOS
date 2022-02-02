#include "sched.h"

#include "assert.h"
#include "elf.h"
#include "fcntl.h"
#include "isa.h"
#include "log.h"
#include "openfat.h"
#include "panic.h"
#include "pmm.h"
#include "printf.h"
#include "sbi.h"
#include "strings.h"
#include "trap.h"
#include "vms.h"

uint64 volatile jiffies = 0;
struct task_struct task[NR_TASK];
int pidmap[NR_TASK];

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

extern FatVol vol;

uint32 alloc_pid() {
  for (uint32 i = 0; i < NR_TASK; i++) {
    if (pidmap[i] == 0) {
      log_debug("[proc] allocate pid %d", i);
      return i;
    }
  }
  panic("no free pid");
  return 0;  // won't go here
}

struct task_struct *alloc_process() {
  int pid = alloc_pid();
  struct task_struct *p = &task[pid];
  p->pid = pid;
  p->state = TASK_RUNNING;
  p->pgtable = alloc_pgtable();
  p->sp = SV39_USER_SP_ADDR;
  return p;
}

void proctest() {
  FatFile file;
  assert(fat_open(&vol, "idle", O_RDONLY, &file) == 0);

  /* allocate new process struct for elf */
  struct task_struct *p = alloc_process();

  /* allocate space for placing elf binary code */
  char *elfcontent = kalloc(file.size);
  log_debug("file.size = %d", file.size);
  assert(fat_read(&file, elfcontent, file.size) == file.size);

  struct elfhdr *header;
  header = (struct elfhdr *)elfcontent;

  if (header->magic != ELF_MAGIC) {
    panic("not ELF format binary code");
  }
  log_debug("[proc] ELF entry: %lx", header->entry);

  /* map PT_LOAD sections */
  struct proghdr *ph;
  uint32 off = 0;

  /* BAD design */
  uint64 SEPC_OFFSET;

  for (uint32 i = 0; i < header->phnum; i++) {
    // offset of current program header
    off = header->phoff + i * sizeof(struct proghdr);
    ph = (struct proghdr *)((char *)elfcontent + off);
    if (ph->type != ELF_PROG_LOAD) continue;
    // ph->off points to offset of content of that header
    // FIXME: you have to ensure that physical address of PT_LOAD section
    // is aligned to PGSIZE, to make sure that vaddr trans correctly without
    // loss of OFFSET. When vaddr is aligned while paddr is not, that happens.

    // why allocate one more time? for map convenience
    // because filesystem not good enough
    char *tmp = kalloc(ph->memsz);
    if (!align(tmp)) panic("...");

    memcpy(tmp, (char *)elfcontent + ph->off, ph->memsz);
    umap(p->pgtable, ph->vaddr, (uint64)(tmp), ph->memsz);
    if (ph->vaddr <= header->entry && header->entry < ph->vaddr + ph->memsz) {
      // should be 0
      SEPC_OFFSET = OFFSET((uint64)(tmp));
    }
    log_debug("elfcontent addr: %lx", (uint64)elfcontent);
    log_debug("tmp addr: %lx", (uint64)tmp);
    log_debug("map %lx to %lx", ph->vaddr, (uint64)(tmp));
    log_debug("ph->off: %lx", ph->off);
  }
  log_debug("map PT_LOAD sections end");

  /* set seperate stack for user process */
  // if we don't set user stack but use same kernel stack
  // then Store/AMO page fault will be triggered.
  uint64 stack;
  // allocate 4 pages for user stack
  stack = (uint64)kalloc(PGSIZE * 4);
  // expect stack been page aligned for map convenience
  if (!align(stack)) {
    panic("stack paddr not aligned");
  }
  umap(p->pgtable, (p->sp) - 4 * PGSIZE, stack - 4 * PGSIZE, PGSIZE * 4);

  // set status.spp = 0, then return to user mode
  sstatus_set(sstatus_get() & ~SSTATUS_SPP);
  // set sepc
  sepc_set((uint64)(header->entry) + SEPC_OFFSET);
  log_debug("return to address 0x%lx", (uint64)(header->entry) + SEPC_OFFSET);
  // set satp.PPN with userspace pagetable
  satp_set((SV39_ADDRESSING_MODE << 60) + ((uint64)(p->pgtable) >> 12));
  // set stack pointer
  asm volatile("mv sp, %0" : : "r"(p->sp));
  // sret
  sreturn();
}

// TODO: syscall!
// int sys_write(unsigned int fd, char *buf, int count) {
//   if (fd == 1) {
//     log_info("%s", buf);
//   }
// }
// typedef int (*fn_ptr)();
// fn_ptr sys_call_table[] = [sys_write];

// void syscall() {
//   // ld	a3,-128(s0)
//   uint64 syscall_num;
//   asm volatile("sd a7 %0" :) asm volatile("csrr %0, time" : "=r"(x));
// }

// TODO: schedule!
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
