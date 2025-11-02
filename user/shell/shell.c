#include "../../Lib/include/graphics.h"
#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/sound.h"
#include "../../Lib/include/ata.h"
#include "../../Lib/include/fat12.h"

#define MAX_COMMAND_LENGTH 256
#define LINE_HEIGHT 10
#define MAX_LINES 100

// Window state
static int window_minimized = 0;
static int window_maximized = 0;

// Normal window dimensions
static int normal_term_x = 30;
static int normal_term_y = 30;
static int normal_term_width = 0;
static int normal_term_height = 0;

// Current window dimensions
static int term_x = 30;
static int term_y = 30;
static int term_width = 0;
static int term_height = 0;

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;
static int current_line = 0;
static int current_wallpaper = 0;

// Cursor visibility
static int cursor_visible = 1;
static int cursor_blink_counter = 0;

// FS mounted flag
static int fs_mounted = 0;

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

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
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

// Draw icons
void draw_minimize_icon(int x, int y) {
    graphics_draw_line(x - 3, y, x + 3, y, RGB(40, 40, 40));
}

void draw_maximize_icon(int x, int y) {
    if (window_maximized) {
        graphics_draw_rect(x - 3, y - 2, 5, 5, RGB(40, 40, 40));
        graphics_draw_rect(x - 1, y - 4, 5, 5, RGB(40, 40, 40));
    } else {
        graphics_draw_rect(x - 3, y - 3, 7, 7, RGB(40, 40, 40));
    }
}

void draw_close_icon(int x, int y) {
    graphics_draw_line(x - 3, y - 3, x + 3, y + 3, RGB(40, 40, 40));
    graphics_draw_line(x - 3, y + 3, x + 3, y - 3, RGB(40, 40, 40));
}

// Draw terminal window
void draw_terminal_window() {
    int width = graphics_get_width();
    int height = graphics_get_height();
    
    if (window_maximized) {
        term_x = 0;
        term_y = 0;
        term_width = width;
        term_height = height;
    } else {
        term_x = normal_term_x;
        term_y = normal_term_y;
        term_width = normal_term_width;
        term_height = normal_term_height;
    }
    
    if (!window_maximized) {
        graphics_fill_rect(term_x + 5, term_y + 5, term_width, term_height, RGB(0, 0, 0));
    }
    
    graphics_fill_rect(term_x, term_y, term_width, term_height, RGB(20, 20, 20));
    
    if (!window_maximized) {
        graphics_draw_rect(term_x, term_y, term_width, term_height, RGB(120, 120, 120));
        graphics_draw_rect(term_x + 1, term_y + 1, term_width - 2, term_height - 2, RGB(100, 100, 100));
    }
    
    // Title bar
    for (int i = 0; i < 20; i++) {
        unsigned char gray = 80 - i * 2;
        graphics_fill_rect(term_x + 2, term_y + 2 + i, term_width - 4, 1, RGB(gray, gray, gray));
    }
    
    graphics_draw_string(term_x + 10, term_y + 7, "SEPPUKU OS Terminal v1.5 SOUND", COLOR_WHITE);
    
    int btn_y = term_y + 12;
    
    // Buttons
    graphics_fill_circle(term_x + term_width - 20, btn_y, 6, RGB(200, 200, 200));
    graphics_fill_circle(term_x + term_width - 20, btn_y, 5, RGB(255, 95, 86));
    draw_close_icon(term_x + term_width - 20, btn_y);
    
    graphics_fill_circle(term_x + term_width - 40, btn_y, 6, RGB(150, 150, 150));
    graphics_fill_circle(term_x + term_width - 40, btn_y, 5, RGB(40, 201, 64));
    draw_maximize_icon(term_x + term_width - 40, btn_y);
    
    graphics_fill_circle(term_x + term_width - 60, btn_y, 6, RGB(100, 100, 100));
    graphics_fill_circle(term_x + term_width - 60, btn_y, 5, RGB(255, 189, 46));
    draw_minimize_icon(term_x + term_width - 60, btn_y);
}

