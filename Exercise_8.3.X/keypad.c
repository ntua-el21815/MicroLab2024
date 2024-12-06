#include "keypad.h"
#include "twi.h"

uint16_t pressed_keys = 0xFF;

uint8_t scan_row(uint8_t row){
    uint8_t masks[] = {0b1110,0b1101,0b1011,0b0111};
    PCA9555_0_write(REG_OUTPUT_1,masks[row-1]);
    uint8_t pressed = PCA9555_0_read(REG_INPUT_1);
    return (pressed >> 4) & 0x0F;
}

uint16_t scan_keypad(void){
    uint16_t result;
    result = (uint16_t) scan_row(4);
    //_delay_ms(10);
    for(int row=3;row>0;row--){
        result = (result<<4) | (uint16_t) scan_row(row);
        //_delay_ms(10);
    }
    return result;
}

uint16_t scan_keypad_rising_edge(){
    uint16_t pressed_keys_tempo = scan_keypad();
    _delay_ms(15); //Delays to combat sparking
    pressed_keys_tempo |= scan_keypad();
    //pressed_keys_tempo = ~pressed_keys | pressed_keys_tempo;
    pressed_keys = pressed_keys_tempo;
    return pressed_keys_tempo;
}

char keypad_to_ascii(){
    char chars[] = {'*','0','#','D','7','8','9','C','4','5','6','B','1','2','3','A'};
    uint16_t pressed = scan_keypad_rising_edge();
    if(pressed == 0xFFFF){
        return ' ';
    }
    int i = 0;
    while(pressed & 0x01){
        pressed = pressed >> 1;
        i++;
    }
    return chars[i];
}

