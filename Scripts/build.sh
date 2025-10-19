#!/bin/bash

echo "Building Custom OS..."

mkdir -p build

echo "[1/5] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/5] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[3/5] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary -e kernel_main build/kernel.o -o build/kernel.bin

echo "[4/5] Creating disk image..."
# Create a 1.44MB floppy image
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null

# Write bootloader to first sector
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null

# Write kernel starting at sector 2
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

echo "[5/5] Build complete!"
ls -lh build/os.img