// Draw minimized taskbar
void draw_minimized_taskbar() {
    int width = graphics_get_width();
    int height = graphics_get_height();
    
    int taskbar_height = 40;
    graphics_fill_rect(0, height - taskbar_height, width, taskbar_height, RGB(40, 40, 40));
    graphics_draw_line(0, height - taskbar_height, width, height - taskbar_height, RGB(100, 100, 100));
    
    int btn_x = 20;
    int btn_y = height - taskbar_height + 5;
    int btn_w = 150;
    int btn_h = 30;
    
    graphics_fill_rect(btn_x, btn_y, btn_w, btn_h, RGB(60, 60, 60));
    graphics_draw_rect(btn_x, btn_y, btn_w, btn_h, RGB(100, 100, 100));
    
    graphics_fill_rect(btn_x + 5, btn_y + 7, 16, 16, RGB(20, 20, 20));
    graphics_draw_rect(btn_x + 5, btn_y + 7, 16, 16, RGB(150, 150, 150));
    graphics_draw_string(btn_x + 8, btn_y + 10, ">_", COLOR_WHITE);
    
    graphics_draw_string(btn_x + 25, btn_y + 10, "Terminal", COLOR_WHITE);
    graphics_draw_string(width - 200, height - taskbar_height + 12, "Press 'r' to restore", RGB(180, 180, 180));
}

// Print text
void shell_println(const char* text, unsigned int color) {
    if (window_minimized) return;
    
    int start_x = term_x + 10;
    int start_y = term_y + 30;
    
    if (current_line >= MAX_LINES) {
        current_line = MAX_LINES - 5;
        draw_wallpaper();
        draw_terminal_window();
    }
    
    graphics_draw_string(start_x, start_y + current_line * LINE_HEIGHT, text, color);
    current_line++;
}

// Redraw prompt line
void redraw_prompt() {
    if (window_minimized) return;
    
    int start_x = term_x + 10;
    int start_y = term_y + 30;
    int y = start_y + current_line * LINE_HEIGHT;
    
    graphics_fill_rect(start_x, y, term_width - 20, LINE_HEIGHT, RGB(20, 20, 20));
    graphics_draw_string(start_x, y, "root@seppuku:~$ ", COLOR_LIGHT_GREEN);
    graphics_draw_string(start_x + 136, y, command_buffer, COLOR_WHITE);
    
    if (cursor_visible) {
        int cursor_x = start_x + 136 + (cmd_index * 8);
        graphics_fill_rect(cursor_x, y, 8, 8, RGB(150, 150, 150));
    }
}

// Toggle functions
void toggle_minimize() {
    window_minimized = !window_minimized;
    draw_wallpaper();
    
    if (window_minimized) {
        draw_minimized_taskbar();
    } else {
        draw_terminal_window();
        redraw_prompt();
    }
}

void toggle_maximize() {
    if (window_minimized) return;
    
    window_maximized = !window_maximized;
    draw_wallpaper();
    draw_terminal_window();
    
    if (window_maximized) {
        current_line = 0;
    }
    
    redraw_prompt();
}

