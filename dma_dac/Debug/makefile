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
all: dma_dac.axf

# Tool invocations
dma_dac.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -L"/home/nxp/Desktop/linuxtest1/CMSISv1p30_LPC17xx/Debug" -Xlinker -Map=dma_dac.map -Xlinker --gc-sections -Xlinker --cref -mcpu=cortex-m3 -mthumb -T "../linker_config/dma_dac.ld" -o"dma_dac.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) dma_dac.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-cat ../Docs/Abstract.txt; arm-none-eabi-size dma_dac.axf; # arm-none-eabi-objcopy -O binary dma_dac.axf dma_dac.bin ; checksum -d dma_dac.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
