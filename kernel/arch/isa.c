#include "isa.h"

uint64 sie_get() {
  uint64 sie;
  asm volatile("csrr %[r], sie" : [r] "=r"(sie));
  return sie;
}

void sie_set(uint64 v) { asm volatile("csrw sie, %0" : : "r"(v)); }

uint64 sip_get() {
  uint64 sip;
  asm volatile("csrr %[r], sip" : [r] "=r"(sip));
  return sip;
}

void sip_set(uint64 v) { asm volatile("csrw sip, %0" : : "r"(v)); }

uint64 sstatus_get() {
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r"(x));
  return x;
}

void sstatus_set(uint64 v) { asm volatile("csrw sstatus, %0" : : "r"(v)); }

uint64 stvec_get() {
  uint64 x;
  asm volatile("csrr %0, stvec" : "=r"(x));
  return x;
}

void stvec_set(uint64 v) { asm volatile("csrw stvec, %0" : : "r"(v)); }

uint64 scause_get() {
  uint64 x;
  asm volatile("csrr %0, scause" : "=r"(x));
  return x;
}

uint64 sepc_get() {
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r"(x));
  return x;
}

void sepc_set(uint64 v) { asm volatile("csrw sepc, %0" : : "r"(v)); }

void sscratch_set(uint64 v) { asm volatile("csrw sscratch, %0" : : "r"(v)); }

uint64 sscratch_get() {
  uint64 x;
  asm volatile("csrr %0, sscratch" : "=r"(x));
  return x;
}

uint64 stval_get() {
  uint64 x;
  asm volatile("csrr %0, stval" : "=r"(x));
  return x;
}

unsigned long mtime_get() {
  uint64 x;
  asm volatile("csrr %0, time" : "=r"(x));
  return x;
}

uint64 satp_get() {
  uint64 x;
  asm volatile("csrr %0, satp" : "=r"(x));
  return x;
}

void satp_set(uint64 v) { asm volatile("csrw satp, %0" : : "r"(v)); }

// disable device interrupts
void intr_off() { sstatus_set(sstatus_get() & ~SSTATUS_SIE); }

// enable device interrupts
void intr_on() { sstatus_set(sstatus_get() | SSTATUS_SIE); }

void flush_tlb() { asm volatile("sfence.vma"); }