#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>

#define DEL 1000U

int main(void){
    DDRD = 0xFF;
    while(1){
        PORTD = 0x00;
        _delay_ms(DEL);
        PORTD = 0xFF;
        _delay_ms(DEL);
    }
}
