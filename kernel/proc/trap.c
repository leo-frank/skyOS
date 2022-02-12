#include "trap.h"

#include "isa.h"
#include "log.h"
#include "sbi.h"
#include "sched.h"
#include "strings.h"
#include "type.h"
// define in entry.S
void trap_entry();

extern char sscratch_stack[];

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

char *interrupt_code_description[] = {
    "Reserved", "Supervisor software interrupt",
    "Reserved", "Machine software interrupt",
    "Reserved", "Supervisor timer interrupt",
    "Reserved", "Machine timer interrupt",
    "Reserved", "Supervisor external interrupt",
    "Reserved", "Machine external interrupt",
    "Reserved", "Reserved",
    "Reserved", "Reserved"};

char *exception_code_description[] = {"Instruction address misaligned",
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

extern void do_timer(struct context *a0);
extern void trap_return(struct context *a0);

// typedef int (*fn_ptr)();
// fn_ptr sys_call_table[] = [sys_write];

extern void syscall(struct context *t);

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
  // printf("epc: %p\n", context->epc);
}
void trap_start(struct context *a0) {
  uint64 tval = stval_get();
  uint64 sepc = sepc_get();
  uint64 cause = scause_get();
  int is_int = (cause & (1l << 63l)) ? 1 : 0;
  int code = cause & 0xff;  // exception code bits 4-0 must be implemented
  if (code >= 16) {
    panic("unexpected trap code");
    return;
  }
  if (is_int) {
    log_debug("interrupt: %s", interrupt_code_description[code]);
    switch (code) {
      case S_TIMER_INT:
        log_debug("tval = 0x%lx", tval);
        log_debug("sepc = 0x%lx", sepc);
        do_timer(a0);
        break;
      default:
        break;
    }
  } else {
    log_debug("exception: %s", exception_code_description[code]);
    switch (code) {
      case ILLEGAL_INSTRUCTION:
        log_info("tval = 0x%lx", tval);
        log_info("sepc = 0x%lx", sepc);
        dump_context(a0);
        panic("ILLEGAL_INSTRUCTION");
        break;
      case SYSCALL_FROM_U_MODE:
        log_debug("tval = 0x%lx", tval);
        log_debug("sepc = 0x%lx", sepc);
        syscall(a0);
        break;
      default:
        log_info("exception: %s", exception_code_description[code]);
        log_info("tval = 0x%lx", tval);
        log_info("sepc = 0x%lx", sepc);
        dump_context(a0);
        panic("UNEXPECTED EXCEPTION");
    }
    sepc_set(sepc + 4);
  }
  trap_return(a0);
  return;
}