#include "trap.h"

#include "isa.h"
#include "log.h"
#include "panic.h"
#include "sbi.h"
#include "type.h"

// define in entry.S
void trap_entry();

extern char sscratch_stack_top[];

void trap_init() {
  // stack that reserve trap context
  sscratch_set((unsigned long)(sscratch_stack_top));
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

extern void do_timer();

void trap_start() {
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
        do_timer();
        break;
      default:
        break;
    }
  } else {
    log_debug("exception: %s", exception_code_description[code]);
    switch (code) {
      case ILLEGAL_INSTRUCTION:
        log_debug("tval = %p", tval);
        log_debug("sepc = %p", sepc);
        break;
    }
    sepc_set(sepc + 4);
  }
  return;
}