#!/bin/bash

# SEPPUKU OS Run Script
# Supports multiple resolutions and configurations

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

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo ""
echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║       SEPPUKU OS Launcher v1.0        ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════╝${NC}"
echo ""

# Check if resolution argument provided
if [ -n "$1" ]; then
    RESOLUTION=$1
else
    echo -e "${YELLOW}Select Resolution:${NC}"
    echo "  1) 1024x768   (XGA - Default, Most Compatible)"
    echo "  2) 1280x1024  (SXGA - Good Performance)"
    echo "  3) 1920x1080  (Full HD - Best Visual)"
    echo "  4) 800x600    (SVGA - Low Res)"
    echo ""
    read -p "Enter choice [1-4] (or press Enter for default): " choice
    
    case $choice in
        2) RESOLUTION="1280x1024" ;;
        3) RESOLUTION="1920x1080" ;;
        4) RESOLUTION="800x600" ;;
        *) RESOLUTION="1024x768" ;;
    esac
fi

echo ""
echo -e "${GREEN}✓${NC} Starting SEPPUKU OS at ${BLUE}${RESOLUTION}${NC}"
echo ""
echo -e "${YELLOW}Quick Controls:${NC}"
echo "  • Ctrl+Alt+F     - Toggle fullscreen"
echo "  • Ctrl+Alt+G     - Release mouse grab"
echo "  • Ctrl+C         - Quit QEMU"
echo ""
echo -e "${CYAN}Starting in 2 seconds...${NC}"
sleep 2

# Determine QEMU display settings based on resolution
case $RESOLUTION in
    "1920x1080")
        SCALE="1.0"
        ;;
    "1280x1024")
        SCALE="1.0"
        ;;
    "1024x768")
        SCALE="1.0"
        ;;
    "800x600")
        SCALE="1.2"
        ;;
    *)
        SCALE="1.0"
        ;;
esac

# Check if KVM is available for better performance
KVM_OPTION=""
if [ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ]; then
    KVM_OPTION="-enable-kvm"
    echo -e "${GREEN}✓${NC} KVM acceleration enabled"
else
    echo -e "${YELLOW}⚠${NC} Running without KVM (slower)"
fi

echo ""
echo -e "${GREEN}════════════════════════════════════════${NC}"
echo ""

# Run QEMU with optimized settings
qemu-system-i386 \
    $KVM_OPTION \
    -drive format=raw,file=build/os.img,index=0,media=disk,if=ide \
    -boot c \
    -m 128M \
    -vga std \
    -display gtk,zoom-to-fit=on,gl=on \
    -name "SEPPUKU OS - $RESOLUTION" \
    -rtc base=localtime \
    -cpu max \
    -soundhw pcspk 2>/dev/null

echo ""
echo -e "${CYAN}SEPPUKU OS terminated.${NC}"
echo ""
