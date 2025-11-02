#ifndef MOUSE_H
#define MOUSE_H

// Mouse state structure
typedef struct {
    int x;
    int y;
    unsigned char buttons;
    char dx;
    char dy;
} mouse_state_t;

// Mouse button flags
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04

// Function prototypes
void mouse_init();
void mouse_get_state(mouse_state_t* state);
int mouse_get_x();
int mouse_get_y();
unsigned char mouse_get_buttons();

// Callback for mouse events (optional)
typedef void (*mouse_handler_t)(mouse_state_t*);
void mouse_set_handler(mouse_handler_t handler);

#endif
