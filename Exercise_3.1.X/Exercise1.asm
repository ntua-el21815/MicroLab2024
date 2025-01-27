.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
    
.def index = r30
.def DC_VALUE = r20
.def temp = r16    
    
.org 0x00
jmp reset
;Timer1 Interrupt Vector (Do nothing!)
.org 0x1A
reti

    
reset:
    ;Initialisation of Z register to read values from table
    ldi ZH,high(dc_values*2)
    ldi ZL,low(dc_values*2)
    ;Stack Initialisation
    ldi temp, high(RAMEND)
    out SPH,temp
    ldi temp, low(RAMEND)
    out SPL,temp
    ldi temp,0xFF
    out DDRB, temp ;Set PORTB as output
    ldi temp,0x00
    out PORTB,temp 
    ldi temp, 0x00
    out DDRD, temp ;SeT PORTD as input
    ;Setting Fast PWM 8-bit (WGM) output with frequency of CLK/256 (CS) connected to PB1 (COM)
    ldi r24,(1<<WGM12) | (0<<WGM13) | (1<<CS12) |(0<<CS11) | (0<<CS10) ; CLK/256 (62500 Hz)
    sts TCCR1B, r24
    ldi r24,(1<<WGM10) | (WGM11<<0) | (0<<COM1A0) | (1<<COM1A1) | (0<<COM1B0) | (0<<COM1B1)
    sts TCCR1A, r24
    ;OCR1A determines the Duty_Cycle.At first it is set to 0.
    clr temp
    sts OCR1AH,temp
    ;Initially the Duty Cycle should be 50% (so value 6 of the table)
    ldi temp,6
    add ZL,temp
    no_carry:
    ; Load the specific entry from SRAM
    lpm DC_VALUE, Z                    ; Load the byte at address in Z into r0
main: 
    ;Since the resolution is 8-bit anyway only OCR1AL plays a role.(Right adjusted)
    sts OCR1AL,DC_VALUE
    cpi ZL,low(dc_values*2)
    breq incr	;If we are at the minimum Duty Cycle we should only be able to increase.
    sbis PIND,3	;If PD3 is pressed the decrease the Duty Cycle by ~ 8%
    dec ZL
    lpm DC_VALUE,Z
    incr:
	cpi ZL,(12+low(dc_values*2))
	breq output ;If we are at the maximum Duty Cycle we should only be able to decrease
	sbis PIND,4 ;If PD4 is pressed the increase the Duty Cycle by ~ 8%
	inc ZL
	lpm DC_VALUE,Z
    output:
    clr r24
    clr r25
    ldi r24,0x60
    ldi r25,0x00
    rcall wait_x_msec	;Just a delay after which we jump back to main and output the Duty Cycle value to OCR1AL
    rjmp main
    
;Delay Functions 
	
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

;Table with pre-computed Duty-Cycle Values
dc_values:
    .db 8,28,48,68,88,108,128,148,168,188,208,228,248