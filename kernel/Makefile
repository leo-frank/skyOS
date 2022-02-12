# ------------------------------------------------------------------------------
# Linker options
# ------------------------------------------------------------------------------
LDFLAGS 			 = -z max-page-size=4096
LDSCRIPT 			 = scripts/qemu-linker.lds

# ------------------------------------------------------------------------------
# SBI options TODO: diff between sbi.bin & sbi, and why they all works ?
# ------------------------------------------------------------------------------
RUSTSBI 			 = sbi/rustsbi-qemu.bin

# ------------------------------------------------------------------------------
# Paths
# ------------------------------------------------------------------------------
PATH_TOOLS_CC        = riscv64-unknown-elf-
PATH_TOOLS_UTIL      = 
BUILD 			     = ./build
DEBUGDIR			 = $(BUILD)/debug
KERNEL               = $(BUILD)/kernel

# ------------------------------------------------------------------------------
# Output directory
# ------------------------------------------------------------------------------
OUTDIR			  = $(BUILD)			\
					$(DEBUGDIR)
# ------------------------------------------------------------------------------
# Development tools
# ------------------------------------------------------------------------------
AR        = $(PATH_TOOLS_CC)ar
AS        = $(PATH_TOOLS_CC)gcc
CC        = $(PATH_TOOLS_CC)gcc
NM        = $(PATH_TOOLS_CC)nm
GDB	      = $(PATH_TOOLS_CC)gdb
GCOV      = $(PATH_TOOLS_CC)gcov
OBJDUMP   = $(PATH_TOOLS_CC)objdump
OBJCOPY   = $(PATH_TOOLS_CC)objcopy
READELF   = $(PATH_TOOLS_CC)readelf
SIZE      = $(PATH_TOOLS_CC)size
LD		  = $(PATH_TOOLS_CC)ld

ECHO      = $(PATH_TOOLS_UTIL)echo
MAKE      = $(PATH_TOOLS_UTIL)make
MKDIR     = $(PATH_TOOLS_UTIL)mkdir
RM        = $(PATH_TOOLS_UTIL)rm
SED       = $(PATH_TOOLS_UTIL)sed

# ------------------------------------------------------------------------------
# QEMU options	TODO: when smp 2, how printf works ?
# ------------------------------------------------------------------------------
QEMU = qemu-system-riscv64
QEMUOPT = -machine virt -m 64M -nographic -smp 1
QEMUOPT += -bios $(RUSTSBI)
QEMUOPT += -kernel $(KERNEL)
QEMUOPT += -initrd fat12.img
# ------------------------------------------------------------------------------
# Compiler flags for the target architecture
# ------------------------------------------------------------------------------

CFLAGS      	= 											  \
					-Wall                        	\
					-Werror												\
					-O0														\
					-fno-omit-frame-pointer				\
					-ggdb													\
					-g														\
					-MD														\
					-mcmodel=medany								\
					-ffreestanding								\
					-fno-common										\
					-nostdlib											\
					-mno-relax										\
					-c														\
					-Iinclude/

# ------------------------------------------------------------------------------
# Object files for linker
# ------------------------------------------------------------------------------
MAIN = main.o

KOBJS			=										\
					entry.o 						\
					sret.o							\
					logo.o 							\
					main.o 							\
					printf.o 						\
					console.o						\
					strings.o 					\
					lock.o 							\
					pmm.o  							\
					buddy.o 						\
					log.o 							\
					vms.o								\
					trap.o 							\
					sched.o 						\
					ramdisk.o						\
					block_dev.o					\
					syscall.o 					\

KFSOBJS = fat_core.o direntry.o mbr.o write.o fstest.o
# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------
run: oskernel binutils mkfs
	@$(QEMU) $(QEMUOPT)

# TODO: since we are using driver for ramdisk, we can't use img files too large.
# so now use fat12.img temporarily whose space is only 1MiB.
mkfs: fat12.img
	@sudo mount fat12.img mnt/
	@sudo cp ../user/idle ./mnt
	@sudo umount mnt

fat12.img:
	dd if=/dev/zero of=$@ bs=512 count=2048
	mkfs -t msdos $@

fat12.img:
	dd if=/dev/zero of=$@ bs=512 count=20480
	mkfs -t msdos $@

fat32.img:
	dd if=/dev/zero of=$@ bs=512 count=81920
	mkfs -t msdos -F 32 $@

oskernel: $(OUTDIR) $(LDSCRIPT) $(MAIN) mkfs
	@$(MAKE) -C arch
	@$(MAKE) -C common
	@$(MAKE) -C drivers
	@$(MAKE) -C fs
	@$(MAKE) -C mm
	@$(MAKE) -C proc
	@$(MAKE) -C sync
	@$(ECHO) ">>>" linking: $(notdir $(KERNEL))
	@$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $(KERNEL) $(addprefix $(BUILD)/, $(KOBJS)) $(addprefix $(BUILD)/, $(KFSOBJS))

# FIXME: use --start-group archives --end-group as a linker arg
# below failed because entry.o was not linked as first file
# @$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $(KERNEL) --start-group $(wildcard $(BUILD)/*.o) --end-group


# debug kernel with qemu's ability. need another terminal for run gdb as commented
debug: oskernel binutils
	@$(QEMU) $(QEMUOPT) -S -gdb tcp::1235
# @$(GDB) $(KERNEL) -q -x ./scripts/gdbinit

# debug qemu itself, this is useful at some point, like finding ramdisk addr settings
debug-qemu: oskernel binutils
	@$(GDB) --args $(QEMU) $(QEMUOPT)

# helper file generated by objdump, readelf
binutils: oskernel
	@if [ ! -d "$(DEBUGDIR)" ]; then $(MKDIR) $(DEBUGDIR); fi
	@$(ECHO) ">>>" objdump: $(notdir $(KERNEL)).asm
	@$(OBJDUMP) --source -D $(KERNEL) > $(DEBUGDIR)/$(notdir $(KERNEL)).asm
	@$(ECHO) ">>>" readelf: $(notdir $(KERNEL)).sym
	@$(READELF) -s $(KERNEL) > $(DEBUGDIR)/$(notdir $(KERNEL)).sym

$(OUTDIR):
	@$(MKDIR) -p $@ 

%.o : %.c
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

clean:
	@$(ECHO) +++ cleaning all
	@$(RM) -f $(BUILD)/*.o  $(BUILD)/*.d  $(BUILD)/kernel
	@$(RM) -f $(DEBUGDIR)/*
	@$(RM) -f -r $(BUILD) $(DEBUGDIR)