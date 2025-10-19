#ifndef KEYBOARD_H
#define KEYBOARD_H

// Special keys
#define KEY_BACKSPACE 0x08
#define KEY_TAB 0x09
#define KEY_ENTER 0x0A
#define KEY_ESC 0x1B
#define KEY_UP 0x80
#define KEY_DOWN 0x81
#define KEY_LEFT 0x82
#define KEY_RIGHT 0x83

// Function prototypes
void keyboard_init();
char keyboard_getchar();
int keyboard_available();

// Callback for key press (optional)
typedef void (*key_handler_t)(char);
void keyboard_set_handler(key_handler_t handler);

#endif
