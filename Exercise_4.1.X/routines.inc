.include "m328PBdef.inc" ;ATmega328P microcontroller definitions

; delay = (1000*F1+14) cycles (about DEL_mS in mSeconds)

.equ FOSC_MHZ=16       ; MHz
.equ DEL_mS=1         ; mS
.equ F2=FOSC_MHZ*DEL_mS
.equ F1=F2/1000
.equ PD3=3
.equ PD2=2
    
.org 0x00
jmp reset

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
wait_1usec:
    push r24              ; (2 cycles)
    push r25              ; (2 cycles) Save r24:r25
    ldi r24,low(F1)
    ldi r25,high(F1)
loop4:
    rcall delay_inner     ; (3+993)=996 cycles
    sbiw r24,1            ; 2 cycles
    brne loop4            ; 1 or 2 cycles

    pop r25               ; (2 cycles)
    pop r24               ; (2 cycles) Restore r24:r25
    ret 
wait_usec:
    push r24
    push r25
    loopa:
	rcall wait_1usec
	sbiw r24,1
	brne loopa
    ret

wait_1msec:
    push r24              ; (2 cycles)
    push r25              ; (2 cycles) Save r24:r25
    ldi r24,low(F2)
    ldi r25,high(F2)
loop5:
    rcall delay_inner     ; (3+993)=996 cycles
    sbiw r24,1            ; 2 cycles
    brne loop5            ; 1 or 2 cycles

    pop r25               ; (2 cycles)
    pop r24               ; (2 cycles) Restore r24:r25
    ret 

wait_msec:
    push r24
    push r25
    my_loop:
	rcall wait_1msec
	sbiw r24,1
	brne my_loop
    ret
  
write_2_nibbles:
    push r24 ; save r24(LCD_Data)
    in r25 ,PIND ; read PIND
    andi r25 ,0x0f ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    pop r24 ; Recover r24(LCD_Data)
    swap r24 ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ret

lcd_data:
    sbi PORTD ,PD2 ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles ; send data
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret

lcd_command:
    cbi PORTD ,PD2 ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles ; send Instruction
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret
    
lcd_clear_display:
    ldi r24 ,0x01 ; clear display command
    rcall lcd_command
    ldi r24 ,low(5) ;
    ldi r25 ,high(5) ; Wait 5 mSec
    rcall wait_msec ;
    ret
    
lcd_init:
    ldi r24 ,low(200) ;
    ldi r25 ,high(200) ; Wait 200 mSec
    rcall wait_msec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x20 ; command to switch to 4 bit mode
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x28 ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24 ,0x0c ; dislay on, cursor off
    rcall lcd_command
    rcall lcd_clear_display
    ldi r24 ,0x06 ; Increase address, no display shift
    rcall lcd_command ;
    ret