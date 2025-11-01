#!/bin/bash

# SEPPUKU OS Debug Run Script
# Shows verbose output for debugging

if [ -d "../boot" ]; then
    cd ..
fi

# Check if OS image exists
if [ ! -f "build/os.img" ]; then
    echo "❌ Error: build/os.img not found!"
    echo ""
    echo "Please build the OS first:"
    echo "  ./Scripts/build_no_fs.sh"
    exit 1
fi

echo "╔════════════════════════════════════════╗"
echo "║    SEPPUKU OS Debug Launcher v1.0     ║"
echo "╚════════════════════════════════════════╝"
echo ""

# Check file size
SIZE=$(stat -f%z "build/os.img" 2>/dev/null || stat -c%s "build/os.img" 2>/dev/null)
echo "OS Image Info:"
echo "  Path: build/os.img"
echo "  Size: $SIZE bytes"
echo ""

# Check if image has bootloader signature
BOOT_SIG=$(od -An -tx2 -N2 -j510 build/os.img | xargs)
if [ "$BOOT_SIG" = "aa55" ]; then
    echo "✓ Bootloader signature found (0xAA55)"
else
    echo "❌ WARNING: Invalid bootloader signature: $BOOT_SIG"
    echo "   Expected: aa55"
fi
echo ""

echo "Starting QEMU with verbose output..."
echo "Press Ctrl+C to quit"
echo "════════════════════════════════════════"
echo ""

# Run with serial output and error messages
qemu-system-i386 \
    -drive format=raw,file=build/os.img,index=0,media=disk,if=ide \
    -boot c \
    -m 128M \
    -vga std \
    -display gtk \
    -serial stdio \
    -name "SEPPUKU OS Debug" \
    -no-reboot \
    -d cpu_reset,guest_errors 2>&1

EXIT_CODE=$?

echo ""
echo "════════════════════════════════════════"
echo "QEMU exited with code: $EXIT_CODE"
echo ""

if [ $EXIT_CODE -ne 0 ]; then
    echo "❌ QEMU encountered an error!"
    echo ""
    echo "Common issues:"
    echo "  1. OS not built correctly - run: ./Scripts/build_no_fs.sh"
    echo "  2. Kernel panic or triple fault"
    echo "  3. Missing bootloader"
    echo ""
    echo "Try rebuilding:"
    echo "  ./Scripts/build_no_fs.sh"
    echo "  ./Scripts/run_debug.sh"
fi
