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
all: i2c.axf

dependents:
	-cd C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug && $(MAKE) clean all

# Tool invocations
i2c.axf: $(OBJS) $(USER_OBJS) C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug/libCMSISv2p00_LPC11Uxx.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\sin\Documents\ARMWork\CMSISv2p00_LPC11Uxx\Debug" -Xlinker -Map="i2c.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "i2c_Debug.ld" -o "i2c.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) i2c.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "i2c.axf"; # arm-none-eabi-objcopy -O binary "i2c.axf" "i2c.bin" ; checksum -p LPC11U14_201 -d "i2c.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug/libCMSISv2p00_LPC11Uxx.a:

-include ../makefile.targets
