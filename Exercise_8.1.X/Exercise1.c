#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<string.h>
#include<stdio.h>
#include<avr/interrupt.h>
#include<math.h>

#define MAKE_ROOM_TEMP 192;

#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2

enum STATUS {NURSE_CALL,CHECK_TEMP,CHECK_PRESSURE,OK};

uint16_t pressed_keys = 0xFF;

// PCA9555 REGISTERS
typedef enum {
    REG_INPUT_0 = 0,
    REG_INPUT_1 = 1,
    REG_OUTPUT_0 = 2,
    REG_OUTPUT_1 = 3,
    REG_POLARITY_INV_0 = 4,
    REG_POLARITY_INV_1 = 5,
    REG_CONFIGURATION_0 = 6,
    REG_CONFIGURATION_1 = 7
} PCA9555_REGISTERS;

//----------- Master Transmitter/Receiver -------------------
#define TW_START 0x08
#define TW_REP_START 0x10

//---------------- Master Transmitter ----------------------
#define TW_MT_SLA_ACK 0x18
#define TW_MT_SLA_NACK 0x20
#define TW_MT_DATA_ACK 0x28

//---------------- Master Receiver ----------------
#define TW_MR_SLA_ACK 0x40
#define TW_MR_SLA_NACK 0x48
#define TW_MR_DATA_NACK 0x58
#define TW_STATUS_MASK 0b11111000
#define TW_STATUS (TWSR0 & TW_STATUS_MASK)

uint8_t msg[] = "NESTORAS KOUMIS";
uint8_t msg2[] = "NIKOLAOS ANGELITSIS";

void twi_init(void)
{
    //initialize TWI clock
    TWSR0 = 0; // PRESCALER_VALUE=1
    TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}

// Read one byte from the twi device (request more data from device)
unsigned char twi_readAck(void)
{
    TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    while(!(TWCR0 & (1<<TWINT)));
    return TWDR0;
}

//Read one byte from the twi device, read is followed by a stop condition
unsigned char twi_readNak(void)
{
    TWCR0 = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR0 & (1<<TWINT)));
    return TWDR0;
}

unsigned char twi_start(unsigned char address)
{
    // Issues a start condition and sends address and transfer direction.
    // return 0 = device accessible, 1= failed to access device
    uint8_t twi_status;
    TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // send START condition
    while(!(TWCR0 & (1<<TWINT)));   // wait until transmission completed
    twi_status = TW_STATUS & 0xF8;
    if ((twi_status != TW_START) && (twi_status != TW_REP_START))
    {
        // check value of TWI Status Register.
        return 1;
    }
    TWDR0 = address;    // send device address
    TWCR0 = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR0 & (1<<TWINT))); // wail until transmission completed and ACK/NACK has been received
    twi_status = TW_STATUS & 0xF8;
    if ( (twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK) )
    {
        // check value of TWI Status Register.
        return 1;
    }
    return 0;
}

void twi_start_wait(unsigned char address)
{
    // Send start condition, address, transfer direction.
    // Use ack polling to wait until device is ready
    uint8_t twi_status;
    while (1)
    {
        TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  // send START condition
        while(!(TWCR0 & (1<<TWINT))); // wait until transmission completed
        twi_status = TW_STATUS & 0xF8;
        if ( (twi_status != TW_START) && (twi_status != TW_REP_START))
        {
            // check value of TWI Status Register.
            continue;
        }
        TWDR0 = address;    // send device address
        TWCR0 = (1<<TWINT) | (1<<TWEN);
        while(!(TWCR0 & (1<<TWINT)));   // wail until transmission completed
        twi_status = TW_STATUS & 0xF8;
        if ( (twi_status == TW_MT_SLA_NACK )||(twi_status ==TW_MR_DATA_NACK) )
        {
            // check value of TWI Status Register.
            /* device busy, send stop condition to terminate write operation */
            TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
            // wait until stop condition is executed and bus released
            while(TWCR0 & (1<<TWSTO));
            continue;
        }
        break;
    }
}

unsigned char twi_write(unsigned char data)
{
    // Send one byte to twi device, Return 0 if write successful or 1 if write failed
    // send data to the previously addressed device
    TWDR0 = data;
    TWCR0 = (1<<TWINT) | (1<<TWEN);
    // wait until transmission completed
    while(!(TWCR0 & (1<<TWINT)));
    if((TW_STATUS & 0xF8) != TW_MT_DATA_ACK) return 1;
    return 0;
}

