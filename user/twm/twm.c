#include "twm.h"
#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/sound.h"

// Global TWM state
static twm_state_t twm;
static int super_key_pressed = 0;

// Get TWM state (for shell to access)
twm_state_t* twm_get_state() {
    return &twm;
}

// Draw status bar at top
static void draw_status_bar() {
    int bar_height = twm.status_bar_height;
    
    // Background
    graphics_fill_rect(0, 0, twm.screen_width, bar_height, RGB(25, 25, 25));
    graphics_draw_line(0, bar_height - 1, twm.screen_width, bar_height - 1, RGB(80, 80, 80));
    
    // Workspace indicators
    int ws_x = 10;
    for (int i = 0; i < MAX_WORKSPACES; i++) {
        int ws_width = 40;
        int ws_height = 20;
        int ws_y = (bar_height - ws_height) / 2;
        
        unsigned int bg_color;
        unsigned int text_color;
        
        if (i == twm.current_workspace) {
            // Active workspace
            bg_color = RGB(100, 100, 100);
            text_color = COLOR_WHITE;
        } else if (twm.workspaces[i].tile_count > 0) {
            // Has tiles
            bg_color = RGB(50, 50, 50);
            text_color = RGB(200, 200, 200);
        } else {
            // Empty
            bg_color = RGB(35, 35, 35);
            text_color = RGB(100, 100, 100);
        }
        
        graphics_fill_rect(ws_x, ws_y, ws_width, ws_height, bg_color);
        graphics_draw_rect(ws_x, ws_y, ws_width, ws_height, RGB(80, 80, 80));
        
        // Draw number
        char num[2] = {'1' + i, '\0'};
        graphics_draw_string(ws_x + 15, ws_y + 6, num, text_color);
        
        ws_x += ws_width + 5;
    }
    
    // System info on right
    graphics_draw_string(twm.screen_width - 250, 12, "SEPPUKU OS", COLOR_WHITE);
    graphics_draw_string(twm.screen_width - 140, 12, "Super+H/J/K/L", RGB(150, 150, 150));
}

// Calculate tile layout for workspace
void twm_relayout(int ws) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (workspace->tile_count == 0) return;
    
    int usable_y = twm.status_bar_height;
    int usable_height = twm.screen_height - usable_y;
    
    if (workspace->tile_count == 1) {
        // Single tile - fullscreen
        workspace->tiles[0].x = 0;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = twm.screen_width;
        workspace->tiles[0].height = usable_height;
    } else if (workspace->tile_count == 2) {
        // Two tiles - split vertical
        int half_width = twm.screen_width / 2;
        
        workspace->tiles[0].x = 0;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = half_width;
        workspace->tiles[0].height = usable_height;
        
        workspace->tiles[1].x = half_width;
        workspace->tiles[1].y = usable_y;
        workspace->tiles[1].width = twm.screen_width - half_width;
        workspace->tiles[1].height = usable_height;
    } else {
        // 3+ tiles - master + stack layout
        int master_width = twm.screen_width * 2 / 3;
        int stack_width = twm.screen_width - master_width;
        int stack_height = usable_height / (workspace->tile_count - 1);
        
        // Master tile
        workspace->tiles[0].x = 0;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = master_width;
        workspace->tiles[0].height = usable_height;
        
        // Stack tiles
        for (int i = 1; i < workspace->tile_count; i++) {
            workspace->tiles[i].x = master_width;
            workspace->tiles[i].y = usable_y + (i - 1) * stack_height;
            workspace->tiles[i].width = stack_width;
            workspace->tiles[i].height = stack_height;
        }
    }
}

// Add a tile to workspace
void twm_add_tile(int ws, int type) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (workspace->tile_count >= MAX_TILES) return;
    
    tile_t* tile = &workspace->tiles[workspace->tile_count];
    tile->type = type;
    tile->active = 1;
    tile->data = 0; // Will be set by shell
    
    workspace->tile_count++;
    workspace->focused_tile = workspace->tile_count - 1;
    
    twm_relayout(ws);
    sound_beep(BEEP_CLICK, 50);
}

