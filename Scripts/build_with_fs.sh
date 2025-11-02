#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS Build with File System   ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null
mkdir -p build

echo ""
echo "[1/13] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin

echo "[2/13] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/13] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o

echo "[4/13] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o

echo "[5/13] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o

echo "[6/13] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o

echo "[7/13] Compiling ATA driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/ata.c -o build/ata.o

echo "[8/13] Compiling FAT12 filesystem..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/fs/fat12.c -o build/fat12.o

echo "[9/13] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o

echo "[10/13] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[11/13] Linking kernel with LINKER SCRIPT..."
ld -m elf_i386 -T Kernel/kernel.ld \
   build/kernel.o \
   build/idt_asm.o \
   build/idt.o \
   build/isr.o \
   build/keyboard.o \
   build/graphics.o \
   build/ata.o \
   build/fat12.o \
   build/shell.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

echo "[12/13] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

echo "[13/13] Padding to 1.44MB..."
truncate -s 1474560 build/os.img

echo ""
echo "✓ Build successful with file system + linker script!"
echo ""
