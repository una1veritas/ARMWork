LIB = libmycore.a

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -g -Os -mlittle-endian -mthumb
CFLAGS += -mcpu=cortex-m4 -ffreestanding -nostdlib
#CFLAGS += -I./inc -I./inc/device_support -I./inc/core_support
CFLAGS += -I./ -I../../STM32_library/stm32f4/STM32F4xx_StdPeriph_Driver/inc

CMSIS_PATH = ../../STM32_library/CMSIS
CFLAGS += -I$(CMSIS_PATH)/Device/ST/STM32F4xx/Include -I$(CMSIS_PATH)/Include

SRCS_PATH = .
SRCS = \
$(SRCS_PATH)/gpio_digital.c 
#../inc/core_support/core_cm4.c

OBJS = $(SRCS:.c=.o)
CPPOBJS = $(SRCS:.cpp=.o)

all: $(LIB) 

$(LIB): $(OBJS)
	$(AR) -r $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
