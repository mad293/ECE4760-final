#include "instruments.h"
#include "midi.h"
#include <inttypes.h>

#define DRUM_CHANNEL 4
#define SYNTH_DRUM 118
#define DRUM_THRES 50
#define MAIN_CHANNEL 2

#define TOM 60
#define SNARE 70
#define CYMBAL 100
#define BASS 30

static int16_t instrument;
static int16_t base_note;
static uint8_t note;


char cmajor[15] = {48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72};
void init_instruments(void) {
  init_drum();
  base_note = 0;
  instrument = 3;
}

void init_drum(void) {
  change_instrument(DRUM_CHANNEL,SYNTH_DRUM);
}

void hit_drum(enum dir a) {
  switch(a) {
    case LEFT:
      send_note(DRUM_CHANNEL, 100, SNARE);
    case RIGHT:
      send_note(DRUM_CHANNEL, 100, TOM);
    case UP:
      send_note(DRUM_CHANNEL, 100, CYMBAL);
    case DOWN:
      send_note(DRUM_CHANNEL, 100, BASS);
  }
}
float old_roll;
char note_on =0;
char handle_notes(float roll)
{

  int8_t sharp = 0;
  if (sharp_on) {
    sharp = 1;
  }
  if (play_on) {
    float froll = roll;
    froll /= 90.0;
    froll *= 8;
    int8_t iroll = (int8_t)froll;
    iroll += 7;
    play_note_on(base_note + cmajor[iroll]);
    play_on = 0;
    note_on =1;
    return 1;
  }else{
    if (play_off) {
      play_note_off();
      play_off = 0;
      note_on =0;
    }
    if (note_on) {
      modulate_note(roll);
    }
    return 0;
  }
}

void play_note_on(uint8_t y) {

  note =(uint8_t)y;
  send_note(MAIN_CHANNEL,100,note);
}
void play_note_off(void) {

  send_note_off(MAIN_CHANNEL,100,note);
}
void melodic_instrument(float roll) {
  if (handle_notes(roll)) {
    old_roll = roll;
  }
}

int16_t convert_roll(float roll) {
  roll -= old_roll;
  roll/=90.0;
  roll*=2000;
  return (2000 + ((int16_t)roll) << 2);
}

void modulate_note(float roll) {
  if (sharp_on) {
    pitch_bend(MAIN_CHANNEL,convert_roll(roll));
  }
}

void play_drum(float x, float y, float z) {

  if (y < -DRUM_THRES) {
    hit_drum(DOWN);
  }
}


void increment_instrument(void) {
  instrument++;
  change_instrument(2,instrument%128);
}

void decrement_instrument(void) {
  instrument--;
  change_instrument(2,instrument%128);
}

void increment_scale(void) {
  base_note++;
}

void decrement_scale(void) {
  base_note--;
}

