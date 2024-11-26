#include "temperature.h"

uint8_t one_wire_reset(){
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4);
    _delay_us(480);
    DDRD &= ~(1<<PD4);
    PORTD &= ~(1<<PD4);
    _delay_us(100);
    uint8_t device_connected = !(PIND & (1 << PD4)); 
    _delay_us(380);
    return device_connected;
}

uint8_t one_wire_receive_bit(){
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4); 
    _delay_us(2);
    DDRD &= ~(1<<PD4);
    PORTD &= ~(1<<PD4);
    _delay_us(10);
    uint8_t received = (PIND & (1<<PD4)) ? 1 : 0;
    _delay_us(48);
    return received;
}

void one_wire_transmit_bit(uint8_t to_transmit){
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4); 
    _delay_us(2);
    if(to_transmit){
        PORTD |= (1<<PD4);
    }
    _delay_us(58);
    DDRD &= ~(1<<PD4);
    PORTD &= ~(1<<PD4);
    _delay_us(1);
    return;
}

uint8_t one_wire_receive_byte(){
    uint8_t rec_byte = 0x00;
    for(int i=0;i<8;i++){
        rec_byte = rec_byte >> 1;
        if(one_wire_receive_bit()){
            rec_byte |= 0x80;
          }
    }
    return rec_byte;
}

void one_wire_transmit_byte(uint8_t to_transmit){
    for(int i=0;i<8;i++){
        one_wire_transmit_bit(to_transmit & 0x01);
        to_transmit = to_transmit >> 1;
    }
    return;
}

int16_t read_temp(){
    if(!one_wire_reset()){
        return 0x8000;
    }
    one_wire_transmit_byte(0xCC);
    one_wire_transmit_byte(0x44);
    while(!one_wire_receive_bit());
    one_wire_reset();
    one_wire_transmit_byte(0xCC);
    one_wire_transmit_byte(0xBE);
    uint16_t lsb = (uint16_t) one_wire_receive_byte();
    uint16_t msb = (uint16_t) one_wire_receive_byte();
    int16_t signed_temp = ((msb << 8) & 0xFF00) | (lsb & 0x00FF);
    return signed_temp;
}

int get_temp_int(int16_t bin_temp){
    uint16_t celsius = bin_temp & 0x07FF;
    char sign = bin_temp & 0xF000;
    if(sign == 0){
        sign = 1;
    }
    else{
        sign = -1;
    }
    return (int) celsius;
}

void get_temp_string(int int_temp,char* temp_string){
    double temperature = (double)int_temp * 0.0625;
    temperature *= 100;
    int temp = (int) temperature;
    temp_string[3] = '.';
    for(int i=5;i>=0;i--){
        if(i==3){
            continue;
        }
        temp_string[i] = temp % 10 + 0x30;
        temp /= 10;
    }
    temp_string[6] = '\n';
    temp_string[7] = '\0';
}