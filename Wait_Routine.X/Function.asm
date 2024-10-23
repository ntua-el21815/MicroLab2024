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
	mov F0,A
	mov temp2,B
	com temp2
	and F0,temp2
	mov temp3,D
	and temp3,temp2
	or F0,temp3
	com F0
	mov F1,A
	mov temp2,C
	com temp2
	or F1,temp2
	mov temp2,B
	mov temp3,D
	com temp3
	or temp3,temp2
	and F1,temp3
	inc A
	subi B,-0x02
	ldi temp2,0x03
	add C,temp2
	ldi temp2,0x04
	add D,temp2
	dec counter
	brne loop
	