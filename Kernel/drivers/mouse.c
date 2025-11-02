#include "../../Lib/include/mouse.h"
#include "../../Lib/include/graphics.h"

// Port I/O helpers
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Mouse state
static mouse_state_t mouse_state = {0, 0, 0, 0, 0};
static unsigned char mouse_cycle = 0;
static char mouse_byte[3];

// Optional mouse event handler
static mouse_handler_t mouse_callback = 0;

// External function to register IRQ handler
extern void irq_install_handler(int irq, void (*handler)(void*));

// Wait for mouse controller
static void mouse_wait(unsigned char type) {
    unsigned int timeout = 100000;
    if (type == 0) {
        // Wait for output buffer
        while (timeout--) {
            if ((inb(0x64) & 1) == 1) {
                return;
            }
        }
    } else {
        // Wait for input buffer
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) {
                return;
            }
        }
    }
}

// Write to mouse
static void mouse_write(unsigned char data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

// Read from mouse
static unsigned char mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

// Mouse interrupt handler
static void mouse_handler(void* regs) {
    unsigned char status = inb(0x64);
    
    if (!(status & 0x20)) {
        return;
    }
    
    mouse_byte[mouse_cycle] = inb(0x60);
    mouse_cycle++;
    
    if (mouse_cycle == 3) {
        mouse_cycle = 0;
        
        // Parse mouse packet
        mouse_state.buttons = mouse_byte[0] & 0x07;
        
        // Get movement deltas
        mouse_state.dx = mouse_byte[1];
        mouse_state.dy = mouse_byte[2];
        
        // Check for overflow or sign bits
        if (mouse_byte[0] & 0x10) {
            mouse_state.dx |= 0xFFFFFF00;
        }
        if (mouse_byte[0] & 0x20) {
            mouse_state.dy |= 0xFFFFFF00;
        }
        
        // Update position
        mouse_state.x += mouse_state.dx;
        mouse_state.y -= mouse_state.dy; // Y is inverted
        
        // Clamp to screen bounds
        int max_x = graphics_get_width() - 1;
        int max_y = graphics_get_height() - 1;
        
        if (mouse_state.x < 0) mouse_state.x = 0;
        if (mouse_state.x > max_x) mouse_state.x = max_x;
        if (mouse_state.y < 0) mouse_state.y = 0;
        if (mouse_state.y > max_y) mouse_state.y = max_y;
        
        // Call callback if registered
        if (mouse_callback) {
            mouse_callback(&mouse_state);
        }
    }
}

// Initialize mouse driver
void mouse_init() {
    unsigned char status;
    
    // Enable auxiliary device
    mouse_wait(1);
    outb(0x64, 0xA8);
    
    // Enable interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);
    
    // Use default settings
    mouse_write(0xF6);
    mouse_read();
    
    // Enable data reporting
    mouse_write(0xF4);
    mouse_read();
    
    // Set initial position to center of screen
    mouse_state.x = graphics_get_width() / 2;
    mouse_state.y = graphics_get_height() / 2;
    mouse_state.buttons = 0;
    mouse_cycle = 0;
    
    // Register mouse interrupt handler (IRQ12)
    irq_install_handler(12, (void (*)(void*))mouse_handler);
}

// Get current mouse state
void mouse_get_state(mouse_state_t* state) {
    state->x = mouse_state.x;
    state->y = mouse_state.y;
    state->buttons = mouse_state.buttons;
    state->dx = mouse_state.dx;
    state->dy = mouse_state.dy;
}

// Get mouse X position
int mouse_get_x() {
    return mouse_state.x;
}

// Get mouse Y position
int mouse_get_y() {
    return mouse_state.y;
}

// Get mouse button states
unsigned char mouse_get_buttons() {
    return mouse_state.buttons;
}

// Set mouse event handler callback
void mouse_set_handler(mouse_handler_t handler) {
    mouse_callback = handler;
}
