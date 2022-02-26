#ifndef _TRAP_H_
#define _TRAP_H_

#include "sched.h"

void trap_init();
void trap_start();

#define TIMER_CLK_RATE (390000000 / 1000)  // timer interrupt interval

enum interrupt_types_enum {
  S_TIMER_INT = 5,
};

enum exception_types_enum {
  ILLEGAL_INSTRUCTION = 5,
  SYSCALL_FROM_U_MODE = 8,
};

void trap_init();
void trap_return_2();
void dump_context(struct context *context);

#endif /* _TRAP_H_ */