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
    
    ;ldi r24,0b00011000 ;Shifting cursor one place to the right.
    ;call lcd_command
    ldi r24,'.'
    call lcd_data
    
    ;Getting value right of decimal place.
    ;ldi r24,0b00011000 ;Shifting cursor one place to the right.
    ;call lcd_command
    ldi r24,10
    mul temp,r24
    mov temp,r0
    mov temp2,r1
    rcall convert_value
    
    ;ldi r24,0b00011000 ;Shifting cursor one place to the right.
    ;call lcd_command
    ldi r24,10
    mul temp,r24
    mov temp,r0
    mov temp2,r1
    rcall convert_value

    in r24,ADIF
    ori r24, (1 << ADIF)
    out ADIF, r24 ; Clear ADC interrupt flag
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
    ser r24
    out DDRD, r24 ; set PORTD as output
    clr r24
    sei ;Enable interrupts.
    
    call lcd_init ;Routine to initialize the LCD display at start up properly.
    clr temp
    clr temp2
main:
    Start_conv:
	lds temp, ADCSRA
	ori temp, (1<<ADSC) ; Set ADSC flag of ADCSRA
	sts ADCSRA, temp
    ldi r25,high(1000)
    ldi r24,low(1000)
    rcall wait_msec ;Waiting for 1000ms = 1s
    rjmp main
    
   
convert_value:
    push r24
    ;Puts into r24 the appropriate character to display on lcd.
    rcall get_digit
    rcall dc_values
    call lcd_data
    ;Result in r24
    pop r24
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
    
get_digit:
    ldi r24,0
    cpi temp2,1
    brpl subtr
    cpi temp,100
    brcs end_hunt
    subtr:
	inc r24
	sbiw temp,50
	sbiw temp,50
	cpi temp2,1
	brpl subtr
	cln
	cpi temp,100
	brcs end_hunt
	rjmp subtr
    end_hunt:
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
    ldi r25,'0'
    cpi r24,0
    breq finished
    ldi r25,'1'
    cpi r24,1
    breq finished
    ldi r25,'2'
    cpi r24,2
    breq finished
    ldi r25,'3'
    cpi r24,3
    breq finished
    ldi r25,'4'
    cpi r24,4
    breq finished
    ldi r25,'5'
    cpi r24,5
    breq finished
    ldi r25,'6'
    cpi r24,6
    breq finished
    ldi r25,'7'
    cpi r24,7
    breq finished
    ldi r25,'8'
    cpi r24,8
    breq finished
    ldi r25,'9'
    finished:
    mov r24,r25
    ret