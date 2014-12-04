#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void init_adc(void) {

  ADMUX = (1<<ADLAR) | (1<<REFS0);
  ADCSRA = (1<<ADEN) | (1<<ADIF) | (1<<ADIE) + 7;
  ADCSRA |= (1<<ADSC);

}

void start_conversion(void) {
  ADCSRA |= (1<<ADSC);
}
