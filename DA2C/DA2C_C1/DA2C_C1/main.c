#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRC &=~ (1<<3);					
	PORTC |= (1<<3);					

	while (1){
		TCCR0A = 0;						//normal mode timer
		TCCR0B = 0X05;					//pre-scaler = 1024
		TCNT0 = 0X00;					//the counter of the timer
	
		if (!(PINC & (1 <<3))){			//if the button is pressed 
			DDRB = (0<<3);				
			DDRB |= (1<<2);	
			int cycle2 = 122.07;		//2s				
	
			while (cycle2 != 0){			
				cycle2--;				//increment the 2s
			while ((TIFR0 & 0X01) == 0);
				TIFR0 = 0X01;			//reset the flag
		}
		DDRB = (0<<3);					//if button is not pressed
	}
	
	DDRB |= (1<<3);
	int cycle412 = 25.2;				//412ms		
	
	while (cycle412 != 0){			
		cycle412--;						//increment 412ms
	while((TIFR0 & 0X01) == 0);			
		TIFR0 = 0X01;					//reset flag
	}
	
	DDRB = (0<<3);
	int cycle337 = 20.569;				//337ms						
	
	while (cycle337 != 0){				
		cycle337--;						//decreases the value by 1
	while ((TIFR0 & 0X01) == 0);
		TIFR0 = 0X01;					//reset flag
		}
	}
	return 1;
}