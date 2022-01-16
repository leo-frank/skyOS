# skyOS

## dates

2021.12.25: I am starting to develop my own operating system in C/C++, "skyOS", let's go !

2021.12.25: Boot skyOS successfully and print logo!

2022.01.01: Happy new year! Implement buddy allocator.

## todo list

- [x] buddy memory allocator

- [ ] SMP support


## toolchain

[ubuntu prebuilt risc-v toolchain](https://static.dev.sifive.com/dev-tools/riscv64-unknown-elf-gcc-8.3.0-2020.04.1-x86_64-linux-ubuntu14.tar.gz)

## notable

A page table must always be aligned to a page boundary.

The physical page number of the root page table is stored in the satp register’s PPN field.

Sv39 page tables contain 2^9 page table entries (PTEs), 2^3 bytes each.
