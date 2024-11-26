#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2

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
uint8_t scan_row(uint8_t row){
    uint8_t masks[] = {0b1110,0b1101,0b1011,0b0111};
    PCA9555_0_write(REG_OUTPUT_1,masks[row-1]);
    uint8_t pressed = PCA9555_0_read(REG_INPUT_1);
    return (pressed >> 4) & 0x0F;
}

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
    _delay_ms(15); //Delays to combat sparking
    pressed_keys_tempo |= scan_keypad();
    //pressed_keys_tempo = ~pressed_keys | pressed_keys_tempo;
    pressed_keys = pressed_keys_tempo;
    return pressed_keys_tempo;
}
char keypad_to_ascii(){
    char chars[] = {'*','0','#','D','7','8','9','C','4','5','6','B','1','2','3','A'};
    uint16_t pressed = scan_keypad_rising_edge();
    int i = 0;
    while(pressed & 0x01){
        pressed = pressed >> 1;
        i++;
    }
    return chars[i];
}

int main(void) {
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1(4-7) as input and EXT_PORT (0-3) as output
    DDRB = 0xFF; //Set PB0-PB3 as output
    PORTB = 0b00000000;
    while(1)
    {
        char read = keypad_to_ascii();
        switch (read) {
            case 'A':
                PORTB = 0b00000001;
                break;
            case '8':
                PORTB = 0b00000010;
                break;
            case '6':
                PORTB = 0b00000100;
                break;
            case '*':
                PORTB = 0b00001000;
                break;
            default:
                PORTB = 0x00;
                break;
        }
        
    }
}