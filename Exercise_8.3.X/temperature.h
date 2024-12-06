#ifndef TEMP_H
#define	TEMP_H

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

uint8_t one_wire_reset();

uint8_t one_wire_receive_bit();

void one_wire_transmit_bit(uint8_t to_transmit);

uint8_t one_wire_receive_byte();

void one_wire_transmit_byte(uint8_t to_transmit);

int16_t read_temp();

double get_temp(int16_t bin_temp);

void get_temp_string(double temperature,char* temp_string);

#endif	/* XC_HEADER_TEMPLATE_H */

