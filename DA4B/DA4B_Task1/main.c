/*
 * DA4B_Task1.c
 *
 * Created: 5/9/2020 1:33:36 AM
 * Author : Meral
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>

void read_adc(void) {
	TCCR0A |= (1<<COM0A1)|(1<<WGM01); //enable CTC mode
	DDRC &= ~(1<<PINC0);	//potentiometer input
	ADMUX = ADMUX = (1<<REFS0);			
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//pre-scaler set to 128
}

void get_adc(){
	volatile unsigned int speed;
	ADCSRA |= (1<<ADSC);				//start converting
	while((ADCSRA & (1<<ADIF)) == 0);	//get the value of speed
		speed = ADC & 0x03FF;
	OCR1A = speed;	//set speed
}

void delay(){
	while (!(TIFR1 & (1<<OCF1A)));
	TIFR1 |= (1<<OCF1A);//set the delay for each pin port
}

int main(void){	
	read_adc();	//adc conversion 
	DDRB = 0x0F;//set portD as output 
	DDRC = 0x00;//set input for potentiometer
	PORTB = 0x00;
	TCCR1B |= 0x0C;//set prescaler 
	
	while(1){
		get_adc();
		//controlling speed
		if(PORTB = 0x09)
			delay();
		if(PORTB = 0x03)
			delay();
		if(PORTB = 0x06)
			delay();
		if(PORTB = 0x0C)
			delay();
	}
}