#include "../../Lib/include/screen.h"
#include "../../Lib/include/keyboard.h"

#define MAX_COMMAND_LENGTH 256

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;

// String compare
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// String length
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// String starts with
int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

// Print prompt
void shell_prompt() {
    screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_print("root@seppuku");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print(":");
    screen_set_color(COLOR_LIGHT_BLUE, COLOR_BLACK);
    screen_print("~");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print("$ ");
}

// Execute command
void shell_execute(const char* cmd) {
    // Skip empty commands
    if (cmd[0] == '\0') {
        return;
    }
    
    // HELP command
    if (strcmp(cmd, "help") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("Available commands:");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("  help    - Show this help message");
        screen_println("  clear   - Clear the screen");
        screen_println("  echo    - Echo text back");
        screen_println("  about   - About SEPPUKU OS");
        screen_println("  reboot  - Reboot the system");
        return;
    }
    
    // CLEAR command
    if (strcmp(cmd, "clear") == 0) {
        screen_clear();
        return;
    }
    
    // ABOUT command
    if (strcmp(cmd, "about") == 0) {
        screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
        screen_println("SEPPUKU OS v1.2");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("A minimalist x86 operating system");
        screen_println("Built from scratch in C and Assembly");
        screen_println("");
        screen_println("Features:");
        screen_println("  - Protected mode");
        screen_println("  - IDT and interrupts");
        screen_println("  - Keyboard driver");
        screen_println("  - VGA text mode");
        screen_println("  - Basic shell");
        return;
    }
    
    // ECHO command
    if (starts_with(cmd, "echo ")) {
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_println(cmd + 5);
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // REBOOT command
    if (strcmp(cmd, "reboot") == 0) {
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_println("Rebooting...");
        // Reboot via keyboard controller
        unsigned char temp;
        __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        while (temp & 0x02) {
            __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        }
        __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
        __asm__ __volatile__("hlt");
        return;
    }
    
    // Unknown command
    screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    screen_print("Unknown command: ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println(cmd);
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_println("Type 'help' for available commands.");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
}

// Initialize shell
void shell_init() {
    cmd_index = 0;
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("");
    screen_println("Welcome to SEPPUKU OS Shell!");
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_println("Type 'help' for available commands.");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    shell_prompt();
}

// Handle keyboard input
void shell_handle_key(char c) {
    if (c == '\n') {
        // Execute command
        screen_putchar('\n');
        command_buffer[cmd_index] = '\0';
        shell_execute(command_buffer);
        cmd_index = 0;
        shell_prompt();
    } else if (c == '\b') {
        // Backspace
        if (cmd_index > 0) {
            cmd_index--;
            screen_putchar('\b');
        }
    } else if (cmd_index < MAX_COMMAND_LENGTH - 1) {
        // Normal character
        command_buffer[cmd_index++] = c;
        screen_putchar(c);
    }
}

// Main shell loop
void shell_run() {
    shell_init();
    
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            shell_handle_key(c);
        }
        __asm__ __volatile__("hlt");
    }
}
