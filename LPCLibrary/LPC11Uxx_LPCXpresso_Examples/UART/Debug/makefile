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
all: UART.axf

dependents:
	-cd C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug && $(MAKE) all
	-cd C:/Users/sin/Documents/ARMWork/LPC11Uxx_LPCXpresso_Examples/LPC11Uxx_Driver_Lib/Debug && $(MAKE) all

# Tool invocations
UART.axf: $(OBJS) $(USER_OBJS) C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug/libCMSISv2p00_LPC11Uxx.a C:/Users/sin/Documents/ARMWork/LPC11Uxx_LPCXpresso_Examples/LPC11Uxx_Driver_Lib/Debug/libLPC11Uxx_Driver_Lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\sin\Documents\ARMWork\LPC11Uxx_LPCXpresso_Examples\LPC11Uxx_Driver_Lib\Debug" -L"C:\Users\sin\Documents\ARMWork\CMSISv2p00_LPC11Uxx\Debug" -Xlinker -Map="UART.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "UART_Debug.ld" -o "UART.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) UART.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "UART.axf"; # arm-none-eabi-objcopy -O binary "UART.axf" "UART.bin" ; checksum -p LPC11U14_201 -d "UART.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
C:/Users/sin/Documents/ARMWork/CMSISv2p00_LPC11Uxx/Debug/libCMSISv2p00_LPC11Uxx.a:
C:/Users/sin/Documents/ARMWork/LPC11Uxx_LPCXpresso_Examples/LPC11Uxx_Driver_Lib/Debug/libLPC11Uxx_Driver_Lib.a:

-include ../makefile.targets
