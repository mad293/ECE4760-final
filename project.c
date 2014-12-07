// This pgm just blinks D.2 for testing the protoboard.
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "imu.h"
#include "capture.h"
#include <util/delay.h>
#include "midi.h"
// serial communication library
#include "uart.h"
// UART file descriptor
// putchar and getchar are in uart.c
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


//timeout values for each task
#define t1 25

// task subroutines
char play_on = 0;
char play_off = 0;
void task1(void);   //blink at 2 or 8 Hz
void initialize(void); //all the usual mcu stuff
void get_pitch_roll(float *pitch, float *roll);
void play_note_off(void);
void play_note_on(uint8_t y);
volatile char chan1;
volatile char sharp_on;
volatile unsigned int time1; //timeout counter
unsigned char led;        //light states
unsigned int ticks ;      // running time

//**********************************************************
//timer 0 compare ISR
ISR (TIMER0_COMPA_vect)
{
  //Decrement the  time if they are not already zero
  if (time1>0)  --time1;
}

//**********************************************************
//Entry point and task scheduler loop
int main(void)
{
  initialize();
  //main task scheduler loop
  while(1)
  {
    if (time1==0){time1=t1; task1();}
  }
}
char channel = 0;
ISR(ADC_vect) {

  uint8_t analog = ADCH;
  if (channel == 0) {


    if (analog > 250 && !chan1) {
      play_on = 1;
      chan1= 1;
    }

    if (analog < 245 && chan1) {
      chan1 = 0;
      play_off = 1;
    }

    ADMUX |= (1<<MUX0);
    ADCSRA |= (1<<ADSC);
    channel = 1;

  } else if (channel == 1) {
    ADMUX &= ~(1<<MUX0);
    if (analog > 240) {
      sharp_on = 1;
    } else {
      sharp_on = 0;
    }
    channel = 0;
  }
}

//**********************************************************
void get_pitch_roll(float *pitch, float *roll) {

  float x,y,z;
  read_accel(&x,&y,&z);
  *pitch = atan2(x, sqrt(y * y) + (z * z));
  *roll = atan2(y, sqrt(x * x) + (z * z));
  *pitch *= 180.0 / M_PI;
  *roll *= 180.0 / M_PI;

}

uint8_t instrument = 16;
char swit =0;
uint8_t note = 60;
uint8_t base_note = 0;
uint16_t time = 0;
#define WAIT_VALUE (300 / t1)
#define RATE_THRES (270)
uint16_t wait = WAIT_VALUE;
//Task 1
void play_note(uint8_t y);
char handle_notes(float);
float old_roll;
void task1(void)
{

  ADCSRA |= (1<<ADSC);
  float x,y,z;

    float roll,pitch;
    get_pitch_roll(&pitch,&roll);
  if(handle_notes(roll)) {
    old_roll = roll;
  }


  if (chan1 && sharp_on) {
    roll -= old_roll;
    roll/=90.0;
    roll*=2000;
    pitch_bend(2,(2000+((int16_t)roll))<<2);
  } else {
    read_gyro(&x,&y,&z);
    if (wait) wait--;
    if (x > RATE_THRES  && !wait) {
      instrument++;
      change_instrument(2,instrument%128);
      wait = WAIT_VALUE;
    } else if (x<-RATE_THRES && !wait) {
      wait = WAIT_VALUE;
      instrument--;
      change_instrument(2,instrument%128);
    } else if(y > RATE_THRES && !wait) {
      wait = WAIT_VALUE;
      base_note -= 1;
    } else if(y < -RATE_THRES && !wait) {
      wait = WAIT_VALUE;
      base_note += 1;
    }
  }
}
char cmajor[15] = {48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72};
char handle_notes(float roll)
{
  int8_t sharp = 0;
  if (sharp_on) {
    sharp = 1;
  }
  if (play_on) {
    float froll = roll;
    froll /= 90.0;
    froll *= 7;
    int8_t iroll = (int8_t)froll;
    iroll += 8;

    play_note_on(base_note+cmajor[iroll]);
    PORTD |= (1<<PD2);
    play_on = 0;
    return 1;
  }else
  if (play_off) {
    play_note_off();
    PORTD &= ~(1<<PD2);
    play_off = 0;
  }
  return 0;
}

void play_note_on(uint8_t y) {

  note =(uint8_t)y;
  send_note(2,100,note);
}
void play_note_off(void) {

  send_note_off(2,100,note);
}

//**********************************************************
//Set it all up
void initialize(void)
{
  //set up the LED port
  DDRD = (1<<PD2) ;  // PORT D.2 is an ouput
  PORTD &= ~(1<<PD2);
  //set up timer 0 for 1 mSec timebase
  TIMSK0= (1<<OCIE0A);  //turn on timer 0 cmp match ISR
  OCR0A = 249;      //set the compare register to 250 time ticks
  //set prescalar to divide by 64
  TCCR0B= 3;
  // turn on clear-on-match
  TCCR0A= (1<<WGM01) ;

  //init the LED status
  led=0x00;

  //init the task timer
  time1=t1;
  //init the UART -- uart_init() is in uart.c
  uart_init();
  stdout = stdin = stderr = &uart_str;
  ADMUX = (1<<ADLAR) | (1<<REFS0);
  ADCSRA = (1<<ADEN) | (1<<ADIF) | (1<<ADIE) + 7;
  sei();
  change_instrument(2,17);
  change_instrument(4,17);
  send_note_off(2,0,0);
  send_note(3,60,60);
  send_note_off(2,0,0);
  send_note(3,60,60);

  init_imu(G_SCALE_245DPS, A_SCALE_2G, M_SCALE_2GS, G_ODR_95_BW_125,A_ODR_50, M_ODR_50);
  // 1600 Hz (0xA)
  //crank up the ISRs
  ADCSRA |= (1<<ADSC);
}


