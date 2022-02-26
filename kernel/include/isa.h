#ifndef ISA_H
#define ISA_H

#include "type.h"

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

enum sstatus_bits_enum {
  SSTATUS_SIE = (1L << 1),  // Supervisor Interrupt Enable
  SSTATUS_SPP = (1L << 8),
  SSTATUS_SUM = (1L << 18),
};

uint64 sie_get();
void sie_set(uint64 v);
uint64 sip_get();
void sip_set(uint64 v);
uint64 sstatus_get();
void sstatus_set(uint64 v);
uint64 stvec_get();
void stvec_set(uint64 v);
uint64 scause_get();
uint64 sepc_get();
void sepc_set(uint64 v);
void sscratch_set(uint64 v);
uint64 sscratch_get();
uint64 stval_get();
unsigned long mtime_get();
uint64 satp_get();
void satp_set(uint64 v);
void intr_off();
void intr_on();
void flush_tlb();

#endif /* ISA_H */