//http://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html


#include "inttypes.h"
#include <stdio.h>
#include <util/delay.h>
#include "uart.h"
#include "midi.h"
void send_byte(uint8_t byte) {
  uart_putchar(byte,stdout);
}

void send_status_byte(uint8_t cmd, uint8_t channel) {
  send_byte((cmd << 4) | (0x0F & channel));
}


void pitch_bend(uint8_t channel, uint16_t value) {
value = 0x3FFF & value;
//Status byte : 1110 CCCC
send_status_byte(0b1110,channel);
//Data byte 1 : 0LLL LLLL
uart_putchar((uint8_t)(0x7F&value),stdout);
//Data byte 2 : 0MMM MMMM
uart_putchar((uint8_t)(0x7F&(value>>7)),stdout);
}

void change_instrument(uint8_t channel, uint8_t instrument) {
  send_status_byte(0b1100,channel);
  send_byte(0x7F&instrument);
}

void change_volume(uint8_t chan, uint8_t vol) {
/*
0xB0 - 0x07 - 0x64
*/

send_status_byte(0xB,1);
send_byte(0x07);
send_byte(vol);

}

void send_controller_command(uint8_t channel, uint8_t controller,uint8_t value) {
  send_status_byte(0b1011,channel);
  send_byte(0x7F&controller);
  send_byte(0x7F&value);
}

void write_test_note(void) {
  //Status byte : 1001 CCCC
  //Data byte 1 : 0PPP PPPP
  //Data byte 2 : 0VVV VVVV
  //change_instrument(1,25);
  //send_controller_command(1,64,127);
  uint8_t status = 0b10010001;
  uint8_t pitch = 0b01111111;
  uint8_t vel = 0b01111111;
    int i;
  for (pitch = 0; pitch < 128; pitch++) {
    uart_putchar(status,stdout);
    uart_putchar(pitch,stdout);
    uart_putchar(vel,stdout);
    for (int i = 0; i< 1024; i++) {
      send_controller_command(1,1,i>>3);
      //send_controller_command(1,10, i>>3);
      //pitch_bend(1,i*2);
      _delay_us(200);
    }
    uart_putchar(0x8F & status,stdout);
    uart_putchar(pitch,stdout);
    uart_putchar(vel,stdout);

  }
}
