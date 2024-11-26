#include<string.h>
#include<stdio.h>
#include "twi.h"
#include "lcd.h"
#include "uart.h"
#include "temperature.h"
#include "keypad.h"

#define MAKE_ROOM_TEMP 192;

enum STATUS {NURSE_CALL,CHECK_TEMP,CHECK_PRESSURE,OK};

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
    char Fail2[] = "2.Fail";
    char Succ2[] = "2.Success";
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output Configuration port 0 register
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
            print_lcd(Fail2);
        }
        else{
            print_lcd(Succ2);
        }
        _delay_ms(1000);
    }
}
