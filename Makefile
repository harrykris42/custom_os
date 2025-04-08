# Common settings
CC = gcc
LD = ld
CFLAGS_32 = -m32 -fno-pic -ffreestanding -c
CFLAGS_64 = -m64 -nostdinc -fno-pic -ffreestanding -c -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ASM = nasm

# Defaults to 32-bit if not specified
all: os-image-32

# 32-bit targets
C_SOURCES_32 = $(wildcard src/kernel/*.c src/drivers/*.c src/cpu/*.c src/shell/*.c)
HEADERS_32 = $(wildcard src/kernel/*.h src/drivers/*.h src/cpu/*.h src/shell/*.h)
OBJ_32 = ${C_SOURCES_32:.c=.o}

boot-32.bin: src/boot/boot.asm
	$(ASM) -f bin src/boot/boot.asm -o boot-32.bin

kernel_entry-32.o: src/kernel/kernel_entry.asm
	$(ASM) -f elf src/kernel/kernel_entry.asm -o kernel_entry-32.o

interrupt-32.o: src/cpu/interrupt.asm
	$(ASM) -f elf src/cpu/interrupt.asm -o interrupt-32.o

%.o: %.c ${HEADERS_32}
	$(CC) $(CFLAGS_32) $< -o $@

kernel-32.bin: kernel_entry-32.o interrupt-32.o ${OBJ_32}
	$(LD) -m elf_i386 -o kernel-32.bin -Ttext 0x1000 kernel_entry-32.o interrupt-32.o ${OBJ_32} --oformat binary

os-image-32: boot-32.bin kernel-32.bin
	dd if=/dev/zero of=os-image-32.bin bs=512 count=2880
	dd if=boot-32.bin of=os-image-32.bin conv=notrunc
	dd if=kernel-32.bin of=os-image-32.bin seek=1 conv=notrunc

run-32: os-image-32
	qemu-system-i386 -drive file=os-image-32.bin,format=raw,index=0,if=floppy

# 64-bit targets
C_SOURCES_64 = src/kernel/kernel64.c \
               src/memory/physical.c \
               src/memory/kmalloc.c \
               src/kernel/low_level.c \
               src/cpu/interrupts.c \
               src/drivers/timer.c \
               src/drivers/keyboard.c

ASM_SOURCES_64 = src/cpu/interrupt_stubs.asm
HEADERS_64 = $(wildcard src/include/*.h src/memory/*.h)
OBJ_64 = ${C_SOURCES_64:.c=.o}

# Compile the ASM files
boot-64.bin: src/boot/boot64.asm
	$(ASM) -f bin src/boot/boot64.asm -o boot-64.bin

kernel_entry-64.o: src/kernel/kernel_entry_64.asm
	$(ASM) -f elf64 src/kernel/kernel_entry_64.asm -o kernel_entry-64.o

interrupt_stubs.o: src/cpu/interrupt_stubs.asm
	$(ASM) -f elf64 src/cpu/interrupt_stubs.asm -o interrupt_stubs.o

# Compile C files
kernel64.o: src/kernel/kernel64.c
	$(CC) $(CFLAGS_64) src/kernel/kernel64.c -o kernel64.o

physical.o: src/memory/physical.c
	$(CC) $(CFLAGS_64) src/memory/physical.c -o physical.o

kmalloc.o: src/memory/kmalloc.c
	$(CC) $(CFLAGS_64) src/memory/kmalloc.c -o kmalloc.o

low_level.o: src/kernel/low_level.c
	$(CC) $(CFLAGS_64) src/kernel/low_level.c -o low_level.o

interrupts.o: src/cpu/interrupts.c
	$(CC) $(CFLAGS_64) src/cpu/interrupts.c -o interrupts.o

timer.o: src/drivers/timer.c
	$(CC) $(CFLAGS_64) src/drivers/timer.c -o timer.o

keyboard.o: src/drivers/keyboard.c
	$(CC) $(CFLAGS_64) src/drivers/keyboard.c -o keyboard.o

# Link everything together
kernel-64.bin: kernel_entry-64.o interrupt_stubs.o kernel64.o physical.o kmalloc.o low_level.o interrupts.o timer.o keyboard.o
	$(LD) -m elf_x86_64 -o kernel-64.bin -Ttext 0x1000 kernel_entry-64.o interrupt_stubs.o kernel64.o physical.o kmalloc.o low_level.o interrupts.o timer.o keyboard.o --oformat binary

os-image-64: boot-64.bin kernel-64.bin
	dd if=/dev/zero of=os-image-64.bin bs=512 count=2880
	dd if=boot-64.bin of=os-image-64.bin conv=notrunc
	dd if=kernel-64.bin of=os-image-64.bin seek=1 conv=notrunc

run-64: os-image-64
	qemu-system-x86_64 -drive file=os-image-64.bin,format=raw,index=0,if=floppy -m 256M

# Clean targets
clean:
	rm -f *.bin *.o *.dis
	rm -f src/kernel/*.o
	rm -f src/drivers/*.o
	rm -f src/cpu/*.o
	rm -f src/shell/*.o
	rm -f src/memory/*.o
