#include "../../Lib/include/graphics.h"
#include "../../Lib/include/keyboard.h"

#define MAX_COMMAND_LENGTH 256
#define LINE_HEIGHT 10
#define MAX_LINES 100

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;
static int current_line = 0;
static int current_wallpaper = 0; // 0=gradient, 1=abstract, 2=wave, 3=geometric, 4=aurora

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

// Draw current wallpaper
void draw_wallpaper() {
    switch (current_wallpaper) {
        case 0:
            graphics_draw_gradient_wallpaper();
            break;
        case 1:
            graphics_draw_abstract_wallpaper();
            break;
        case 2:
            graphics_draw_wave_wallpaper();
            break;
        case 3:
            graphics_draw_geometric_wallpaper();
            break;
        case 4:
            graphics_draw_aurora_wallpaper();
            break;
        default:
            graphics_draw_gradient_wallpaper();
    }
}

// Draw terminal window
// Draw terminal window
void draw_terminal_window() {
    int width = graphics_get_width();
    int height = graphics_get_height();
    
    int term_x = 30;
    int term_y = 30;
    int term_width = width - 60;
    int term_height = height - 60;
    
    // Window shadow
    graphics_fill_rect(term_x + 5, term_y + 5, term_width, term_height, RGB(0, 0, 0));
    
    // Window background - dark gray
    graphics_fill_rect(term_x, term_y, term_width, term_height, RGB(20, 20, 20));
    
    // Window border - light gray
    graphics_draw_rect(term_x, term_y, term_width, term_height, RGB(120, 120, 120));
    graphics_draw_rect(term_x + 1, term_y + 1, term_width - 2, term_height - 2, RGB(100, 100, 100));
    
    // Title bar with gradient
    for (int i = 0; i < 20; i++) {
        unsigned char gray = 80 - i * 2;
        graphics_fill_rect(term_x + 2, term_y + 2 + i, term_width - 4, 1, RGB(gray, gray, gray));
    }
    
    graphics_draw_string(term_x + 10, term_y + 7, "SEPPUKU OS Terminal v1.2 - 1080p Edition", COLOR_WHITE);
    
    // Window control buttons - all grayscale
    graphics_fill_circle(term_x + term_width - 20, term_y + 12, 5, RGB(200, 200, 200));
    graphics_fill_circle(term_x + term_width - 35, term_y + 12, 5, RGB(150, 150, 150));
    graphics_fill_circle(term_x + term_width - 50, term_y + 12, 5, RGB(100, 100, 100));
}


// Print text
void shell_println(const char* text, unsigned int color) {
    int term_x = 40;
    int term_y = 60;
    
    if (current_line >= MAX_LINES) {
        // Scroll
        current_line = MAX_LINES - 5;
        draw_wallpaper();
        draw_terminal_window();
    }
    
    graphics_draw_string(term_x, term_y + current_line * LINE_HEIGHT, text, color);
    current_line++;
}

