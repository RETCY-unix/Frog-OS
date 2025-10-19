// SEPPUKU OS - Working Kernel
// NO HELPER FUNCTIONS - Direct VGA writes only

void kernel_main() {
    unsigned char* vga = (unsigned char*)0xB8000;
    int i;
    
    // Clear screen
    for (i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i] = ' ';
        vga[i + 1] = 0x0F;
    }
    
    // LINE 0: Top border
    const char* l0 = "================================================================================";
    for (i = 0; l0[i] != '\0'; i++) {
        vga[i * 2] = l0[i];
        vga[i * 2 + 1] = 0x0B; // Cyan
    }
    
    // LINE 1: Title
    const char* l1 = "                            SEPPUKU OS - v1.0                                  ";
    for (i = 0; l1[i] != '\0'; i++) {
        vga[160 + i * 2] = l1[i];
        vga[160 + i * 2 + 1] = 0x0C; // Light red
    }
    
    // LINE 2: Bottom border
    for (i = 0; l0[i] != '\0'; i++) {
        vga[320 + i * 2] = l0[i];
        vga[320 + i * 2 + 1] = 0x0B;
    }
    
    // LINE 3: Empty
    
    // LINE 4: Booting message
    const char* l4 = "  Booting system...";
    for (i = 0; l4[i] != '\0'; i++) {
        vga[640 + i * 2] = l4[i];
        vga[640 + i * 2 + 1] = 0x0E; // Yellow
    }
    
    // LINE 5: Empty
    
    // LINE 6: OK Message 1
    const char* l6 = "  [OK] Protected mode enabled";
    for (i = 0; l6[i] != '\0'; i++) {
        vga[960 + i * 2] = l6[i];
        vga[960 + i * 2 + 1] = 0x0A; // Green
    }
    
    // LINE 7: OK Message 2
    const char* l7 = "  [OK] VGA text mode (80x25) initialized";
    for (i = 0; l7[i] != '\0'; i++) {
        vga[1120 + i * 2] = l7[i];
        vga[1120 + i * 2 + 1] = 0x0A;
    }
    
    // LINE 8: OK Message 3
    const char* l8 = "  [OK] Kernel loaded at 0x1000";
    for (i = 0; l8[i] != '\0'; i++) {
        vga[1280 + i * 2] = l8[i];
        vga[1280 + i * 2 + 1] = 0x0A;
    }
    
    // LINE 9: OK Message 4
    const char* l9 = "  [OK] Screen driver active";
    for (i = 0; l9[i] != '\0'; i++) {
        vga[1440 + i * 2] = l9[i];
        vga[1440 + i * 2 + 1] = 0x0A;
    }
    
    // LINE 10: Empty
    
    // LINE 11: Success message
    const char* l11 = "  >>> BOOT SUCCESSFUL <<<";
    for (i = 0; l11[i] != '\0'; i++) {
        vga[1760 + i * 2] = l11[i];
        vga[1760 + i * 2 + 1] = 0x0A; // Bright green
    }
    
    // LINE 12: Empty
    
    // LINE 13: Status header
    const char* l13 = "  System Status:";
    for (i = 0; l13[i] != '\0'; i++) {
        vga[2080 + i * 2] = l13[i];
        vga[2080 + i * 2 + 1] = 0x0B; // Cyan
    }
    
    // LINE 14: Status info
    const char* l14 = "    - Kernel version: 1.0";
    for (i = 0; l14[i] != '\0'; i++) {
        vga[2240 + i * 2] = l14[i];
        vga[2240 + i * 2 + 1] = 0x0F;
    }
    
    // LINE 15: Status info
    const char* l15 = "    - Architecture: x86 (32-bit)";
    for (i = 0; l15[i] != '\0'; i++) {
        vga[2400 + i * 2] = l15[i];
        vga[2400 + i * 2 + 1] = 0x0F;
    }
    
    // LINE 16: Status info
    const char* l16 = "    - Memory: 32 MB";
    for (i = 0; l16[i] != '\0'; i++) {
        vga[2560 + i * 2] = l16[i];
        vga[2560 + i * 2 + 1] = 0x0F;
    }
    
    // LINE 17: Empty
    
    // LINE 18: Next steps
    const char* l18 = "  Next Steps:";
    for (i = 0; l18[i] != '\0'; i++) {
        vga[2880 + i * 2] = l18[i];
        vga[2880 + i * 2 + 1] = 0x0D; // Light magenta
    }
    
    // LINE 19: Todo
    const char* l19 = "    [ ] Add IDT and interrupts";
    for (i = 0; l19[i] != '\0'; i++) {
        vga[3040 + i * 2] = l19[i];
        vga[3040 + i * 2 + 1] = 0x07;
    }
    
    // LINE 20: Todo
    const char* l20 = "    [ ] Add keyboard driver";
    for (i = 0; l20[i] != '\0'; i++) {
        vga[3200 + i * 2] = l20[i];
        vga[3200 + i * 2 + 1] = 0x07;
    }
    
    // LINE 21: Todo
    const char* l21 = "    [ ] Implement shell";
    for (i = 0; l21[i] != '\0'; i++) {
        vga[3360 + i * 2] = l21[i];
        vga[3360 + i * 2 + 1] = 0x07;
    }
    
    // LINE 22: Empty
    
    // LINE 23: Footer
    const char* l23 = "  System halted. Press Ctrl+Alt+R to reboot QEMU.";
    for (i = 0; l23[i] != '\0'; i++) {
        vga[3680 + i * 2] = l23[i];
        vga[3680 + i * 2 + 1] = 0x0E; // Yellow
    }
    
    // Halt CPU
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
