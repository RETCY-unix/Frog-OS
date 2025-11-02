#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS Build with Mouse Support ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

# Clean build
echo "Cleaning old build..."
rm -rf build/*.o build/*.bin build/os.img 2>/dev/null

mkdir -p build

echo ""
echo "[1/12] Assembling bootloader..."
nasm -f bin boot/boot_vesa.asm -o build/boot.bin
if [ $? -ne 0 ]; then
    echo "❌ Bootloader assembly failed!"
    exit 1
fi

echo "[2/12] Assembling IDT handlers..."
nasm -f elf32 Kernel/idt.asm -o build/idt_asm.o
if [ $? -ne 0 ]; then
    echo "❌ IDT assembly failed!"
    exit 1
fi

echo "[3/12] Compiling graphics driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/graphics.c -o build/graphics.o
if [ $? -ne 0 ]; then
    echo "❌ Graphics compilation failed!"
    exit 1
fi

echo "[4/12] Compiling keyboard driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/keyboard.c -o build/keyboard.o
if [ $? -ne 0 ]; then
    echo "❌ Keyboard compilation failed!"
    exit 1
fi

echo "[5/12] Compiling mouse driver..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/drivers/mouse.c -o build/mouse.o
if [ $? -ne 0 ]; then
    echo "❌ Mouse compilation failed!"
    exit 1
fi

echo "[6/12] Compiling shell..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c user/shell/shell.c -o build/shell.o
if [ $? -ne 0 ]; then
    echo "❌ Shell compilation failed!"
    exit 1
fi

echo "[7/12] Compiling ISR handler..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/isr.c -o build/isr.o
if [ $? -ne 0 ]; then
    echo "❌ ISR compilation failed!"
    exit 1
fi

echo "[8/12] Compiling IDT..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/idt.c -o build/idt.o
if [ $? -ne 0 ]; then
    echo "❌ IDT compilation failed!"
    exit 1
fi

echo "[9/12] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pie -fno-PIC -c Kernel/kernel.c -o build/kernel.o
if [ $? -ne 0 ]; then
    echo "❌ Kernel compilation failed!"
    exit 1
fi

echo "[10/12] Linking kernel..."
ld -m elf_i386 -Ttext 0x1000 --oformat binary \
   -e kernel_main \
   build/kernel.o build/graphics.o build/keyboard.o build/mouse.o build/shell.o \
   build/idt.o build/isr.o build/idt_asm.o \
   -o build/kernel.bin

if [ $? -ne 0 ]; then
    echo "❌ Kernel linking failed!"
    exit 1
fi

echo "[11/12] Creating OS image..."
cat build/boot.bin build/kernel.bin > build/os.img

echo "[12/12] Padding to 1.44MB..."
truncate -s 1474560 build/os.img

echo ""
echo "════════════════════════════════════════"
echo "✓ Build successful with mouse support!"
echo "════════════════════════════════════════"
echo ""

# Show file sizes
echo "Build artifacts:"
ls -lh build/*.bin build/os.img 2>/dev/null | awk '{print "  " $9 " - " $5}'

echo ""
echo "New features added:"
echo "  ✓ PS/2 Mouse driver"
echo "  ✓ Mouse cursor rendering"
echo "  ✓ Window dragging support"
echo "  ✓ Clickable window controls"
echo ""
echo "Ready to run!"
echo "  ./Scripts/run_debug.sh"
echo ""
