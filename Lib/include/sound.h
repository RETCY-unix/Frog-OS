#ifndef SOUND_H
#define SOUND_H

// Initialize sound driver
void sound_init();

// Play a tone at given frequency (Hz)
void sound_play(unsigned int frequency);

// Stop sound
void sound_stop();

// Play a beep for given duration (milliseconds)
void sound_beep(unsigned int frequency, unsigned int duration_ms);

// Common musical notes (frequencies in Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Sound effects
#define BEEP_DEFAULT 800
#define BEEP_ERROR   200
#define BEEP_SUCCESS 1000
#define BEEP_CLICK   600

#endif
