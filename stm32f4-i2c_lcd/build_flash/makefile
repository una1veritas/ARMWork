################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include armcore/subdir.mk
-include StdPeriph_Driver/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: stm32f4-i2c_lcd.elf

# Tool invocations
stm32f4-i2c_lcd.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian  -mfloat-abi=soft -nostartfiles -Wl,-T,C:/Users/Sin/Documents/Eclipse/ARMWork/stm32f4-i2c_lcd/stm32f4xx_flash.ld -o "stm32f4-i2c_lcd.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) stm32f4-i2c_lcd.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -Obinary stm32f4-i2c_lcd.elf flash.bin; arm-none-eabi-objdump -S stm32f4-i2c_lcd.elf > stm32f4-i2c_lcd.lst
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
