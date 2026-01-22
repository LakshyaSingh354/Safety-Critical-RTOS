CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m3 -mthumb -ffreestanding -nostdlib -O0 -g


SRC := $(shell find . -name '*.c')
ASM := $(shell find . -name '*.s')
OBJ := $(SRC:.c=.o) $(ASM:.s=.o)

LD := boot/linker.ld
OUT := firmware.elf

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -T $(LD) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)