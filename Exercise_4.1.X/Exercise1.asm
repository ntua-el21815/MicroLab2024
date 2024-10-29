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
    ;Remember to make cursor shifts!!!!!!!!!!!!!
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
    rcall to_voltage
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
    ldi temp,100
    clr temp2
main:
    Start_conv:
;	lds temp, ADCSRA
;	ori temp, (1<<ADSC) ; Set ADSC flag of ADCSRA
;	sts ADCSRA, temp
;    ldi r25,high(1000)
;    ldi r24,low(1000)
;    rcall wait_msec ;Waiting for 1000ms = 1s
    ; Below just for debugging !!!!!!!!
    rcall to_voltage
    conv:
    rcall convert_value
    rcall div_10
    rjmp conv
    
   
convert_value:
    push temp2
    push temp
    ;Puts into r24 the appropriate character to display on lcd.
    rcall mod_10
    ldi ZL,low(dc_values*2)
    add ZL,temp ;Get from the table the value for the number.
    lpm r24,Z	;Load it into r24 to give it to lcd_data.
    pop temp
    pop temp2
    ret

to_voltage:
    push r0
    push r1
    push r22
    ;Multiply ADC value with 25
    mov r0,temp
    mov r1,temp2
    ldi r22,25
    rcall mu16
    ;Divide by 16
    lsr r1
    ror r0
    lsr r1
    ror r0
    lsr r1
    ror r0
    lsr r1
    ror r0
    ;Multiply by 5
    ldi r22,5
    rcall mu16
    ;Divide by 16
    lsr r1
    ror r0
    lsr r1
    ror r0
    lsr r1
    ror r0
    lsr r1
    ror r0
    ;Move result to expected registers.
    mov temp,r0
    mov temp2,r1
    pop r22
    pop r1
    pop r0
    ret
    
mod_10:
    cpi temp,10
    brlt done
    div:
	sbiw temp,10
	cpi temp,10
	brge div
    done:
    ret
    
div_10:
    push r25
    push r24
    mov r24,temp
    mov r25,temp2
    clr temp
    clr temp2
    divd:
	sbiw r24,10
	adiw temp,1
	cpi r24,0
	brge divd
    sbiw temp,1
    pop r24
    pop r25
    ret

mu16:
    push r22
    push r23
    push r24
    push r25
    ; r0 low byte of the 16-bit number
    ; r1 high byte of the 16-bit number
    ; r22 8-bit multiplier
    clr r23                 
    clr r24                
    
    mov r25,r1		    ; Backup r1
    
    mul r0, r22             ; r0 * r22 -> r0:r1 (r0 = low, r1 = high)
    mov r24, r0             ; Store low byte result in r24
    mov r23, r1             ; Store high byte result in r23

    mul r25, r22            ; r25 * r22 -> r0:r1 (r0 = low, r1 = high)

    add r23, r0             ; Add low result to r23
    brcc no_carry
    inc r23                 ; Increment high byte if carry

no_carry:
    add r23, r1             ; Add high result to r23
    
    mov r0,r24
    mov r1,r23
    
    pop r25
    pop r24
    pop r23
    pop r22
    ret

dc_values:
    .db '0','1','2','3','4','5','6','7','8','9'