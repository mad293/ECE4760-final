// This pgm just blinks D.2 for testing the protoboard.
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "imu.h"
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
/**
* @brief if 1 a note is played
*/
char play_on = 0;
/**
* @brief if 0 a note is turned off
*/
char play_off = 0;
void task1(void);   //blink at 2 or 8 Hz
void initialize(void); //all the usual mcu stuff
void get_pitch_roll(float *pitch, float *roll);
void play_note_off(void);
void play_note_on(uint8_t y);
volatile char chan1;
/**
* @brief whether or not the middle note is held down.
*/
volatile char adc_chan1_on;
/**
* @brief task1 counter
*/
volatile unsigned int time1; //timeout counter

//**********************************************************
//timer 0 compare ISR
/**
 * @brief This ISR keeps a millisecond timer for timing the main task.
 *
 */
ISR (TIMER0_COMPA_vect)
{
  //Decrement the  time if they are not already zero
  if (time1>0)  --time1;
}

//**********************************************************

/**
 *Entry point and task scheduler loop
 *
 */
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

/**
 * @brief Rather than waiting in while loop for the ADC readings, we use the ADC interupt that sets the values asynchronously.
 * This function checks to see if the values of the flex resistors have indicted whether a finger has been bent or not. If the
 * value of Chanel 0 of the ADC is greater than 250 we set a flag to play a note, if it is less than 245, we set a flag to end
 * the note. The other flex resitor, on the middle finger has a larger range so determining whether it is on or off is easier,
 * we merely check to see if it is greater than 240, and set the appropiate flag.
 *
 */
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
      adc_chan1_on = 1;
    } else {
      adc_chan1_on = 0;
    }
    channel = 0;
  }
}

//**********************************************************

/**
 * @brief This function reads the accelerometer and calculates the pitch and roll of the device.
 *
 * @param pitch pointer to a float that will contain the current pitch measured by the accelerometer.
 * @param roll pointer to a float that will contain the current roll measured by the accelerometer.
 */
void get_pitch_roll(float *pitch, float *roll) {

  float x,y,z;
  read_accel(&x,&y,&z);
  *pitch = atan2(x, sqrt((y * y) + (z * z)));
  *roll = atan2(y, sqrt((x * x) + (z * z)));
  *pitch *= 180.0 / M_PI;
  *roll *= 180.0 / M_PI;

}

/**
* @brief counter to keep track of what instrument is on the MIDI channel 
*/
uint8_t instrument = 0;
/**
* @brief Current note being played
*/
uint8_t note = 60;
/**
* @brief base_note for transposing the C-major scale
*/
uint8_t base_note = 0;
uint16_t time = 0;
#define WAIT_VALUE (300 / t1)
#define RATE_THRES (240)

uint16_t wait = WAIT_VALUE;
char handle_notes(float);

/**
* @brief used to keep track of the roll so we can bend the pitch relative to the note it was played at.
*/
float old_roll;

/**
 * @brief This function is the function that gets called every 25ms that handles the main logic, it reads the sensor values, checks flags set by the
 * ADC interrupt vector, and sends the according MIDI signals.
 */
void task1(void)
{

  ADCSRA |= (1<<ADSC);
  float x,y,z;

  float roll,pitch;
  get_pitch_roll(&pitch,&roll);
  //Save the old roll if we play a new note so we don't bend the note too much.
  if(handle_notes(roll)) {
    old_roll = roll;
  }


  // bend the pitch if the middle finger is down.
  if (chan1 && adc_chan1_on) {
    roll -= old_roll;
    roll/=90.0;
    roll*=2000;
    pitch_bend(2,(2000+((int16_t)roll))<<2);
  } else {
    read_gyro(&x,&y,&z);
    // We don't want to change the instrument or key too often
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
/**
* @brief C-major scale in MIDI notes can add or subtract a value to transpose to different keys
*/
char cmajor[15] = {48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72};
/**
* @brief Takes the current roll and determines if a note needs to be played or turned off. It calculates the note from the roll. We are playing notes only on a major scale. It can be transposed up or down.
*
* @param roll - roll of the glove
*
* @return 1 if a new note was played, 0 otherwise
*/
char handle_notes(float roll)
{
  int8_t sharp = 0;
  if (adc_chan1_on) {
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

/**
* @brief Sends the MIDI note on signal on the serial line with the note specified in the parameter at velocity 100.
* stores the note in a global variable so we can turn it off at a later time.
*
* @param n - note to play.
*/
void play_note_on(uint8_t n) {

  note =(uint8_t)n;
  send_note(2,100,note);
}


/**
* @brief Turns the note we had stored in the note global varible off.
*/
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

  //init the task timer
  time1=t1;

  //init the UART -- uart_init() is in uart.c
  uart_init();
  stdout = stdin = stderr = &uart_str;
  ADMUX = (1<<ADLAR) | (1<<REFS0);
  ADCSRA = (1<<ADEN) | (1<<ADIF) | (1<<ADIE) + 7;
  sei();


  init_imu(G_SCALE_245DPS, A_SCALE_2G, M_SCALE_2GS, G_ODR_95_BW_125,A_ODR_50, M_ODR_50);
  // 1600 Hz (0xA)
  //crank up the ISRs
  ADCSRA |= (1<<ADSC);
}


