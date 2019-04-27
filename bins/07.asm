  LDX #$00
  LDY #$00
firstloop:
  TXA
  STA $0200,Y
  PHA
  INX
  INY
  STA $0200,Y
  INY
  PHA
  CPY #$20
  BNE firstloop ;loop until Y is $10
secondloop:
  PLA
  STA $0200,Y
  INY
  CPY #$40      ;loop until Y is $20
  BNE secondloop