// Execute command
void shell_execute(const char* cmd) {
    current_line++;
    
    if (cmd[0] == '\0') {
        return;
    }
    
    // SOUND COMMANDS - THE NEW STUFF!
    if (strcmp(cmd, "beep") == 0) {
        shell_println("*BEEP!*", COLOR_YELLOW);
        sound_beep(BEEP_DEFAULT, 200);
        return;
    }
    
    if (strcmp(cmd, "chime") == 0) {
        shell_println("Playing startup chime...", COLOR_CYAN);
        sound_beep(NOTE_C4, 100);
        for (volatile int i = 0; i < 1000000; i++);
        sound_beep(NOTE_E4, 100);
        for (volatile int i = 0; i < 1000000; i++);
        sound_beep(NOTE_G4, 150);
        shell_println("Done!", COLOR_GREEN);
        return;
    }
    
    if (strcmp(cmd, "siren") == 0) {
        shell_println("WEEEOOO WEEEOOO!", COLOR_RED);
        for (int i = 0; i < 3; i++) {
            sound_beep(800, 200);
            for (volatile int j = 0; j < 2000000; j++);
            sound_beep(400, 200);
            for (volatile int j = 0; j < 2000000; j++);
        }
        shell_println("Emergency services on the way!", COLOR_YELLOW);
        return;
    }
    
    // FILE SYSTEM COMMANDS
    if (strcmp(cmd, "mount") == 0) {
        if (fs_mounted) {
            shell_println("File system already mounted", COLOR_YELLOW);
            return;
        }
        
        shell_println("Initializing ATA driver...", COLOR_CYAN);
        ata_init();
        
        shell_println("Mounting FAT12 file system...", COLOR_CYAN);
        fat12_init();
        
        fs_mounted = 1;
        shell_println("File system mounted successfully!", COLOR_GREEN);
        sound_beep(BEEP_SUCCESS, 100);
        return;
    }
    
    if (strcmp(cmd, "ls") == 0) {
        if (!fs_mounted) {
            shell_println("File system not mounted. Use 'mount' first.", COLOR_RED);
            sound_beep(BEEP_ERROR, 150);
            return;
        }
        
        fat12_entry_t entries[20];
        int count = fat12_list_files(entries, 20);
        
        if (count == 0) {
            shell_println("No files found (or empty disk)", COLOR_YELLOW);
        } else {
            shell_println("Files:", COLOR_CYAN);
            for (int i = 0; i < count; i++) {
                char filename[13];
                int j = 0;
                for (int k = 0; k < 8 && entries[i].filename[k] != ' '; k++) {
                    filename[j++] = entries[i].filename[k];
                }
                if (entries[i].filename[8] != ' ') {
                    filename[j++] = '.';
                    for (int k = 8; k < 11 && entries[i].filename[k] != ' '; k++) {
                        filename[j++] = entries[i].filename[k];
                    }
                }
                filename[j] = '\0';
                
                shell_println(filename, COLOR_WHITE);
            }
        }
        return;
    }
    
    if (starts_with(cmd, "cat ")) {
        if (!fs_mounted) {
            shell_println("File system not mounted. Use 'mount' first.", COLOR_RED);
            return;
        }
        
        const char* filename = cmd + 4;
        unsigned char buffer[512];
        
        int size = fat12_read_file(filename, buffer, 512);
        
        if (size < 0) {
            shell_println("File not found or read error", COLOR_RED);
            sound_beep(BEEP_ERROR, 150);
        } else if (size == 0) {
            shell_println("(empty file)", COLOR_YELLOW);
        } else {
            for (int i = 0; i < size; i++) {
                char c = buffer[i];
                if (c == '\n' || c == '\r') {
                    current_line++;
                } else if (c >= 32 && c <= 126) {
                    char str[2] = {c, '\0'};
                    int start_x = term_x + 10;
                    int start_y = term_y + 30;
                    graphics_draw_string(start_x + (i % 80) * 8, start_y + current_line * LINE_HEIGHT, str, COLOR_YELLOW);
                }
            }
            current_line++;
        }
        return;
    }
    
    if (starts_with(cmd, "write ")) {
        if (!fs_mounted) {
            shell_println("File system not mounted. Use 'mount' first.", COLOR_RED);
            return;
        }
        
        const char* args = cmd + 6;
        char filename[20];
        int i = 0;
        
        while (args[i] && args[i] != ' ' && i < 19) {
            filename[i] = args[i];
            i++;
        }
        filename[i] = '\0';
        
        if (args[i] != ' ') {
            shell_println("Usage: write <filename> <content>", COLOR_RED);
            return;
        }
        
        const char* content = args + i + 1;
        int len = strlen(content);
        
        if (len == 0) {
            shell_println("Content cannot be empty", COLOR_RED);
            return;
        }
        
        int result = fat12_write_file(filename, (unsigned char*)content, len);
        
        if (result < 0) {
            shell_println("Write failed", COLOR_RED);
            sound_beep(BEEP_ERROR, 150);
        } else {
            shell_println("File written successfully!", COLOR_GREEN);
            sound_beep(BEEP_SUCCESS, 100);
        }
        return;
    }
    
    if (starts_with(cmd, "rm ")) {
        if (!fs_mounted) {
            shell_println("File system not mounted. Use 'mount' first.", COLOR_RED);
            return;
        }
        
        const char* filename = cmd + 3;
        
        int result = fat12_delete_file(filename);
        
        if (result < 0) {
            shell_println("Delete failed (file not found?)", COLOR_RED);
            sound_beep(BEEP_ERROR, 150);
        } else {
            shell_println("File deleted", COLOR_GREEN);
            sound_beep(600, 80);
        }
        return;
    }
    
    // EXISTING COMMANDS
    if (strcmp(cmd, "minimize") == 0 || strcmp(cmd, "min") == 0) {
        toggle_minimize();
        return;
    }
    
    if (strcmp(cmd, "maximize") == 0 || strcmp(cmd, "max") == 0) {
        toggle_maximize();
        return;
    }
    
    if (strcmp(cmd, "help") == 0) {
        shell_println("Available commands:", COLOR_CYAN);
        shell_println("  help       - Show this help", COLOR_WHITE);
        shell_println("  clear      - Clear screen", COLOR_WHITE);
        shell_println("  about      - About SEPPUKU OS", COLOR_WHITE);
        shell_println("  sysinfo    - System information", COLOR_WHITE);
        shell_println("  echo <txt> - Echo text", COLOR_WHITE);
        shell_println("  wallpaper  - Change wallpaper", COLOR_WHITE);
        shell_println("  test       - Run test", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("Sound Commands:", COLOR_CYAN);
        shell_println("  beep       - Simple beep", COLOR_WHITE);
        shell_println("  chime      - Startup chime", COLOR_WHITE);
        shell_println("  siren      - Police siren!", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("File System Commands:", COLOR_CYAN);
        shell_println("  mount      - Mount file system", COLOR_WHITE);
        shell_println("  ls         - List files", COLOR_WHITE);
        shell_println("  cat <file> - Read file", COLOR_WHITE);
        shell_println("  write <f> <content> - Write file", COLOR_WHITE);
        shell_println("  rm <file>  - Delete file", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("  minimize   - Minimize window", COLOR_WHITE);
        shell_println("  maximize   - Maximize window", COLOR_WHITE);
        shell_println("  reboot     - Reboot system", COLOR_WHITE);
        return;
    }
    
    if (strcmp(cmd, "clear") == 0) {
        draw_wallpaper();
        draw_terminal_window();
        current_line = 0;
        return;
    }
    
    if (strcmp(cmd, "test") == 0) {
        shell_println("Test command works!", COLOR_GREEN);
        shell_println("Shell is functioning correctly", COLOR_WHITE);
        shell_println("Keyboard interrupt working", COLOR_WHITE);
        shell_println("Graphics rendering working", COLOR_WHITE);
        shell_println("Sound system active!", COLOR_GREEN);
        sound_beep(NOTE_C5, 100);
        return;
    }
    
    if (starts_with(cmd, "echo ")) {
        const char* text = cmd + 5;
        shell_println(text, COLOR_YELLOW);
        return;
    }
    
    if (strcmp(cmd, "about") == 0) {
        shell_println("========================================", COLOR_LIGHT_CYAN);
        shell_println("SEPPUKU OS v1.5", COLOR_LIGHT_RED);
        shell_println("========================================", COLOR_LIGHT_CYAN);
        shell_println("A modern x86 OS with graphics", COLOR_WHITE);
        shell_println("Built from scratch in C and Assembly", COLOR_YELLOW);
        shell_println("Now with PC Speaker sound!", COLOR_GREEN);
        shell_println("", COLOR_WHITE);
        sound_beep(NOTE_C4, 80);
        for (volatile int i = 0; i < 500000; i++);
        sound_beep(NOTE_E4, 80);
        for (volatile int i = 0; i < 500000; i++);
        sound_beep(NOTE_G4, 100);
        return;
    }
    
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
        }
        
        shell_println("  Graphics: VESA VBE 2.0+", COLOR_WHITE);
        shell_println("  CPU Mode: 32-bit Protected", COLOR_WHITE);
        shell_println("  Interrupts: Enabled (IDT)", COLOR_WHITE);
        shell_println("  Window Manager: Active", COLOR_GREEN);
        shell_println("  Sound: PC Speaker Active", COLOR_GREEN);
        
        if (fs_mounted) {
            shell_println("  File System: FAT12 (mounted)", COLOR_GREEN);
        } else {
            shell_println("  File System: Not mounted", COLOR_YELLOW);
        }
        return;
    }
    
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
        } else if (strcmp(type, "abstract") == 0) {
            current_wallpaper = 1;
        } else if (strcmp(type, "wave") == 0) {
            current_wallpaper = 2;
        } else if (strcmp(type, "geometric") == 0) {
            current_wallpaper = 3;
        } else if (strcmp(type, "aurora") == 0) {
            current_wallpaper = 4;
        } else {
            shell_println("Unknown wallpaper. Type 'wallpaper' for options.", COLOR_RED);
            return;
        }
        
        draw_wallpaper();
        if (!window_minimized) {
            draw_terminal_window();
        } else {
            draw_minimized_taskbar();
        }
        current_line = 0;
        shell_println("Wallpaper changed!", COLOR_GREEN);
        sound_beep(BEEP_SUCCESS, 80);
        return;
    }
    
    if (strcmp(cmd, "reboot") == 0) {
        shell_println("Rebooting system...", COLOR_YELLOW);
        sound_beep(BEEP_ERROR, 300);
        
        for (volatile int i = 0; i < 50000000; i++);
        
        unsigned char temp;
        __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        while (temp & 0x02) {
            __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        }
        __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
        __asm__ __volatile__("hlt");
        return;
    }
    
    shell_println("Unknown command. Type 'help' for available commands.", COLOR_LIGHT_RED);
    sound_beep(BEEP_ERROR, 100);
}

