.ORG 0
	CBI ddrc, 3			//set bit 3 in port c for input
	SBI portc, 3		//PortC.3 is set to take in input
	SBI ddrb, 2			// set bit 2 in port b for output
	CBI portb, 2		// PortB.2 is off	
LOOP:
	SBI  DDRB, 3		//portb output
	SBI  PORTB, 3		//turn on PortB.3
	LDI  R18, 31		//loading for delay 
   	LDI  R19, 65		//
   	LDI  R20, 90		//
	CALL DELAY337		//calling delay for 337
	
	CBI  PORTB, 3		//turn "OFF" PortB.3
	
	LDI  R21, 31		//loading for delay 412
    LDI  R22, 113
    LDI  R23, 31
	CALL DELAY412		//calling for delay

	SBIC pinc, 3		//if Pinc.3 is clear, skip next
	JMP LOOP			//button is not pressed, loop back 
	CBI PORTB, 2		
	CALL DELAY200		// wait for 2 seconds
	SBI portb, 2		// PortB.2 is turned on after the delay
	JMP LOOP			// loop back and check for button press

DELAY200:				// Delay for 2 seconds
	LDI  r18, 164
    LDI  r19, 255
    LDI  r20, 200
again: 
	DEC  r20
    BRNE again
    DEC  r19
    BRNE again
    DEC  r18
    BRNE again
	RET

DELAY337:			//delay 337
	DEC  r20
  	BRNE DELAY337
    DEC  r19
    BRNE DELAY337
    DEC  r18
    BRNE DELAY337
    NOP
	RET
DELAY412:			//delay 412
	DEC  r23
    BRNE DELAY412
    DEC  r22
    BRNE DELAY412
    DEC  r21
    BRNE DELAY412
    NOP
	RET