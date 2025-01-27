.org 0x0000
rjmp reset

.include "m328PBdef.inc" ;ATmega328P microcontroller definitions
.include "routines.inc"

reset:
    ; initialise stack pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r24, HIGH(RAMEND)
    out SPH, r24