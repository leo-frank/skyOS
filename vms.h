#pragma once
#include "type.h"
// Sv39 page tables contain 2 9 page table entries(PTEs), eight bytes each.
typedef uint64 pte;
typedef pte* pg_table;

#define valid(PTE) (PTE & 0b1)
#define readable(PTE) (PTE & 0b10)
#define writable(PTE) (PTE & 0b100)
#define executable(PTE) (PTE & 0b1000)

#define align(a) ((a & 0xfff) == 0)

#define PTE_V 0b1
#define PTE_R 0b10
#define PTE_W 0b100
#define PTE_X 0b1000

// get PPN from PTE
#define pte2ppn(PTE) (((PTE) >> 10) & 0xffffffffff)  // 44
#define pte2ppn0(PTE) (pte2ppn(PTE) & 0x1ff)
#define pte2ppn1(PTE) ((pte2ppn(PTE) >> 9) & 0x1ff)
#define pte2ppn2(PTE) ((pte2ppn(PTE) >> 9) & 0x3ffffff)
#define pte2ppnx(PTE, x) (pte2ppn #x(PTE))
#define va2offset(va) ((va)&0xc)
// copy from Xv6
#define OFFSETSHIFT 12
#define PXMASK 0x1FF  // 9 bits
#define PXSHIFT(level) (OFFSETSHIFT + (9 * (level)))
#define PX(level, va) ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)
#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))
void vms_init();