#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS - TWM Build v2.0         ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null
mkdir -p build

echo ""
echo "[1/15] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin
if [ $? -ne 0 ]; then
    echo "❌ Bootloader failed!"
    exit 1
fi

echo "[2/15] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o
if [ $? -ne 0 ]; then
    echo "❌ IDT assembly failed!"
    exit 1
fi

echo "[3/15] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o
if [ $? -ne 0 ]; then
    echo "❌ ISR failed!"
    exit 1
fi

echo "[4/15] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o
if [ $? -ne 0 ]; then
    echo "❌ IDT failed!"
    exit 1
fi

echo "[5/15] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o
if [ $? -ne 0 ]; then
    echo "❌ Graphics failed!"
    exit 1
fi

echo "[6/15] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o
if [ $? -ne 0 ]; then
    echo "❌ Keyboard failed!"
    exit 1
fi

echo "[7/15] Compiling sound driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/sound.c -o build/sound.o
if [ $? -ne 0 ]; then
    echo "❌ Sound failed!"
    exit 1
fi

echo "[8/15] Compiling ATA driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/ata.c -o build/ata.o
if [ $? -ne 0 ]; then
    echo "❌ ATA failed!"
    exit 1
fi

echo "[9/15] Compiling FAT12 filesystem..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/fs/fat12.c -o build/fat12.o
if [ $? -ne 0 ]; then
    echo "❌ FAT12 failed!"
    exit 1
fi

echo "[10/15] Compiling TWM (Tiling Window Manager)..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/twm/twm.c -o build/twm.o
if [ $? -ne 0 ]; then
    echo "❌ TWM failed!"
    exit 1
fi

echo "[11/15] Compiling shell (TWM integrated)..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o
if [ $? -ne 0 ]; then
    echo "❌ Shell failed!"
    exit 1
fi

echo "[12/15] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o
if [ $? -ne 0 ]; then
    echo "❌ Kernel failed!"
    exit 1
fi

echo "[13/15] Linking kernel with TWM..."
ld -m elf_i386 -T Kernel/kernel.ld \
   build/kernel.o \
   build/idt_asm.o \
   build/idt.o \
   build/isr.o \
   build/keyboard.o \
   build/sound.o \
   build/graphics.o \
   build/ata.o \
   build/fat12.o \
   build/twm.o \
   build/shell.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

echo "[14/15] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

echo "[15/15] Padding to 1.44MB..."
truncate -s 1474560 build/os.img

echo ""
echo "╔════════════════════════════════════════╗"
echo "║  ✓ BUILD SUCCESSFUL - TWM EDITION!    ║"
echo "╚════════════════════════════════════════╝"
echo ""
echo "NEW: Tiling Window Manager (i3/hyprland style)"
echo ""
echo "Keyboard Shortcuts:"
echo "  ESC+1-5    - Switch workspace"
echo "  ESC+Enter  - New terminal tile"
echo "  ESC+J/K    - Focus next/prev tile"
echo "  ESC+Q      - Close focused tile"
echo ""
echo "Run with: ./Scripts/run_debug.sh"
echo ""
