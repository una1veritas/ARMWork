#################################################
# MAKEFILE For STM32F4xxx Devices 				#
# (c) 20120630 Nemui Trinomius					#
# http://nemuisan.blog.bai.ne.jp				#
#################################################

# Environment Dependent!!! This Environment assure under WINDOWS !!
# Throw path into YOUR environments for each Operation Systems.
export PATH = %SYSTEMROOT%;$(TOOLDIR)/bin;$(OCDIR);$(DFUDIR);$(MAKEDIR)

# Toolchain prefix (i.e arm-none-eabi -> arm-none-eabi-gcc.exe)
TCHAIN  = arm-none-eabi

# OpenOCD prefix
OCD		= openocd
# Select OpenOCD Transport
OCDMODE = SWD
#OCDMODE = JTAG
# OpenOCD For FT2232 Devices Special Setting
# If u use WinUSB as Device Driver,uncomment this!
MPSSE	= ftdi
#MPSSE	= 

# Development Tools based on GNU Compiler Collection
DEVTOOL = LAUNCHPAD
#DEVTOOL = YAGARTO
#DEVTOOL = DEVKITARM
#DEVTOOL = SOURCERY

# Check BuildTools
ifeq ($(DEVTOOL),LAUNCHPAD)
 TOOLDIR = C:/Devz/ARM/Launchpad
 REMOVAL = rm
else ifeq ($(DEVTOOL),YAGARTO)
 TOOLDIR = C:/Devz/ARM/Yagarto
 REMOVAL = rm
else ifeq ($(DEVTOOL),DEVKITARM)
 TOOLDIR = C:/Devz/ARM/devkitARM
 REMOVAL = rm
else ifeq ($(DEVTOOL),SOURCERY)
 TOOLDIR = C:/Devz/ARM/Sourcery
 REMOVAL = rm
else
 $(error SET BUILD-TOOLS AT FIRST!!)
endif

# Set UNIX-Like tools (Coreutils)
MAKEDIR = C:/Devz/Coreutils/bin

# Set Flasher and Debugger
OCDIR	= C:/Devz/ARM/OCD
ifeq ($(OCDMODE),SWD)
OCD_CMD = -s $(OCDIR)/tcl						\
		  -f target/stm32f4discovery_flash.cfg
#OCD_CMD = -s $(OCDIR)/tcl						\
#		  -f interface/vsllink_swd.cfg			\
#		  -f target/stm32f4x_flash.cfg
else
OCD_CMD = -s $(OCDIR)/tcl						\
		  -f interface/$(MPSSE)/jtagkey2.cfg 	\
          -f target/stm32f4x_flash.cfg
endif

DFUDIR	= C:/Devz/ARM/ST/DFUse/BIN
		  
WSHELL  = cmd
MSGECHO = echo.exe
GDBDIR  = C:/Devz/ARM/insight/bin
INSIGHT = $(GDBDIR)/arm-none-eabi-insight
# Environment Dependent!!!


# OPTIMIZE Definition
OPTIMIZE		= fast
#OPTIMIZE		= 2

# FPU Definition
ifeq ($(DEVTOOL),LAUNCHPAD)
 # Only Launchpad has hard-float library in free!
 USING_FPU		= -mfloat-abi=hard  -mfpu=fpv4-sp-d16
else
 USING_FPU		= -mfloat-abi=softfp  -mfpu=fpv4-sp-d16
 #USING_FPU		= -mfloat-abi=hard  -mfpu=fpv4-sp-d16
 #USING_FPU		= -mfloat-abi=soft
endif

# GCC4.6.x Specific Option
ifneq ($(OPTIMIZE),0)
USE_LTO			= -flto-partition=none -fipa-sra
#USE_LTO			= -flto -fipa-sra
endif
ALIGNED_ACCESS	= -mno-unaligned-access

# Apprication Version
APP_VER = W.I.P


# Board and MPU Definitions
EVAL_BOARD    	= USE_STM32F4DISCOVERY
#EVAL_BOARD    	= USE_REDBULL

