//http://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
//http://www.midi.org/techspecs/midimessages.php

#include "inttypes.h"
#include <stdio.h>
#include <util/delay.h>
#include "uart.h"
#include "midi.h"


#define m4(x) (x&0x0F) // mask out the upper 4 bits
#define m7(x) (x&0x7F) // mask out the upper bit
#define m14(x) (x&0x3FFF) // mask out the upper 2 bits of a 16 bit number


// These two functions turn 0b00HHHHHHHLLLLLLL into 0b0HHHHHHH and 0b0LLLLLLL
#define up7(x) (0x7F&(x>>7))
#define lo7(x) (0x7F&(x))

void send_byte(uint8_t byte) {
  uart_putchar(byte,stdout);
}

void send_status_byte(uint8_t cmd, uint8_t channel) {
  send_byte((cmd << 4) | m4(channel));
}


void pitch_bend(uint8_t channel, int16_t value) {
  value = value >> 2;

  send_status_byte(PITCH,channel);
  send_byte((uint8_t)(lo7(value)));
  send_byte((uint8_t)(up7(value)));
}

void change_instrument(uint8_t channel, uint8_t instrument) {
  send_status_byte(INSTRUMENT,channel);
  send_byte(m7(instrument));
}

void change_volume(uint8_t chan, uint8_t vol) {
  send_controller_command(chan,VOLUME,vol);
}

void send_controller_command(uint8_t channel, uint8_t controller,uint8_t value) {
  send_status_byte(0b1011,channel);
  send_byte((controller));
  send_byte((value));
}

void send_note(uint8_t channel, uint8_t vel, uint8_t pitch) {
  send_status_byte(NOTE_ON,channel);
  send_byte(pitch);
  send_byte(vel);
}

void send_note_off(uint8_t channel, uint8_t vel, uint8_t pitch) {
  send_status_byte(NOTE_OFF,channel);
  send_byte(pitch);
  send_byte(vel);
}
