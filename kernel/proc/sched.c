#include "sched.h"

#include "elf.h"
#include "fcntl.h"
#include "isa.h"
#include "log.h"
#include "openfat.h"
#include "pmm.h"
#include "printf.h"
#include "sbi.h"
#include "strings.h"
#include "trap.h"
#include "vms.h"

uint64 volatile jiffies = 0;
struct task_struct task[NR_TASK];
struct task_struct *current_task;

// pidmap[i] = 1, means process i is live
int pidmap[NR_TASK];

extern pg_table k_pgtable;
extern void sreturn();
extern void flush_tlb();
extern void end();
extern void trap_return(struct context *a0);

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
extern char sscratch_stack[];
void do_timer() {
  uint64 status = sstatus_get();
  char status_spp = (status & SSTATUS_SPP) >> 8;
  if (status_spp == 0) {
    log_debug("timer interrupt originated from u-mode");
  } else if (status_spp == 1) {
    log_debug("timer interrupt originated from s-mode");
  }
  jiffies++;
  current_task->counter--;
  // show_process_virtual_mem_map(current_task);
  sbi_set_timer(mtime_get() + TIMER_CLK_RATE);
  memcpy(sscratch_stack, &(current_task->context), sizeof(struct context));
  trap_return((struct context *)sscratch_stack);
  // if (status_spp == 0) {
  //   // we only switch if timer interrupt from u-mode
  //   // save context
  //   memcpy(&(current_task->context), a0, sizeof(current_task->context));
  //   // then schedule
  //   schedule();
  // }
}

extern FatVol vol;

uint32 alloc_pid() {
  for (uint32 i = 0; i < NR_TASK; i++) {
    if (pidmap[i] == 0) {
      pidmap[i] = 1;
      log_debug("[proc] allocate pid %d", i);
      return i;
    }
  }
  panic("no free pid");
  return 0;  // won't go here
}

uint32 get_process_count() {
  uint32 cnt = 0;
  for (uint32 i = 0; i < NR_TASK; i++) {
    if (pidmap[i] != 0) {
      cnt++;
    }
  }
  return cnt;
}

void show_process_virtual_mem_map(struct task_struct *p) {
  uint32 i = 0;
  printf("pid: %u, name: %s\n", p->pid, p->filename);
  for (i = 0; i < MAX_VMA_NUM; i++) {
    if (p->mem_map[i].mem_range != 0) {
      printf("%u: [0x%lx, 0x%lx]\n", i, p->mem_map[i].mem_start,
             p->mem_map[i].mem_start + p->mem_map[i].mem_range);
    }
  }
  printf("\n");
}

struct task_struct *alloc_process() {
  int pid = alloc_pid();
  struct task_struct *p = &task[pid];
  p->pid = pid;
  p->state = TASK_RUNNING;
  p->pgtable = alloc_pgtable();
  p->counter = TASK_INITIAL_COUNTER;
  (p->context).sp = SV39_USER_SP_ADDR;
  return p;
}

// switch to another process's userspace, including the pagetable
void switch_to(struct task_struct *next) {
  // set status.spp = 0, then return to user mode
  sstatus_set(sstatus_get() & ~SSTATUS_SPP);
  // set satp.PPN with userspace pagetable
  satp_set((SV39_ADDRESSING_MODE << 60) + ((uint64)(next->pgtable) >> 12));
  // sfence.vma
  flush_tlb();
  memcpy(sscratch_stack, &(current_task->context), sizeof(struct context));
  trap_return((struct context *)sscratch_stack);
}

void schedule() {
  uint i;
  // default next task is current_task, that means no task switch happens.
  struct task_struct *p, *next = current_task;
  // find task with max counter
  for (i = 0; i < NR_TASK; i++) {
    p = &task[i];
    if (p->pgtable != NULL && p->state == TASK_RUNNING && p != current_task &&
        p->counter > next->counter) {
      next = p;
      break;
    }
  }
  if (next) {
    current_task = next;
    switch_to(next);
  }
  return;
}

// copy current's physical memory and map on p
void copy_mem_from(struct task_struct *p, struct task_struct *current) {
  uint32 i;
  sstatus_set(sstatus_get() | SSTATUS_SUM);
  for (i = 0; i < MAX_VMA_NUM; i++) {
    if (current->mem_map[i].mem_range != 0) {
      char *pa = kalloc(current->mem_map[i].mem_range);
      memcpy(pa, (char *)current->mem_map[i].mem_start,
             current->mem_map[i].mem_range);
      umap(p, current->mem_map[i].mem_start, (uint64)pa,
           current->mem_map[i].mem_range);
    }
  }
  sstatus_set(sstatus_get() & ~SSTATUS_SUM);
}

void execve(char *filename) {
  FatFile file;
  assert(fat_open(&vol, filename, O_RDONLY, &file) == 0);

  /* allocate new process struct for elf */
  struct task_struct *p = alloc_process();
  memcpy(p->filename, filename, sizeof(filename));

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
    umap(p, ph->vaddr, (uint64)(tmp), ph->memsz);
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
  umap(p, (p->context).sp - 4 * PGSIZE, stack - 4 * PGSIZE, PGSIZE * 4);

  /* set process context for later switch */
  p->context.sepc = (uint64)(header->entry) + SEPC_OFFSET;

  current_task = p;

  show_process_virtual_mem_map(p);

  switch_to(p);
}

void vma_insert(struct task_struct *p, uint64 va, uint64 size, uint32 flag) {
  uint32 i;
  for (i = 0; i < MAX_VMA_NUM; i++) {
    if (p->mem_map[i].mem_range == 0) {
      p->mem_map[i].mem_start = va;
      p->mem_map[i].mem_range = size;
      p->mem_map[i].flags = flag;
      return;
    }
  }
}

void kmap(pg_table pg, uint64 va, uint64 pa, uint64 size) {
  map(pg, va, pa, size, 0);
}

void umap(struct task_struct *p, uint64 va, uint64 pa, uint64 size) {
  vma_insert(p, va, size, PTE_U);
  map(p->pgtable, va, pa, size, PTE_U);
}