#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LED_THRESHOLD_1 200
#define LED_THRESHOLD_2 400
#define LED_THRESHOLD_3 600
#define LED_THRESHOLD_4 800

//We chose PB3,PB4 as input for lowering and increasing the brightness.
unsigned char DC_VALUE=128;

int array[13]={8,28,48,68,88,108,128,148,168,188,208,228,248};
int i=6;

void display_avg(unsigned int adc_value) {
    PORTD = 0x00; //Clearing PORTD
    //LEDs will be lit according to the exercise chart.
    if (adc_value <= LED_THRESHOLD_1) {
        PORTD |= (1 << PD0);
        return;
    } else if (adc_value <= LED_THRESHOLD_2) {
        PORTD |= (1 << PD1);
        return;
    } else if (adc_value <= LED_THRESHOLD_3) {
        PORTD |= (1 << PD2);
        return;
    } else if (adc_value <= LED_THRESHOLD_4) {
        PORTD |= (1 << PD3); 
        return;
    } else {
        PORTD |= (1 << PD4);
        return;
    }
}

int main() {
    TCCR1A = (1<<WGM10) | (1<<COM1A1);//set TMRIA in fast PWM 8 bit mode with non-inverted output  prescale=8
    TCCR1B = (1<<WGM12) | (1<<CS12);

    DDRB = 0b00100111; //set PB1 as output and PB3,PB4 as inputs.
    DDRD = 0x00;
    
    unsigned int adc_sum = 0;
    unsigned int adc_avg = 0;

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC, prescaler 128
    DDRD = 0xFF;        // Set PORTD as output

    int delays = 0;
    while (1) {
        if(delays == 1){
            OCR1AL = DC_VALUE;
            if (!(PINB & (1 << PB4))) {
                if (i<12) {
                    i+= 1;
                    DC_VALUE=array[i];
                    OCR1AL = DC_VALUE;  
                }  
            }
            if (!(PINB & (1 << PB3))) {
                if (i > 0) {
                    i-= 1;
                    DC_VALUE=array[i];
                    OCR1AL = DC_VALUE;

                }
            }
            delays = 0; //We wait for 2 delays to get input from PINB due to sparking effect.
        }
        
        adc_sum = 0;

    
        for (int i = 0; i < 16; i++) {
            ADCSRA |= (1 << ADSC); //Start conversion 
            while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
            adc_sum += ADC; //Adding the conversion value to the sum.
        }

        // Dividing adc_sum by 16.For performance reasons by shifting left
        adc_avg = adc_sum >> 4;

        display_avg(adc_avg);

        _delay_ms(100);  //Delay of 100ms
        delays += 1;
    }

    return 0;
}