EXEC = main.elf

LD_DIR = linker

CC = arm-none-eabi-gcc

CFLAGS = -std=c99
CFLAGS += -g
CFLAGS += -T $(LD_DIR)/generic-hosted.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m3

OUT = build
INC_DIR = include
SRC_DIR = src

OBJS : main.o

main.o: main.c 
	$(CC) $(CFLAGS) -c -o $(OUT)/$@ main.c

all:
	$(CC) $(CFLAGS) -o $(EXEC) $(OUT)/main.o

qemu:
	qemu-system-arm -M versatilepb \
                    -m 128M \
                    -cpu cortex-m3 \
                    -nographic \
                    -monitor null \
                    -serial null \
                    -semihosting \
                    -kernel $(EXEC)

clean:
	rm -f $(OUT)/main.o $(OUT)/stack.o $(EXEC)
