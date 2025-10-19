#!/bin/bash
qemu-system-i386 -drive format=raw,file=build/os.img,if=floppy
