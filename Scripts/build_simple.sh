#!/bin/bash

echo "Building MINIMAL SEPPUKU OS..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/4] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/4] Compiling kernel (standalone)..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -nostdlib -c Kernel/kernel.c -o build/kernel.o

echo "[3/4] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o \
   -o build/kernel.bin

echo "[4/4] Creating disk image..."
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

echo ""
echo "=========================================="
echo "Build complete!"
echo "=========================================="
ls -lh build/boot.bin build/kernel.bin 2>/dev/null
echo ""
echo "Bootloader size: $(stat -c%s build/boot.bin 2>/dev/null || echo 'N/A') bytes"
echo "Kernel size: $(stat -c%s build/kernel.bin 2>/dev/null || echo 'N/A') bytes"
echo ""

# Disassemble first few instructions to verify
echo "First kernel instructions:"
objdump -D -b binary -m i386 -M intel build/kernel.bin 2>/dev/null | head -20

echo ""
echo "Run with: ./Scripts/run.sh"
echo ""
