################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include Utilities/STM32_EVAL/STM3240_41_G_EVAL/subdir.mk
-include STM32F4xx_StdPeriph_Driver/src/subdir.mk
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
all: uSDCard.elf

# Tool invocations
uSDCard.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -nostartfiles -nostdlib -Wl,-T,/Users/sin/Documents/github/ARMWork/STM32F4xx_DSP_StdPeriph_Lib_V1.0.1/Project/STM32F4xx_StdPeriph_Examples/SDIO/uSDCard/stm32f4xx_flash.ld -o "uSDCard.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) uSDCard.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -Obinary uSDCard.elf flash.bin; arm-none-eabi-objdump -S uSDCard.elf > uSDCard.lst
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
