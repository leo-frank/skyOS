# ------------------------------------------------------------------------------
# Paths
# ------------------------------------------------------------------------------
PATH_TOOLS_CC        = riscv64-unknown-elf-
PATH_TOOLS_UTIL      = 
BUILD 			     = .

# ------------------------------------------------------------------------------
# Linker options
# ------------------------------------------------------------------------------
# -Ttext HexNumber
LDFLAGS 			 = -z max-page-size=4096 -N -e main -Ttext 1000

# ------------------------------------------------------------------------------
# Development tools
# ------------------------------------------------------------------------------
CC = $(PATH_TOOLS_CC)gcc
LD = $(PATH_TOOLS_CC)ld
ECHO = $(PATH_TOOLS_UTIL)echo

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
					-I./include/									\

# ------------------------------------------------------------------------------
# Source & Object files TODO: better config way
# ------------------------------------------------------------------------------
OBJ = idle.o strings.o printf.o sys.o
DFILES = $(OBJ:.o=.d)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------
objs: $(OBJ)
	@$(LD) $(LDFLAGS) -o idle idle.o strings.o printf.o sys.o

%.o : %.c
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

%.o : %.S
	@$(ECHO) +++ compile: $<
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$(basename $<).o

.PHONY: clean install

clean:	
	@rm -rf $(addprefix $(BUILD)/, $(DFILES)) $(addprefix $(BUILD)/, $(OBJ))
	@rm -rf idle

-include *.d
