#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t counter;		//sets the counter throughout the program

ISR(TIMER0_OVF_vect){
	counter++;					//increment
}

void button(){
	DDRB = (0<<3);						

	TCCR0A = (1<<WGM01);		//normal mode operations 
	OCR0A=120;					//compare register
	
	TIMSK0 = (1<<OCR0A);		//enable timer interrupt
	sei();						//enable global interrupt
	
	DDRB |= (1<<2);
	while (counter = 122);		//id counter is > than 122
	DDRB = (0<<2);		
	counter = 0;				//counter = 0
	DDRB = (1<<3);		
}

int main(void){
	DDRC &=~ (1<<3);			
	PORTC |= (1<<3);			
	TCCR0A = (1<<WGM01);		//CTC mode 
	TCCR0B = 0X05;				//pre-scaler = 1024
	TCNT0 = 0X00;				//counter= 0
	OCR0A=64;
	TIMSK0 = (1<<OCR0A);		//enable timer interrupt
	sei();						//enable global interrupt
	DDRB = (1<<3);			
	while (1){                  
		if (!(PINC & (1<<3))){
			button();
		}
				
		PORTB |=(1<<3);			
		_delay_ms(412);				//412ms delay
		PORTB &=~(1<<3);			
		_delay_ms(337);				//337ms	delay
	}
	return 1;
}