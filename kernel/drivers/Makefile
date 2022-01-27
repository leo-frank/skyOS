# ------------------------------------------------------------------------------
# Paths
# ------------------------------------------------------------------------------
PATH_TOOLS_CC        = riscv64-unknown-elf-
PATH_TOOLS_UTIL      = 
BUILD 			     = ../build

# ------------------------------------------------------------------------------
# Development tools
# ------------------------------------------------------------------------------
CC= $(PATH_TOOLS_CC)gcc
ECHO      = $(PATH_TOOLS_UTIL)echo

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
					-I../include/									\

# ------------------------------------------------------------------------------
# Source & Object files
# ------------------------------------------------------------------------------
SRC = block_dev.c console.c ramdisk.c
OBJ = $(SRC:.c=.o)
DFILES = $(SRC:.c=.d)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------
objs: $(OBJ)

%.o : %.c
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

.PHONY: clean install

clean:	
	-rm -rf $(addprefix $(BUILD)/, $(DFILES)) $(addprefix $(BUILD)/, $(OBJ))

-include *.d