
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "imu.h"
#include "capture.h"
#include <util/delay.h>
#include "midi.h"
#include "adc.h"
#include "instruments.h"
#include "process.h"

// serial communication library
#include "uart.h"
// UART file descriptor
// putchar and getchar are in uart.c
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define C0HITHRES (250)
#define C0LOTHRES (245)
#define C1HITHRES (240)
//timeout values for each task
#define t1 10

void task1(void);   //blink at 2 or 8 Hz

void initialize(void); //all the usual mcu stuff

void get_pitch_roll(float *pitch, float *roll);
volatile unsigned int time1; //timeout counter
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
    if (time1==0){task1();time1=t1;}
  }
}
char channel = 0;
ISR(ADC_vect) {

  uint8_t analog = ADCH;
  if (channel == 0) {

    if (analog > C0HITHRES && !chan1) {
      PORTD |= 1<<PD2;
      play_on = 1;
      chan1= 1;
    }

    if (analog < C0LOTHRES && chan1) {
      PORTD &= ~(1<<PD2);
      chan1 = 0;
      play_off = 1;
    }

    ADMUX |= (1<<MUX0);
    ADCSRA |= (1<<ADSC);
    channel = 1;

  } else if (channel == 1) {
    ADMUX &= ~(1<<MUX0);
    if (analog > C1HITHRES) {
      sharp_on = 1;
    } else {
      sharp_on = 0;
    }
    channel = 0;
  }
}

//**********************************************************

uint8_t instrument = 16;
char swit =0;
uint8_t note = 60;
int8_t base_note = 0;
uint16_t time = 0;
#define WAIT_VALUE (500 / t1)
#define RATE_THRES (220)
uint16_t wait = WAIT_VALUE;
//Task 1
void play_note(uint8_t y);
char handle_notes(float);
float roll = 0.0;
float pitch =0.0;
int16_t print = 250;
void task1(void)
{

  if (wait > 0) {
    --wait;
  } else {
    wait=WAIT_VALUE;
    float x,y,z;
    read_gyro(&x,&y,&z);
    if (x > RATE_THRES) {
      decrement_instrument();
    } else if (x < -RATE_THRES) {
      increment_instrument();
    } else if (y > RATE_THRES) {
      decrement_scale();
    } else if (y < -RATE_THRES) {
      increment_scale();
    }
    return;
  }

  start_conversion();
  float raw_pitch, raw_roll;
  get_pitch_roll(&raw_pitch, &raw_roll);
  melodic_instrument(raw_roll);


}


//**********************************************************
//Set it all up
void initialize(void)
{

  //set up timer 0 for 1 mSec timebase
  TIMSK0= (1<<OCIE0A);  //turn on timer 0 cmp match ISR
  OCR0A = 249;      //set the compare register to 250 time ticks
  //set prescalar to divide by 64
  TCCR0B= 3;
  // turn on clear-on-match
  TCCR0A= (1<<WGM01) ;

  DDRD |= 1<<PD2;
  PORTB &= ~(1<<PD2);
  //init the task timer
  time1=t1;

  //init the UART -- uart_init() is in uart.c
  uart_init();
  stdout = stdin = stderr = &uart_str;

  init_drum();
  init_adc();

  sei();
  init_imu(G_SCALE_245DPS, A_SCALE_2G, M_SCALE_2GS, G_ODR_95_BW_125,A_ODR_3125, M_ODR_3125);
}


