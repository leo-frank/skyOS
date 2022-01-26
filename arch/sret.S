.section .text

.globl sreturn
sreturn:
  sret

.globl flush_tlb
flush_tlb:
  sfence.vma zero, zero