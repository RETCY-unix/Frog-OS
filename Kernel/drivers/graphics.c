#include "../../Lib/include/graphics.h"

// Framebuffer info
static unsigned int* framebuffer = 0;
static unsigned short screen_width = 0;
static unsigned short screen_height = 0;
static unsigned short pitch = 0;
static unsigned char bpp = 0;

// Simple 8x8 font (ASCII 32-126)
static const unsigned char font8x8[95][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // !
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // "
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // #
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // $
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // %
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // &
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // '
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // (
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // )
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // *
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // +
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // ,
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // .
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // /
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // 0
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // 1
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // 2
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // 3
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // 4
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // 5
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // 6
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // 7
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // 8
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // 9
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // :
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // ;
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // <
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // =
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // >
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // ?
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // @
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // A
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // B
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // C
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // D
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // E
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // F
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // G
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // H
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // I
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // J
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // K
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // L
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // M
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // N
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // O
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // P
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // Q
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // R
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // S
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // T
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // V
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // W
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // X
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // Y
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // Z
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // [
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // backslash
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // ]
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // ^
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // _
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // `
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // a
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // b
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // c
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // d
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // e
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // f
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // g
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // h
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // i
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // j
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // k
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // l
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // m
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // n
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // o
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // p
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // q
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // r
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // s
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // t
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // u
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // v
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // w
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // x
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // y
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // z
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // {
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // |
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // }
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ~
};

// Initialize graphics from bootloader info at 0x7E00
void graphics_init() {
    framebuffer = (unsigned int*)(*(unsigned int*)0x7E00);
    screen_width = *(unsigned short*)0x7E04;
    screen_height = *(unsigned short*)0x7E06;
    pitch = *(unsigned short*)0x7E08;
    bpp = *(unsigned char*)0x7E0A;
}

unsigned short graphics_get_width() {
    return screen_width;
}

unsigned short graphics_get_height() {
    return screen_height;
}

// Optimized pixel setting
void graphics_put_pixel(int x, int y, unsigned int color) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return;
    }
    
    if (bpp == 32) {
        unsigned int* pixel = (unsigned int*)((unsigned char*)framebuffer + y * pitch + x * 4);
        *pixel = color;
    } else if (bpp == 24) {
        unsigned char* pixel = (unsigned char*)framebuffer + y * pitch + x * 3;
        pixel[0] = color & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 16) & 0xFF;
    }
}

unsigned int graphics_get_pixel(int x, int y) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return 0;
    }
    
    if (bpp == 32) {
        unsigned int* pixel = (unsigned int*)((unsigned char*)framebuffer + y * pitch + x * 4);
        return *pixel;
    } else if (bpp == 24) {
        unsigned char* pixel = (unsigned char*)framebuffer + y * pitch + x * 3;
        return pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);
    }
    return 0;
}

// Fast clear using optimized memory writes
void graphics_clear(unsigned int color) {
    if (bpp == 32) {
        unsigned int* fb32 = (unsigned int*)framebuffer;
        int pixels = (screen_height * pitch) / 4;
        for (int i = 0; i < pixels; i++) {
            fb32[i] = color;
        }
    } else {
        for (int y = 0; y < screen_height; y++) {
            for (int x = 0; x < screen_width; x++) {
                graphics_put_pixel(x, y, color);
            }
        }
    }
}

void graphics_fill_rect(int x, int y, int width, int height, unsigned int color) {
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            graphics_put_pixel(x + dx, y + dy, color);
        }
    }
}

void graphics_draw_rect(int x, int y, int width, int height, unsigned int color) {
    for (int dx = 0; dx < width; dx++) {
        graphics_put_pixel(x + dx, y, color);
        graphics_put_pixel(x + dx, y + height - 1, color);
    }
    for (int dy = 0; dy < height; dy++) {
        graphics_put_pixel(x, y + dy, color);
        graphics_put_pixel(x + width - 1, y + dy, color);
    }
}

void graphics_draw_line(int x1, int y1, int x2, int y2, unsigned int color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        graphics_put_pixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void graphics_draw_circle(int cx, int cy, int radius, unsigned int color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        graphics_put_pixel(cx + x, cy + y, color);
        graphics_put_pixel(cx - x, cy + y, color);
        graphics_put_pixel(cx + x, cy - y, color);
        graphics_put_pixel(cx - x, cy - y, color);
        graphics_put_pixel(cx + y, cy + x, color);
        graphics_put_pixel(cx - y, cy + x, color);
        graphics_put_pixel(cx + y, cy - x, color);
        graphics_put_pixel(cx - y, cy - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Filled circle
void graphics_fill_circle(int cx, int cy, int radius, unsigned int color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                graphics_put_pixel(cx + x, cy + y, color);
            }
        }
    }
}

