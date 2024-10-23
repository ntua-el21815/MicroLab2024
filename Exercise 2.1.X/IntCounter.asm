.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
 
.equ FOSC_MHZ=16 ;Microcontroller operating frequency in MHz
.equ DEL_ms=500 ;Delay in ms (valid number from 1 to 4095)
.equ DEL_ms2=5 ;Delay in ms (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_ms ; delay_ms routine: (1000 DEL NU+6) cycles
.equ DEL_NU2=FOSC_MHZ*DEL_ms2
.def counter=r18
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1
    
ISR1:
    push r25
    push r24
    in r24,SREG
    push r24
    start:
	;Routine to counter spark on push button.
	ldi r24, low (DEL_NU2) 
	ldi r25, high (DEL_NU2)
	rcall delay_ms
	in r24,EIFR
	andi r24,0x01
	brne start
    in r20,PIND
    andi r20,0x20
    breq return
    inc counter
    cpi counter,0x40
    brne return
    ldi counter,0x0
return:
    out PORTC,counter
    ldi r24, (1 << INTF1)
    out EIFR, r24 ; Clear external interrupt 1 flag
    pop r24
    out SREG,r24
    pop r24
    pop r25
    reti
    
reset:
    ;Init Stack Pointer
    ldi r24, LOW (RAMEND)
    out SPL, r24
    ldi r24, HIGH (RAMEND)
    out SPH, r24
    ;Init PORTB as output
    ser r26
    out DDRB, r26
    out DDRC,r26
    clr r26
    out DDRD,r26
    ; Interrupt on rising edge of INTO pin
    ldi r24, (1 << ISC11) | (1 << ISC10) 
    sts EICRA, r24
    ;Enable the INTO interrupt (PD2)
    ldi r24, (1 << INT1)
    out EIMSK, r24
    sei ; Sets the Global Interrupt Flag
    clr counter;Intially counter = 0
loop1:
    clr r26
loop2:
    out PORTB, r26
    
    ldi r24, low (DEL_NU) ;Set delay (number of cycles)
    ldi r25, high (DEL_NU) 
    rcall delay_ms
    
    inc r26
    
    cpi r26, 16 ;compare 126 with 16
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




