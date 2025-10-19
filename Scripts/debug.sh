#!/bin/bash

echo "================================"
echo "SEPPUKU OS - Debug Information"
echo "================================"
echo ""

if [ -d "../boot" ]; then
    cd ..
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found!"
    exit 1
fi

echo "📁 Build Directory Contents:"
ls -lh build/
echo ""

# Check bootloader
if [ -f "build/boot.bin" ]; then
    size=$(stat -c%s build/boot.bin)
    echo "✅ Bootloader: $size bytes"
    
    if [ $size -eq 512 ]; then
        echo "   ✅ Correct size (512 bytes)"
    else
        echo "   ⚠️  Warning: Should be exactly 512 bytes"
    fi
    
    # Check for boot signature
    signature=$(xxd -s 510 -l 2 -p build/boot.bin)
    if [ "$signature" = "55aa" ]; then
        echo "   ✅ Boot signature present (0x55AA)"
    else
        echo "   ❌ Boot signature missing! Found: 0x$signature"
    fi
else
    echo "❌ boot.bin not found!"
fi

echo ""

# Check kernel
if [ -f "build/kernel.bin" ]; then
    size=$(stat -c%s build/kernel.bin)
    echo "✅ Kernel: $size bytes"
    
    # Calculate sectors needed
    sectors=$((($size + 511) / 512))
    echo "   📊 Requires $sectors sectors"
    
    if [ $sectors -gt 50 ]; then
        echo "   ⚠️  Warning: Kernel needs more than 50 sectors!"
        echo "   💡 Update boot.asm to load more sectors (currently: 50)"
    fi
else
    echo "❌ kernel.bin not found!"
fi

echo ""

# Check disk image
if [ -f "build/os.img" ]; then
    size=$(stat -c%s build/os.img)
    echo "✅ Disk Image: $size bytes"
    
    expected=$((2880 * 512))
    if [ $size -eq $expected ]; then
        echo "   ✅ Correct floppy size (1.44MB)"
    else
        echo "   ⚠️  Expected $expected bytes"
    fi
    
    # Check first sector
    echo ""
    echo "🔍 First 16 bytes of disk image:"
    xxd -l 16 build/os.img
else
    echo "❌ os.img not found!"
fi

echo ""
echo "================================"
