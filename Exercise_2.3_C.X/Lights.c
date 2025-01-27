/*
 * main.c
 *
 * Created: 16/10/2024 12:31:38 PM
 *  Author: Nikolaos Angelitsis , Nestoras Koumis
 */ 

#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

volatile int counter = 5000; //Counts time in ms to track how much time the lights remain on.
volatile int refresh = 0;

ISR(INT1_vect) // External INT1 Interrupt Service Routine
{
	if(counter < 5000 && counter > 0){
		refresh = 1; //Sets refresh to 1 when counter has not finished yet
                    //But interrupt has been triggered.
	}
	counter = 0; //Reset counter to 0 and start timer again.
	EIFR = (1 << INTF1); // Clear the flag of interrupt INTF1
}

int main(){
	// Interrupt on rising edge of INT1 pin
	EICRA=(1<<ISC11) | (0<<ISC10);
	EIMSK=(1<<INT1); // Enable the INT1 interrupt (PD3))
	sei(); // Enable global interrupts
	DDRB=0xFF; // Set PORTB as output
	PORTB=0x00; // Turn off all LEDs of PORTB
	while(1)
	{
		while(counter < 5000){
			if(refresh && counter < 500){
				PORTB = 0xFF;
                //0.5s flashing of all the leds.
			}
			else{
				PORTB = 0x01;
                //5s flashing of just the one main light.
			}
			counter ++;
			_delay_ms(1); // Delay 1 mS
		}
        if(counter >= 5000){
            //PORTB is off when no interrupt has been triggered.
            refresh = 0;
            PORTB = 0x00;
        }
	}
}
