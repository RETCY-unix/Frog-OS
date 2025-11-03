#include "twm.h"
#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/sound.h"

// Global TWM state
static twm_state_t twm;
static int super_key_pressed = 0;
static int needs_full_redraw = 1;

// Get TWM state (for shell to access)
twm_state_t* twm_get_state() {
    return &twm;
}

// Check if full redraw is needed
int twm_needs_redraw() {
    return needs_full_redraw;
}

// Clear redraw flag
void twm_clear_redraw_flag() {
    needs_full_redraw = 0;
}

// Request full redraw
void twm_request_redraw() {
    needs_full_redraw = 1;
}

// Draw status bar at top
static void draw_status_bar() {
    int bar_height = twm.status_bar_height;
    
    // Background
    graphics_fill_rect(0, 0, twm.screen_width, bar_height, RGB(20, 20, 20));
    graphics_draw_line(0, bar_height - 1, twm.screen_width, bar_height - 1, RGB(100, 100, 100));
    
    // Workspace indicators
    int ws_x = 10;
    for (int i = 0; i < MAX_WORKSPACES; i++) {
        int ws_width = 50;
        int ws_height = 24;
        int ws_y = (bar_height - ws_height) / 2;
        
        unsigned int bg_color;
        unsigned int text_color;
        unsigned int border_color;
        
        if (i == twm.current_workspace) {
            // Active workspace - bright
            bg_color = RGB(80, 80, 80);
            text_color = COLOR_WHITE;
            border_color = RGB(150, 150, 150);
        } else if (twm.workspaces[i].tile_count > 0) {
            // Has tiles - medium
            bg_color = RGB(40, 40, 40);
            text_color = RGB(180, 180, 180);
            border_color = RGB(80, 80, 80);
        } else {
            // Empty - dark
            bg_color = RGB(25, 25, 25);
            text_color = RGB(100, 100, 100);
            border_color = RGB(50, 50, 50);
        }
        
        graphics_fill_rect(ws_x, ws_y, ws_width, ws_height, bg_color);
        graphics_draw_rect(ws_x, ws_y, ws_width, ws_height, border_color);
        
        // Draw workspace number
        char num[2] = {'1' + i, '\0'};
        graphics_draw_string(ws_x + 20, ws_y + 8, num, text_color);
        
        ws_x += ws_width + 8;
    }
    
    // System info on right
    graphics_draw_string(twm.screen_width - 200, 10, "SEPPUKU OS", COLOR_WHITE);
    graphics_draw_string(twm.screen_width - 200, 22, "ESC = Super", RGB(120, 120, 120));
}

// Calculate tile layout for workspace (i3 master-stack style)
void twm_relayout(int ws) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (workspace->tile_count == 0) return;
    
    int usable_y = twm.status_bar_height + 2;
    int usable_height = twm.screen_height - usable_y - 2;
    int usable_width = twm.screen_width - 4;
    
    if (workspace->tile_count == 1) {
        // Single tile - fullscreen
        workspace->tiles[0].x = 2;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = usable_width;
        workspace->tiles[0].height = usable_height;
    } else if (workspace->tile_count == 2) {
        // Two tiles - split vertical 50/50
        int half_width = usable_width / 2;
        int gap = 2;
        
        workspace->tiles[0].x = 2;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = half_width - gap;
        workspace->tiles[0].height = usable_height;
        
        workspace->tiles[1].x = half_width + gap + 2;
        workspace->tiles[1].y = usable_y;
        workspace->tiles[1].width = half_width - gap;
        workspace->tiles[1].height = usable_height;
    } else {
        // 3+ tiles - master (60%) + stack (40%)
        int master_width = (usable_width * 3) / 5;
        int stack_width = usable_width - master_width - 4;
        int gap = 2;
        
        // Master tile (left side, full height)
        workspace->tiles[0].x = 2;
        workspace->tiles[0].y = usable_y;
        workspace->tiles[0].width = master_width;
        workspace->tiles[0].height = usable_height;
        
        // Stack tiles (right side, split vertically)
        int stack_count = workspace->tile_count - 1;
        int stack_height = usable_height / stack_count;
        
        for (int i = 1; i < workspace->tile_count; i++) {
            workspace->tiles[i].x = master_width + gap + 2;
            workspace->tiles[i].y = usable_y + (i - 1) * stack_height;
            workspace->tiles[i].width = stack_width;
            workspace->tiles[i].height = stack_height - (i < workspace->tile_count - 1 ? gap : 0);
        }
    }
}

// Add a tile to workspace
void twm_add_tile(int ws, int type) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (workspace->tile_count >= MAX_TILES) {
        sound_beep(BEEP_ERROR, 80);
        return;
    }
    
    tile_t* tile = &workspace->tiles[workspace->tile_count];
    tile->type = type;
    tile->active = 1;
    tile->data = 0;
    
    workspace->tile_count++;
    workspace->focused_tile = workspace->tile_count - 1;
    
    twm_relayout(ws);
    needs_full_redraw = 1;
    sound_beep(BEEP_CLICK, 50);
}

