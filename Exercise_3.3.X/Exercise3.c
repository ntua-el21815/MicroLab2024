#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define MAX_DC 248    // 98% Duty Cycle
#define MIN_DC 8 

unsigned char DC_VALUE=148;

int array[13]={8,28,48,68,88,108,128,148,168,188,208,228,248};
int i=7;

int main (void){

    int mode=1;

    TCCR1A = (1<<WGM10) | (1<<COM1A1);//set TMRIA in fast PWM 8 bit mode with non-inverted output//prescale=8
    TCCR1B = (1<<WGM12) | (1<<CS11);

    DDRB |= 0b00111111; //set PB5-PB0 pins as output
    DDRD &= ~((1 << PD1) | (1 << PD2) | (1 << PD6) | (1 << PD7)); //input

    ADMUX = (1 << REFS0);                    // Vref = AVCC, ?????? ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    OCR1AL = DC_VALUE;

    while (1)
    {
       if (!(PIND & (1 << PD6))) {      
            mode = 1;
            _delay_ms(200);
        }
        if (!(PIND & (1 << PD7))) {
            mode = 2;
            _delay_ms(200);
        }

       if (mode == 1) {
               DC_VALUE=array[i];
               OCR1AL = DC_VALUE;
               mode1();          
            } 
       else if (mode == 2) {
                mode2();  
            }

    }
}

void mode1(void){
     
    if (!(PIND & (1 << PD1))) {
       
        if (i<12) {
             i+= 1;
             DC_VALUE=array[i];
            OCR1AL = DC_VALUE;
        }
        _delay_ms(200);   
                               
    }
    if (!(PIND & (1 << PD2))) {
        if (i > 0) {
            i-= 1;
            DC_VALUE=array[i];
            OCR1AL = DC_VALUE;
            
        }
        _delay_ms(200);
    }

}

void mode2(void){
    ADCSRA |= (1 << ADSC);                   // Start conversion
   
    DC_VALUE = ADC /4;
    OCR1AL = DC_VALUE;
}
