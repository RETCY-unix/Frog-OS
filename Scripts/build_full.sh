#!/bin/bash

echo "Building SEPPUKU OS with full features..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/10] Assembling bootloader..."
nasm -f bin boot/boot.asm -o build/boot.bin

echo "[2/10] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/10] Compiling screen driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/screen.c -o build/screen.o

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
   build/kernel.o build/screen.o build/keyboard.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

echo "[10/10] Creating disk image..."
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

echo ""
echo "=========================================="
echo "Build complete!"
echo "=========================================="
ls -lh build/boot.bin build/kernel.bin build/os.img 2>/dev/null
echo ""
echo "Bootloader size: $(stat -c%s build/boot.bin 2>/dev/null) bytes"
echo "Kernel size: $(stat -c%s build/kernel.bin 2>/dev/null) bytes"
echo ""
echo "Run with: ./Scripts/run.sh"
echo ""
