################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/mp3dec/real/subdir.mk
-include src/mp3dec/subdir.mk
-include src/codec/subdir.mk
-include src/ChibiOS_2.4.0/os/various/subdir.mk
-include src/ChibiOS_2.4.0/os/ports/common/ARMCMx/subdir.mk
-include src/ChibiOS_2.4.0/os/ports/GCC/ARMCMx/STM32F4xx/subdir.mk
-include src/ChibiOS_2.4.0/os/ports/GCC/ARMCMx/subdir.mk
-include src/ChibiOS_2.4.0/os/kernel/src/subdir.mk
-include src/ChibiOS_2.4.0/os/hal/src/subdir.mk
-include src/ChibiOS_2.4.0/os/hal/platforms/STM32F4xx/subdir.mk
-include src/ChibiOS_2.4.0/os/hal/platforms/STM32/GPIOv2/subdir.mk
-include src/ChibiOS_2.4.0/os/hal/platforms/STM32/subdir.mk
-include src/ChibiOS_2.4.0/ext/fatfs/src/option/subdir.mk
-include src/ChibiOS_2.4.0/ext/fatfs/src/subdir.mk
-include src/ChibiOS_2.4.0/boards/ST_STM32F4_DISCOVERY/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
F4D_MP3.hex \

SECONDARY_LIST += \
F4D_MP3.lst \

SECONDARY_SIZE += \
F4D_MP3.siz \


# All Target
all: F4D_MP3.elf secondary-outputs

# Tool invocations
F4D_MP3.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Linux GCC C Linker'
	arm-none-eabi-gcc -T"C:/Users/Watz/Documents/STM32/eclipse_workspace/F4D_MP3/src/ChibiOS_2.4.0/os/ports/GCC/ARMCMx/STM32F4xx/ld/STM32F407xG.ld" -nostartfiles -Wl,-Map,F4D_MP3.map -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -g3 -gdwarf-2 -o "F4D_MP3.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

F4D_MP3.hex: F4D_MP3.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex F4D_MP3.elf  "F4D_MP3.hex"
	@echo 'Finished building: $@'
	@echo ' '

F4D_MP3.lst: F4D_MP3.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Listing'
	arm-none-eabi-objdump -h -S F4D_MP3.elf > "F4D_MP3.lst"
	@echo 'Finished building: $@'
	@echo ' '

F4D_MP3.siz: F4D_MP3.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Print Size'
	arm-none-eabi-size  --format=berkeley F4D_MP3.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) F4D_MP3.elf
	-@echo ' '

post-build:
	-'cmd.exe' /c ..\postbuild.bat
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
