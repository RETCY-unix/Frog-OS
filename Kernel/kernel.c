#include "../Lib/include/screen.h"

// We'll test WITHOUT IDT and keyboard first

int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void kernel_main() {
    // Initialize screen driver
    screen_init();
    
    // Display welcome banner
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("========================================");
    screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    screen_println("       SEPPUKU OS - v0.1");
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("========================================");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    
    screen_set_color(COLOR_YELLOW, COLOR_BLACK);
    screen_println("Initializing system...");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_print("[OK] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("VGA text mode initialized");
    
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_print("[OK] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("Screen driver loaded");
    
    screen_println("");
    screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_println("SUCCESS! Basic kernel is working!");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    screen_println("Next steps:");
    screen_println("  - Add IDT (interrupts)");
    screen_println("  - Add keyboard driver");
    screen_println("  - Add shell");
    
    // Halt the CPU
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
