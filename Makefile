BUILD_DIR=./build
ISO_DIR=./isodir

BOOT_SRCS := boot.asm
BOOT_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.asm.o, $(BOOT_SRCS))

INCLUDE_SRCS_ASM := io.asm interrupts.asm tables.asm
INCLUDE_OBJS_ASM := $(patsubst %.asm, $(BUILD_DIR)/%.asm.o, $(INCLUDE_SRCS_ASM))

KERNEL_SRCS := kernel.c io.c str.c serial.c tables.c interrupts.c
KERNEL_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(KERNEL_SRCS))

HEADERS = $(wildcard *.h)

OS_BIN_FILE := myos.bin
OS_BIN := $(BUILD_DIR)/$(OS_BIN_FILE)
OS_ISO := myos.iso

.PHONY: all
all: $(OS_ISO)

.PHONY: kernel
kernel: $(KERNEL_OBJS)

$(BUILD_DIR)/%.asm.o: %.asm
	nasm -felf32 $< -o $@

$(BUILD_DIR)/%.c.o: %.c $(HEADERS)
	i686-elf-gcc -c $< -g -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(OS_BIN): $(KERNEL_OBJS) $(BOOT_OBJS) $(INCLUDE_OBJS_ASM)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $? -lgcc

$(OS_ISO): $(OS_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $< $(ISO_DIR)/boot/$(OS_BIN_FILE)
	cp grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)

.PHONY: run-qemu
run-qemu: $(OS_ISO)
	./check-grub.sh && qemu-system-i386 -serial stdio -d guest_errors -cdrom $<

.PHONY: run-qemu-debug
run-qemu-debug: $(OS_ISO)
	./check-grub.sh && qemu-system-i386 -s -serial stdio -d guest_errors -cdrom $<

.PHONY: format
format:
	clang-format -i *.c && clang-format -i *.h

clean:
	rm -f $(BUILD_DIR)/*
	rm -rf $(ISO_DIR)/*
