################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: lpclcd.axf

# Tool invocations
lpclcd.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="lpclcd.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "lpclcd_Debug.ld" -o "lpclcd.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) lpclcd.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	arm-none-eabi-size "lpclcd.axf"
	arm-none-eabi-objcopy -O binary "lpclcd.axf" "lpclcd.bin"
	checksum -p LPC11U24_401 -d "lpclcd.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
