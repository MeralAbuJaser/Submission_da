/*
 * DA3B.c
 *
 * Created: 4/18/2020 4:50:29 PM
 * Author : Meral
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#include <stdio.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <avr/io.h>

volatile float adc_fah;//declare float value for Fahrenheit temp
volatile float adc_cel;//declare float value for celcius temp
volatile float temp;

void USART_init(void){ 
	UBRR0H = UBRRH_VALUE; 
	UBRR0L = UBRRL_VALUE; 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //8-bit data 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   //Enable RX and TX  
}

//Send data to the serial port 
void USART_tx_string( char *data ){ 
	while ((*data != '\0')){ //while the register is empty enter date
		while (!(UCSR0A & (1 <<UDRE0))); 
			UDR0 = *data; 
			data++; //increment data location forward
	}   
}

void read_adc(void){
	unsigned char i = 10;
	adc_fah = 0;
	adc_cel = 0;
	temp = 0;
	while(i--){
		ADCSRA |=(1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		temp += ADC;
	}
	adc_fah = (temp/6)-4; //convert ADC value to fahrenheit
	adc_cel = (temp/18);//convert fahrenheit to celcius
}

ISR(TIMER0_OVF_vect){
	char fahrenheit[20];//character array for fahrenheit degree
	char celcius[20];//character array for celcius degree

	for (int i = 0; i <= 30; i++) {
		while (TCNT0 != 255) {}	// loop runs until TCNT = 255
		TCNT0 = 0;		// reset TCNT
	}
	
	read_adc();
	
	USART_tx_string("Fahrenheit degree: ");
	snprintf(fahrenheit,sizeof(fahrenheit), "%f\r\n", adc_fah);//stores the value in the array
	USART_tx_string(fahrenheit);//prints the fahrenheit value 

	USART_tx_string("Celcius degree: ");
	snprintf(celcius,sizeof(celcius), "%f\r\n", adc_cel);//stores the value in the array
	USART_tx_string(celcius);//prints the celsius value 
	USART_tx_string("\n\n");
	
	TCNT0 = 0;
}

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

int main(void){
	USART_init();	//call function to initialize 
	TCCR0A = 0x00;	//normal mode timer
	TCCR0B = 0x05;	//set pre0scaler = 1024
	TCNT0 = 0;		//counter = 0
	TIMSK0 = (1<<TOIE0);	//enable interrupt
	sei();	//enable global
	adc_init();//initialize adc
	
	while(1){}
}
