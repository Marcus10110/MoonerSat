# stm32 makefile
BINARY = main
OBJS = systick.o ../RFM22B.o
CFLAGS += -std=c99 -I. -I.. -DUSE_STM32=1
OPENCM3_DIR = ../libopencm3
LDSCRIPT = stm32f4-discovery.ld
include libopencm3.target.mk 
