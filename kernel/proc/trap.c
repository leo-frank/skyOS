#include "trap.h"

#include "isa.h"
#include "log.h"
#include "sbi.h"
#include "sched.h"
#include "strings.h"
#include "syscall_define.h"
#include "trap.h"
#include "type.h"

extern char sscratch_stack[];
extern struct task_struct *current_task;

// define in entry.S
extern void trap_entry();
extern void trap_return(struct context *a0);

void trap_init() {
  // stack that reserve trap context
  sscratch_set((unsigned long)(sscratch_stack));
  // set the supervisor trap handler
  stvec_set((unsigned long)trap_entry);
  // enable supervisor interrupts
  sstatus_set(sstatus_get() | SSTATUS_SIE);
  // enable supervisor timer, soft, external interrupts
  sie_set(sie_get() | SIE_STIE | SIE_SSIE | SIE_SEIE);
  sbi_set_timer(mtime_get() + TIMER_CLK_RATE);
}

static char *inter_infos[] = {"Reserved", "Supervisor software interrupt",
                              "Reserved", "Machine software interrupt",
                              "Reserved", "Supervisor timer interrupt",
                              "Reserved", "Machine timer interrupt",
                              "Reserved", "Supervisor external interrupt",
                              "Reserved", "Machine external interrupt",
                              "Reserved", "Reserved",
                              "Reserved", "Reserved"};

static char *exception_infos[] = {"Instruction address misaligned",
                                  "Instruction access fault",
                                  "Illegal instruction",
                                  "Breakpoint",
                                  "Load address misaligned",
                                  "Load access fault",
                                  "Store/AMO address misaligned",
                                  "Store/AMO access fault",
                                  "Environment call from U-mode",
                                  "Environment call from S-mode",
                                  "Reserved",
                                  "Environment call from M-mode",
                                  "Instruction page fault",
                                  "Load page fault",
                                  "Reserved",
                                  "Store/AMO page fault"};

void dump_context(struct context *context) {
  printf("a0: %p\t", context->a0);
  printf("a1: %p\n", context->a1);
  printf("a2: %p\t", context->a2);
  printf("a3: %p\n", context->a3);
  printf("a4: %p\t", context->a4);
  printf("a5: %p\n", context->a5);
  printf("a6: %p\t", context->a6);
  printf("a7: %p\n", context->a7);
  printf("t0: %p\t", context->t0);
  printf("t1: %p\n", context->t1);
  printf("t2: %p\t", context->t2);
  printf("t3: %p\n", context->t3);
  printf("t4: %p\t", context->t4);
  printf("t5: %p\n", context->t5);
  printf("t6: %p\t", context->t6);
  printf("s0: %p\n", context->s0);
  printf("s1: %p\t", context->s1);
  printf("s2: %p\n", context->s2);
  printf("s3: %p\t", context->s3);
  printf("s4: %p\n", context->s4);
  printf("s5: %p\t", context->s5);
  printf("s6: %p\n", context->s6);
  printf("s7: %p\t", context->s7);
  printf("s8: %p\n", context->s8);
  printf("s9: %p\t", context->s9);
  printf("s10: %p\n", context->s10);
  printf("s11: %p\t", context->s11);
  printf("ra: %p\n", context->ra);
  printf("sp: %p\t", context->sp);
  printf("gp: %p\n", context->gp);
  printf("tp: %p\t", context->tp);
  printf("epc: %p\n", context->sepc);
}

static void handle_interrupt(uint64 const code) {
  if (code == S_TIMER_INT) {
    do_timer();
  } else {
    panic("unexpected interrupt code: %s", inter_infos[code]);
  }
}

static void handle_exception(uint64 const code) {
  if (code == SYSCALL_FROM_U_MODE) {
    syscall();
  } else {
    dump_context(&(current_task->context));
    panic("unexpected exception code: %s", exception_infos[code]);
  }
}

void trap_start(struct context *sscratch_stack_ptr) {
  uint64 scause, code, inter;
  scause = scause_get();
  code = scause & 0xff;
  inter = (scause & (1l << 63l)) ? 1 : 0;
  if (inter) {
    memcpy(&(current_task->context), sscratch_stack_ptr,
           sizeof(struct context));
    handle_interrupt(code);
    log_error("should never go here");
  } else {
    memcpy(&(current_task->context), sscratch_stack_ptr,
           sizeof(struct context));
    // since fork may be invoked, which results in endless fork,
    // so add sepc before handle_exception
    (current_task->context).sepc += 4;  // must before handle_exception
    handle_exception(code);
    trap_return_2();
  }
}

// wrapper of trap_return
void trap_return_2() {
  memcpy(sscratch_stack, &(current_task->context), sizeof(struct context));
  trap_return((struct context *)sscratch_stack);
}