// Remove a tile from workspace
void twm_remove_tile(int ws, int tile_index) {
    workspace_t* workspace = &twm.workspaces[ws];
    
    if (tile_index >= workspace->tile_count || workspace->tile_count == 0) return;
    
    // Don't allow removing the last tile
    if (workspace->tile_count == 1) {
        sound_beep(BEEP_ERROR, 80);
        return;
    }
    
    // Shift tiles down
    for (int i = tile_index; i < workspace->tile_count - 1; i++) {
        workspace->tiles[i] = workspace->tiles[i + 1];
    }
    
    workspace->tile_count--;
    
    // Adjust focus
    if (workspace->focused_tile >= workspace->tile_count) {
        workspace->focused_tile = workspace->tile_count - 1;
    }
    
    twm_relayout(ws);
    needs_full_redraw = 1;
    sound_beep(600, 50);
}

// Focus next tile (like i3's "focus right/down")
void twm_focus_next() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    if (workspace->tile_count <= 1) return;
    
    workspace->focused_tile = (workspace->focused_tile + 1) % workspace->tile_count;
    needs_full_redraw = 1;
    sound_beep(BEEP_CLICK, 30);
}

// Focus previous tile (like i3's "focus left/up")
void twm_focus_prev() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    if (workspace->tile_count <= 1) return;
    
    workspace->focused_tile--;
    if (workspace->focused_tile < 0) {
        workspace->focused_tile = workspace->tile_count - 1;
    }
    needs_full_redraw = 1;
    sound_beep(BEEP_CLICK, 30);
}

// Switch workspace
void twm_switch_workspace(int ws) {
    if (ws < 0 || ws >= MAX_WORKSPACES || ws == twm.current_workspace) return;
    
    twm.current_workspace = ws;
    needs_full_redraw = 1;
    sound_beep(800, 40);
}

// Draw all tiles in current workspace (ONLY draws borders and backgrounds)
void twm_draw() {
    workspace_t* workspace = &twm.workspaces[twm.current_workspace];
    
    // Draw status bar
    draw_status_bar();
    
    // Draw tiles (borders and backgrounds only!)
    for (int i = 0; i < workspace->tile_count; i++) {
        tile_t* tile = &workspace->tiles[i];
        
        unsigned int border_color;
        unsigned int bg_color = RGB(15, 15, 15);
        int border_width;
        
        if (i == workspace->focused_tile) {
            // Focused tile - bright border
            border_color = RGB(120, 120, 120);
            border_width = 2;
        } else {
            // Unfocused tile - dim border
            border_color = RGB(50, 50, 50);
            border_width = 1;
        }
        
        // Draw background
        graphics_fill_rect(tile->x, tile->y, tile->width, tile->height, bg_color);
        
        // Draw border
        for (int b = 0; b < border_width; b++) {
            graphics_draw_rect(
                tile->x + b, 
                tile->y + b, 
                tile->width - b * 2, 
                tile->height - b * 2, 
                border_color
            );
        }
    }
}

// Initialize TWM
void twm_init() {
    twm.screen_width = graphics_get_width();
    twm.screen_height = graphics_get_height();
    twm.status_bar_height = 35;
    twm.current_workspace = 0;
    needs_full_redraw = 1;
    
    // Initialize all workspaces
    for (int i = 0; i < MAX_WORKSPACES; i++) {
        twm.workspaces[i].tile_count = 0;
        twm.workspaces[i].focused_tile = 0;
    }
    
    // Add first terminal tile to workspace 0
    twm_add_tile(0, TILE_TERMINAL);
}

// Handle keyboard for TWM shortcuts
// Returns 1 if TWM handled the key (needs redraw), 0 otherwise
int twm_handle_key(char c) {
    // ESC acts as "Super/Mod" key
    if (c == 0x1B) {
        super_key_pressed = 1;
        return 0;
    }
    
    if (super_key_pressed) {
        super_key_pressed = 0;
        
        // Super+1-5: Switch workspace
        if (c >= '1' && c <= '5') {
            twm_switch_workspace(c - '1');
            return 1;
        }
        
        // Super+Enter: New terminal tile
        if (c == '\n') {
            twm_add_tile(twm.current_workspace, TILE_TERMINAL);
            return 1;
        }
        
        // Super+Q: Close focused tile
        if (c == 'q' || c == 'Q') {
            workspace_t* ws = &twm.workspaces[twm.current_workspace];
            if (ws->tile_count > 1) {
                twm_remove_tile(twm.current_workspace, ws->focused_tile);
            } else {
                sound_beep(BEEP_ERROR, 80);
            }
            return 1;
        }
        
        // Super+J: Focus next (vim down)
        if (c == 'j' || c == 'J') {
            twm_focus_next();
            return 1;
        }
        
        // Super+K: Focus previous (vim up)
        if (c == 'k' || c == 'K') {
            twm_focus_prev();
            return 1;
        }
        
        // Super+H: Focus left (future: move between tiles horizontally)
        if (c == 'h' || c == 'H') {
            twm_focus_prev();
            return 1;
        }
        
        // Super+L: Focus right (future: move between tiles horizontally)
        if (c == 'l' || c == 'L') {
            twm_focus_next();
            return 1;
        }
    }
    
    return 0;
}
