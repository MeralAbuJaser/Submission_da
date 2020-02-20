;
; Assignement1A.asm
;
; Created: 2/19/2020 7:04:12 PM
; Author : Meral
;

.org 0

LDI R16, 0x00 ;R16 to R19 is the multiplicand
LDI R17, 0x10
LDI R18, 0x10
LDI R19, 0x10
LDI R20, 0x00 ;R20 to R23 is the multiplier
LDI R21, 0x01
LDI R22, 0x01
LDI R23, 0x01
LDI R24, 0x00 ;R24 to R31 hold the results
LDI R25, 0x00
LDI R26, 0x00
LDI R27, 0x00
LDI R28, 0x00
LDI R29, 0x00
LDI R30, 0x00
LDI R31, 0x00

	SUB	R31,R31     ;Clear R32 and Carry Flag
	ADD	R28,R16      ;Add Multipicand into Answer
    ADC	R29,R17      ;
	ADC	R30,R18      ;
    ADC	R31,R19      ; 
LOOP:
    LSR R24		;shifting answer to the right
    ROR R25		;
    ROR R26		;
    ROR R27		;
	ROR R28		;
	ROR R29		;
	ROR R30		;
	ROR R31		;
	LSR R20		;shift multiplier to the right
	ROR	R21		;
	ROR R22		;
	ROR R23		;
	BRCC COUNTER;If Carry Clear skip addition
	BRCS ADDITION

ADDITION:
	ADD R27,R19	;add multiplicand to answer
	ADC R26,R18 ;
	ADC R25,R17 ;
    ADC R24,R16 ;
	RJMP COUNTER;jump to counter

COUNTER:
	INC R0		;increment R0
	SBRS R0,5	;check if counter is 32
	BRNE LOOP	;go back to loop
	RJMP LAST_SHIFT
LAST_SHIFT:
	LSR R24 ;shift answer to the right
	ROR R25 ;
	ROR R26 ;
	ROR R27 ;
	ROR R28 ;
	ROR R29 ;
	ROR R30 ;
	ROR R31 ;
DONE:
