#include "../../Lib/include/screen.h"

// Current cursor position
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;

// Current color (default: white on black)
static unsigned char current_color = 0x0F;

// VGA memory pointer
static unsigned char* vga_memory = (unsigned char*)VGA_MEMORY;

// Helper function to create color byte
static unsigned char make_color(unsigned char fg, unsigned char bg) {
    return (bg << 4) | (fg & 0x0F);
}

// Helper function to get memory offset
static int get_offset(int x, int y) {
    return (y * VGA_WIDTH + x) * 2;
}

// Initialize screen
void screen_init() {
    cursor_x = 0;
    cursor_y = 0;
    current_color = make_color(COLOR_WHITE, COLOR_BLACK);
    screen_clear();
}

// Clear the entire screen
void screen_clear() {
    int i;
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vga_memory[i] = ' ';
        vga_memory[i + 1] = current_color;
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Scroll screen up by one line
void screen_scroll() {
    int i;
    
    // Move all lines up by one
    for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
        vga_memory[i] = vga_memory[i + VGA_WIDTH * 2];
    }
    
    // Clear the last line
    int offset = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
    for (i = 0; i < VGA_WIDTH * 2; i += 2) {
        vga_memory[offset + i] = ' ';
        vga_memory[offset + i + 1] = current_color;
    }
    
    cursor_y = VGA_HEIGHT - 1;
}

// Put a single character on screen
void screen_putchar(char c) {
    int offset;
    
    // Handle special characters
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            offset = get_offset(cursor_x, cursor_y);
            vga_memory[offset] = ' ';
            vga_memory[offset + 1] = current_color;
        }
    } else {
        // Normal printable character
        offset = get_offset(cursor_x, cursor_y);
        vga_memory[offset] = c;
        vga_memory[offset + 1] = current_color;
        cursor_x++;
    }
    
    // Handle line wrap
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    // Handle scrolling
    if (cursor_y >= VGA_HEIGHT) {
        screen_scroll();
    }
}

// Print a string
void screen_print(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        screen_putchar(str[i]);
        i++;
    }
}

// Print a string with newline
void screen_println(const char* str) {
    screen_print(str);
    screen_putchar('\n');
}

// Set foreground and background color
void screen_set_color(unsigned char fg, unsigned char bg) {
    current_color = make_color(fg, bg);
}
