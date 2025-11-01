#!/bin/bash

echo "╔════════════════════════════════════════╗"
echo "║   SEPPUKU OS Build Verification       ║"
echo "╚════════════════════════════════════════╝"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

# Check if kernel.c has filesystem references
echo "Checking kernel.c..."
if grep -q "fat12_init" Kernel/kernel.c 2>/dev/null; then
    echo "❌ ERROR: kernel.c still has filesystem code!"
    echo "   You need to replace Kernel/kernel.c with the version I provided"
    echo ""
    echo "The kernel.c should NOT have these lines:"
    echo "  - #include \"../Lib/include/ata.h\""
    echo "  - #include \"../Lib/include/fat12.h\""
    echo "  - ata_init();"
    echo "  - fat12_init();"
    echo ""
    exit 1
else
    echo "✓ kernel.c looks clean (no filesystem references)"
fi

# Check if shell.c has filesystem references  
echo "Checking shell.c..."
if grep -q "fat12_read_file" user/shell/shell.c 2>/dev/null; then
    echo "❌ ERROR: shell.c still has filesystem code!"
    echo "   You need to replace user/shell/shell.c with the version I provided"
    echo ""
    echo "The shell.c should NOT have:"
    echo "  - #include \"../../Lib/include/fat12.h\""
    echo "  - ls, cat, rm, touch commands"
    echo ""
    exit 1
else
    echo "✓ shell.c looks clean (no filesystem references)"
fi

echo ""
echo "════════════════════════════════════════"
echo "Files verified! Now building..."
echo "════════════════════════════════════════"
echo ""

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

echo "[9/10] Linking kernel..."
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

# Pad to 1.44MB
truncate -s 1474560 build/os.img

echo ""
echo "════════════════════════════════════════"
echo "✓ Build successful!"
echo "════════════════════════════════════════"
echo ""

# Show file sizes
echo "Build artifacts:"
ls -lh build/*.bin build/os.img 2>/dev/null | awk '{print "  " $9 " - " $5}'

echo ""
echo "Ready to run!"
echo "  ./Scripts/run_debug.sh"
echo ""
