;******************************************************************
;                               EXAMPLE1
;******************************************************************
.include "m328PBdef.inc"
.equ NUM=9

reset:
    ldi r24 , low(RAMEND)	; initialize stack pointer
    out SPL , r24
    ldi r24 , high(RAMEND)
    out SPH , r24
    
start:
    LDI Zh, HIGH(Table*2)   ; ?(zl=R30, zh=R31) <-- Table address 
    LDI Zl, LOW(Table*2)    ; multiply by two for byte access	
			    
    ldi r21,NUM		    	; load number
 
    clr	r18
    add zl, r21		    	;  access the appropriate table element
    adc zh, r18 
 
    lpm			    		; R0 <-- (Z)
    mov r22,r0		
    
    rjmp start

Table: 	 	 	 			; table values organized by word
.DW 0x0100,0x0904,0x1910,0x3124,0x5140 
.DW 0x7964,0xA990,0xE1C4



