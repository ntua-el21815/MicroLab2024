;Exercise 2A
.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
 
.equ FOSC_MHZ=16 ;Microcontroller operating frequency in MHz
.equ DEL_ms=2000 ;Delay in ms (valid number from 1 to 4095)
.equ DEL_NU=FOSC_MHZ*DEL_ms ; delay_ms routine: (1000 DEL NU+6) cycles
    
    
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
    ; Interrupt on rising edge of INTO pin
    ldi r24, (1 << ISC11) | (1 << ISC10) 
    sts EICRA, r24
    ;Enable the INTO interrupt (PD2)
    ldi r24, (1 << INT1)
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


