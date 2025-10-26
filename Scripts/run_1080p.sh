#!/bin/bash

if [ -d "../boot" ]; then
    cd ..
fi

if [ ! -f "build/os.img" ]; then
    echo "Error: build/os.img not found!"
    echo "Please run ./Scripts/build_1080p.sh first"
    exit 1
fi

echo "Starting SEPPUKU OS in 1080p mode..."
echo "Press Ctrl+Alt+F to toggle fullscreen"
echo "Press Ctrl+Alt+G to release mouse"
echo "Press Ctrl+C to quit"
echo ""

qemu-system-i386 \
    -drive format=raw,file=build/os.img,index=0,if=floppy \
    -boot a \
    -m 64M \
    -vga std \
    -display gtk,zoom-to-fit=on \
    -full-screen
