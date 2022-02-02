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
struct task_struct {
  uint32 pid;
  uint32 fpid;  // father pid
  uint16 state;
  int exit;
  pg_table pgtable;  // user process pagetable
  uint64 sp;         // user process intial stack sp
  char kstack[KSTACKSIZE];
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