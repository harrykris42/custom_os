# Common settings
CC = gcc
LD = ld
CFLAGS_64 = -m64 -nostdinc -fno-pic -ffreestanding -c -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ASM = nasm

# 64-bit targets
C_SOURCES_64 = src/kernel/kernel64.c \
               src/kernel/util.c \
               src/memory/physical.c \
               src/memory/kmalloc.c \
               src/kernel/low_level.c \
               src/cpu/interrupts.c \
               src/drivers/timer.c \
               src/drivers/keyboard.c \
               src/drivers/screen64.c \
               src/terminal/terminal64.c

ASM_SOURCES_64 = src/cpu/interrupt_stubs.asm
HEADERS_64 = $(wildcard src/include/*.h src/memory/*.h src/cpu/*.h src/drivers/*.h src/terminal/*.h)

# Default target
all: os-image-64

# Compile the ASM files
boot-64.bin: src/boot/boot64.asm
	$(ASM) -f bin src/boot/boot64.asm -o boot-64.bin

kernel_entry-64.o: src/kernel/kernel_entry_64.asm
	$(ASM) -f elf64 src/kernel/kernel_entry_64.asm -o kernel_entry-64.o

interrupt_stubs.o: src/cpu/interrupt_stubs.asm
	$(ASM) -f elf64 src/cpu/interrupt_stubs.asm -o interrupt_stubs.o

# Compile C files
kernel64.o: src/kernel/kernel64.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/kernel/kernel64.c -o kernel64.o

physical.o: src/memory/physical.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/memory/physical.c -o physical.o

kmalloc.o: src/memory/kmalloc.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/memory/kmalloc.c -o kmalloc.o

low_level.o: src/kernel/low_level.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/kernel/low_level.c -o low_level.o

interrupts.o: src/cpu/interrupts.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/cpu/interrupts.c -o interrupts.o

timer.o: src/drivers/timer.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/drivers/timer.c -o timer.o

keyboard.o: src/drivers/keyboard.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/drivers/keyboard.c -o keyboard.o

screen64.o: src/drivers/screen64.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/drivers/screen64.c -o screen64.o

terminal64.o: src/terminal/terminal64.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/terminal/terminal64.c -o terminal64.o

util.o: src/kernel/util.c $(HEADERS_64)
	$(CC) $(CFLAGS_64) src/kernel/util.c -o util.o

# Link everything together
kernel-64.bin: kernel_entry-64.o interrupt_stubs.o kernel64.o physical.o kmalloc.o low_level.o interrupts.o timer.o keyboard.o screen64.o terminal64.o util.o
	$(LD) -m elf_x86_64 -o kernel-64.bin -Ttext 0x1000 kernel_entry-64.o interrupt_stubs.o kernel64.o physical.o kmalloc.o low_level.o interrupts.o timer.o keyboard.o screen64.o terminal64.o util.o --oformat binary

os-image-64: boot-64.bin kernel-64.bin
	dd if=/dev/zero of=os-image-64.bin bs=512 count=2880
	dd if=boot-64.bin of=os-image-64.bin conv=notrunc
	dd if=kernel-64.bin of=os-image-64.bin seek=1 conv=notrunc

run: os-image-64
	qemu-system-x86_64 -drive file=os-image-64.bin,format=raw,index=0,if=floppy -m 256M -monitor stdio

# Clean targets
clean:
	rm -f *.bin *.o *.dis
	rm -f src/kernel/*.o
	rm -f src/drivers/*.o
	rm -f src/cpu/*.o
	rm -f src/shell/*.o
	rm -f src/memory/*.o
	rm -f src/terminal/*.o