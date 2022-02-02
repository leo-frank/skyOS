#include "trap.h"

#include "isa.h"
#include "log.h"
#include "panic.h"
#include "sbi.h"
#include "type.h"

// define in entry.S
void trap_entry();

extern char sscratch_stack[];

struct trapframe {
  /* NOT USED */
  /*   0 */ uint64 kernel_satp;  // kernel page table
  /*   8 */ uint64 kernel_sp;    // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;  // usertrap() 切换到内核后我们会跳到这里执行。
  /*  24 */ uint64 epc;            // saved user program counter
  /*  32 */ uint64 kernel_hartid;  // saved kernel tp

  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

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

extern void do_timer();
extern void syscall();
extern void trap_return(struct trapframe *a0);

void trap_start(struct trapframe *a0) {
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
        log_debug("tval = 0x%lx", tval);
        log_debug("sepc = 0x%lx", sepc);
        panic("ILLEGAL_INSTRUCTION");
        break;
      case SYSCALL_FROM_U_MODE:
        // syscall();
        break;
      default:
        log_debug("tval = 0x%lx", tval);
        log_debug("sepc = 0x%lx", sepc);
        panic("UNEXPECTED EXCEPTION");
    }
    sepc_set(sepc + 4);
  }
  trap_return(a0);
  return;
}