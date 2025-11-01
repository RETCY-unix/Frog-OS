#!/bin/bash

if [ -d "../boot" ]; then
    cd ..
fi

if [ ! -f "build/os.img" ]; then
    echo "Error: build/os.img not found!"
    echo "Please run ./Scripts/build_filesystem.sh first"
    exit 1
fi

echo "Starting SEPPUKU OS with File System..."
echo "Press Ctrl+Alt+F to toggle fullscreen"
echo "Press Ctrl+Alt+G to release mouse"
echo "Press Ctrl+C to quit"
echo ""

# Run with disk as IDE drive (not floppy)
qemu-system-i386 \
    -drive format=raw,file=build/os.img,index=0,media=disk,if=ide \
    -boot c \
    -m 64M \
    -vga std \
    -display gtk,zoom-to-fit=on
