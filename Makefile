CC = x86_64-elf-gcc
LD = x86_64-elf-ld
EMU = qemu-system-x86_64

KERNEL_CFLAGS := -c --freestanding -Wall -Wextra -pedantic -Isrc/kernel/include
KERNEL_LDFLAGS := -n -nostdlib --no-warn-rwx-segments -Tsrc/kernel/link.ld

KERNEL_OBJS = $(patsubst %.S, %.o, $(shell find src/kernel -name '*.S'))
KERNEL_OBJS += $(patsubst %.c, %.o, $(shell find src/kernel -name '*.c'))

.PHONY: all, run, debug, clean

all: image.iso

%.o: %.S
	$(CC) $(KERNEL_CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(KERNEL_CFLAGS) -o $@ $^

gigel-kernel: $(KERNEL_OBJS)
	$(LD) $(KERNEL_LDFLAGS) -o $@ $^

image.iso: gigel-kernel
	mkdir -p sysroot/boot
	cp -rf util/grub sysroot/boot
	cp -rf gigel-kernel sysroot/boot
	grub-mkrescue -o $@ sysroot

run: image.iso
	$(EMU) -smp 4 -m 512M -enable-kvm -cdrom $^ -no-reboot -serial stdio

debug: image.iso
	$(EMU) -smp 4 -m 512M -d int -cdrom $^ -no-reboot -serial stdio

clean:
	rm -rf $(KERNEL_OBJS)
	rm -rf gigel-kernel image.iso
	rm -rf serial.log sysroot
