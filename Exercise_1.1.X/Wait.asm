.include "m328PBdef.inc"

; delay = (1000*F1+14) cycles (about DEL_mS in mSeconds)

.equ FOSC_MHZ=16        ; MHz
.equ DEL_mS=1    ; mS
.equ F1=FOSC_MHZ*DEL_mS
.equ MY_DELAY = 100;
    
; initialize stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    
; init portd as output
    ser r26
    out DDRD, r26
    ldi r24, low(MY_DELAY)         
    ldi r25, high(MY_DELAY)

loop1:
    ser r26
    out PORTD, r26
    rcall wait_x_msec     ; mS
    clr r26
    out PORTD, r26   
    rcall wait_x_msec      ; mS
    rjmp loop1

;this routine is used to produce a delay 993 cycles
delay_inner:
    ldi r23, 247          ; 1 cycle
loop3:
    dec r23               ; 1 cycle
    nop                   ; 1 cycle
    brne loop3            ; 1 or 2 cycles
    nop                   ; 1 cycle
    ret                   ; 4 cycles

;this routine is used to produce a delay of (1000*Fl+14) cycles
wait_1_msec:
    push r26              ; (2 cycles)
    push r27             ; (2 cycles) Save r26:r27

loop4:
    rcall delay_inner     ; (3+993)=996 cycles
    sbiw r26,1            ; 2 cycles
    brne loop4            ; 1 or 2 cycles

    pop r27              ; (2 cycles)
    pop r26               ; (2 cycles) Restore r26:r27
    ret                   ; 4 cycles

wait_x_msec:
    push r24 
    push r25   
    ldi r26, low(F1)         
    ldi r27, high(F1)
loop5:
    rcall wait_1_msec    
    sbiw r24,1           
    brne loop5          

    pop r25               
    pop r24               
    ret                   

    
    