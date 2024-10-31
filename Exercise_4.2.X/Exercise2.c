#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

char to_display[10] = {'0','1','2','3','4','5','6','7','8','9'};

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
}

void convert(double voltage,char* output){
    //Gets voltage with accuracy of 2 decimal places and converts it to characters.
    voltage *= 100;
    int volt = (int) voltage;
    output[3] = to_display[volt % 10];
    volt /= 10;
    output[2] = to_display[volt % 10];
    output[1] = '.';
    volt /= 10;
    output[0] = to_display[volt % 10];
    return;
}

int main(){
    ADMUX = (1 << REFS0) | (1 << MUX0); //Setting ADC1 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111101; //Setting PC1 as input for ADC1
    DDRD = 0xFF;
    lcd_init();
    while(1){
        ADCSRA |= (1 << ADSC); //Start conversion 
        while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
        double ADC_value = (double) ADC;
        double voltage = (double) (ADC_value/1024)*5;
        char chars[4];
        convert(voltage,chars);
         __asm__ __volatile__("nop");
        lcd_clear_display();
        for(int i=0;i<4;i++){
            lcd_data(chars[i]);
        }
        _delay_ms(1000); //1 second delay
         __asm__ __volatile__("nop");
    }
}

