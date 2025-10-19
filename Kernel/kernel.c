#include "../Lib/include/screen.h"
#include "../Lib/include/idt.h"
#include "../Lib/include/keyboard.h"
#include "../user/shell/shell.h"

void kernel_main() {
    screen_init();
    
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("================================================================================");
    screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    screen_println("                            SEPPUKU OS - v1.2                                   ");
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("================================================================================");
    screen_println("");
    
    screen_set_color(COLOR_YELLOW, COLOR_BLACK);
    screen_println("Initializing system...");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_print("[OK] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("Protected mode enabled");
    
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_print("[OK] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("VGA text mode initialized");
    
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_print("[OK] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("Screen driver loaded");
    
    screen_set_color(COLOR_YELLOW, COLOR_BLACK);
    screen_print("[...] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print("Initializing IDT...");
    idt_init();
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_println(" OK");
    
    screen_set_color(COLOR_YELLOW, COLOR_BLACK);
    screen_print("[...] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print("Initializing keyboard...");
    keyboard_init();
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_println(" OK");
    
    screen_set_color(COLOR_YELLOW, COLOR_BLACK);
    screen_print("[...] ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print("Starting shell...");
    screen_set_color(COLOR_GREEN, COLOR_BLACK);
    screen_println(" OK");
    
    screen_println("");
    screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_println(">>> BOOT SUCCESSFUL <<<");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    
    // Wait a moment (simple delay)
    for (volatile int i = 0; i < 50000000; i++);
    
    // Clear screen before starting shell
    screen_clear();
    
    // Start shell with clean screen
    shell_run();
}
