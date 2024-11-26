#ifndef KEYPAD_H
#define KEYPAD_H

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

uint8_t scan_row(uint8_t row);

uint16_t scan_keypad(void);

uint16_t scan_keypad_rising_edge();

char keypad_to_ascii();


#endif	/* XC_HEADER_TEMPLATE_H */

