#include<string.h>
#include<stdio.h>
#include "twi.h"
#include "lcd.h"
#include "uart.h"
#include "temperature.h"
#include "keypad.h"

#define MAKE_ROOM_TEMP 192;

enum STATUS {NURSE_CALL,CHECK_TEMP,CHECK_PRESSURE,OK};

void send_string(char* mystr){
    for(int i=0;i<strlen(mystr);i++){
        usart_transmit(mystr[i]);
    }
}

void get_ans(char* ans){
    int i=0;
    while((ans[i] = usart_receive())!='\n'){
        i++;
    }
    ans[i] = '\0';
}

double get_pressure(){
    ADCSRA |= (1 << ADSC); //Start conversion 
    while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
    double ADC_value = (double) ADC;
    return (ADC_value * 20)/1024;
}


int get_status(){
    char read1;
    while((read1 = keypad_to_ascii()) == ' ');
    char read2;
    while((read2 = keypad_to_ascii()) == read1){
        if(read2 == ' '){
            break;
        }
    }
    while(read2 == ' '){
        read2 = keypad_to_ascii();
    }
    while(read2 == keypad_to_ascii());
    return 0;
}

int main() {
    char connect[] = "ESP:connect\n";
    char url[] = "ESP:url: \"http://192.168.1.250:5000/data\"\n";
    char Fail1[] = "1.Fail";
    char Succ1[] = "1.Success";
    char error_msg[] = "No Device";
    ADMUX = (1 << REFS0) | (0<<MUX0) | (0<<MUX1) | (0<<MUX2) ; //Setting ADC1 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111110; //Setting PC1 as input for ADC1
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output Configuration port 0 register
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1(4-7) as input and EXT_PORT (0-3) as output
    lcd_init();
    usart_init(103);
    while(1){
        send_string(connect);
        char answer[8];
        get_ans(answer);
        lcd_clear_display();
        if(strcmp(answer,"Fail") == 0){
            print_lcd(Fail1);
        }
        else{
            print_lcd(Succ1);
        }
        send_string(url);
        get_ans(answer);
        lcd_clear_display();
        if(strcmp(answer,"Fail") == 0){
            print_lcd(Fail1);
        }
        else{
            print_lcd(Succ1);
        }
        int16_t temp = read_temp();
        if(temp == 0x8000){
            send_string(error_msg);
            _delay_ms(1000);
            continue;
        }
        int temp_int = get_temp_int(temp) + MAKE_ROOM_TEMP;
        char temp_string[8];
        get_temp_string(temp_int,temp_string);
        send_string(temp_string);
        get_ans(answer);
        double pressure = get_pressure();
        char press[9];
        snprintf(press, 9,"%.2f\n",pressure);
        send_string(press);
        _delay_ms(1000);
    }
}
