#include<string.h>
#include<stdio.h>
#include "twi.h"
#include "lcd.h"
#include "uart.h"
#include "temperature.h"
#include "keypad.h"

#define MAKE_ROOM_TEMP 12;

void get_ans(char* ans){
    int i=0;
    while((ans[i] = usart_receive())!='\n'){
        i++;
    }
    ans[i] = '\0';
}

void send_string(const char* mystr){
    for(int i=0;i<strlen(mystr);i++){
        usart_transmit(mystr[i]);
    }
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
