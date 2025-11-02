#include "../../Lib/include/graphics.h"
#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/sound.h"
#include "../../Lib/include/ata.h"
#include "../../Lib/include/fat12.h"
#include "../twm/twm.h"

#define MAX_COMMAND_LENGTH 256
#define LINE_HEIGHT 10
#define MAX_LINES 100

// Terminal instance structure
typedef struct {
    char command_buffer[MAX_COMMAND_LENGTH];
    int cmd_index;
    int current_line;
    int scroll_offset;
    char line_buffer[MAX_LINES][256];
    int cursor_visible;
    int cursor_blink_counter;
} terminal_t;

// Multiple terminal instances (one per tile)
static terminal_t terminals[6];
static int current_wallpaper = 3;
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

// Draw wallpaper
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

// Get current terminal instance
static terminal_t* get_current_terminal() {
    twm_state_t* twm = twm_get_state();
    workspace_t* ws = &twm->workspaces[twm->current_workspace];
    
    if (ws->tile_count == 0) return &terminals[0];
    
    int focused = ws->focused_tile;
    if (focused < 0 || focused >= 6) focused = 0;
    
    return &terminals[focused];
}

// Get tile bounds for current terminal
static void get_terminal_bounds(int* x, int* y, int* width, int* height) {
    twm_state_t* twm = twm_get_state();
    workspace_t* ws = &twm->workspaces[twm->current_workspace];
    
    if (ws->tile_count == 0) {
        *x = 0;
        *y = 0;
        *width = graphics_get_width();
        *height = graphics_get_height();
        return;
    }
    
    int focused = ws->focused_tile;
    if (focused < 0 || focused >= ws->tile_count) focused = 0;
    
    tile_t* tile = &ws->tiles[focused];
    *x = tile->x + 3;
    *y = tile->y + 3;
    *width = tile->width - 6;
    *height = tile->height - 6;
}

// Print text in terminal
void shell_println(const char* text, unsigned int color) {
    terminal_t* term = get_current_terminal();
    
    if (term->current_line >= MAX_LINES) {
        term->current_line = MAX_LINES - 1;
    }
    
    // Store line in buffer
    int i = 0;
    while (text[i] && i < 255) {
        term->line_buffer[term->current_line][i] = text[i];
        i++;
    }
    term->line_buffer[term->current_line][i] = '\0';
    
    term->current_line++;
}

// Redraw terminal contents
void redraw_terminal() {
    terminal_t* term = get_current_terminal();
    
    int x, y, width, height;
    get_terminal_bounds(&x, &y, &width, &height);
    
    // Clear terminal area
    graphics_fill_rect(x, y, width, height, RGB(20, 20, 20));
    
    // Draw stored lines
    int line_y = y + 10;
    for (int i = 0; i < term->current_line && i < MAX_LINES; i++) {
        if (line_y + LINE_HEIGHT > y + height - 20) break;
        graphics_draw_string(x + 10, line_y, term->line_buffer[i], COLOR_WHITE);
        line_y += LINE_HEIGHT;
    }
    
    // Draw prompt
    int prompt_y = y + height - 20;
    graphics_draw_string(x + 10, prompt_y, "root@seppuku:~$ ", COLOR_LIGHT_GREEN);
    graphics_draw_string(x + 146, prompt_y, term->command_buffer, COLOR_WHITE);
    
    // Draw cursor
    if (term->cursor_visible) {
        int cursor_x = x + 146 + (term->cmd_index * 8);
        graphics_fill_rect(cursor_x, prompt_y, 8, 8, RGB(150, 150, 150));
    }
}

