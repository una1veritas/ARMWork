LIB = ../libSTM32F4-Discovery.a

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -g -Os -mlittle-endian -mthumb
CFLAGS += -mcpu=cortex-m4 -ffreestanding -nostdlib
#CFLAGS += -I./inc -I./inc/device_support -I./inc/core_support
CFLAGS += -I./ -I../../stm32f4/STM32F4xx_StdPeriph_Driver/inc

CMSIS_PATH = ../../CMSIS
CFLAGS += -I$(CMSIS_PATH)/Device/ST/STM32F4xx/Include -I$(CMSIS_PATH)/Include

SRCS_PATH = ./STM32F4-Discovery
SRCS = \
$(SRCS_PATH)/stm32f4_discovery_audio_codec.c \
$(SRCS_PATH)/stm32f4_discovery_lis302dl.c \
$(SRCS_PATH)/stm32f4_discovery.c 
#../inc/core_support/core_cm4.c

OBJS = $(SRCS:.c=.o)

all: $(LIB) 

$(LIB): $(OBJS)
	$(AR) -r $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
