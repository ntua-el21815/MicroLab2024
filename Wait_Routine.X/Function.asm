.include "m328PBdef.inc"

.DEF A=r16 
.DEF B=r17 
.DEF C=r18
.DEF D=r19
.DEF F0=r20
.DEF F1=r21
.DEF temp2=r22
.DEF temp3=r23
.DEF counter=r24
.cseg
.org 0 
	
    ldi counter,6
    ldi A,0x51
    ldi B,0x41
    ldi C,0x21
    ldi D,0x01
loop:
    ;F0 caclulation
    mov F0,A	; A
    mov temp2,B	; B
    com temp2	; B'
    and F0,temp2 ; (A and B')
    mov temp3,D	; D
    and temp3,temp2 ;(D and B')
    or F0,temp3	; (A and B') or (D and B')
    com F0  ; not ((A and B') or (D and B'))
    ;F1 calculation
    mov F1,A	; A
    mov temp2,C	; C
    com temp2	; C'
    or F1,temp2	; A or C'
    mov temp2,B	; B
    mov temp3,D ; D
    com temp3	; D'
    or temp3,temp2  ; B or D'
    and F1,temp3    ; (A or C') and (B or D')
    ;Changing values for next iteration
    inc A
    subi B,-0x02
    subi C,-0x03
    subi D,-0x04
    dec counter
    brne loop ;If 6 iterations have been made stop.
end:
    nop