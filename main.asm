.ORG 0
	JMP LOOP
.ORG 0x02
	JMP EX0_ISR			//initializing stack
	LDI R22,HIGH(RAMEND)//
	OUT SPH,R22			//
	LDI R22,LOW(RAMEND)	//
	SBI DDRB, 2			//portb is set as output
	CBI PORTB, 3		//portb.3 is set to low

LOOP:
	LDI R22, 0X2		//falling edge triggered
	STS EICRA, R22		//INT0 controller
	SBI DDRB, 3		
	SBI PORTB, 3		//set to high
	CALL DELAY2			//337.5ms delay
	CBI PORTB, 3		//set to low
	CALL DELAY1			//412.5ms delay
	LDI R22, 1 << INT0	//falling edge triggered
	OUT EIMSK, R22
	SEI					//enabling interrupts
JMP LOOP

EX0_ISR:
	CBI DDRB, 3
	SBI DDRB, 2
	CALL DELAY3 //delaying by 2s
	CBI DDRB, 2 //outputting 0 to PB2

DELAY1://412.5ms delay loop
	LDI R16, 230
	LDI R17, 243
	LDI R18, 32

AGAIN1: 
	DEC  R16
    BRNE AGAIN1
    DEC  R17	
    BRNE AGAIN1
    DEC  R18
    BRNE AGAIN1
RET

DELAY2://337.5ms delay loop
	LDI R16, 250
	LDI R17, 237
	LDI R18, 37

AGAIN2: 
	DEC  R16
    BRNE AGAIN2
    DEC  R17
    BRNE AGAIN2
    DEC  R18
    BRNE AGAIN2
RET

DELAY3://2 second deley loop
	LDI R16, 164
	LDI R17, 255
	LDI R18, 255

AGAIN3: 
	DEC  R16
    BRNE AGAIN3
    DEC  R17
    BRNE AGAIN3
    DEC  R18
    BRNE AGAIN3
RET