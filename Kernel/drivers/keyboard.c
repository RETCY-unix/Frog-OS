#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/screen.h"

// Keyboard scancode to ASCII map (US layout)
static unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Shift key pressed map
static unsigned char keyboard_map_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_read = 0;
static int buffer_write = 0;

// Keyboard state
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock = 0;

// Optional key handler callback
static key_handler_t key_callback = 0;

// External function to register IRQ handler
extern void irq_install_handler(int irq, void (*handler)(void*));

// Add character to buffer
static void keyboard_buffer_put(char c) {
    int next = (buffer_write + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_read) {
        keyboard_buffer[buffer_write] = c;
        buffer_write = next;
    }
}

// Get character from buffer
static char keyboard_buffer_get() {
    if (buffer_read == buffer_write) {
        return 0;
    }
    char c = keyboard_buffer[buffer_read];
    buffer_read = (buffer_read + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Check if buffer has data
int keyboard_available() {
    return buffer_read != buffer_write;
}

// Keyboard interrupt handler
static void keyboard_handler(void* regs) {
    unsigned char scancode;
    
    // Read scancode from keyboard
    __asm__ __volatile__("inb %1, %0" : "=a"(scancode) : "Nd"((unsigned short)0x60));
    
    // Check for key release (bit 7 set)
    if (scancode & 0x80) {
        // Key released
        scancode &= 0x7F;
        
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 0;
        } else if (scancode == 0x38) {
            alt_pressed = 0;
        }
    } else {
        // Key pressed
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 1;
        } else if (scancode == 0x38) {
            alt_pressed = 1;
        } else if (scancode == 0x3A) {
            caps_lock = !caps_lock;
        } else {
            // Convert scancode to ASCII
            char c = 0;
            
            if (shift_pressed || caps_lock) {
                c = keyboard_map_shift[scancode];
            } else {
                c = keyboard_map[scancode];
            }
            
            if (c != 0) {
                keyboard_buffer_put(c);
                
                // Call callback if registered
                if (key_callback) {
                    key_callback(c);
                }
            }
        }
    }
}

// Initialize keyboard driver
void keyboard_init() {
    buffer_read = 0;
    buffer_write = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    caps_lock = 0;
    
    // Register keyboard interrupt handler (IRQ1)
    irq_install_handler(1, (void (*)(void*))keyboard_handler);
}

// Get a character (blocking)
char keyboard_getchar() {
    while (!keyboard_available()) {
        __asm__ __volatile__("hlt");
    }
    return keyboard_buffer_get();
}

// Set key handler callback
void keyboard_set_handler(key_handler_t handler) {
    key_callback = handler;
}
