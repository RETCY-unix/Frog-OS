#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS Build with File System   ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Change to project root if in Scripts directory
if [ -d "../boot" ]; then
    cd ..
fi

# Verify we're in the right directory
if [ ! -d "boot" ] || [ ! -d "Kernel" ]; then
    echo "❌ Error: Must run from project root or Scripts directory"
    echo "Current directory: $(pwd)"
    exit 1
fi

# Clean build
echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null
mkdir -p build

echo ""
echo "[1/13] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin
if [ $? -ne 0 ]; then
    echo "❌ Bootloader assembly failed!"
    exit 1
fi

echo "[2/13] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o
if [ $? -ne 0 ]; then
    echo "❌ IDT assembly failed!"
    exit 1
fi

echo "[3/13] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o
if [ $? -ne 0 ]; then
    echo "❌ Graphics compilation failed!"
    exit 1
fi

echo "[4/13] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o
if [ $? -ne 0 ]; then
    echo "❌ Keyboard compilation failed!"
    exit 1
fi

echo "[5/13] Compiling file system stubs..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/fs/fs_stubs.c -o build/fs_stubs.o
if [ $? -ne 0 ]; then
    echo "❌ FS stubs compilation failed!"
    exit 1
fi

echo "[7/13] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o
if [ $? -ne 0 ]; then
    echo "❌ Shell compilation failed!"
    exit 1
fi

echo "[8/13] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o
if [ $? -ne 0 ]; then
    echo "❌ ISR compilation failed!"
    exit 1
fi

echo "[9/13] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o
if [ $? -ne 0 ]; then
    echo "❌ IDT compilation failed!"
    exit 1
fi

echo "[10/13] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o
if [ $? -ne 0 ]; then
    echo "❌ Kernel compilation failed!"
    exit 1
fi

echo "[11/13] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/keyboard.o build/fs_stubs.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Kernel linking failed!"
    exit 1
fi

echo "[12/13] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

echo "[13/13] Padding to 1.44MB..."
truncate -s 1474560 build/os.img

echo ""
echo "════════════════════════════════════════"
echo "✓ Build successful with file system!"
echo "════════════════════════════════════════"
echo ""

# Show file sizes
echo "Build artifacts:"
ls -lh build/*.bin build/os.img 2>/dev/null | awk '{print "  " $9 " - " $5}'

echo ""
echo "New features added:"
echo "  ✓ File system commands (stubs for now)"
echo "  ✓ mount, ls, cat, write, rm"
echo "  ✓ Safe linking - no crashes!"
echo ""
echo "Ready to run!"
echo "  ./Scripts/run_debug.sh"
echo ""
