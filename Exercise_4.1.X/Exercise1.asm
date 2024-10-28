.org 0x0000
rjmp reset
.org 0x02A ;ADC1 conversion completion interrupt address.
rjmp ADC1_INT

.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
.include "routines.asm"

.def temp = r26
.def temp2 = r27
.def ADC_L = r20
.def ADC_H = r21
    
ADC1_INT:
    push temp
    push temp2
    push r25
    push r24
    in r24,SREG
    push r24
    ;Interrupt service routine for when ADC conversion has been finalised.
    lds ADC_L,ADCL ; Read ADC result(Left adjusted)
    lds ADC_H,ADCH
    
    rcall lcd_clear_display ;Clear Display before output
    
    mov temp,ADC_L
    mov temp2,ADC_H
    
    ;Getting value left of decimal place
    rcall convert_value
    rcall lcd_data
    rcall div_10
    
    ldi r24,'.'
    rcall lcd_data
    
    ;Getting value right of decimal place.
    rcall convert_value
    rcall lcd_data
    rcall div_10
    
    rcall convert_value
    rcall lcd_data
    rcall div_10

    ldi r24, (1 << INTF1)
    out EIFR, r24 ; Clear external interrupt 1 flag
    pop r24
    out SREG,r24
    pop r24
    pop r25
    pop temp2
    pop temp
    reti
 
reset:
    ;Store table address in Z double register
    ldi ZH,high(dc_values*2)
    ldi ZL,low(dc_values*2)
    
    ; initialize stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
    ldi temp,(1 << REFS0) | (1 << MUX0)
    sts ADMUX,temp ;Setting ADC1 as analog input.VREF AVCC with external capacitor.
    ldi temp,(1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
    sts ADCSRA,temp ;Enabling ADC,Enabling ADC interrupts and selecting 128 prescaler for freq.
    ldi temp,DDRC
    andi temp,0b11111101 ;Setting PC1 as input for ADC1
    out DDRC,temp
    sei ;Enable interrupts.
    
    rcall lcd_init ;Routine to initialize the LCD display at start up properly.
main:
    Start_conv:
;	lds temp, ADCSRA
;	ori temp, (1<<ADSC) ; Set ADSC flag of ADCSRA
;	sts ADCSRA, temp
    ldi r25,high(1000)
    ldi r24,low(1000)
    rcall wait_msec ;Waiting for 1000ms = 1s
    rjmp main
    
   
convert_value:
    push temp
    push temp2
    rcall to_voltage
    rcall mod_10
    add ZL,temp ;Get from the table the value for the number.
    lpm r24,Z	;Load it into r24 to give it to lcd_data.
    pop temp2
    pop temp
    ret

to_voltage:
    ; We do the conversion by having a base 488 and then reducing it to 48 for bigger voltages.
    cpi temp,100
    brge too_big
    cpi temp2,0
    brge too_big
    ;;;;; Look into what you'll do here.
    mov temp,r0
    mov temp2,r1
    ret
    too_big:
	;;;; Same.
   
mod_10:
    div:
	sbiw temp,10
	cpi temp,0
	brge div
    subi temp,-10
    ret
    
div_10:
    push temp
    push r24
    ldi r24,9
    sub_10:
	sbiw temp,10
	dec r24
	brne sub_10
    pop r24
    pop temp
    ret
    
dc_values:
    .db '0','1','2','3','4','5','6','7','8','9'