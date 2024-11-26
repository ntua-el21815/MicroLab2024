#ifndef UART_H
#define	UART_H

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>


/* Routine: usart_init
Description:
This routine initializes the
usart as shown below.
------- INITIALIZATIONS -------
Baud rate: 9600 (Fck= 8MH)
Asynchronous mode
Transmitter on
Reciever on
Communication parameters: 8 Data ,1 Stop, no Parity
--------------------------------
parameters: ubrr to control the BAUD.
return value: None.*/
void usart_init(unsigned int ubrr);
/* Routine: usart_transmit
Description:
This routine sends a byte of data
5
using usart.
parameters:
data: the byte to be transmitted
return value: None. */
void usart_transmit(uint8_t data);
/* Routine: usart_receive
Description:
This routine receives a byte of data
from usart.
parameters: None.
return value: the received byte */
uint8_t usart_receive();

void send_string(char* mystr);

#endif	/* XC_HEADER_TEMPLATE_H */

