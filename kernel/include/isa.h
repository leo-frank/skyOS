#pragma once

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

#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_SPP (1L << 8)
#define SSTATUS_SUM (1L << 18)

static inline uint64 sie_get() {
  uint64 sie;
  asm volatile("csrr %[r], sie" : [r] "=r"(sie));
  return sie;
}

static inline void sie_set(uint64 v) {
  asm volatile("csrw sie, %0" : : "r"(v));
}

static inline uint64 sip_get() {
  uint64 sip;
  asm volatile("csrr %[r], sip" : [r] "=r"(sip));
  return sip;
}

static inline void sip_set(uint64 v) {
  asm volatile("csrw sip, %0" : : "r"(v));
}

static inline uint64 sstatus_get() {
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r"(x));
  return x;
}

static inline void sstatus_set(uint64 v) {
  asm volatile("csrw sstatus, %0" : : "r"(v));
}

static inline uint64 stvec_get() {
  uint64 x;
  asm volatile("csrr %0, stvec" : "=r"(x));
  return x;
}

static inline void stvec_set(uint64 v) {
  asm volatile("csrw stvec, %0" : : "r"(v));
}

static inline uint64 scause_get() {
  uint64 x;
  asm volatile("csrr %0, scause" : "=r"(x));
  return x;
}

static inline uint64 sepc_get() {
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r"(x));
  return x;
}

static inline void sepc_set(uint64 v) {
  asm volatile("csrw sepc, %0" : : "r"(v));
}

static inline void sscratch_set(uint64 v) {
  asm volatile("csrw sscratch, %0" : : "r"(v));
}

static inline uint64 sscratch_get() {
  uint64 x;
  asm volatile("csrr %0, sscratch" : "=r"(x));
  return x;
}

static inline uint64 stval_get() {
  uint64 x;
  asm volatile("csrr %0, stval" : "=r"(x));
  return x;
}

static inline unsigned long mtime_get() {
  uint64 x;
  asm volatile("csrr %0, time" : "=r"(x));
  return x;
}

static inline uint64 satp_get() {
  uint64 x;
  asm volatile("csrr %0, satp" : "=r"(x));
  return x;
}

static inline void satp_set(uint64 v) {
  asm volatile("csrw satp, %0" : : "r"(v));
}

// disable device interrupts
static inline void intr_off() { sstatus_set(sstatus_get() & ~SSTATUS_SIE); }

// enable device interrupts
static inline void intr_on() { sstatus_set(sstatus_get() | SSTATUS_SIE); }