// Remove a tile from workspace
void twm_remove_tile(int ws, int tile_index) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (tile_index >= workspace->tile_count) return;
    
    // Shift tiles down
    for (int i = tile_index; i < workspace->tile_count - 1; i++) {
        workspace->tiles[i] = workspace->tiles[i + 1];
    }
    
    workspace->tile_count--;
    
    if (workspace->focused_tile >= workspace->tile_count && workspace->tile_count > 0) {
        workspace->focused_tile = workspace->tile_count - 1;
    }
    
    twm_relayout(ws);
    sound_beep(600, 50);
}

// Focus next tile
void twm_focus_next() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    if (workspace->tile_count <= 1) return;
    
    workspace->focused_tile = (workspace->focused_tile + 1) % workspace->tile_count;
    sound_beep(BEEP_CLICK, 30);
}

// Focus previous tile
void twm_focus_prev() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    if (workspace->tile_count <= 1) return;
    
    workspace->focused_tile--;
    if (workspace->focused_tile < 0) {
        workspace->focused_tile = workspace->tile_count - 1;
    }
    sound_beep(BEEP_CLICK, 30);
}

// Switch workspace
void twm_switch_workspace(int ws) {
    if (ws < 0 || ws >= MAX_WORKSPACES || ws == twm.current_workspace) return;
    
    twm.current_workspace = ws;
    sound_beep(800, 40);
}

// Draw all tiles in current workspace
void twm_draw() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    // Draw status bar
    draw_status_bar();
    
    // Draw tiles
    for (int i = 0; i < workspace->tile_count; i++) {
        tile_t* tile = &workspace->tiles[i];
        
        // Draw border (thicker if focused)
        unsigned int border_color = (i == workspace->focused_tile) ? 
            RGB(150, 150, 150) : RGB(60, 60, 60);
        
        int border_width = (i == workspace->focused_tile) ? 3 : 1;
        
        for (int b = 0; b < border_width; b++) {
            graphics_draw_rect(
                tile->x + b, 
                tile->y + b, 
                tile->width - b * 2, 
                tile->height - b * 2, 
                border_color
            );
        }
        
        // Fill background
        graphics_fill_rect(
            tile->x + border_width, 
            tile->y + border_width,
            tile->width - border_width * 2, 
            tile->height - border_width * 2,
            RGB(20, 20, 20)
        );
    }
}

// Initialize TWM
void twm_init() {
    twm.screen_width = graphics_get_width();
    twm.screen_height = graphics_get_height();
    twm.status_bar_height = 30;
    twm.current_workspace = 0;
    
    // Initialize all workspaces
    for (int i = 0; i < MAX_WORKSPACES; i++) {
        twm.workspaces[i].tile_count = 0;
        twm.workspaces[i].focused_tile = 0;
    }
    
    // Add first terminal tile to workspace 0
    twm_add_tile(0, TILE_TERMINAL);
}

// Handle keyboard for TWM shortcuts
void twm_handle_key(char c) {
    // Super key detection (we'll use ESC as Super for now since we don't have Super)
    // In real implementation, you'd detect Windows/Super key scancode
    
    if (c == 0x1B) { // ESC acts as "Super" key
        super_key_pressed = 1;
        return;
    }
    
    if (super_key_pressed) {
        super_key_pressed = 0;
        
        // Super+1-5: Switch workspace
        if (c >= '1' && c <= '5') {
            twm_switch_workspace(c - '1');
            return;
        }
        
        // Super+Enter: New terminal tile
        if (c == '\n') {
            twm_add_tile(twm.current_workspace, TILE_TERMINAL);
            return;
        }
        
        // Super+Q: Close focused tile
        if (c == 'q' || c == 'Q') {
            workspace_t* ws = &twm.workspaces[twm.current_workspace];
            if (ws->tile_count > 1) {
                twm_remove_tile(twm.current_workspace, ws->focused_tile);
            }
            return;
        }
        
        // Super+J: Focus next (vim down)
        if (c == 'j' || c == 'J') {
            twm_focus_next();
            return;
        }
        
        // Super+K: Focus previous (vim up)
        if (c == 'k' || c == 'K') {
            twm_focus_prev();
            return;
        }
        
        // Super+H/L: Could be used for resizing later
    }
}