ifeq ($(EVAL_BOARD),USE_STM32F4DISCOVERY)
 SUBMODEL			= STM32F407VGT6
 MPU_DENSITY		= STM32F4XX
 HSE_CLOCK 			= 8000000
 USE_EXT_SRAM   	= 
 USE_TOUCH_SENCE 	=
 USE_AUDIO_LIB  	= USE_CS43L22_CODEC
 USE_SOUND_WAV  	= USE_WAVE_OUT
 USE_SOUND_MP3		= USE_MP3_HELIX

else ifeq ($(EVAL_BOARD),USE_REDBULL)
 SUBMODEL			= STM32F407ZGT6
 MPU_DENSITY		= STM32F4XX
 HSE_CLOCK 			= 8000000
 USE_EXT_SRAM   	= EXT_SRAM_SUPPORT
 USE_TOUCH_SENCE 	= USE_ADS7843
 USE_AUDIO_LIB  	=
 USE_SOUND_WAV  	=
 USE_SOUND_MP3		=

else
 $(error TARGET BOARD IS NOT DEFINED!!)
endif
PERIF_DRIVER    = USE_STDPERIPH_DRIVER


# Display Drivers Definition
# Use Display Device? MUST See lcddrv.mk
include ./lcddrv.mk

# For JPEG Support
#USE_JPEG_LIB    = USE_TINYJPEG_LIB
USE_JPEG_LIB    = USE_IJG_LIB

# For PNG Support
USE_PNG_LIB		= USE_LIBPNG

# For GIF Support
USE_GIF_LIB		= USE_GIFLIB


# Use Display Fonts? 
#USE_FONTSIZE    = FONT8x8
USE_FONTSIZE    = FONT10x10
#USE_FONTSIZE    = FONT12x10
#USE_FONTSIZE    = FONT12x12
#USE_FONTSIZE    = FONT12x12K
#USE_FONTSIZE    = FONT12x12W
#USE_FONTSIZE    = FONT14x14
#USE_FONTSIZE    = FONT14x14W
#USE_FONTSIZE    = FONT16x16
#USE_FONTSIZE    = FONT16x16W
#USE_FONTSIZE    = FONT20x20
#USE_FONTSIZE    = FONT_DIGIT
USE_KANJI		= USE_KANJIFONT

# Use FreeRTOS?
OS_SUPPORT		= BARE_METAL
#OS_SUPPORT		= USE_FREERTOS


# Synthesis makefile Defines
DEFZ = $(SUBMODEL)   $(EVAL_BOARD)   $(MPU_DENSITY)  $(PERIF_DRIVER)    $(VECTOR_START)    \
	   $(OS_SUPPORT) $(USE_EXT_SRAM)
# Defines if Display and misc Drivers
DEFZ += $(USE_DISPLAY) $(USE_FONTSIZE) $(USE_KANJI) $(USE_TOUCH_SENCE)  $(USE_XMSTN)	   \
        $(USE_JPEG_LIB) $(USE_PNG_LIB) $(USE_GIF_LIB) $(USE_SOUND_MP3)  $(USE_SOUND_WAV)
SYNTHESIS_DEFS	= $(addprefix -D,$(DEFZ)) 							\
				 -DPACK_STRUCT_END=__attribute\(\(packed\)\) 		\
				 -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
				 -DMPU_SUBMODEL=\"$(SUBMODEL)\"						\
				 -DAPP_VERSION=\"$(APP_VER)\"						\
				 -DHSE_VALUE=$(HSE_CLOCK)UL

# TARGET definition
TARGET 		= main
TARGET_ELF  = $(TARGET).elf
TARGET_SREC = $(TARGET).s19
TARGET_HEX  = $(TARGET).hex
TARGET_BIN  = $(TARGET).bin
TARGET_LSS  = $(TARGET).lss
TARGET_DFU  = $(TARGET).dfu
TARGET_SYM  = $(TARGET).sym

# define CMSIS LIBRARY PATH
FWLIB  			= ./lib/STM32F4xx_StdPeriph_Driver
USBLIB 			=
CMSISLIB 		= ./lib/CMSIS
CMSIS_DEVICE 	= $(CMSISLIB)/Device/ST/STM32F4xx
CMSIS_CORE		= $(CMSISLIB)/Include

