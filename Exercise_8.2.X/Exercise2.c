#include <string.h>
#include <stdio.h>
#include "twi.h"
#include "lcd.h"
#include "uart.h"
#include "temperature.h"
#include "keypad.h"

#define MAKE_ROOM_TEMP 12
#define CALL_BUTTON '3'
#define CHECK_BUTTON '#'
#define MIN_PRESSURE 4
#define MAX_PRESSURE 12
#define MIN_TEMPERATURE 34
#define MAX_TEMPERATURE 37

typedef enum {
    IDLE = 0,NURSE_CALL,CHECK_TEMP,CHECK_PRESSURE,OK
} status;

const char text_status[5][25] = {"Idle","NURSE CALL","CHECK TEMPERATURE","CHECK PRESSURE","OK"};

void send_string(const char* mystr){
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


status get_status(double temperature,double pressure,status previous){
    char read;
    if((read = keypad_to_ascii()) == ' '){
        return IDLE;
    }
    if(read == CALL_BUTTON){
        return NURSE_CALL;
    }
    if(read == CHECK_BUTTON){
        if(pressure < MIN_PRESSURE || pressure > MAX_PRESSURE){
            return CHECK_PRESSURE;
        }
        if(temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE){
            return CHECK_TEMP;
        }
        return OK;
    }
    return IDLE;
}

int main() {
    const char connect[] = "ESP:connect\n";
    const char url[] = "ESP:url: \"http://192.168.1.250:5000/data\"\n";
    char answer[8];
    ADMUX = (1 << REFS0) | (0<<MUX0) | (0<<MUX1) | (0<<MUX2) ; //Setting ADC0 as analog input.VREF AVCC with external capacitor.
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enabling ADC and selecting 128 prescaler for freq.
    DDRC = 0b11111110; //Setting PC1 as input for ADC1
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output Configuration port 0 register
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1(4-7) as input and EXT_PORT (0-3) as output
    lcd_init();
    usart_init(103);
    status current_status = IDLE;
    // Sending connection request#
    send_string(connect);
    get_ans(answer);
    lcd_clear_display();
    while(strcmp(answer,"Fail") == 0){
        print_lcd("1.Fail");
        _delay_ms(500);
        send_string(connect);
        get_ans(answer);
        lcd_clear_display();
    }
    print_lcd("1.Success");
    // Sending the url of the receiving server
    send_string(url);
    get_ans(answer);
    lcd_clear_display();
    while(strcmp(answer,"Fail") == 0){
        print_lcd("2.Fail");
        send_string(url);
        get_ans(answer);
        lcd_clear_display();
    }
    print_lcd("2.Success");
    unsigned int delay_cyc = 10;
    while(1){
        int16_t int_temp = read_temp();
        if(int_temp == 0x8000){
            char no_dev[] = "No Device";
            lcd_clear_display();
            print_lcd(no_dev);
            _delay_ms(1000);
            continue;
        }
        
        double temp = get_temp(int_temp) + MAKE_ROOM_TEMP;
        double pressure = get_pressure();
        current_status = get_status(temp,pressure,current_status);
        char to_display[30];
        snprintf(to_display,20,"%.2f C %.2f cm H20",temp,pressure);
        if(current_status != IDLE){
            lcd_clear_display();
            print_lcd(to_display);
            lcd_command(0xC0);
            print_lcd(text_status[current_status]);
        }
        if(delay_cyc == 0){
            lcd_command(0x18);
            delay_cyc = 10;
        }
        delay_cyc --;           
    }
}