void graphics_draw_char(int x, int y, char c, unsigned int color) {
    if (c < 32 || c > 126) return;
    
    const unsigned char* glyph = font8x8[c - 32];
    
    for (int row = 0; row < 8; row++) {
        unsigned char byte = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (byte & (1 << col)) {
                graphics_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void graphics_draw_string(int x, int y, const char* str, unsigned int color) {
    int cx = x;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            y += 8;
        } else {
            graphics_draw_char(cx, y, *str, color);
            cx += 8;
        }
        str++;
    }
}

// ========== WALLPAPER FUNCTIONS ==========

// Generate a beautiful gradient wallpaper
void graphics_draw_gradient_wallpaper() {
    for (int y = 0; y < screen_height; y++) {
        // Create smooth gradient from top to bottom
        float t = (float)y / screen_height;
        
        // Purple to blue gradient
        unsigned char r = (unsigned char)(20 + t * 40);
        unsigned char g = (unsigned char)(10 + t * 70);
        unsigned char b = (unsigned char)(60 + t * 120);
        
        unsigned int color = RGB(r, g, b);
        
        // Fast horizontal line draw
        for (int x = 0; x < screen_width; x++) {
            graphics_put_pixel(x, y, color);
        }
    }
}

// Draw abstract wallpaper with circles
void graphics_draw_abstract_wallpaper() {
    // Base gradient
    graphics_draw_gradient_wallpaper();
    
    // Add decorative circles at various positions
    int circles[][4] = {
        {100, 100, 60, RGB(80, 40, 120)},
        {screen_width - 120, 80, 80, RGB(60, 80, 160)},
        {screen_width / 3, screen_height / 4, 100, RGB(100, 50, 140)},
        {screen_width - 200, screen_height - 150, 120, RGB(70, 90, 180)},
        {150, screen_height - 120, 90, RGB(90, 60, 150)},
        {screen_width / 2, screen_height / 2, 140, RGB(50, 70, 130)},
        {screen_width / 4, screen_height * 3 / 4, 70, RGB(110, 80, 160)},
        {screen_width * 3 / 4, screen_height / 3, 85, RGB(65, 95, 170)}
    };
    
    // Draw semi-transparent circles (multiple passes for alpha effect)
    for (int i = 0; i < 8; i++) {
        int cx = circles[i][0];
        int cy = circles[i][1];
        int r = circles[i][2];
        unsigned int color = circles[i][3];
        
        // Draw multiple concentric circles for glow effect
        for (int j = 0; j < 3; j++) {
            graphics_draw_circle(cx, cy, r - j * 2, color);
        }
    }
}

// Draw wave pattern wallpaper
void graphics_draw_wave_wallpaper() {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            // Create wave patterns using sine-like approximation
            int wave1 = (x / 20 + y / 30) % 60;
            int wave2 = (x / 30 - y / 20) % 50;
            
            unsigned char r = 15 + wave1;
            unsigned char g = 25 + wave2;
            unsigned char b = 80 + (wave1 + wave2) / 2;
            
            graphics_put_pixel(x, y, RGB(r, g, b));
        }
    }
}

// Draw modern geometric wallpaper
void graphics_draw_geometric_wallpaper() {
    // Dark blue base
    graphics_clear(RGB(15, 20, 45));
    
    // Draw diagonal stripes
    for (int i = 0; i < screen_width + screen_height; i += 80) {
        for (int t = 0; t < 40; t++) {
            int x1 = i - screen_height + t;
            int y1 = 0;
            int x2 = i + t;
            int y2 = screen_height;
            
            if (x1 >= 0 && x1 < screen_width) {
                graphics_draw_line(x1, y1, x2 < screen_width ? x2 : screen_width, 
                                 y2, RGB(25, 35, 70));
            }
        }
    }
    
    // Add some accent circles
    graphics_fill_circle(screen_width / 4, screen_height / 4, 100, RGB(40, 60, 100));
    graphics_fill_circle(screen_width * 3 / 4, screen_height * 3 / 4, 120, RGB(50, 70, 110));
    graphics_draw_circle(screen_width / 2, screen_height / 2, 150, RGB(60, 90, 140));
    graphics_draw_circle(screen_width / 2, screen_height / 2, 155, RGB(60, 90, 140));
    graphics_draw_circle(screen_width / 2, screen_height / 2, 160, RGB(60, 90, 140));
}

// Aurora/Northern Lights effect
void graphics_draw_aurora_wallpaper() {
    // Dark sky background
    for (int y = 0; y < screen_height; y++) {
        float t = (float)y / screen_height;
        unsigned char r = (unsigned char)(5 + t * 15);
        unsigned char g = (unsigned char)(10 + t * 20);
        unsigned char b = (unsigned char)(25 + t * 35);
        
        for (int x = 0; x < screen_width; x++) {
            graphics_put_pixel(x, y, RGB(r, g, b));
        }
    }
    
    // Aurora waves (simplified sine approximation)
    for (int y = screen_height / 3; y < screen_height * 2 / 3; y++) {
        for (int x = 0; x < screen_width; x++) {
            // Simple wave calculation
            int wave = ((x / 10) % 40) - 20;
            int distance = y - (screen_height / 2 + wave);
            
            if (distance < 0) distance = -distance;
            
            if (distance < 60) {
                unsigned char g = 200 - distance * 3;
                unsigned char b = 150 - distance * 2;
                unsigned char r = 50;
                
                // Blend with background
                unsigned int current = graphics_get_pixel(x, y);
                unsigned char cr = (current >> 16) & 0xFF;
                unsigned char cg = (current >> 8) & 0xFF;
                unsigned char cb = current & 0xFF;
                
                cr = (cr + r) / 2;
                cg = (cg + g) / 2;
                cb = (cb + b) / 2;
                
                graphics_put_pixel(x, y, RGB(cr, cg, cb));
            }
        }
    }
    
    // Add stars
    for (int i = 0; i < 200; i++) {
        int x = (i * 97 + 13) % screen_width;
        int y = (i * 73 + 29) % (screen_height / 2);
        graphics_put_pixel(x, y, COLOR_WHITE);
        if (i % 5 == 0) {
            graphics_put_pixel(x+1, y, COLOR_WHITE);
            graphics_put_pixel(x, y+1, COLOR_WHITE);
        }
    }
}
