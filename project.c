// This pgm just blinks D.2 for testing the protoboard.
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "imu.h"
#include "capture.h"
#include "midi.h"
// serial communication library
#include "uart.h"
// UART file descriptor
// putchar and getchar are in uart.c
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


//timeout values for each task
#define t1 1

// task subroutines
void task1(void);   //blink at 2 or 8 Hz
void initialize(void); //all the usual mcu stuff

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



void printHeading(float hx, float hy)
{
  hx =-hx;
  hy =-hy;
  float heading;
  
  if (hy > 0)
  {
    heading = 90 - (atan(hx / hy) * (180 / M_PI));
  }
  else if (hy < 0)
  {
    heading = - (atan(hx / hy) * (180 / M_PI));
  }
  else // hy = 0
  {
    if (hx < 0) heading = 180;
    else heading = 0;
  }
  
  printf("Heading: %2.2f\n",heading);
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

//**********************************************************
char swit =1;
int16_t pitch = 0;
//Task 1
void task1(void)
{
  //PORTD^=(1<<PD2);
  float x,y,z;
  //avg_mag(&x,&y,&z,20);
  //printHeading(x,y);
  //printf("x:%2.2f:y:%2.2f:z:%2.2f\n",x,y,z);
  //print time to test USART
  //read_accel(&x,&y,&z);
  //printf("x:%02.2f:y:%02.2f:z:%02.2f\n",x,y,z);
  if (pitch > 32000 && swit) {
    swit = ! swit;
  } else if (pitch < -32000 && !swit) {
    swit = ! swit;
  }
  pitch_bend(1,pitch);
  read_gyro(&x,&y,&z);
  int yy;
  yy = y;
  if( yy> 200) yy = 200;
  if (yy < 200) yy = -200;
  pitch = yy << 7;
  if (x < 0) x = -x;
  uint8_t xx = x;
  //change_volume(1,xx);
  /*
  char c = '+';
  int16_t yy = (int16_t)y;
  if (yy<0) { yy=-yy; c = '-';}
  if( yy> 200) yy = 200;
  if (yy < 10) yy = 0;
  for(int i = 0; i < yy; i++) {
    printf("%c",c);
  }
  if (yy > 2) {
   printf("\n");
  }
  //for (int i = 0; i <
  //printf("x:%02.2f:y:%02.2f:z:%02.2f\n",x,y,z);
  //print time to test USART

  */
}


//**********************************************************
//Set it all up
void initialize(void)
{
  //set up the LED port
  DDRD = (1<<PORTD2) ;  // PORT D.2 is an ouput

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
  sei();
  change_instrument(1,17);
  send_note_off(1,0,0);
  send_note(1,60,60);

  init_imu(G_SCALE_245DPS, A_SCALE_2G, M_SCALE_2GS, G_ODR_95_BW_125,A_ODR_1600, M_ODR_50);
    // 1600 Hz (0xA)
  //crank up the ISRs
}


