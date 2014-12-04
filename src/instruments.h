#include <inttypes.h>
volatile char chan1;
volatile char sharp_on;
volatile char play_off;
volatile char play_on;

enum dir {
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

void hit_drum(enum dir a);
void init_drum(void);
void play_drum(float x, float y, float z);

void play_note_off(void);
void play_note_on(uint8_t y);

void init_instruments(void);
void increment_instrument(void);
void decrement_instrument(void);
void increment_scale(void);
void decrement_scale(void);
void modulate_note(float roll);
void melodic_instrument(float roll);
