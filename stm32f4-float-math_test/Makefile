TOOL_PREFIX = /usr/local/summon

PRG_PREFIX = $(TOOL_PREFIX)/bin/arm-none-eabi-
LIB_PREFIX = $(TOOL_PREFIX)/arm-none-eabi/lib/thumb/cortex-m4/float-abi-hard/fpuv4-sp-d16/

CC      = $(PRG_PREFIX)gcc
LD      = $(PRG_PREFIX)gcc
CP      = $(PRG_PREFIX)objcopy
OD      = $(PRG_PREFIX)objdump
STM32FLASH = ~/bin/stm32_flash.pl

# -mfloat-abi=name
#    Specifies which floating-point ABI to use. 
#    "soft": Causes GCC to generate output containing library calls for 
#            floating-point operations. Same as -msoft-float. 
#    "softfp": Allows the generation of code using hardware floating-point 
#            instructions, but still uses the soft-float calling conventions. 
#    "hard": Allows generation of floating-point instructions and uses 
#            FPU-specific calling conventions. Same as -mhard-float
#    Note that the hard-float and soft-float ABIs are not link-compatible; 
#    you must compile your entire program with the same ABI, and link with a 
#    compatible set of libraries. 
# 
# -msoft-float, -mhard-float: Use -mfloat-abi=name instead. 
# 
# -mfpu=fpv4-sp-d16
#    Specify which FPU is available. I think, this is clear from the 
#    -mcpu= but OTOH, we explicitly link to fpv4-sp-d16 libraries, so let's 
#    keep it in there to be sure. 
# 
# -fsingle-precision-constant
#    Ensure that 1.0 is treated as 1.0f. Avoids that intermediate DOUBLE 
#    floats are generated. Writing the 'f' suffix is better but error-prone. 
# 
# -mfix-cortex-m3-ldrd
#    Is automatically enabled for Cortex-M3. 
# 
# -fno-common : unclear if needed
# 
# -lm -lc
#    Required during linking for sqrtf() etc. 
#
CFLAGS  =  -I. -I../STM32Library/CMSIS/STM32F4xx -I../STM32Library/CMSIS/Include \
	-I../STM32Library/STM32F4xx_StdPeriph_Driver/inc \
	-c -fno-common -O2 -g -D"assert_param(x)=" \
	-mcpu=cortex-m4 -mthumb \
	-mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	-fsingle-precision-constant
LFLAGS  = -Tstm32_flash.ld -nostartfiles -L$(LIB_PREFIX)
LFLAGS_END = -lm -lc
CPFLAGS = -Obinary
ODFLAGS = -S

all: main.bin

clean:
	-rm -f main.lst *.o main.elf main.lst main.bin 
	-rm -f mandelbrot

main.bin: main.elf
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.elf main.bin
	$(OD) $(ODFLAGS) main.elf > main.lst

main.elf: main.o stm32_flash.ld stm32f4xx_it.o system_stm32f4xx.o startup_stm32f4xx.o stm32f4xx_gpio.o stm32f4xx_rcc.o
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.elf main.o mandelbrot.o stm32f4xx_it.o system_stm32f4xx.o startup_stm32f4xx.o \
		stm32f4xx_gpio.o stm32f4xx_rcc.o $(LFLAGS_END)

main.o: main.c mandelbrot.c mandelbrot.h
	@ echo ".compiling"
	$(CC) $(CFLAGS) main.c mandelbrot.c stm32f4xx_it.c  system_stm32f4xx.c startup_stm32f4xx.s \
		stm32f4xx_gpio.c stm32f4xx_rcc.c

run: main.bin
	$(STM32FLASH) main.bin

mandelbrot: mandelbrot.c
	gcc -O2 -DTEST_ON_HOST=1 mandelbrot.c -o mandelbrot
	./mandelbrot

gdb:
	$(PRG_PREFIX)gdb -ex "target remote localhost:3333" \
		-ex "set remote hardware-breakpoint-limit 6" \
		-ex "set remote hardware-watchpoint-limit 4" main.elf
