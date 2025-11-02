#!/bin/bash

if [ -d "../boot" ]; then
    cd ..
fi

echo "Starting SEPPUKU OS with PC Speaker audio..."
echo ""

qemu-system-i386 \
    -drive format=raw,file=build/os.img,index=0,media=disk,if=ide \
    -boot c \
    -m 128M \
    -vga std \
    -display gtk \
    -audiodev id=audio0,driver=pa \
    -machine pcspk-audiodev=audio0 \
    -name "SEPPUKU OS Sound Test"
