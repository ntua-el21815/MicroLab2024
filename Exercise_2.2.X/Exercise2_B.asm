;Exercise 2B
.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
 
.equ FOSC_MHZ=16 ;Microcontroller operating frequency in MHz
.equ DEL_ms=2000 ;Delay in ms (valid number from 1 to 4095)
.equ DEL_ms2=5 ;Delay in ms (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_ms ; delay_ms routine: (1000 DEL NU+6) cycles
.equ DEL_NU2=FOSC_MHZ*DEL_ms2

.org 0x0
rjmp reset
.org 0x2
rjmp ISR0  

find_out:
    ;Antistoixoume ton arithmo ton pathmenon koumpion ston binary 
    ;arithmo pou exei tosous assous ksekinontas apo to bit 0.
    mov r26,r25 ;If r25 == 0 or r25 == 1 then it is already translated.
    ;Checking for each case (0,1,2,3,4)
    cpi r25,2
    in r23,SREG
    sbrc r23,1 ;Skips next instruction if r23(1) == 0,which is the zero flag
    ldi r26,3
    cpi r25,3
    in r23,SREG
    sbrc r23,1
    ldi r26,7
    cpi r25,4
    in r23,SREG
    sbrc r23,1
    ldi r26,15
    ret
ISR0:
    push r26
    push r25
    push r24
    in r24,SREG
    push r24
    start:
	;To counter sparking
	ldi r24, low (DEL_NU2) ;Set delay (number of cycles)
	ldi r25, high (DEL_NU2)
	rcall delay_ms
	in r24,EIFR
	andi r24,0x01
	brne start
    ;Start of main body of routine
    clr r25
    in r20,PINB	;Reading the contents of PORTB	
    ;Determining how many buttons from PB0-PB3 are pressed at the moment
    sbrc r20,0	
    inc r25
    sbrc r20,1
    inc r25
    sbrc r20,2
    inc r25
    sbrc r20,3
    inc r25
    ldi r24,4
    sub r24,r25 ;Metrame assous opote afairoume apo to 4 wste na paroume ta mhdenika.
    mov r25,r24
    rcall find_out ;We pass r25 to find_out routine to get bits instead of the number.
    out PORTC,r26
    ;End of main body of routine
    ldi r24, (1 << INTF1)
    out EIFR, r24 ; Clear external interrupt 1 flag
    pop r24
    out SREG,r24
    pop r24
    pop r25
    pop r26
    reti
   
reset:
    ;Init Stack Pointer
    ldi r24, LOW (RAMEND)
    out SPL, r24
    ldi r24, HIGH (RAMEND)
    out SPH, r24
    ;Init PORTC as output
    ser r26
    out DDRC,r26
    ;Init PORTD as input
    clr r26
    out DDRD,r26
    ; Interrupt on rising edge of INT0 pin
    ldi r24, (0 << ISC00) | (0 << ISC01) ;Interrupt on low value
    sts EICRA, r24
    ;Enable the INT0 interrupt (PD2)
    ldi r24, (1 << INT0)
    out EIMSK, r24
    sei ; Sets the Global Interrupt Flag
loop1:
    clr r26
loop2:
    out PORTC, r26
    
    ldi r24, low (DEL_NU) ;Set delay (number of cycles)
    ldi r25, high (DEL_NU) 
    rcall delay_ms
    
    inc r26
    
    cpi r26, 32 ;compare r26 with 32
    breq loop1
    rjmp loop2
    
; delay of 1000 F1+6 cycles (almost equal to 1000*F1 cycles)
delay_ms:

;total delay of next 4 insruction group 1+ (249*4-1)-996 cycles 
    ldi r23, 249 ;(1 cycle)
loop_inn:
    dec r23 ; 1 cycle
    nop ; 1 cycle
    brne loop_inn ; 1 or 2 cycles
    
    sbiw r24,1 ; 2 cycles
    brne delay_ms ;1 or 2 cycles
    
    ret ; 4 cycles