// Execute command
void shell_execute(const char* cmd) {
    terminal_t* term = get_current_terminal();
    term->current_line++;
    
    if (cmd[0] == '\0') {
        return;
    }
    
    // SOUND COMMANDS
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
            char line[256];
            int line_pos = 0;
            for (int i = 0; i < size; i++) {
                char c = buffer[i];
                if (c == '\n' || c == '\r' || line_pos >= 250) {
                    line[line_pos] = '\0';
                    shell_println(line, COLOR_YELLOW);
                    line_pos = 0;
                } else if (c >= 32 && c <= 126) {
                    line[line_pos++] = c;
                }
            }
            if (line_pos > 0) {
                line[line_pos] = '\0';
                shell_println(line, COLOR_YELLOW);
            }
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
    if (strcmp(cmd, "help") == 0) {
        shell_println("Available commands:", COLOR_CYAN);
        shell_println("", COLOR_WHITE);
        shell_println("Tiling WM Shortcuts:", COLOR_CYAN);
        shell_println("  ESC+1-5    - Switch workspace", COLOR_WHITE);
        shell_println("  ESC+Enter  - New terminal tile", COLOR_WHITE);
        shell_println("  ESC+J/K    - Focus next/prev tile", COLOR_WHITE);
        shell_println("  ESC+Q      - Close focused tile", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("Commands:", COLOR_CYAN);
        shell_println("  help       - Show this help", COLOR_WHITE);
        shell_println("  clear      - Clear screen", COLOR_WHITE);
        shell_println("  about      - About SEPPUKU OS", COLOR_WHITE);
        shell_println("  sysinfo    - System information", COLOR_WHITE);
        shell_println("  wallpaper  - Change wallpaper", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("Sound:", COLOR_CYAN);
        shell_println("  beep, chime, siren", COLOR_WHITE);
        shell_println("", COLOR_WHITE);
        shell_println("File System:", COLOR_CYAN);
        shell_println("  mount, ls, cat, write, rm", COLOR_WHITE);
        return;
    }
    
    if (strcmp(cmd, "clear") == 0) {
        term->current_line = 0;
        for (int i = 0; i < MAX_LINES; i++) {
            term->line_buffer[i][0] = '\0';
        }
        return;
    }
    
    if (strcmp(cmd, "test") == 0) {
        shell_println("Test command works!", COLOR_GREEN);
        shell_println("Tiling WM active!", COLOR_WHITE);
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
        shell_println("SEPPUKU OS v2.0 - TILING WM EDITION", COLOR_LIGHT_RED);
        shell_println("========================================", COLOR_LIGHT_CYAN);
        shell_println("Keyboard-driven tiling window manager", COLOR_WHITE);
        shell_println("Press ESC then other keys for shortcuts", COLOR_YELLOW);
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
        
        shell_println("  Window Manager: Tiling (i3-style)", COLOR_GREEN);
        shell_println("  Graphics: VESA VBE 2.0+", COLOR_WHITE);
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
        twm_draw();
        redraw_terminal();
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
    terminal_t* term = get_current_terminal();
    
    // Let TWM handle it first (for shortcuts)
    twm_handle_key(c);
    
    // If it's ESC, don't process in terminal
    if (c == 0x1B) {
        return;
    }
    
    if (c == '\n') {
        term->command_buffer[term->cmd_index] = '\0';
        shell_execute(term->command_buffer);
        term->cmd_index = 0;
        term->command_buffer[0] = '\0';
    } else if (c == '\b') {
        if (term->cmd_index > 0) {
            term->cmd_index--;
            term->command_buffer[term->cmd_index] = '\0';
        }
    } else if (term->cmd_index < MAX_COMMAND_LENGTH - 1) {
        term->command_buffer[term->cmd_index++] = c;
        term->command_buffer[term->cmd_index] = '\0';
    }
}

// Initialize shell
void shell_init() {
    // Initialize all terminals
    for (int i = 0; i < 6; i++) {
        terminals[i].cmd_index = 0;
        terminals[i].current_line = 0;
        terminals[i].scroll_offset = 0;
        terminals[i].cursor_visible = 1;
        terminals[i].cursor_blink_counter = 0;
        terminals[i].command_buffer[0] = '\0';
        for (int j = 0; j < MAX_LINES; j++) {
            terminals[i].line_buffer[j][0] = '\0';
        }
    }
    
    current_wallpaper = 3;
    fs_mounted = 0;
    
    // Initialize TWM
    twm_init();
    
    draw_wallpaper();
    twm_draw();
    
    shell_println("", COLOR_WHITE);
    shell_println("SEPPUKU OS v2.0 - TILING WM EDITION", COLOR_LIGHT_CYAN);
    shell_println("", COLOR_WHITE);
    shell_println("Keyboard Shortcuts:", COLOR_YELLOW);
    shell_println("  ESC+1-5    - Switch workspace", COLOR_WHITE);
    shell_println("  ESC+Enter  - New terminal tile", COLOR_WHITE);
    shell_println("  ESC+J/K    - Focus next/prev tile", COLOR_WHITE);
    shell_println("  ESC+Q      - Close tile", COLOR_WHITE);
    shell_println("", COLOR_WHITE);
    shell_println("Type 'help' for commands", COLOR_LIGHT_GRAY);
    shell_println("", COLOR_WHITE);
    
    redraw_terminal();
}

// Main shell loop
void shell_run() {
    shell_init();
    
    int frame_counter = 0;
    
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            shell_handle_key(c);
            
            // Redraw everything
            draw_wallpaper();
            twm_draw();
            redraw_terminal();
        }
        
        frame_counter++;
        if (frame_counter >= 50000) {
            frame_counter = 0;
            
            terminal_t* term = get_current_terminal();
            term->cursor_blink_counter++;
            if (term->cursor_blink_counter >= 30) {
                term->cursor_blink_counter = 0;
                term->cursor_visible = !term->cursor_visible;
                redraw_terminal();
            }
        }
        
        __asm__ __volatile__("hlt");
    }
}