// Redraw prompt line
void redraw_prompt() {
    int term_x = 40;
    int term_y = 60;
    int y = term_y + current_line * LINE_HEIGHT;
    
    // Clear prompt line
    graphics_fill_rect(term_x, y, graphics_get_width() - 80, LINE_HEIGHT, RGB(20, 25, 40));
    
    // Draw prompt
    graphics_draw_string(term_x, y, "root@seppuku:~$ ", COLOR_LIGHT_GREEN);
    
    // Draw command
    graphics_draw_string(term_x + 136, y, command_buffer, COLOR_WHITE);
    
    // Draw cursor
    int cursor_x = term_x + 136 + (cmd_index * 8);
    graphics_fill_rect(cursor_x, y, 8, 8, RGB(100, 200, 255));
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
        shell_println("  help       - Show this help", COLOR_WHITE);
        shell_println("  clear      - Clear screen", COLOR_WHITE);
        shell_println("  about      - About SEPPUKU OS", COLOR_WHITE);
        shell_println("  sysinfo    - System information", COLOR_WHITE);
        shell_println("  colors     - Show color test", COLOR_WHITE);
        shell_println("  wallpaper  - Change wallpaper", COLOR_WHITE);
        shell_println("  echo <txt> - Echo text", COLOR_WHITE);
        shell_println("  reboot     - Reboot system", COLOR_WHITE);
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
        shell_println("========================================", COLOR_LIGHT_CYAN);
        shell_println("SEPPUKU OS v1.2 - 1080p Edition", COLOR_LIGHT_RED);
        shell_println("========================================", COLOR_LIGHT_CYAN);
        shell_println("A modern x86 OS with VESA graphics", COLOR_WHITE);
        shell_println("Built from scratch in C and Assembly", COLOR_YELLOW);
        shell_println("Features: Protected mode, IDT, KB, GFX", COLOR_GRAY);
        shell_println("", COLOR_WHITE);
        return;
    }
    
    // SYSINFO
    if (strcmp(cmd, "sysinfo") == 0) {
        shell_println("System Information:", COLOR_CYAN);
        
        int w = graphics_get_width();
        int h = graphics_get_height();
        
        shell_println("  Resolution: ", COLOR_WHITE);
        if (w == 1920 && h == 1080) {
            shell_println("    1920x1080 (Full HD)", COLOR_GREEN);
        } else if (w == 1280 && h == 1024) {
            shell_println("    1280x1024 (SXGA)", COLOR_GREEN);
        } else if (w == 1024 && h == 768) {
            shell_println("    1024x768 (XGA)", COLOR_GREEN);
        } else {
            shell_println("    Custom Resolution", COLOR_YELLOW);
        }
        
        shell_println("  Graphics: VESA VBE 2.0+", COLOR_WHITE);
        shell_println("  CPU Mode: 32-bit Protected", COLOR_WHITE);
        shell_println("  Interrupts: Enabled (IDT)", COLOR_WHITE);
        return;
    }
    
    // COLORS
    if (strcmp(cmd, "colors") == 0) {
        shell_println("Color Palette Test:", COLOR_WHITE);
        shell_println("RED - Primary Alert", COLOR_RED);
        shell_println("GREEN - Success Status", COLOR_GREEN);
        shell_println("BLUE - Information", COLOR_BLUE);
        shell_println("YELLOW - Warning", COLOR_YELLOW);
        shell_println("CYAN - Highlight", COLOR_CYAN);
        shell_println("MAGENTA - Special", COLOR_MAGENTA);
        shell_println("ORANGE - Notice", COLOR_ORANGE);
        shell_println("PURPLE - Debug", COLOR_PURPLE);
        return;
    }
    
    // WALLPAPER
    if (strcmp(cmd, "wallpaper") == 0) {
        shell_println("Wallpaper Options:", COLOR_CYAN);
        shell_println("  wallpaper gradient  - Smooth gradient", COLOR_WHITE);
        shell_println("  wallpaper abstract  - Abstract circles", COLOR_WHITE);
        shell_println("  wallpaper wave      - Wave pattern", COLOR_WHITE);
        shell_println("  wallpaper geometric - Modern geometric", COLOR_WHITE);
        shell_println("  wallpaper aurora    - Aurora lights", COLOR_WHITE);
        return;
    }
    
    if (starts_with(cmd, "wallpaper ")) {
        const char* type = cmd + 10;
        
        if (strcmp(type, "gradient") == 0) {
            current_wallpaper = 0;
            shell_println("Wallpaper set to: Gradient", COLOR_GREEN);
        } else if (strcmp(type, "abstract") == 0) {
            current_wallpaper = 1;
            shell_println("Wallpaper set to: Abstract", COLOR_GREEN);
        } else if (strcmp(type, "wave") == 0) {
            current_wallpaper = 2;
            shell_println("Wallpaper set to: Wave", COLOR_GREEN);
        } else if (strcmp(type, "geometric") == 0) {
            current_wallpaper = 3;
            shell_println("Wallpaper set to: Geometric", COLOR_GREEN);
        } else if (strcmp(type, "aurora") == 0) {
            current_wallpaper = 4;
            shell_println("Wallpaper set to: Aurora", COLOR_GREEN);
        } else {
            shell_println("Unknown wallpaper. Type 'wallpaper' for options.", COLOR_RED);
            return;
        }
        
        // Redraw with new wallpaper
        draw_wallpaper();
        draw_terminal_window();
        current_line = 0;
        shell_println("Wallpaper changed!", COLOR_GREEN);
        return;
    }
    
    // ECHO
    if (starts_with(cmd, "echo ")) {
        shell_println(cmd + 5, COLOR_YELLOW);
        return;
    }
    
    // REBOOT
    if (strcmp(cmd, "reboot") == 0) {
        shell_println("Rebooting system...", COLOR_YELLOW);
        
        // Wait a moment
        for (volatile int i = 0; i < 50000000; i++);
        
        // Keyboard controller reboot
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
    shell_println("Unknown command. Type 'help' for available commands.", COLOR_LIGHT_RED);
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
    current_wallpaper = 3; // Start with geometric wallpaper
    
    draw_wallpaper();
    draw_terminal_window();
    
    shell_println("", COLOR_WHITE);
    shell_println("Welcome to SEPPUKU OS - 1080p Edition!", COLOR_LIGHT_CYAN);
    
    int w = graphics_get_width();
    int h = graphics_get_height();
    shell_println("Display initialized successfully!", COLOR_GREEN);
    
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
