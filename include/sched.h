#include "type.h"

#define NR_TASK 64

// task state info
#define TASK_RUNNING \
  0  // running in userspace, running in kernel, waiting for running
#define TASK_SLEEPING 1  // sleeping for resource
#define TASK_STOPPED 2   // when receive SIGSTOP, temporary stopped
#define TASK_ZOMBIE 4

#define KSTACKSIZE (1 << 12)
typedef struct task_struct {
  uint pid;
  uint fpid;  // father pid
  uint16 state;
  int exit;
  char stack[KSTACKSIZE];
  /* time related info */
  uint64 counter;  // ticks remain for running
  uint64 priority;
  uint64 utime;  // ticks running in userspace
  uint64 stime;  // ticks running in kernel
  uint64 start_time;
  /* signal related info */
  uint signal;   // signal bit map
  uint blocked;  // blcoked signal bit
  // struct sigaction sigaction[32];
} task_struct;