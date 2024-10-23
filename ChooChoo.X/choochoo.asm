.include "m328PBdef.inc"
.include "Wait.inc"  
.DEF train=r16
.DEF T=r20

start:
    ser r26
    out DDRD,r26
    out DDRC,r26
    clr TC
    out PORTC,T
    ldi train,0x80
    out PORTD,train
    rjmp left
shift_right:
    set
    bld T,0x00
    out PORTC,T
    rcall wait_1_s
right:
    cpi train,0x80
    breq shift_left
    lsl train
    out PORTD,train
    rcall wait_1_s
    rjmp right
shift_left:
    clt
    bld T,0x00
    out PORTC,T
    rcall wait_1_s
left:
    cpi train,0x01
    breq shift_right
    lsr train
    out PORTD,train
    rcall wait_1_s
    rjmp left