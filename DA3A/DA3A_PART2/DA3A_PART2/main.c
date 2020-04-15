/*
Meral Abu-Jaser
Assignment 3A. Task 2
*/

#define F_CPU 16000000UL
#define BAUD 9600

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <time.h>
#include <avr/interrupt.h>
volatile uint8_t counter = 0; 

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

ISR(TIMER0_OVF_vect){
	counter++;	//increment counter
}

int main(void){
	srand(time(NULL));	//random number generator
	USART_init();	//call function to initialize 
	TCCR0A = 0x00;	//normal mode timer
	TCCR0B = 0x05;	//set pre0scaler = 1024
	TCNT0 = 0;	//counter = 0
	TIMSK0 = (1<<TOIE0);	//enable interrupt
	sei();	//enable global
	
	float float_num; //to hold random floating values
	char char_num[20]; //buffer for whole numbers
	char char_float[20]; //buffer for float

	while (1){
		if(counter > 61){
		itoa(rand(), char_num, 10); //convert integer to string
		float_num = rand()*0.15;
		snprintf(char_float,sizeof(char_float),"%f\r\n",float_num);//prints formatted output
		
		USART_tx_string("Hello! this is Meral and this is my DA3A task 2 code\n"); //print the string
		USART_tx_string(char_num); //print the int value 
		USART_tx_string(" \n"); 
		USART_tx_string(char_float); // print the floating value
		
		counter = 0; //reset the counter
		}
	}
}
