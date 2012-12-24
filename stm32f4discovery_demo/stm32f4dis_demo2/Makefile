#!/usr/bin/make

MAKEFILE_INC = $(wildcard ./Makefile.inc)
ifneq ($(strip ${MAKEFILE_INC}), )
  include ${MAKEFILE_INC}
else
  MAKEFILE_INC = $(wildcard ../Makefile.inc)
  ifneq ($(strip ${MAKEFILE_INC}), )
    include ${MAKEFILE_INC}
  endif
endif

PROJECT	= template

SRCDIR = src
INCDIR = inc

### Source files and search directories
CSRC	:= $(subst $(SRCDIR)/,,$(wildcard ${SRCDIR}/*.c))
ASRC	:= $(subst $(SRCDIR)/,,$(wildcard ${SRCDIR}/*.S))
CSRCARM	=
ASRCARM	=
VPATH   =

### Optimization level (0, 1, 2, 3, 4 or s)
OPTIMIZE = s

### C Standard level (c89, gnu89, c99 or gnu99)
CSTD = gnu89

### Processor type and use Thumb(-2) mode for CSRC/ASRC files (YES or NO)
CPU   = cortex-m4
THUMB = YES
HAVEFPU = YES
##HAVEFPU = NO
##FLOAT_ABI= hard
FLOAT_ABI= softfp

ifeq ($(HAVEFPU),YES)
  ifeq ($(FLOAT_ABI),hard)
    FLOAT_ABI= hard
  else
    FLOAT_ABI= softfp
  endif
else
  FLOAT_ABI= float
endif

### Linker script for the target MCU
ifeq ($(ON_SRAM),YES)
  LINKSCRIPT = stm32f4xx_ram.ld
  DEFS = VECTOR_ON_SRAM
else
  LINKSCRIPT = stm32f4xx.ld
  DEFS =
endif

### Output file format (ihex, bin or both) and debugger type
OUTPUT	= ihex
DEBUG	= dwarf-2

### Include dirs, library dirs and definitions
MATHLIB	= -lm
LIBS	=
LIBDIRS	=
INCDIRS	= ${INCDIR} 
DEFS	+=
ADEFS	=

### Warning contorls
WARNINGS = all extra

### Object output directory
OBJDIR = obj


### Programs to build porject
ifeq ($(FLOAT_ABI),hard)
  PRFIX   = arm-hardfloat-eabi
else
  PRFIX   = arm-none-eabi
endif
CC      = ${PRFIX}-gcc
OBJCOPY = ${PRFIX}-objcopy
OBJDUMP = ${PRFIX}-objdump
SIZE    = ${PRFIX}-size
NM      = ${PRFIX}-nm


# Define all object files
COBJ      = $(CSRC:.c=.o)
AOBJ      = $(ASRC:.S=.o)
COBJARM   = $(CSRCARM:.c=.o)
AOBJARM   = $(ASRCARM:.S=.o)
COBJ      := $(addprefix $(OBJDIR)/,$(COBJ))
AOBJ      := $(addprefix $(OBJDIR)/,$(AOBJ))
COBJARM   := $(addprefix $(OBJDIR)/,$(COBJARM))
AOBJARM   := $(addprefix $(OBJDIR)/,$(AOBJARM))
PROJECT   := $(OBJDIR)/$(PROJECT)


### Compiler flags
ifeq ($(THUMB),YES)
THUMBFLAG = -mthumb
THUMBIW = -mthumb-interwork
LD_THUMB =
else
THUMBFLAG =
THUMBIW =
LD_THUMB =
endif

ifeq ($(HAVEFPU),YES)
  ifeq ($(FLOAT_ABI),hard)
    FPUFLAG = -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fipa-sra
  else
    FPUFLAG = -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fipa-sra
  endif
else
  FPUFLAG = -mfloat-abi=soft -fipa-sra
endif
# -mfloat-abi= option
#    soft:   Full software floating point.
#    (belows are need option -mfpu=fpv4-sp-d16 for cortex-M4F)
#    softfp: Use the FPU, but remain compatible with soft-float code.
#    hard:   Full hardware floating point. 

# Flags for C files
CFLAGS += -std=$(CSTD)
CFLAGS += -g$(DEBUG) -fno-schedule-insns2
CFLAGS += -O$(OPTIMIZE)
CFLAGS += $(addprefix -W,$(WARNINGS))
CFLAGS += $(addprefix -I,$(INCDIRS))
CFLAGS += $(addprefix -D,$(DEFS))
CFLAGS += -Wp,-M,-MP,-MT,$(OBJDIR)/$(*F).o
##CFLAGS += -MF,$(OBJDIR)/$(*F).d


# Assembler flags
ASFLAGS += $(addprefix -D,$(ADEFS)) -Wa,-g$(DEBUG)


# Linker flags
# LDFLAGS += -nostdlib
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-Map=$(PROJECT).map,--cref,--gc-sections
#LDFLAGS += -Wl,-mcpu=$(CPU)
LDFLAGS += -lc -lgcc
LDFLAGS += $(patsubst %,-L%,$(LIBDIRS)) $(patsubst %,-l%,$(LIBS))
LDFLAGS += $(MATHLIB)
LDFLAGS += -T$(LINKSCRIPT)


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mcpu=$(CPU) $(THUMBIW) $(FPUFLAG) -I. $(CFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) $(THUMBIW) $(FPUFLAG) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: version build size

ifeq ($(OUTPUT),ihex)
build: elf hex lst sym
hex: $(PROJECT).hex
else
ifeq ($(OUTPUT),binary)
build: elf bin lst sym
bin: $(PROJECT).bin
else
ifeq ($(OUTPUT),both)
build: elf hex bin lst sym
hex: $(PROJECT).hex
bin: $(PROJECT).bin
else
$(error "Invalid format: $(OUTPUT)")
endif
endif
endif

elf: $(PROJECT).elf
lst: $(PROJECT).lst 
sym: $(PROJECT).sym


# Display compiler version information.
version :
	@$(CC) --version

# Create final output file (.hex or .bin) from ELF output file.
%.hex: %.elf
	@echo
	$(OBJCOPY) -S -O ihex $< $@

%.bin: %.elf
	@echo
	$(OBJCOPY) -S -O binary $< $@

# Create extended listing file from ELF output file.
%.lst: %.elf
	@echo
	$(OBJDUMP) -h -S -C $< > $@
### ここのtestコマンドでエラーになる場合は，ライブラリとオブジェクトのモードが異なる
### armとthumb混じりか-mfloat-abiの相違などが考えられるのでreadelfコマンドなどを使用
### して，ライブラリとオブジェクトのモードなどを確認すること。
### (cortexM?では無効な BLX即値命令が生成されているので実機でhardfaultになるのを防止)
ifeq ($(THUMB),YES)
	@test 0 -eq `grep '	blx	[^r]' $@ | wc -L` || (echo '*** BLX imm instruction found! ***' && test 0 -ne 0)
endif


# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	$(NM) -n $< > $@

# Display size of file.
size:
	@echo
	$(SIZE) -A -x $(PROJECT).elf


# Link: create ELF output file from object files.
%.elf:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ)
	@echo
	@echo Linking...
	$(CC) $(THUMBFLAG) $(LD_THUMB) $(ALL_CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM or Thumb(-2)
$(COBJ) : $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARM) : $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo
	@echo $< :
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM or Thumb(-2)
$(AOBJ) : $(OBJDIR)/%.o : $(SRCDIR)/%.S
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_ASFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : $(OBJDIR)/%.o : $(SRCDIR)/%.S
	@echo
	@echo $< :
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
	@echo
	rm -f -r $(OBJDIR)/* | exit 0


# Include the dependency files.
-include $(shell mkdir $(OBJDIR) 2>/dev/null) ## $(wildcard $(OBJDIR)/*.d)

