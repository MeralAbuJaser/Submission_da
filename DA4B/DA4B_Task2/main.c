/*
 * DA4B_Task2.c
 *
 * Created: 5/10/2020 5:54:14 PM
 * Author : Meral
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int adc_motor;	//to store the potentiometer value

// Initialize ADC
void adc_init(void) {
	/**Setup and enable ADC**/
	ADMUX =  (0<<REFS1) | //Reference selection bits
	(1<<REFS0) | //AVcc - external cap at AREF (5)V
	(0<<ADLAR) | //ADC right adjust result
	(1<<MUX2)  | //Analog channel selection bits
	(0<<MUX1)  | //ADC4 (PC4 PIN27)
	(0<<MUX0);
	
	ADCSRA = (1<<ADEN)  | //ADC enable
	(0<<ADSC)  | //ADC start conversion
	(0<<ADATE) | //ADC auto trigger enable
	(0<<ADIF)  | //ADC interrupt flag
	(0<<ADIE)  | //ADC interrupt enable
	(1<<ADPS2) | //ADC Prescaler select bits
	(1<<ADPS1) | //128 AS PRESCALAR SELECION BIT
	(1<<ADPS0);	 //Select channel
}

void read_adc(void){
	unsigned char i=4;	//number of samples 		
	adc_motor = 0;	
	while(i--){
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_motor += ADC;
	}
	adc_motor = adc_motor/4;// average of samples
}

int main(void){
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11); 
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //pre-scaler setup 
	ICR1 = 4999;			//Period = 20ms Standard
	DDRB |= (1<<PB1);		//output
	adc_init();	
	
	while(1) {
		read_adc();
		if (adc_motor >= 1023) {
			OCR1A = 535;	// 180 degree
			_delay_ms(500);
		}
		else if (adc_motor <= 20) {
			OCR1A = 97;	// 0 degree
			_delay_ms(500);
		}
	}
	return 0;
}