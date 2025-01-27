.include "m328PBdef.inc"
.include "Wait.inc"  
.DEF train=r16
.DEF T=r20

start:
    ser r26 
    out DDRD,r26    ;Initialise PORTD as output
    out DDRC,r26    ;Initialise PORTC as output
    clr T	    ;Direction register initially 0 (left)
    out PORTC,T	    ;Direction register is visualised on PORTC
    ldi train,0x80
    out PORTD,train ;The "train" will be visualised on PORTD
    rjmp left
shift_right:
    set		    ;Change the T flag to indicate right (1)
    bld T,0x00	    ;Load the T flag onto the Direction register
    out PORTC,T
    rcall wait_1_s  ;Wait an extra second when switiching direction.
right:
    cpi train,0x80  ;If the left-most bit (MSB) has been reached switch direction
    breq shift_left
    lsl train	    ;Left shift to move bit to next position   
    out PORTD,train
    rcall wait_1_s  ;One second delay between each movement
    rjmp right
shift_left:
    clt		    ;Change the T flag to indicate left (0)
    bld T,0x00	    ;Load the T flag onto the Direction register
    out PORTC,T
    rcall wait_1_s  ;Wait an extra second when switiching direction.
left:
    cpi train,0x01  ;If the right-most bit (LSB) has been reached switch direction
    breq shift_right
    lsr train	    ;Right shift to move bit to next position   
    out PORTD,train 
    rcall wait_1_s  ;One second delay between each movement
    rjmp left