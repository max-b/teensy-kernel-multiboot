BUILD_DIR=./build
ISO_DIR=./isodir

BOOT_SRCS := $(wildcard *.asm)
BOOT_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(BOOT_SRCS))

KERNEL_SRCS := kernel.c
KERNEL_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(KERNEL_SRCS))

OS_BIN_FILE := myos.bin
OS_BIN := $(BUILD_DIR)/$(OS_BIN_FILE)
OS_ISO := myos.iso

all: $(BOOT_OBJS) $(KERNEL_OBJS)

$(BOOT_OBJS): $(BOOT_SRCS)
	nasm -felf32 $< -o $@

$(KERNEL_OBJS): $(KERNEL_SRCS)
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(OS_BIN): $(KERNEL_OBJS) $(BOOT_OBJS)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $< -lgcc

$(OS_ISO): $(OS_BIN)
	cp $< $(ISO_DIR)/boot/$(OS_BIN_FILE)
	cp grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ --verbose $(ISO_DIR)

.PHONY: run-qemu
run-qemu: $(OS_ISO)
	./check-grub.sh && qemu-system-i386 -cdrom $<

clean: 
	rm -f $(BUILD_DIR)/*
	rm -f $(ISO_DIR)/*
