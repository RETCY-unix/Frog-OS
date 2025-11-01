#!/bin/bash

echo "Building SEPPUKU OS (Graphics Edition - No Filesystem)..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/10] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin

echo "[2/10] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/10] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o

echo "[4/10] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o

echo "[5/10] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o

echo "[6/10] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o

echo "[7/10] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o

echo "[8/10] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[9/10] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/keyboard.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

echo "[10/10] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

# Pad to 1.44MB
truncate -s 1474560 build/os.img

echo ""
echo "=========================================="
echo "Build complete! SEPPUKU OS (No FS)"
echo "=========================================="
echo ""
echo "Run with: qemu-system-i386 -drive format=raw,file=build/os.img"
echo ""
