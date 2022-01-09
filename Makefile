# ------------------------------------------------------------------------------
# Linker options
# ------------------------------------------------------------------------------
LDFLAGS 			 = -z max-page-size=4096
LDSCRIPT 			 = ./qemu.ld

# ------------------------------------------------------------------------------
# SBI options TODO: diff between sbi.bin & sbi, and why they all works ?
# ------------------------------------------------------------------------------
RUSTSBI 			 = ./rustsbi-qemu.bin

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
QEMUOPT = -machine virt -m 128M -nographic -smp 1
QEMUOPT += -bios $(RUSTSBI)
QEMUOPT += -kernel $(KERNEL)

# ------------------------------------------------------------------------------
# Compiler flags for the target architecture
# ------------------------------------------------------------------------------

CFLAGS      	= 	-Wall                        	\
					-Werror							\
					-O0								\
					-fno-omit-frame-pointer			\
					-ggdb							\
					-g								\
					-MD								\
					-mcmodel=medany					\
					-ffreestanding					\
					-fno-common						\
					-nostdlib						\
					-mno-relax						\
					-c								\

# ------------------------------------------------------------------------------
# Kernel object files TODO: a better way to config KOBJS
# ------------------------------------------------------------------------------
KOBJS			=	entry.o 						\
					logo.o 							\
					main.o 							\
					printf.o 						\
					console.o						\
					strings.o 						\
					panic.o  						\
					lock.o 							\
					pmm.o  							\
					buddy.o 						\
					log.o 							\

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------
run: oskernel binutils
	@$(QEMU) $(QEMUOPT)

debug: oskernel binutils
	@$(QEMU) $(QEMUOPT) -S -gdb tcp::1235
# @$(GDB) $(KERNEL) -q -x ./gdbinit

oskernel: $(OUTDIR) $(KOBJS) $(LDSCRIPT)
	@if [ ! -d "$(BUILD)" ]; then $(MKDIR) $(BUILD); fi
	@$(ECHO) ">>>" linking: $(notdir $(KERNEL))
	@$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $(KERNEL) $(addprefix $(BUILD)/, $(KOBJS))

%.o : %.c
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

%.o : %.S
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

binutils: oskernel
	@if [ ! -d "$(DEBUGDIR)" ]; then $(MKDIR) $(DEBUGDIR); fi
	@$(ECHO) ">>>" objdump: $(notdir $(KERNEL)).asm
	@$(OBJDUMP) --source -D $(KERNEL) > $(DEBUGDIR)/$(notdir $(KERNEL)).asm
	@$(ECHO) ">>>" readelf: $(notdir $(KERNEL)).sym
	@$(READELF) -s $(KERNEL) > $(DEBUGDIR)/$(notdir $(KERNEL)).sym

$(OUTDIR):
	@$(MKDIR) -p $@ 

clean:
	@$(ECHO) +++ cleaning all
	@$(RM) -f $(BUILD)/*.o  $(BUILD)/*.d  $(BUILD)/kernel
	@$(RM) -f $(DEBUGDIR)/*
	@$(RM) -f -r $(BUILD) $(DEBUGDIR)