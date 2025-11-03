#ifndef TWM_H
#define TWM_H

#include "../../Lib/include/graphics.h"

// Tile types
#define TILE_TERMINAL 0
#define TILE_EMPTY    1

// Maximum tiles and workspaces
#define MAX_TILES 6
#define MAX_WORKSPACES 5

// Tile structure
typedef struct {
    int x, y, width, height;
    int type;
    int active;
    void* data; // Pointer to terminal state or other data
} tile_t;

// Workspace structure
typedef struct {
    tile_t tiles[MAX_TILES];
    int tile_count;
    int focused_tile;
} workspace_t;

// TWM state
typedef struct {
    workspace_t workspaces[MAX_WORKSPACES];
    int current_workspace;
    int screen_width;
    int screen_height;
    int status_bar_height;
} twm_state_t;

// Function prototypes
void twm_init();
int twm_handle_key(char c);  // Returns 1 if TWM handled key, 0 if not
void twm_draw();
void twm_add_tile(int workspace, int type);
void twm_remove_tile(int workspace, int tile_index);
void twm_focus_next();
void twm_focus_prev();
void twm_switch_workspace(int workspace);
void twm_relayout(int workspace);
twm_state_t* twm_get_state();
int twm_needs_redraw();
void twm_clear_redraw_flag();
void twm_request_redraw();

#endif
