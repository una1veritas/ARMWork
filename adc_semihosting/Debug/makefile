################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: adc_semihosting.axf

# Tool invocations
adc_semihosting.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/nxp/Desktop/linuxtest1/CMSISv1p30_LPC17xx/Debug" -L"/home/nxp/Desktop/linuxtest1/lib_small_printf_m3/Lib" -Xlinker -Map=adc_semihosting.map -Xlinker --cref -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "adc_semihosting_Debug.ld" -o"adc_semihosting.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) adc_semihosting.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-cat ../Docs/Abstract.txt; arm-none-eabi-size adc_semihosting.axf; # arm-none-eabi-objcopy -O binary adc_semihosting.axf adc_semihosting.bin ; checksum -d adc_semihosting.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
