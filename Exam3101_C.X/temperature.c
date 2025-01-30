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
    one_wire_transmit_byte(0xCC);   //Bypass option for multiple devices (only one is present in our case)
    one_wire_transmit_byte(0x44);   //Start a temperature measurement
    while(!one_wire_receive_bit()); //When it is done a '1' bit will be sent
    one_wire_reset();               //Re-initiate device
    one_wire_transmit_byte(0xCC);   //Bypass option for multiple devices (only one is present in our case)
    one_wire_transmit_byte(0xBE);   //Send command to read the 16 bit temperature measurement
    uint16_t lsb = (uint16_t) one_wire_receive_byte();  //Get lsb of measurement
    uint16_t msb = (uint16_t) one_wire_receive_byte();  //Get msb of measurement
    //Returned Value is of the form SSSSS{temp} S->Sign temp->0.5C accurate temperature
    int16_t signed_temp = ((msb << 8) & 0xFF00) | (lsb & 0x00FF);
    return signed_temp;
}

/* 
 *  VERY IMPORTANT This routine works ONLY for DS18B20 and should be altered
 *  in order to work for the DS1820
 */
double get_temp(int16_t bin_temp){
    uint16_t celsius = bin_temp & 0x07FF;   //Unsigned value of temperature
    char sign = bin_temp & 0xF000;  //Sign bits (1 -> - , 0 -> +)
    if(sign == 0){
        //Temperature is in this case positive
        sign = 1;
    }
    else{
        //Temperature is in this case negative
        sign = -1;
    }
    double temperature = (double)celsius * 0.0625;
    return temperature;
}

void get_temp_string(double temperature,char* temp_string){
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
    temp_string[6] = '\0';
}