/*
 * Midterm2_task2.c
 *
 * Created: 5/9/2020 4:03:22 PM
 * Author : Meral
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
volatile int adc_value;

// Initialize ADC
void adc_init(void) {
	/**Setup and enable ADC**/
	ADMUX =  (0<<REFS1) | //Reference selection bits
	(1<<REFS0) | //AVcc - external cap at AREF (5)V
	(1<<ADLAR) | //ADC right adjust result
	(0<<MUX2)  | //Analog channel selection bits
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

void read_adc(){
	ADCSRA |= (1 << ADSC);			//enable conversion 
	while(ADCSRA & (1<< ADSC));
	adc_value = ADCH;				//value of the Potentiometer is stored in adc_value
}

int main(void){
	int num;
	adc_init();
	DDRD |= (1<< DDRD4) | (1<< DDRD5) | (1<< DDRD6);		//pins PD4, PD5, and PD6 are outputs
	OCR0A = 128;							//50% duty cycle
	TCCR0A |= (1<< COM0A1) | (1<<WGM01) | (1<< WGM00);	//enable non-inverting, Fast PWM
	TCCR0B |= (1<< CS00);					//pre-scaler 1
	
	while (1){
		read_adc();
		OCR0A = adc_value;	
		//clockwise direction 
		if(num == 1){
			PORTD|=(1<<DDD5);							
			PORTD&=~(1<<DDD4);								
			_delay_ms(5000);
		}
		num = 0;	//set value to counterclockwise
		_delay_ms(1000);
		//counterclockwise direction 
		if(num == 0){
			PORTD&=~(1<<DDD5);								
			PORTD|=(1<<DDD4);							
			_delay_ms(5000);
			num = 1;	//set value to clockwise
		}
	}	
	return 0;
}
