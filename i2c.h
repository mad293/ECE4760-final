#ifndef _I2C_H_
#define _I2C_H_

#include <avr/io.h>

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_status(void);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_write(uint8_t data);

#endif