// Handle keyboard input
void shell_handle_key(char c) {
    if (window_minimized) {
        if (c == 'r' || c == 'R') {
            toggle_minimize();
        }
        return;
    }
    
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
    current_wallpaper = 3;
    window_minimized = 0;
    window_maximized = 0;
    fs_mounted = 0;
    
    int width = graphics_get_width();
    int height = graphics_get_height();
    normal_term_width = width - 60;
    normal_term_height = height - 60;
    
    term_width = normal_term_width;
    term_height = normal_term_height;
    
    draw_wallpaper();
    draw_terminal_window();
    
    shell_println("", COLOR_WHITE);
    shell_println("Welcome to SEPPUKU OS v1.5!", COLOR_LIGHT_CYAN);
    shell_println("Now with PC Speaker sound!", COLOR_GREEN);
    shell_println("Type 'beep', 'chime', or 'siren' to test!", COLOR_YELLOW);
    shell_println("Type 'help' for all commands.", COLOR_LIGHT_GRAY);
    shell_println("", COLOR_WHITE);
    
    redraw_prompt();
}

// Main shell loop
void shell_run() {
    shell_init();
    
    int frame_counter = 0;
    
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            shell_handle_key(c);
        }
        
        frame_counter++;
        if (frame_counter >= 50000) {
            frame_counter = 0;
            
            cursor_blink_counter++;
            if (cursor_blink_counter >= 30) {
                cursor_blink_counter = 0;
                cursor_visible = !cursor_visible;
                if (!window_minimized) {
                    redraw_prompt();
                }
            }
        }
        
        __asm__ __volatile__("hlt");
    }
}
