#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


void convert(double voltage,int* output){
    //Gets voltage with accuracy of 2 decimal places and converts it to characters.
    voltage *= 100;
    voltage = (int) voltage;
    output[3] = to_display[voltage % 10];
    output[2] = '.';
    voltage /= 10;
    output[1] = to_display[voltage % 10];
    voltage /= 10;
    output[0] = to_display[voltage % 10];
    return;
}

void out_disp(int* chars){
    // Code to display the charachters on the lcd display.
    return;
}

int to_display[10] = {'0','1','2','3','4','5','6','7','8','9'};

int main(){
    ADMUX = (1 << REFS0) | (1 << MUX0); //Setting ADC1 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111101; //Setting PC1 as input for ADC1
    while(1){
        ADCSRA |= (1 << ADSC); //Start conversion 
        while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
        int ADC_value = ADC;
        double voltage = (ADC_value/1024)*5;
        out_disp(voltage);
        _delay_ms(1000); //1 second delay
    }
}
