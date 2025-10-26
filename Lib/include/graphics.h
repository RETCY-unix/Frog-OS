#ifndef GRAPHICS_H
#define GRAPHICS_H

// Framebuffer info structure (matches bootloader)
typedef struct {
    unsigned int framebuffer;
    unsigned short width;
    unsigned short height;
    unsigned short pitch;
    unsigned char bpp;
} fb_info_t;

// Initialize graphics from bootloader info
void graphics_init();

// Get screen dimensions
unsigned short graphics_get_width();
unsigned short graphics_get_height();

// Pixel manipulation
void graphics_put_pixel(int x, int y, unsigned int color);
unsigned int graphics_get_pixel(int x, int y);

// Drawing primitives
void graphics_clear(unsigned int color);
void graphics_fill_rect(int x, int y, int width, int height, unsigned int color);
void graphics_draw_rect(int x, int y, int width, int height, unsigned int color);
void graphics_draw_line(int x1, int y1, int x2, int y2, unsigned int color);
void graphics_draw_circle(int cx, int cy, int radius, unsigned int color);

// Text rendering (8x8 font)
void graphics_draw_char(int x, int y, char c, unsigned int color);
void graphics_draw_string(int x, int y, const char* str, unsigned int color);

// RGB color helper
#define RGB(r, g, b) (((r) << 16) | ((g) << 8) | (b))

// Common colors (24/32-bit RGB)
#define COLOR_BLACK         0x000000
#define COLOR_WHITE         0xFFFFFF
#define COLOR_RED           0xFF0000
#define COLOR_GREEN         0x00FF00
#define COLOR_BLUE          0x0000FF
#define COLOR_YELLOW        0xFFFF00
#define COLOR_CYAN          0x00FFFF
#define COLOR_MAGENTA       0xFF00FF
#define COLOR_ORANGE        0xFF8000
#define COLOR_PURPLE        0x8000FF
#define COLOR_PINK          0xFF69B4
#define COLOR_LIME          0x00FF00
#define COLOR_DARK_GRAY     0x404040
#define COLOR_GRAY          0x808080
#define COLOR_LIGHT_GRAY    0xC0C0C0
#define COLOR_DARK_BLUE     0x000080
#define COLOR_LIGHT_BLUE    0x87CEEB
#define COLOR_DARK_GREEN    0x006400
#define COLOR_LIGHT_GREEN   0x90EE90
#define COLOR_DARK_RED      0x8B0000
#define COLOR_BROWN         0x8B4513
#define COLOR_LIGHT_RED     0xFF6666
#define COLOR_LIGHT_CYAN    0xE0FFFF

#endif
