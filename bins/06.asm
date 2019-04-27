start:
;number of loops to perform
LDA #$10
STA $A0


outer:
;number of bytes to color in a loop
LDY #$03
;LDX #$00

loop:
;LDA #$01
STA $0200, X
INX
DEY
BNE loop

;outer loop handling
DEC $A0
LDA $A0
BNE outer

;overwrite the address for drawing and loop once again

LDA #$30
ADC $0607 ;add 3*16 for the next color strip
STA $0607
LDA #$90
CMP $0607
;LDA $0607
BNE start