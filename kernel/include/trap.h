#pragma once
void trap_init();
void trap_start();

#define TIMER_CLK_RATE (390000000 / 200)  // timer interrupt interval

#define SIE_SEIE \
  (1 << 9)  // interrupt-pending bit for supervisor-level external interrupts
#define SIP_SEIP \
  SIE_SEIE  // interrupt-enable bit for supervisor-level external interrupts
#define SIE_STIE \
  (1 << 5)  // interrupt-pending bit for supervisor-level timer interrupts
#define SIP_STIP \
  SIE_STIE  // interrupt-enable bit for supervisor-level timer interrupts
#define SIE_SSIE \
  (1 << 1)  // interrupt-pending bit for supervisor-level software interrupts
#define SIP_SSIP \
  SIE_STIE  // interrupt-enable bit for supervisor-level software interrupts

#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_SPP (1L << 8)

// Interrupt Exception Code
#define S_TIMER_INT 5
// non-Interrupt Exception Code
#define ILLEGAL_INSTRUCTION 2
#define SYSCALL_FROM_U_MODE 8