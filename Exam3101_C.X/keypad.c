#include "keypad.h"
#include "twi.h"

uint16_t pressed_keys = 0xFF;

//The functions scans a row of the keypad for pressed keys
uint8_t scan_row(uint8_t row){
    uint8_t masks[] = {0b1110,0b1101,0b1011,0b0111};
    PCA9555_0_write(REG_OUTPUT_1,masks[row-1]);
    uint8_t pressed = PCA9555_0_read(REG_INPUT_1);
    return (pressed >> 4) & 0x0F;
}

//The function scans the whole keypad for pressed keys from top to bottom
uint16_t scan_keypad(void){
    uint16_t result;
    result = (uint16_t) scan_row(4);
    for(int row=3;row>0;row--){
        result = (result<<4) | (uint16_t) scan_row(row);
    }
    return result;
}

uint16_t scan_keypad_rising_edge(){
    uint16_t pressed_keys_tempo = scan_keypad();
    _delay_ms(15); //Delay to combat sparking effect on push button
    pressed_keys_tempo |= scan_keypad();    //Discards any buttons that did not stay pressed
    pressed_keys = pressed_keys_tempo;
    return pressed_keys_tempo;
}

//Simple function that converts *ONLY* the first recognised pressed button to ascii
char keypad_to_ascii(){
    char chars[] = {'*','0','#','D','7','8','9','C','4','5','6','B','1','2','3','A'};
    uint16_t pressed = scan_keypad_rising_edge();
    //If no key was pressed return space ' '
    if(pressed == 0xFFFF){
        return ' ';
    }
    //Shifting through the keys till a zero (pressed) is found
    int i = 0;
    while(pressed & 0x01){
        pressed = pressed >> 1;
        i++;
    }
    return chars[i];
}

