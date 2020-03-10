DUTY_CYCLE:
	SBI  DDRB, 3		//portb output
	SBI  PORTB, 3		//turn on PortB.3
	LDI  R18, 31		//loading for delay
    LDI  R19, 65		//
    LDI  R20, 90		//
	CALL DELAY337		//calling delay for 337
	CBI	 PORTB, 3		//turn "OFF" PortB.3
	LDI  R21, 31		//loading for delay
    LDI  R22, 113
    LDI  R23, 31
	CALL DELAY412
JMP DUTY_CYCLE				//jump to duty cycle

DELAY337:					//delay 337
	DEC  r20
    BRNE DELAY337
    DEC  r19
    BRNE DELAY337
    DEC  r18
    BRNE DELAY337
    NOP
    RET
DELAY412:					//delay 412
	DEC  r23
    BRNE DELAY412
    DEC  r22
    BRNE DELAY412
    DEC  r21
    BRNE DELAY412
    NOP
	RET