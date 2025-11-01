#!/bin/bash

echo "Building SEPPUKU OS with FAT12 File System..."

if [ -d "../boot" ]; then
    cd ..
fi

mkdir -p build

echo "[1/13] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin

echo "[2/13] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o

echo "[3/13] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o

echo "[4/13] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o

echo "[5/13] Compiling ATA disk driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/ata.c -o build/ata.o

echo "[6/13] Compiling FAT12 file system..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/fs/fat12.c -o build/fat12.o

echo "[7/13] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o

echo "[8/13] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o

echo "[9/13] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o

echo "[10/13] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o

echo "[11/13] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/keyboard.o build/ata.o build/fat12.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

echo "[12/13] Creating FAT12 disk image..."
# Create a 1.44MB floppy image with FAT12
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null

# Format as FAT12
mkfs.fat -F 12 -n "SEPPUKUOS" build/os.img >/dev/null 2>&1

# Write bootloader and kernel
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

echo "[13/13] Creating test files..."
# Mount the image temporarily and add test files
mkdir -p build/mnt 2>/dev/null
if command -v sudo &> /dev/null; then
    sudo mount -o loop build/os.img build/mnt 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "Hello from SEPPUKU OS!" | sudo tee build/mnt/readme.txt >/dev/null 2>&1
        echo "This is a test file" | sudo tee build/mnt/test.txt >/dev/null 2>&1
        sudo umount build/mnt 2>/dev/null
    fi
fi
rmdir build/mnt 2>/dev/null

echo ""
echo "=========================================="
echo "Build complete! SEPPUKU OS File System"
echo "=========================================="
echo ""
echo "Features:"
echo "  - FAT12 file system"
echo "  - ATA disk driver"
echo "  - File operations: ls, cat, echo >, rm, touch"
echo ""
echo "Run with: ./Scripts/run_1080p.sh"
echo ""
