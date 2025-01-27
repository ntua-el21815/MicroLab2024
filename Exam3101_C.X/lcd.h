#ifndef LCD_H
#define	LCD_H

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<string.h>

void write_nibbles(uint8_t data);

void lcd_data(unsigned char data);

void lcd_command(unsigned char cmd);

void lcd_clear_display(void);

void lcd_init(void);

void print_lcd(const char* mystr);

#endif

