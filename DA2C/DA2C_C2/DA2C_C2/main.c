#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
volatile uint8_t counter;		//sets the counter throughout the program


ISR(TIMER0_OVF_vect){
	counter++;					//increment overflow
}

void button(){
	DDRB = (0<<3);			
	counter = 0;				//reset counter
	TIMSK0 = (1<<TOIE0);	
	
	sei();						//enable global interrupt
	DDRB |= (1<<2);					
	
	while (counter < 122);     //if counter is less than 122
		DDRB = (0<<2);
		counter = 0;			//reset counter
		DDRB = (1<<3);				
}

int main(void){
	DDRC &=~ (1<<3);	
	PORTC |= (1<<3);	
			
	TCCR0A = 0;					//normal mode timer 
	TCCR0B = 0X05;				//pre-scaler = 1024
	TCNT0 = 0X00;				//counter = 0
	TIMSK0 = (1<<TOIE0);		//timer interrupt
	sei();						
	DDRB = (1<<3);			
	while (1){                  							
		if (!(PINC & (1<<3))){		
			button();			//call the button function
		}
		
		PORTB |=(1<<3);	
		_delay_ms(412);			//412ms delay  
		PORTB &=~(1<<3);			
		_delay_ms(337);			//337ms delay
	}
	return 1;
}

