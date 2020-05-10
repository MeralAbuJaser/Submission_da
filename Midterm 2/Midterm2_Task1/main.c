/*
 * Midterm2_Task1.c
 *
 * Created: 5/9/2020 3:59:25 AM
 * Author : Meral
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	int num;
	DDRD |= (1<< DDRD4) | (1<< DDRD5) | (1<< DDRD6);		//pins PD4, PD5, and PD6 are outputs
	OCR0A = 128;											//50% duty cycle
	TCCR0A |= (1<< COM0A1) | (1<<WGM01) | (1<< WGM00);		//enable non-inverting, Fast PWM
	TCCR0B |= (1<< CS00);									//prescaler 1
	
	while (1){
		//clockwise direction 
		if(num == 1){
			PORTD|=(1<<DDD5);							
			PORTD&=~(1<<DDD4);								
			_delay_ms(3000);
			num = 0;	//set value to counterclockwise
		}
		//counterclockwise direction 
		if(num == 0){
			PORTD&=~(1<<DDD5);								
			PORTD|=(1<<DDD4);							
			_delay_ms(3000);
			num = 1;	//set value to clockwise
		}
	}
}
