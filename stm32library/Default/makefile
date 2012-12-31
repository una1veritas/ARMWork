################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include armcore/subdir.mk
-include Utilities/jnosky/subdir.mk
-include Utilities/STM32L-DISCOVERY/subdir.mk
-include Utilities/STM32F4-Discovery/subdir.mk
-include Utilities/STM32F3_Discovery/subdir.mk
-include Utilities/STM3240_41_G_EVAL/subdir.mk
-include Utilities/Olimex_STM32-E407/subdir.mk
-include Utilities/Common/subdir.mk
-include STM32_USB_OTG_Driver/src/subdir.mk
-include STM32_USB_HOST_Library/Core/src/subdir.mk
-include STM32_USB_HOST_Library/Class/MSC/src/subdir.mk
-include STM32_USB_HOST_Library/Class/HID/src/subdir.mk
-include STM32_USB_Device_Library/Core/src/subdir.mk
-include STM32_USB_Device_Library/Class/msc/src/subdir.mk
-include STM32_USB_Device_Library/Class/hid/src/subdir.mk
-include STM32_USB_Device_Library/Class/dfu/src/subdir.mk
-include STM32_USB_Device_Library/Class/cdc/src/subdir.mk
-include STM32_USB_Device_Library/Class/audio/src/subdir.mk
-include STM32_USB-FS-Device_Driver/src/subdir.mk
-include STM32_TouchSensing_Driver/src/subdir.mk
-include STM32_ETH_Driver/src/subdir.mk
-include STM32L1xx_StdPeriph_Driver/src/subdir.mk
-include STM32F4xx_StdPeriph_Driver/src/subdir.mk
-include STM32F30x_StdPeriph_Driver/src/subdir.mk
-include STM32F2xx_StdPeriph_Driver/src/subdir.mk
-include STM32F10x_StdPeriph_Driver/src/subdir.mk
-include CMSIS/STM32L1xx/subdir.mk
-include CMSIS/STM32F4xx/startup/TASKING/subdir.mk
-include CMSIS/STM32F4xx/subdir.mk
-include CMSIS/STM32F2xx/startup/TASKING/subdir.mk
-include CMSIS/STM32F2xx/subdir.mk
-include CMSIS/STM32F10x/startup/TASKING/subdir.mk
-include CMSIS/STM32F10x/subdir.mk
-include CMSIS/Device/ST/STM32L1xx/subdir.mk
-include CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/subdir.mk
-include CMSIS/Device/ST/STM32F4xx/Source/Templates/subdir.mk
-include CMSIS/Device/ST/STM32F30x/Source/Templates/subdir.mk
-include CMSIS/Device/ST/STM32F2xx/Source/Templates/TASKING/subdir.mk
-include CMSIS/Device/ST/STM32F2xx/Source/Templates/subdir.mk
-include CMSIS/Device/ST/STM32F10x/Source/Templates/TASKING/subdir.mk
-include CMSIS/Device/ST/STM32F10x/Source/Templates/subdir.mk
-include CMSIS/DSP_Lib/Source/TransformFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/SupportFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/StatisticsFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/MatrixFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/FilteringFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/FastMathFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/ControllerFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/ComplexMathFunctions/subdir.mk
-include CMSIS/DSP_Lib/Source/CommonTables/subdir.mk
-include CMSIS/DSP_Lib/Source/BasicMathFunctions/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_variance_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_sin_cos_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_signal_converge_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_matrix_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_linear_interp_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_graphic_equalizer_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_fir_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_fft_bin_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_dotproduct_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_convolution_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/arm_class_marks_example/subdir.mk
-include CMSIS/DSP_Lib/Examples/Common/Source/subdir.mk
-include CMSIS/DSP_Lib/Examples/Common/GCC/subdir.mk
-include CMSIS/DSP_Lib/Examples/Common/subdir.mk
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
all: STM32Library..elf

# Tool invocations
STM32Library..elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-none-eabi-g++  -o "STM32Library..elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) STM32Library..elf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
