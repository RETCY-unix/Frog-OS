#include "../../Lib/include/graphics.h"
#include "../../Lib/include/keyboard.h"

#define MAX_COMMAND_LENGTH 256
#define LINE_HEIGHT 10
#define MAX_LINES 60

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;
static int current_line = 0;

// String utilities
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

// Draw wallpaper
void draw_wallpaper() {
    int width = graphics_get_width();
    int height = graphics_get_height();
    
    // Simple gradient background
    for (int y = 0; y < height; y++) {
        unsigned char r = 10 + (y * 30) / height;
        unsigned char g = 20 + (y * 50) / height;
        unsigned char b = 80 + (y * 100) / height;
        unsigned int color = RGB(r, g, b);
        
        for (int x = 0; x < width; x++) {
            graphics_put_pixel(x, y, color);
        }
    }
    
    // Add decorative circles
    graphics_draw_circle(100, 100, 40, RGB(60, 80, 150));
    graphics_draw_circle(100, 100, 35, RGB(60, 80, 150));
    
    graphics_draw_circle(width - 100, 100, 50, RGB(80, 60, 150));
    graphics_draw_circle(width - 100, 100, 45, RGB(80, 60, 150));
    
    graphics_draw_circle(width / 2, height - 100, 60, RGB(100, 70, 140));
    graphics_draw_circle(width / 2, height - 100, 55, RGB(100, 70, 140));
}

// Draw terminal window
void draw_terminal_window() {
    int width = graphics_get_width();
    int height = graphics_get_height();
    
    int term_x = 20;
    int term_y = 20;
    int term_width = width - 40;
    int term_height = height - 40;
    
    // Window shadow
    graphics_fill_rect(term_x + 3, term_y + 3, term_width, term_height, RGB(0, 0, 0));
    
    // Window background
    graphics_fill_rect(term_x, term_y, term_width, term_height, RGB(20, 20, 30));
    
    // Window border
    graphics_draw_rect(term_x, term_y, term_width, term_height, RGB(100, 150, 200));
    
    // Title bar
    graphics_fill_rect(term_x + 2, term_y + 2, term_width - 4, 16, RGB(50, 80, 120));
    graphics_draw_string(term_x + 10, term_y + 5, "SEPPUKU OS Terminal v1.2", COLOR_WHITE);
    
    // Window buttons
    graphics_fill_rect(term_x + term_width - 14, term_y + 6, 8, 8, COLOR_RED);
    graphics_fill_rect(term_x + term_width - 26, term_y + 6, 8, 8, COLOR_YELLOW);
    graphics_fill_rect(term_x + term_width - 38, term_y + 6, 8, 8, COLOR_GREEN);
}

// Print text
void shell_println(const char* text, unsigned int color) {
    int term_x = 30;
    int term_y = 50;
    
    if (current_line >= MAX_LINES) {
        // Simple scroll: just reset
        current_line = MAX_LINES - 5;
        draw_wallpaper();
        draw_terminal_window();
    }
    
    graphics_draw_string(term_x, term_y + current_line * LINE_HEIGHT, text, color);
    current_line++;
}

// Redraw prompt line
void redraw_prompt() {
    int term_x = 30;
    int term_y = 50;
    int y = term_y + current_line * LINE_HEIGHT;
    
    // Clear prompt line
    graphics_fill_rect(term_x, y, graphics_get_width() - 60, LINE_HEIGHT, RGB(20, 20, 30));
    
    // Draw prompt
    graphics_draw_string(term_x, y, "root@seppuku:~$ ", COLOR_LIGHT_GREEN);
    
    // Draw command
    graphics_draw_string(term_x + 136, y, command_buffer, COLOR_WHITE);
    
    // Draw cursor
    int cursor_x = term_x + 136 + (cmd_index * 8);
    graphics_fill_rect(cursor_x, y, 8, 8, COLOR_WHITE);
}

// Execute command
void shell_execute(const char* cmd) {
    current_line++;
    
    if (cmd[0] == '\0') {
        return;
    }
    
    // HELP
    if (strcmp(cmd, "help") == 0) {
        shell_println("Available commands:", COLOR_CYAN);
        shell_println("  help    - Show this help", COLOR_WHITE);
        shell_println("  clear   - Clear screen", COLOR_WHITE);
        shell_println("  about   - About SEPPUKU OS", COLOR_WHITE);
        shell_println("  colors  - Show color test", COLOR_WHITE);
        shell_println("  echo <text> - Echo text", COLOR_WHITE);
        shell_println("  reboot  - Reboot system", COLOR_WHITE);
        return;
    }
    
    // CLEAR
    if (strcmp(cmd, "clear") == 0) {
        draw_wallpaper();
        draw_terminal_window();
        current_line = 0;
        return;
    }
    
    // ABOUT
    if (strcmp(cmd, "about") == 0) {
        shell_println("SEPPUKU OS v1.2 - Graphical Edition", COLOR_LIGHT_RED);
        shell_println("A minimalist x86 OS with VESA graphics", COLOR_WHITE);
        
        int w = graphics_get_width();
        int h = graphics_get_height();
        if (w == 1024 && h == 768) {
            shell_println("Resolution: 1024x768", COLOR_YELLOW);
        } else if (w == 1280 && h == 1024) {
            shell_println("Resolution: 1280x1024", COLOR_YELLOW);
        } else {
            shell_println("Resolution: Custom", COLOR_YELLOW);
        }
        
        shell_println("Built from scratch in C and Assembly", COLOR_WHITE);
        return;
    }
    
    // COLORS
    if (strcmp(cmd, "colors") == 0) {
        shell_println("Color test:", COLOR_WHITE);
        shell_println("RED TEXT", COLOR_RED);
        shell_println("GREEN TEXT", COLOR_GREEN);
        shell_println("BLUE TEXT", COLOR_BLUE);
        shell_println("YELLOW TEXT", COLOR_YELLOW);
        shell_println("CYAN TEXT", COLOR_CYAN);
        shell_println("MAGENTA TEXT", COLOR_MAGENTA);
        return;
    }
    
    // ECHO
    if (starts_with(cmd, "echo ")) {
        shell_println(cmd + 5, COLOR_YELLOW);
        return;
    }
    
    // REBOOT
    if (strcmp(cmd, "reboot") == 0) {
        shell_println("Rebooting...", COLOR_YELLOW);
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
    shell_println("Unknown command. Type 'help' for commands.", COLOR_LIGHT_RED);
}

// Handle keyboard input
void shell_handle_key(char c) {
    if (c == '\n') {
        command_buffer[cmd_index] = '\0';
        shell_execute(command_buffer);
        cmd_index = 0;
        command_buffer[0] = '\0';
        redraw_prompt();
    } else if (c == '\b') {
        if (cmd_index > 0) {
            cmd_index--;
            command_buffer[cmd_index] = '\0';
            redraw_prompt();
        }
    } else if (cmd_index < MAX_COMMAND_LENGTH - 1) {
        command_buffer[cmd_index++] = c;
        command_buffer[cmd_index] = '\0';
        redraw_prompt();
    }
}

// Initialize shell
void shell_init() {
    cmd_index = 0;
    current_line = 0;
    
    draw_wallpaper();
    draw_terminal_window();
    
    shell_println("", COLOR_WHITE);
    shell_println("Welcome to SEPPUKU OS - Graphical Edition!", COLOR_LIGHT_CYAN);
    shell_println("Type 'help' for available commands.", COLOR_LIGHT_GRAY);
    shell_println("", COLOR_WHITE);
    
    redraw_prompt();
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