# include PATH
INCPATHS	 = 	./							\
				./inc						\
				$(FWLIB)/inc  				\
				$(USBLIB)/Core/inc			\
				$(CMSIS_DEVICE)/Include		\
				$(CMSIS_CORE)				\
				$(LIBINCDIRS)
INCLUDES     = $(addprefix -I ,$(INCPATHS))

# Set library PATH
LIBPATHS     = $(FWLIB) $(USBLIB)
LIBRARY_DIRS = $(addprefix -L,$(LIBPATHS))
# if you use math-library, put "-lm" 
MATH_LIB	 =	-lm

# LinkerScript PATH
LINKER_PATH =  ./lib/linker
LINKER_DIRS = $(addprefix -L,$(LINKER_PATH)) 

# Object definition
OBJS 	 = $(CFILES:%.c=%.o) 
LIBOBJS  = $(LIBCFILES:%.c=%.o) $(SFILES:%.s=%.o)

# C code PATH
SOURCE  = ./src
CFILES = \
 $(SOURCE)/$(TARGET).c 				\
 $(SOURCE)/hw_config.c				\
 $(SOURCE)/rtc_support.c			\
 $(SOURCE)/uart_support.c			\
 $(SOURCE)/stm32f4xx_it.c			\
 $(SOURCE)/systick.c				\
 $(SOURCE)/syscalls.c				\
 $(SOURCE)/ff_support.c 			\
 $(SOURCE)/term_io.c				\
 $(SOURCE)/xprintf.c


#/*----- Display library PATH -----*/	
AUDIO_LIB	= ./lib/audio
ifneq ($(USE_AUDIO_LIB),)
include $(AUDIO_LIB)/audio_drv.mk
endif

#/*----- Display library PATH -----*/	
DISPLAY_LIB	= ./lib/display
ifneq ($(USE_DISPLAY),)
include $(DISPLAY_LIB)/display_cfg.mk
endif

#/*----- FONX2 Driver library PATH -----*/	
FONTX2_LIB	= ./lib/FONTX2
ifneq ($(USE_FONTSIZE),)
include $(FONTX2_LIB)/fontx2_drv.mk
endif

#/*----- xMSTN Display library PATH -----*/	
xMSTN_LIB	= ./lib/xMSTN
ifneq ($(USE_XMSTN),)
include $(xMSTN_LIB)/xmstn_drv.mk
endif


#/*----- FatFs library PATH -----*/	
FATFS = ./lib/ff
LIBINCDIRS += $(FATFS)
ifeq ($(SUBMODEL),STM32F407VGT6)
CFILES += \
 $(FATFS)/ff.c 							\
 $(FATFS)/mmc_stm32f4.c 				\
 $(FATFS)/ff_rtc_if.c 					\
 $(FATFS)/option/cc932.c
else
CFILES += \
 $(FATFS)/ff.c 							\
 $(FATFS)/sdio_stm32f4.c 				\
 $(FATFS)/diskio_sdio.c 				\
 $(FATFS)/ff_rtc_if.c 					\
 $(FATFS)/option/cc932.c
endif

#/*----- STARTUP code PATH -----*/
STARTUP_DIR = $(CMSIS_DEVICE)/Source/Templates/gcc_ride7
ifeq ($(OS_SUPPORT),USE_FREERTOS)
SFILES += \
	$(SOURCE)/startup_stm32f4xx_rtos.s
else
SFILES += \
	$(STARTUP_DIR)/startup_stm32f4xx.s
endif



