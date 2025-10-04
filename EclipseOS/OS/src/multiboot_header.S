.section .multiboot
.align 8

#define MULTIBOOT2_HEADER_MAGIC  0xe85250d6
#define MULTIBOOT2_ARCHITECTURE 0
#define MULTIBOOT2_HEADER_LENGTH 16
#define MULTIBOOT2_CHECKSUM -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

.long MULTIBOOT2_HEADER_MAGIC
.long MULTIBOOT2_ARCHITECTURE
.long MULTIBOOT2_HEADER_LENGTH
.long MULTIBOOT2_CHECKSUM


	# Set up the stack and call kernel_main
.section .text
.global _start
_start:
	mov $0x20000, %esp   # Set up stack pointer (adjust address as needed)
	call kernel_main
	cli
	hlt
