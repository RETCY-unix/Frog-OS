#!/bin/bash

# Go to project root if we're in Scripts folder
if [ -d "../boot" ]; then
    cd ..
fi

qemu-system-i386 -drive format=raw,file=build/os.img,if=floppy
