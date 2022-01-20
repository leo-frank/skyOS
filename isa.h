#pragma once

#include "type.h"

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
static inline void sret() { asm volatile("sret" ::); }