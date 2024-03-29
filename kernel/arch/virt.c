#include "type.h"

enum {
  VIRT_DEBUG,
  VIRT_MROM,
  VIRT_TEST,
  VIRT_CLINT,
  VIRT_PLIC,
  VIRT_UART0,
  VIRT_VIRTIO,
  VIRT_DRAM,
  VIRT_PCIE_MMIO,
  VIRT_PCIE_PIO,
  VIRT_PCIE_ECAM
};
#define VIRT_PLIC_ENABLE_BASE 0x2000
#define VIRT_PLIC_ENABLE_STRIDE 0x80
#define VIRT_PLIC_CONTEXT_BASE 0x200000
#define VIRT_PLIC_CONTEXT_STRIDE 0x1000
static const struct MemmapEntry {
  uint64 base;
  uint64 size;
} virt_memmap[] = {
    [VIRT_DEBUG] = {0x0, 0x100},
    [VIRT_MROM] = {0x1000, 0x11000},
    [VIRT_TEST] = {0x100000, 0x1000},
    [VIRT_CLINT] = {0x2000000, 0x10000},
    [VIRT_PLIC] = {0xc000000, 0x4000000},
    [VIRT_UART0] = {0x10000000, 0x100},
    [VIRT_VIRTIO] = {0x10001000, 0x1000},
    [VIRT_DRAM] = {0x80000000, 0x0},
    [VIRT_PCIE_MMIO] = {0x40000000, 0x40000000},
    [VIRT_PCIE_PIO] = {0x03000000, 0x00010000},
    [VIRT_PCIE_ECAM] = {0x30000000, 0x10000000},
};
