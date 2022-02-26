#pragma once

#include "type.h"
#include "vms.h"
#define NR_TASK 64

// task state info
#define TASK_RUNNING \
  0  // running in userspace, running in kernel, waiting for running
#define TASK_SLEEPING 1  // sleeping for resource
#define TASK_STOPPED 2   // when receive SIGSTOP, temporary stopped
#define TASK_ZOMBIE 4

#define KSTACKSIZE (1 << 12)

// TODO: fake counter, not a good role
#define TASK_INITIAL_COUNTER 256

struct context {
  /*  0 */ uint64 ra;
  /*  8 */ uint64 sp;
  /*  16 */ uint64 gp;
  /*  24 */ uint64 tp;
  /*  32 */ uint64 t0;
  /*  40 */ uint64 t1;
  /*  48 */ uint64 t2;
  /*  56 */ uint64 s0;
  /*  64 */ uint64 s1;
  /*  72 */ uint64 a0;
  /*  80 */ uint64 a1;
  /*  88 */ uint64 a2;
  /*  96 */ uint64 a3;
  /* 104 */ uint64 a4;
  /* 112 */ uint64 a5;
  /* 120 */ uint64 a6;
  /* 128 */ uint64 a7;
  /* 136 */ uint64 s2;
  /* 144 */ uint64 s3;
  /* 152 */ uint64 s4;
  /* 160 */ uint64 s5;
  /* 168 */ uint64 s6;
  /* 176 */ uint64 s7;
  /* 184 */ uint64 s8;
  /* 192 */ uint64 s9;
  /* 200 */ uint64 s10;
  /* 208 */ uint64 s11;
  /* 216 */ uint64 t3;
  /* 224 */ uint64 t4;
  /* 232 */ uint64 t5;
  /* 240 */ uint64 t6;
  /* 248 */ uint64 sepc;
};

#define MAX_VMA_NUM 5
struct vma {
  uint64 mem_start;
  uint64 mem_range;
  uint32 flags;
};
struct task_struct {
  char filename[12];
  uint32 pid;
  uint32 fpid;  // father pid
  uint16 state;
  int exit;
  char kstack[KSTACKSIZE];
  struct context context;
  /* memory map */
  pg_table pgtable;  // user process pagetable
  struct vma mem_map[MAX_VMA_NUM];
  /* time related info */
  uint64 counter;  // ticks remain for running
  uint64 priority;
  uint64 utime;  // ticks running in userspace
  uint64 stime;  // ticks running in kernel
  uint64 start_time;
  /* signal related info */
  uint32 signal;   // signal bit map
  uint32 blocked;  // blcoked signal bit
  // struct sigaction sigaction[32];
};

void proctest(char *);
void show_process_virtual_mem_map(struct task_struct *p);
uint32 get_process_count();
void kmap(pg_table pg, uint64 va, uint64 pa, uint64 size);
void umap(struct task_struct *pg, uint64 va, uint64 pa, uint64 size);
struct task_struct *alloc_process();
void copy_mem_from(struct task_struct *p, struct task_struct *current);
void schedule();