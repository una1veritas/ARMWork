################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include uVision/subdir.mk
-include stdperiph_driver_src/subdir.mk
-include armcore/subdir.mk
-include EWARM/Debug/Obj/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: stm32f4-i2c.elf

# Tool invocations
stm32f4-i2c.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mlittle-endian -nostartfiles -Wl,-T,/Users/sin/Documents/Eclipse/Workspace/stm32f4-i2c/stm32f4xx_flash.ld -o "stm32f4-i2c.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) stm32f4-i2c.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -Obinary stm32f4-i2c.elf flash.bin; arm-none-eabi-objdump -S stm32f4-i2c.elf > stm32f4-i2c.lst
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
