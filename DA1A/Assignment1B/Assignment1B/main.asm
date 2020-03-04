;
; Assignment 1B.asm
;
; Created: 3/3/2020 9:27:03 AM
; Author : Meral
;
.ORG 0
.EQU STARTADDS = 0x0300		//location to store 200 numbers
.EQU LOCATION7 = 0x0500		//location to store numbers divisable by 7	
.EQU LOCATION3 = 0x0600		//location to store numbers divisable by 3
.EQU LOCATIONB = 0x0700		//location to store numbers divisable by both 7 and 3
.EQU LOCATIONN = 0x0800		//location to store numbers not divisable by 3 nor 7

	START:
		//Loading the location for each case 
		LDI		XL, LOW(STARTADDS)	//pointer X stores 200 numbers
		LDI		XH, HIGH(STARTADDS)	//
	
		LDI		YH, HIGH(LOCATION7)	//pointer Y holds values divisable by 7
		LDI		YL, LOW(LOCATION7)	//
		LDI		R22, LOW(LOCATION7)	//loading the lower bit into memory

		LDI		ZL, LOW(LOCATION3)	//pointer Z holds values divisable by 3
		LDI		ZH, HIGH(LOCATION3)	//
		LDI		R23, LOW(LOCATION3)	//loading the lower bit into memory

		LDI		R16, 26				//R16 holds the value 26 in hex
		LDI		R17, 4				//R17 holds the value 4 to find numbers divisable ny 7
		LDI		R18, 1				//R18 holds the value 1 to find numbers divisable by 3
		
		//clearing registers to make sure they hold no value
		CLR R1
		CLR R2
		CLR R3
		CLR R4
		CLR R5
		CLR R6
		CLR R7
		CLR R8
		CLR R9
		CLR R10
		CLR R11
		CLR R12
	//loop to load in the values and store 200 numbers by increamenting R16 
	LOAD:		
		CPI		R16, 226	//Once R16 = 226 the loop while exit and the program will execute
		BREQ	DONE

			ST		X+, R16	//storing number at location 0x0300
			INC		R17		//R17 = 5
			INC		R18		//R18 = 2
			CPI		R17, 7	//compare R17 with 7
			BREQ	DIVBY7	//if R17 = 7 then go to branch DIVBY7
			CPI		R18, 3	//compare R18 with 3
			BREQ	DIVBY3	//if R18 = 3 then go to branch DIVBY3
			RJMP	DIVBYNONE	//Unconditional jump to branch DIVBYBOTH
				//loop to calculate numbers divisable both by 7 and 3
				DIVBYBOTH:
					LDI		R18, 0				
					MOV		YL, R24		
					LDI		YH, high(LOCATIONB)
					RJMP	BOTH
				BOTH:
					ST		Y, R16				
					INC		R24					
					ADD		R12, R16				
					ADC		R11, R0
					ADC		R10, R0
					INC		R16	
					RJMP	LOAD

				//loop to calculate number divisable by 7
				DIVBY7:
					LDI		R17, 0					
					CPI		R18, 3				
					MOV		YL, R22				
					LDI		YH, high(LOCATION7)
					BREQ	DIVBYBOTH		
					RJMP	ADD7
				//loop to sum the values that are divisable by 7
				ADD7:
					ST		Y, R16					
					INC		R22	//count
					ADD		R6, R16				
					ADC		R5, R0	//add with carry
					ADC		R4, R0	//
					INC		R16		//increament R16
					RJMP	LOAD
				//loop to calculate number divisable by 3
				DIVBY3:
					LDI		R18, 0				
					MOV		ZL, R23				
					LDI		ZH, high(LOCATION3)
					RJMP	ADD3
				//loop to sum the values that are divisable by 3
				ADD3:
					ST		Z, R16					
					INC		R23	//count
					ADD		R9, R16			
					ADC		R8, R0	//add with carry
					ADC		R7, R0	//
					INC		R16		//increament R16
					RJMP	LOAD
				//loop to calculate the numbers that are not divisable by 3 nor 7
				DIVBYNONE:
					MOV		 ZL, R25				
					LDI		 ZH, high(LOCATIONN)
					RJMP	 NONE
				//loop to sum the value of numbers that are not divisable
				NONE:
					ST		 Z, R16			
					INC		 R25	//count 
					ADD		 R15, R16									
					ADC		 R14, R0	//add with carry
					ADC		 R13, R0	//add with carry
					INC		 R16	
					RJMP	 LOAD

DONE: RJMP	DONE