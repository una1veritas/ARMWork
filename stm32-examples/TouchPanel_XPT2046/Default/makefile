################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := del

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
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
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
USER_OBJS \

SECONDARY_FLASH += \
TouchPanel_XPT2046.hex \

SECONDARY_LIST += \
TouchPanel_XPT2046.lst \

SECONDARY_SIZE += \
TouchPanel_XPT2046.siz \


# All Target
all: TouchPanel_XPT2046.elf

# Tool invocations
	@echo 'No tool found that can build the extension specified with the build artifact name $@'
USER_OBJS: $(OBJS) $(USER_OBJS)
	@echo 'Invoking: ARM Windows GCC C++ Linker'
	arm-elf-g++ -Wl,-Map,TouchPanel_XPT2046.map -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "USER_OBJS" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building: $@'
	@echo ' '

TouchPanel_XPT2046.hex: TouchPanel_XPT2046.elf
	@echo 'Invoking: ARM Windows GNU Create Flash Image'
	arm-elf-objcopy -O ihex TouchPanel_XPT2046.elf  "TouchPanel_XPT2046.hex"
	@echo 'Finished building: $@'
	@echo ' '

TouchPanel_XPT2046.lst: TouchPanel_XPT2046.elf
	@echo 'Invoking: ARM Windows GNU Create Listing'
	arm-elf-objdump -h -S TouchPanel_XPT2046.elf > "TouchPanel_XPT2046.lst"
	@echo 'Finished building: $@'
	@echo ' '

TouchPanel_XPT2046.siz: TouchPanel_XPT2046.elf
	@echo 'Invoking: ARM Windows GNU Print Size'
	arm-elf-size  --format=berkeley TouchPanel_XPT2046.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(C++_DEPS)$(SECONDARY_SIZE)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) TouchPanel_XPT2046.elf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
