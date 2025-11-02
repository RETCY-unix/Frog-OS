#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS Build WITHOUT File System ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Change to project root if in Scripts directory
if [ -d "../boot" ]; then
    cd ..
fi

# Clean build
echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null
mkdir -p build

echo ""
echo "[1/10] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin
if [ $? -ne 0 ]; then
    echo "❌ Bootloader assembly failed!"
    exit 1
fi

echo "[2/10] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o
if [ $? -ne 0 ]; then
    echo "❌ IDT assembly failed!"
    exit 1
fi

echo "[3/10] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o
if [ $? -ne 0 ]; then
    echo "❌ Graphics compilation failed!"
    exit 1
fi

echo "[4/10] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o
if [ $? -ne 0 ]; then
    echo "❌ Keyboard compilation failed!"
    exit 1
fi

echo "[5/10] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o
if [ $? -ne 0 ]; then
    echo "❌ Shell compilation failed!"
    exit 1
fi

echo "[6/10] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o
if [ $? -ne 0 ]; then
    echo "❌ ISR compilation failed!"
    exit 1
fi

echo "[7/10] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o
if [ $? -ne 0 ]; then
    echo "❌ IDT compilation failed!"
    exit 1
fi

echo "[8/10] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o
if [ $? -ne 0 ]; then
    echo "❌ Kernel compilation failed!"
    exit 1
fi

echo "[9/10] Linking kernel (NO FILE SYSTEM)..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/keyboard.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Kernel linking failed!"
    exit 1
fi

echo "[10/10] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img
truncate -s 1474560 build/os.img

echo ""
echo "════════════════════════════════════════"
echo "✓ Build successful (NO file system)"
echo "════════════════════════════════════════"
echo ""
echo "This build EXCLUDES file system to test keyboard"
echo ""
echo "Ready to run:"
echo "  ./Scripts/run_debug.sh"
echo ""
