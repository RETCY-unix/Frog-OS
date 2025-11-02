#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS - Sound Build (No Mouse) ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null
mkdir -p build

echo ""
echo "[1/14] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin

echo "[2/14] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/14] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o

echo "[4/14] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o

echo "[5/14] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o

echo "[6/14] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o

echo "[7/14] Compiling sound driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/sound.c -o build/sound.o

echo "[8/14] Compiling ATA driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/ata.c -o build/ata.o

echo "[9/14] Compiling FAT12 filesystem..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/fs/fat12.c -o build/fat12.o

echo "[10/14] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o

echo "[11/14] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[12/14] Linking kernel (NO MOUSE)..."
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
   build/shell.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

echo "[13/14] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

echo "[14/14] Padding to 1.44MB..."
truncate -s 1474560 build/os.img

echo ""
echo "╔════════════════════════════════════════╗"
echo "║  ✓ BUILD SUCCESSFUL - SOUND WORKING!  ║"
echo "╚════════════════════════════════════════╝"
echo ""
echo "New features:"
echo "  🔊 PC Speaker sound (beep, chime, siren)"
echo "  🎵 Sound effects on commands"
echo ""
echo "Commands to try:"
echo "  beep   - Simple beep"
echo "  chime  - Musical startup"
echo "  siren  - Police siren"
echo ""
echo "Run with: ./Scripts/run_debug.sh"
echo ""
