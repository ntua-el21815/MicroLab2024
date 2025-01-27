#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char to_display[10] = {'0','1','2','3','4','5','6','7','8','9'};
char msg[] = "GAS DETECTED!";
char msg2[] = "CLEAR";
volatile char blink = 0;
volatile int level;

void write_nibbles(unsigned char data) {
    unsigned char high_nibble = data & 0xF0; // Get the upper 4 bits
    unsigned char low_nibble = ((data & 0x0F) << 4) & 0xF0; // Get the lower 4 bits
    
    // Write high nibble
    PORTD = high_nibble + (PIND & 0x0F); // Set high nibble while keeping lower bits
    PORTD |= (1 << PD3);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    PORTD &= ~(1 << PD3);

    // Write low nibble
    PORTD = (PIND & 0x0F) + low_nibble; // Set low nibble while keeping lower bits
    PORTD |= (1 << PD3);
   __asm__ __volatile__("nop");
   __asm__ __volatile__("nop");
    PORTD &= ~(1 << PD3);
}

void lcd_data(unsigned char data) {
    PORTD |= (1 << PD2);
    write_nibbles(data); 
    _delay_us(250);           
}

void lcd_command(unsigned char cmd) {
    PORTD &= ~(1 << PD2);
    write_nibbles(cmd);      
    _delay_us(250);         
}

void lcd_clear_display(void) {
    lcd_command(0x01);
    _delay_ms(5);      
}

void lcd_init(void) {
    _delay_ms(200);  
    
    for(int i=0;i<3;i++){
        PORTD = 0x30;
        PORTD |= (1 << PD3);
         __asm__ __volatile__("nop");  
         __asm__ __volatile__("nop");
        PORTD &= ~(1 << PD3); 
        _delay_us(250);
    }
    PORTD = 0x20;
    PORTD |= (1 << PD3); 
     __asm__ __volatile__("nop");
     __asm__ __volatile__("nop");
    PORTD &= ~(1 << PD3); 
    _delay_us(250);
    lcd_command(0x28);    
    lcd_command(0x0C);
    lcd_clear_display();  
    lcd_command(0x06);
    return;
}

int main(){
    lcd_init();
    ADMUX = (1 << REFS0) | (1 << MUX1); //Setting ADC2 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111011; //Setting PC2 as input for ADC2
    DDRD = 0xFF;    //PORTD as output
    DDRB = 0xFF;    //PORTB as output
    sei();          //Enable interrupts
    
    while(1){
        ADCSRA |= (1 << ADSC); //Start ADC conversion 
        //When the conversion stops an interrupt will be triggered
   
        lcd_clear_display();
        if(level > 3){
            for(int i=0;i<13;i++){
                lcd_data(msg[i]);
            }
            if(blink == 0){
                PORTB = level;
                blink = 1;
            }
            else{
                PORTB = 0x00;
                blink = 0;
            }
        }
        else{
            for(int i=0;i<5;i++){
                lcd_data(msg2[i]);
            }
            PORTB = level;
        }
        _delay_ms(100); //100 milliseconds delay
    }
}

ISR(ADC_vect) // ADC Interrupt Service Routine
{
    //The values correspond to ppms 35,70,105,...
    int ADC_value = ADC;
    if(ADC_value < 171){
        level = 0b00001;
    }
    else if(ADC_value < 342){
        level = 0b00011;
    }
    else if(ADC_value < 513){
        level = 0b00111;
    }
    else if(ADC_value < 684){
        level = 0b01111;
    }
    else if(ADC_value < 855){
        level = 0b11111;
    }
    else{
        level = 0b111111;
    }
}
