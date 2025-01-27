.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
 

.equ DEL_NU=500 ; delay_ms routine: (1000 DEL NU+6) cycles

.equ DEL_NU2=5000 ; delay_ms routine: (1000 DEL NU+6) cycles

.equ DEL_NU3=5 ; delay_ms routine: (1000 DEL NU+6) cycles

.def flag=r22 ;Just a flag to see if the timer of the lights finished its cycle.

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1
    
 
ISR1:
    push r26
    push r25
    push r24
    in r24,SREG
    push r24
    start:
	;Routine to deal with sparks of the push button.
	ldi r24, low (DEL_NU3)
	ldi r25, high (DEL_NU3)
	rcall wait_x_msec
	in r24,EIFR
	andi r24,0x01
	brne start
    ;Main body of the routine
    in r24,PORTB
    andi r24,0x01 ;If 1east significant bit of PORTB (our light) is set then we have refresh
    breq no_refresh ;Otherwise there is no refresh 
    ser r26
    out PORTB,r26
    ldi r24, low (DEL_NU) ;0.5s delay for refresh
    ldi r25, high (DEL_NU) 
    rcall wait_x_msec
    
    no_refresh:
	ldi r26 ,0x01
	out PORTB,r26
	ldi flag,0x00
    ;End of main body
    ldi r24, (1 << INTF1)
    out EIFR, r24 ; Clear external interrupt 1 flag
    pop r24
    out SREG,r24
    pop r24
    pop r25
    pop r26
    
    ldi r24, low (DEL_NU2) ;Reset delay before exiting interrupt
    ldi r25, high (DEL_NU2) ;To assure that we get 5s delay
    
    reti
   
reset:
    ;Init Stack Pointer
    ldi r24, LOW (RAMEND)
    out SPL, r24
    ldi r24, HIGH (RAMEND)
    out SPH, r24
    ;Init PORTB as output
    ser r26
    out DDRB,r26
    ; Interrupt on rising edge of INT1 pin
    ldi r24, (1 << ISC11) | (1 << ISC10) 
    sts EICRA, r24
    ;Enable the INT1 interrupt (PD3)
    ldi r24, (1 << INT1)
    out EIMSK, r24
    sei ; Sets the Global Interrupt Flag
    ldi flag,0x01
    clr r26
    out PORTB,r26
main:
    cpi flag,0x01   ;If the flag is 1 we need to trun on the light
    breq main
    ldi r26,0x01
    out PORTB,r26
    ldi r24, low (DEL_NU2) ;5s delay for the light.
    ldi r25, high (DEL_NU2) 
    rcall wait_x_msec
    ldi flag,0x01
    clr r26
    out PORTB,r26
    rjmp main

    
    ;this routine is used to produce a delay 993 cycles
delay_inner:
    ldi r23, 247          ; 1 cycle
loop2:
    dec r23               ; 1 cycle
    nop                   ; 1 cycle
    brne loop2            ; 1 or 2 cycles
    nop                   ; 1 cycle
    ret                   ; 4 cycles

delay_outer:
    rcall delay_inner     ; (3+993)=996 cycles
    subi r22,1            ; 2 cycles
    brne delay_outer      ; 1 or 2 cycles
    ret     ; 4 cycles
      
wait_x_msec:
    rcall wait_1_ms     
    sbiw r24, 1            
    brne wait_x_msec
    ret
wait_1_ms:
   ldi r22,0x10      
   rcall delay_outer 
   ret