#/*----- STM32 library PATH -----*/
LIBCFILES = \
 $(FWLIB)/src/misc.c \
 $(FWLIB)/src/stm32f4xx_flash.c 	\
 $(FWLIB)/src/stm32f4xx_gpio.c 		\
 $(FWLIB)/src/stm32f4xx_fsmc.c 		\
 $(FWLIB)/src/stm32f4xx_rcc.c 		\
 $(FWLIB)/src/stm32f4xx_adc.c 		\
 $(FWLIB)/src/stm32f4xx_dma.c 		\
 $(FWLIB)/src/stm32f4xx_tim.c 		\
 $(FWLIB)/src/stm32f4xx_rtc.c 		\
 $(FWLIB)/src/stm32f4xx_sdio.c 		\
 $(FWLIB)/src/stm32f4xx_spi.c 		\
 $(FWLIB)/src/stm32f4xx_usart.c 	\
 $(FWLIB)/src/stm32f4xx_pwr.c 		\
 $(FWLIB)/src/stm32f4xx_i2c.c 		\
 $(FWLIB)/src/stm32f4xx_syscfg.c 	\
 ./src/system_stm32f4xx.c
 
#/*----- STM32 Debug library -----*/
ifeq ($(OPTIMIZE),0)
CFILES += \
 ./lib/IOView/stm32f4xx_io_view.c
else
endif


# TOOLCHAIN SETTING
CC 			= $(TCHAIN)-gcc
CPP 		= $(TCHAIN)-g++
OBJCOPY 	= $(TCHAIN)-objcopy
OBJDUMP 	= $(TCHAIN)-objdump
SIZE 		= $(TCHAIN)-size
AR 			= $(TCHAIN)-ar
LD 			= $(TCHAIN)-gcc
NM 			= $(TCHAIN)-nm
REMOVE		= $(REMOVAL) -f
REMOVEDIR 	= $(REMOVAL) -rf

# C and ASM FLAGS
CFLAGS  = -MD -mcpu=cortex-m4 -march=armv7e-m -mtune=cortex-m4
CFLAGS += -mthumb -mlittle-endian $(ALIGNED_ACCESS)
CFLAGS += -mapcs-frame -mno-sched-prolog $(USING_FPU)
CFLAGS += -std=gnu99
CFLAGS += -gdwarf-2 -O$(OPTIMIZE) $(USE_LTO)
CFLAGS += -fno-strict-aliasing -fsigned-char
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-schedule-insns2
CFLAGS += --param max-inline-insns-single=1000
CFLAGS += -fno-common -fno-hosted
CFLAGS += -Wall -Wno-array-bounds
#CFLAGS += -Wdouble-promotion
#CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) 
CFLAGS += $(SYNTHESIS_DEFS)  

# Linker FLAGS
LDFLAGS  = -mcpu=cortex-m4 -march=armv7e-m -mthumb
LDFLAGS += -u g_pfnVectors -Wl,-static -Wl,--gc-sections -nostartfiles
LDFLAGS += -Wl,-Map=$(TARGET).map
LDFLAGS += $(LIBRARY_DIRS) $(LINKER_DIRS) $(MATH_LIB)
LDFLAGS +=-T$(LINKER_PATH)/$(MPU_DENSITY).ld

# Object Copy and dfu generation FLAGS
OBJCPFLAGS = -O
OBJDUMPFLAGS = -h -S -C
DFU	  = hex2dfu
DFLAGS = -w


# Build Object
all: gccversion clean build buildinform sizeafter
build: $(TARGET_ELF) $(TARGET_LSS) $(TARGET_SYM) $(TARGET_HEX) $(TARGET_SREC) $(TARGET_BIN)

.SUFFIXES: .o .c .s   

$(TARGET_LSS): $(TARGET_ELF)
	@$(MSGECHO)
	@$(MSGECHO) Disassemble: $@
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@ 
$(TARGET_SYM): $(TARGET_ELF)
	@$(MSGECHO)
	@$(MSGECHO) Symbol: $@
	$(NM) -n $< > $@
$(TARGET).hex: $(TARGET).elf
	@$(MSGECHO)
	@$(MSGECHO) Objcopy: $@
	$(OBJCOPY) $(OBJCPFLAGS) ihex $^ $@    
$(TARGET).s19: $(TARGET).elf
	@$(MSGECHO)
	@$(MSGECHO) Objcopy: $@
	$(OBJCOPY) $(OBJCPFLAGS) srec $^ $@ 