unsigned char twi_rep_start(unsigned char address)
{
    // Send repeated start condition, address, transfer direction
    //Return: 0 device accessible
    // 1 failed to access device
    return twi_start( address );
}

void twi_stop(void)
{
    // Terminates the data transfer and releases the twi bus
    // send stop condition
    TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    // wait until stop condition is executed and bus released
    while(TWCR0 & (1<<TWSTO));
}

void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
    twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
    twi_write(reg);
    twi_write(value);
    twi_stop();
}

uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
    uint8_t ret_val;
    twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
    twi_write(reg);
    twi_rep_start(PCA9555_0_ADDRESS + TWI_READ);
    ret_val = twi_readNak();
    twi_stop();
    return ret_val;
}


void write_nibbles(uint8_t data) {
    uint8_t high_nibble = data & 0xF0; // Get the upper 4 bits
    uint8_t low_nibble = ((data & 0x0F) << 4) & 0xF0; // Get the lower 4 bits
    
    // Write high nibble
    uint8_t reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, high_nibble + (reg & 0x0F)); // Set high nibble while keeping lower bits
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg | 0x08);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,reg & 0xF7);

    // Write low nibble
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,(reg & 0x0F) + low_nibble); // Set low nibble while keeping lower bits
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg | 0x08);
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg & 0xF7);
}

void lcd_data(unsigned char data) {
    uint8_t reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg | 0x04);
    write_nibbles(data); 
    _delay_us(250);           
}

void lcd_command(unsigned char cmd) {
    uint8_t reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg & 0xFB);    
    write_nibbles(cmd);      
    _delay_us(250);         
}

void lcd_clear_display(void) {
    lcd_command(0x01);
    _delay_ms(5);      
}

void lcd_init(void) {
    _delay_ms(200);  
    uint8_t reg;
    for(int i=0;i<3;i++){
        PCA9555_0_write(REG_OUTPUT_0, 0x30);
        reg = PCA9555_0_read(REG_OUTPUT_0);
        PCA9555_0_write(REG_OUTPUT_0, reg | 0x08);
         __asm__ __volatile__("nop");  
         __asm__ __volatile__("nop");
        reg = PCA9555_0_read(REG_OUTPUT_0);
        PCA9555_0_write(REG_OUTPUT_0, reg & 0XF7); 
        _delay_us(250);
    }
    PCA9555_0_write(REG_OUTPUT_0, 0x20);
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg | 0x08);
     __asm__ __volatile__("nop");
     __asm__ __volatile__("nop");
    reg = PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, reg & 0XF7);  
    _delay_us(250);
    lcd_command(0x28);    
    lcd_command(0x0C);
    lcd_clear_display();  
    lcd_command(0x06);
    return;
}

/* Routine: usart_init
Description:
This routine initializes the
usart as shown below.
------- INITIALIZATIONS -------
Baud rate: 9600 (Fck= 8MH)
Asynchronous mode
Transmitter on
Reciever on
Communication parameters: 8 Data ,1 Stop, no Parity
--------------------------------
parameters: ubrr to control the BAUD.
return value: None.*/
void usart_init(unsigned int ubrr){
    UCSR0A=0;
    UCSR0B=(1<<RXEN0)|(1<<TXEN0);
    UBRR0H=(unsigned char)(ubrr>>8);
    UBRR0L=(unsigned char)ubrr;
    UCSR0C=(3 << UCSZ00);
    return;
}
/* Routine: usart_transmit
Description:
This routine sends a byte of data
5
using usart.
parameters:
data: the byte to be transmitted
return value: None. */
void usart_transmit(uint8_t data){
    while(!(UCSR0A&(1<<UDRE0)));
    UDR0=data;
}
/* Routine: usart_receive
Description:
This routine receives a byte of data
from usart.
parameters: None.
return value: the received byte */
uint8_t usart_receive(){
    while(!(UCSR0A&(1<<RXC0)));
    return UDR0;
}

void print_lcd(char* mystr){
    for(int i=0;i<strlen(mystr);i++){
        lcd_data(mystr[i]);
    }
    return;
}

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

double get_pressure(){
    ADCSRA |= (1 << ADSC); //Start conversion 
    while (ADCSRA & (1 << ADSC)); //Waiting for conversion to end.
    double ADC_value = (double) ADC;
    return (ADC_value * 20)/1024;
}

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
