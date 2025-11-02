
#include "../Lib/include/graphics.h"
#include "../Lib/include/idt.h"
#include "../Lib/include/keyboard.h"
#include "../Lib/include/ata.h"
#include "../user/shell/shell.h"

void kernel_main() {
    // Initialize graphics mode
    graphics_init();

    // Clear screen
    graphics_clear(RGB(0, 0, 0));

    // Draw boot logo
    int cx = graphics_get_width() / 2;
    int cy = 100;

    // Draw circles for logo
    for (int r = 50; r > 30; r -= 2) {
       graphics_draw_circle(cx, cy, r, RGB(200, 200, 200));
    }

    // Draw title
    graphics_draw_string(cx - 120, cy + 70, "SEPPUKU OS v1.4", COLOR_WHITE);
    graphics_draw_string(cx - 80, cy + 85, "File System Edition", COLOR_LIGHT_CYAN);

    // Boot messages
    int y = cy + 110;
    graphics_draw_string(100, y, "[OK] Protected mode enabled", COLOR_GREEN);
    y += 12;

    graphics_draw_string(100, y, "[OK] VESA graphics initialized", COLOR_GREEN);
    y += 12;

    // Display resolution
    int w = graphics_get_width();
    int h = graphics_get_height();
    graphics_draw_string(100, y, "[OK] Resolution: ", COLOR_GREEN);
    if (w == 1024 && h == 768) {
        graphics_draw_string(228, y, "1024x768", COLOR_WHITE);
    } else if (w == 1280 && h == 1024) {
        graphics_draw_string(228, y, "1280x1024", COLOR_WHITE);
    } else if (w == 1920 && h == 1080) {
        graphics_draw_string(228, y, "1920x1080", COLOR_WHITE);
    } else {
        graphics_draw_string(228, y, "Unknown", COLOR_WHITE);
    }
    y += 12;

    graphics_draw_string(100, y, "[...] Initializing IDT...", COLOR_YELLOW);

    // Small delay
    for (volatile int i = 0; i < 1000000; i++);

    idt_init();
    graphics_draw_string(260, y, " OK", COLOR_GREEN);
    y += 12;

    graphics_draw_string(100, y, "[...] Initializing keyboard...", COLOR_YELLOW);

    // Small delay
    for (volatile int i = 0; i < 1000000; i++);

    keyboard_init();
    graphics_draw_string(292, y, " OK", COLOR_GREEN);
    y += 12;

    // Don't initialize ATA here - let shell do it safely
    graphics_draw_string(100, y, "[OK] ATA driver loaded", COLOR_GREEN);
    y += 12;

    graphics_draw_string(100, y, "[...] Starting shell...", COLOR_YELLOW);
    graphics_draw_string(244, y, " OK", COLOR_GREEN);
    y += 12;

    y += 20;
    graphics_draw_string(100, y, ">>> BOOT SUCCESSFUL <<<", COLOR_LIGHT_GREEN);

    // Wait before starting shell
    for (volatile int i = 0; i < 50000000; i++);

    // Start graphical shell
    shell_run();
}
