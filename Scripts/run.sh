#!/bin/bash

# Go to project root if we're in Scripts folder
if [ -d "../boot" ]; then
    cd ..
fi

# Check if os.img exists
if [ ! -f "build/os.img" ]; then
    echo "Error: build/os.img not found!"
    echo "Please run ./Scripts/build.sh first"
    exit 1
fi

echo "Starting SEPPUKU OS in QEMU..."
echo "Press Ctrl+Alt+G to release mouse"
echo "Press Ctrl+C in terminal to quit"
echo ""

# Run QEMU with better options
qemu-system-i386 \
    -drive format=raw,file=build/os.img,index=0,if=floppy \
    -boot a \
    -m 32M \
    -monitor stdio
