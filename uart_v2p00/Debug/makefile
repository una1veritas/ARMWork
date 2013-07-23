################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include hardware/LPCxpresso_1769/cores/arduino/subdir.mk
-include DES-oute/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: uart.axf

# Tool invocations
uart.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/Users/sin/Documents/LPCxpresso/workspace/CMSISv2p00_LPC17xx/Debug" -Xlinker -Map=uart.map -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "uart_v2p00_Debug.ld" -o"uart.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) uart.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-cat ../Docs/Abstract.txt; arm-none-eabi-size uart.axf; # arm-none-eabi-objcopy -O binary uart.axf uart.bin ; checksum -d uart.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
