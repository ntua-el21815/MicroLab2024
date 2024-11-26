#include "lcd.h"
#include "twi.h"

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

void print_lcd(char* mystr){
    for(int i=0;i<strlen(mystr);i++){
        lcd_data(mystr[i]);
    }
    return;
}