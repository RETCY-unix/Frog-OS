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
void graphics_fill_circle(int cx, int cy, int radius, unsigned int color);

// Text rendering (8x8 font)
void graphics_draw_char(int x, int y, char c, unsigned int color);
void graphics_draw_string(int x, int y, const char* str, unsigned int color);

// Wallpaper functions
void graphics_draw_gradient_wallpaper();
void graphics_draw_abstract_wallpaper();
void graphics_draw_wave_wallpaper();
void graphics_draw_geometric_wallpaper();
void graphics_draw_aurora_wallpaper();

// RGB color helper
#define RGB(r, g, b) (((r) << 16) | ((g) << 8) | (b))

// Common colors (24/32-bit RGB)

// Replace all the COLOR_* definitions with:
// Replace all the COLOR_* definitions with:
#define COLOR_BLACK         0x000000
#define COLOR_WHITE         0xFFFFFF
#define COLOR_RED           0xFFFFFF  // Changed to white
#define COLOR_GREEN         0xFFFFFF  // Changed to white
#define COLOR_BLUE          0xFFFFFF  // Changed to white
#define COLOR_YELLOW        0xFFFFFF  // Changed to white
#define COLOR_CYAN          0xFFFFFF  // Changed to white
#define COLOR_MAGENTA       0xFFFFFF  // Changed to white
#define COLOR_ORANGE        0xFFFFFF  // Changed to white
#define COLOR_PURPLE        0xFFFFFF  // Changed to white
#define COLOR_PINK          0xFFFFFF  // Changed to white
#define COLOR_LIME          0xFFFFFF  // Changed to white
#define COLOR_DARK_GRAY     0x404040  // Keep as dark gray
#define COLOR_GRAY          0x808080  // Keep as medium gray
#define COLOR_LIGHT_GRAY    0xC0C0C0  // Keep as light gray
#define COLOR_DARK_BLUE     0xFFFFFF  // Changed to white
#define COLOR_LIGHT_BLUE    0xFFFFFF  // Changed to white
#define COLOR_DARK_GREEN    0xFFFFFF  // Changed to white
#define COLOR_LIGHT_GREEN   0xFFFFFF  // Changed to white
#define COLOR_DARK_RED      0xFFFFFF  // Changed to white
#define COLOR_BROWN         0xFFFFFF  // Changed to white
#define COLOR_LIGHT_RED     0xFFFFFF  // Changed to white
#define COLOR_LIGHT_CYAN    0xFFFFFF  // Changed to white

// Modern UI colors - all grayscale now
#define COLOR_UI_BG         RGB(0, 0, 0)      // Black
#define COLOR_UI_SURFACE    RGB(30, 30, 30)   // Dark gray
#define COLOR_UI_PRIMARY    RGB(80, 80, 80)   // Medium gray
#define COLOR_UI_ACCENT     RGB(150, 150, 150) // Light gray
#define COLOR_UI_TEXT       RGB(255, 255, 255) // White

#endif

