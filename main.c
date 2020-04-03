#define F_CPU 16000000L
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRB |= (1<<3);		//DDRB.3 is output
	PORTB &=~(1<<3);	//PORTB.3 is output on low
	EICRA = 0X02;		//external interrupt on falling edge
	EIMSK = (1<<INT0); 	//external interrupt INT0

	sei ();				

	while(1){
		PORTB |=(1<<3); //"LED ON"
		_delay_ms(412); //delay 412ms 
		PORTB &=~(1<<3);//"LED OFF"
		_delay_ms(337); //delay 337ms
	}
	return 1;
}
ISR (INT0_vect){
	DDRB &= ~(1<<3);		//PORTB.3 output low DDRB.3
	DDRB |= (1<<2);			//PORT.2 output high
	_delay_ms(2000);		//delay 2s
	PORTB &=~(1<<2);		//PORTB.3 output High
	DDRB &=~(1<<2);			//PORTB.2 output LOW DDRB.2
	DDRB |= (1<<3);			//PORT.3 output high
}
