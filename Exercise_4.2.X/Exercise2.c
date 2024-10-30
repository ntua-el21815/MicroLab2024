#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

char to_display[10] = {'0','1','2','3','4','5','6','7','8','9'};


void convert(double voltage,char* output){
    //Gets voltage with accuracy of 2 decimal places and converts it to characters.
    voltage *= 100;
    int volt = (int) voltage;
    output[3] = to_display[volt % 10];
    output[2] = '.';
    volt /= 10;
    output[1] = to_display[volt % 10];
    volt /= 10;
    output[0] = to_display[volt % 10];
    return;
}

void out_disp(int* chars){
    //Code to display the charachters on the lcd display. 
    return;
}

int main(){
    ADMUX = (1 << REFS0) | (1 << MUX0); //Setting ADC1 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111101; //Setting PC1 as input for ADC1
    while(1){
        ADCSRA |= (1 << ADSC); //Start conversion 
        while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
        int ADC_value = 100;
        double voltage = (double) (ADC_value/1024)*5;
        char chars[4];
        convert(voltage,chars);
        _delay_ms(10); //1 second delay
    }
}