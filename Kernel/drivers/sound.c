#include "../../Lib/include/sound.h"

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define PIT_CHANNEL_2   0x42
#define PIT_COMMAND     0x43
#define PC_SPEAKER_PORT 0x61

void sound_init() {
    sound_stop();
}

void sound_play(unsigned int frequency) {
    if (frequency == 0) {
        sound_stop();
        return;
    }
    
    unsigned int divisor = 1193180 / frequency;
    
    outb(PIT_COMMAND, 0xB6);
    outb(PIT_CHANNEL_2, (unsigned char)(divisor & 0xFF));
    outb(PIT_CHANNEL_2, (unsigned char)((divisor >> 8) & 0xFF));
    
    unsigned char temp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, temp | 3);
}

void sound_stop() {
    unsigned char temp = inb(PC_SPEAKER_PORT) & 0xFC;
    outb(PC_SPEAKER_PORT, temp);
}

static void delay_ms(unsigned int ms) {
    // MUCH LONGER DELAY - multiply by 100000 instead of 1000
    for (unsigned int i = 0; i < ms * 100000; i++) {
        __asm__ __volatile__("nop");
    }
}

void sound_beep(unsigned int frequency, unsigned int duration_ms) {
    sound_play(frequency);
    delay_ms(duration_ms);
    sound_stop();
}
