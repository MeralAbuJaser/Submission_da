/*
 * DA4A.c
 *
 * Created: 5/7/2020 7:18:14 PM
 * Author : Meral
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

float dutyCycle = 0;			
volatile uint8_t power = 0;		//ON/OFF power

float readADC(){
	ADCSRA |= (1<<6);				//enable ADC conversion
	while (!(ADCSRA&(1<<4)));		//waits until ADIF is set
	ADCSRA |= (1<<4);				//clear flag
	return ADC;						//returns adc read value
}
void enableADC(){
	ADMUX = 0x40;			//right justified
	ADCSRA = 0x87;			//enables ADC with 128 pre-scaler
}
void enablePWM(){
	TCCR0A = 0x83;			//fast PWM on OC0A in non-inverting mode
	TCCR0B = 0x02;			//fast PWM, 1024 prescaler
}
ISR(PCINT1_vect) {
	if (PINC & (1<<PINC1)){
		power = ~power;					//invert current state
		if(power){
			DDRD |= (1<<PIND6);			//output OC0A
			DDRB &=~(1<<PINB3);		
			enablePWM();
		} 
		if(!power){
			DDRD &= ~(1<<PIND6);	//input
			DDRB |=(1<<PINB3);	
		}
	}
	PCIFR = (1<<PCIF1);				//clear interrupt flag
}
int main(void) {
	DDRC &= ~(1<<PINC0);  //potentiometer input
	enableADC();
	DDRC &= ~(1 << PINC1);	//input
	PCICR = (1 << PCIE1);	//interrupt
	PCMSK1 = (1 << PCINT9);	//pin change interrupt 
	sei();					//enable global interrupt

	while (1) {
		dutyCycle = 95*(readADC()/1023);		//calculated ratio of 95%
		OCR0A = (dutyCycle/100)*255;			//calculate the duty cycle
	}
}
