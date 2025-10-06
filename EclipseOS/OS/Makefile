# === Directories ===
SRC     := src
BUILD   := build
INCLUDE := include

# === Tools ===
CC   := gcc
AS   := nasm
LD   := ld
QEMU := qemu-system-i386

# === Flags ===
CFLAGS  := -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -I$(INCLUDE) -Isrc/common -Isrc/tinyprintf
LDFLAGS := -m elf_i386 -T linker.ld

# === Files ===
SOURCES := $(wildcard $(SRC)/*.c $(SRC)/common/gest/*.c $(SRC)/common/Programs/*.c $(SRC)/common/updates/*.c)
OBJECTS := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))

BOOT       := boot
BOOT_BIN   := $(BUILD)/boot.bin
HEADER     := $(SRC)/multiboot_header.S
HEADER_OBJ := $(BUILD)/mb_header.o

KERNEL_BIN := $(BUILD)/kernel.bin
IMAGE      := $(BUILD)/os-image.bin
ISO        := $(BUILD)/EclipseOS.iso

# === Default Target ===
all: $(IMAGE)

# === Create build directory if needed ===
$(BUILD):
	@mkdir -p $(BUILD)

# === Bootloader ===
$(BOOT_BIN): $(BOOT)/boot.asm | $(BUILD)
	$(AS) -f bin $< -o $@

# === Multiboot Header ===
$(HEADER_OBJ): $(HEADER) | $(BUILD)
	$(CC) -m32 -c $< -o $@

# === Compile C sources ===
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# === Link kernel ===
$(KERNEL_BIN): $(HEADER_OBJ) $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# === Raw bootable image ===
$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

# === ISO bootable image ===
iso: $(ISO)

$(ISO): $(KERNEL_BIN) grub.cfg
	mkdir -p iso/boot/grub
	cp $< iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/
	grub2-mkrescue -o $@ iso

# === Run emulator ===
run: $(IMAGE)
	$(QEMU) -drive format=raw,file=$<

run-iso: iso $(DISK)
	$(QEMU) -cdrom $(ISO) -drive format=raw,file=$(DISK) -m 2048M -no-reboot -no-shutdown

# === Virtual disk ===
DISK := $(BUILD)/disk.img

disk: $(DISK)

$(DISK):
	dd if=/dev/zero of=$(DISK) bs=1M count=1024  # Create 1GB disk image

run-disk: $(IMAGE) $(DISK)
	$(QEMU) -drive format=raw,file=$(IMAGE) -drive format=raw,file=$(DISK)

# === Clean build ===
clean:
	rm -rf $(BUILD) iso

.PHONY: all clean run run-iso disk run-disk iso
