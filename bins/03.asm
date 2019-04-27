;number of loops to perform
LDA #$10
STA $A0

;number of bytes to color in a loop

outer:
LDY #$0A
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

