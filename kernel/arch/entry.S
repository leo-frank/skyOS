    .section .text
    .globl _entry
_entry:
    add t0, a0, 1
    slli t0, t0, 14
    la sp, boot_stack
    add sp, sp, t0
    call main

loop:
    j loop

    .globl trap_entry
    .align 4
trap_entry:
    # sscratch holds smode trap stack address,
    # swap current sp and smode trap stack address
    csrrw sp, sscratch, sp
    // call the C trap handler in trap.c
    call trap_start
    # restore old sp value
    csrrw sp, sscratch, sp
    sret

    .section .bss.stack
    .align 12
    .globl boot_stack
boot_stack:
    .space 4096 * 4 * 2
    .globl boot_stack_top
boot_stack_top:
    .globl sscratch_stack
sscratch_stack:
    .space 4096
    .globl sscratch_stack_top
sscratch_stack_top:
