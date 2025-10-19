#!/bin/bash

echo "Building TEST kernel..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/4] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/4] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[3/4] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary -e kernel_main build/kernel.o -o build/kernel.bin

echo "[4/4] Creating disk image..."
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

echo ""
echo "Build complete!"
ls -lh build/kernel.bin
echo ""