$(TARGET).bin: $(TARGET).elf
	@$(MSGECHO)
	@$(MSGECHO) Objcopy: $@
	$(OBJCOPY) $(OBJCPFLAGS) binary $< $@ 
$(TARGET).dfu: $(TARGET).hex
	@$(MSGECHO)
	@$(MSGECHO) Make STM32 dfu: $@
	$(DFU) $(DFLAGS) $< $@
	@$(MSGECHO)
$(TARGET).elf: $(OBJS) $(SUBMODEL)_lib.a
	@$(MSGECHO) Link: $@
	$(LD) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@$(MSGECHO)

$(SUBMODEL)_lib.a: $(LIBOBJS)
	@$(MSGECHO) Archive: $@
	$(AR) cr $@ $(LIBOBJS)    
	@$(MSGECHO)
.c.o:
	@$(MSGECHO) Compile: $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
	@$(MSGECHO)
.s.o:
	@$(MSGECHO) Assemble: $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
	@$(MSGECHO)

# Object Size Informations
sizeafter:
	@$(MSGECHO) 
	@$(MSGECHO) Built Object Informations:
	@$(MSGECHO) === Total Binary Size ===
	@$(SIZE) $(TARGET).hex
	@$(MSGECHO) === Verbose ELF Size ===
	@$(SIZE) $(TARGET).elf
	@$(SIZE) -A -x $(TARGET).elf

# Display compiler version information.
gccversion : 
	@$(CC) --version
	@$(MSGECHO) 

buildinform :
	@$(MSGECHO) 
	@$(MSGECHO) 
	@$(MSGECHO) Built Informations:
	@$(MSGECHO) USING_SYSTEM = $(OS_SUPPORT)
	@$(MSGECHO) USING_DISPLAY = $(USE_DISPLAY)
	@$(MSGECHO) USING_DEVBOARD = $(EVAL_BOARD)

# Flash and Debug Program
debug :
	$(WSHELL) /c start /B $(INSIGHT) $(TARGET).elf
	$(OCD) $(OCD_CMD) -c "soft_reset_halt"
program :
	$(OCD) $(OCD_CMD) -c "mt_flash $(TARGET).elf"
#	$(OCD) $(OCD_CMD) -c "eraser"
#	$(OCD) $(OCD_CMD) -c "mt_flash_bin $(TARGET).bin 0x08000000"


# Drop files into dust-shoot
.PHONY clean:
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(SUBMODEL)_lib.a
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).s19
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(TARGET).dfu
	$(REMOVE) $(wildcard *.stackdump)
	$(REMOVE) $(OBJS)
	$(REMOVE) $(AOBJ)
	$(REMOVE) $(LIBOBJS)
	$(REMOVE) $(LST)
	$(REMOVE) $(CFILES:.c=.lst)
	$(REMOVE) $(CFILES:.c=.d)
	$(REMOVE) $(LIBCFILES:.c=.lst)
	$(REMOVE) $(LIBCFILES:.c=.d)
	$(REMOVE) $(SFILES:.s=.lst)
	$(REMOVE) $(wildcard ./lib/IOView/*.d)
	$(REMOVE) $(wildcard ./lib/IOView/*.lst)
	$(REMOVE) $(wildcard ./lib/IOView/*.o)
	$(REMOVE) $(wildcard $(DISPLAY_DRV_SRC)/*.d)
	$(REMOVE) $(wildcard $(DISPLAY_DRV_SRC)/*.lst)
	$(REMOVE) $(wildcard $(DISPLAY_DRV_SRC)/*.o)
	$(REMOVE) $(wildcard $(FATFS)/*.d)
	$(REMOVE) $(wildcard $(FATFS)/*.lst)
	$(REMOVE) $(wildcard $(FATFS)/*.o)
	$(REMOVE) $(wildcard $(CMSIS_DEVICE)/*.d)
	$(REMOVE) $(wildcard $(CMSIS_DEVICE)/*.lst)
	$(REMOVE) $(wildcard $(CMSIS_DEVICE)/*.o)
	$(REMOVEDIR) .dep
	@$(MSGECHO)

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex bin lss sym clean clean_list program
