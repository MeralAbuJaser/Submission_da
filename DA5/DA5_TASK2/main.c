/*
 * DA5_TASK2.c
 *
 * Created: 5/5/2020 9:02:20 PM
 * Author : Meral
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <util/delay.h>

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 
							   0x49, 0x41, 0x1F, 0x01, 0x09};
/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

//variables to use for calculations
volatile uint8_t data;	//to send data
volatile uint8_t adc_value;	//to read the analoge value
volatile uint8_t adc_cel;	//to save the temperture value

void USART_init(void){ 
	UBRR0H = UBRRH_VALUE; 
	UBRR0L = UBRRL_VALUE; 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //8-bit data 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   //Enable RX and TX  
}

//Send data to the serial port 
void USART_tx_string(char *data ){ 
	while ((*data != '\0')){ //while the register is empty enter date
		while (!(UCSR0A & (1 <<UDRE0))); 
			UDR0 = *data; 
			data++; //increment data location forward
	}   
}

void enableADC(){
	ADMUX |= (1<<REFS0)|(1<<MUX2);	//AREF ,analog channel bit selection 					
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1); //Enable ADC
}

uint8_t Read_ADC() {
	DDRC &= ~(1<<4);				//PINC4 set as input
	enableADC();					//enable adc
	ADCSRA |= (1<<ADSC);			//adc conversion 
	while (!(ADCSRA&(1<<4)));		//Wait
	ADCSRA |= (1<<4);				//clear ADIF
	return ADC;						//returns adc value
}

void SPIinitialize() {
	DDRD |= (1<<PIND4);				//sets latch clock pin as output
	DDRB |= (1<<PINB3);				//sets MOSI pin as output 
	DDRB |= (1<<PINB5);		
	PORTD &= ~(1<<PIND4);			//sets clock output to zero
	SPCR0 = (1<< MSTR)| 0xFF;		//enable SPI mode as master
}

void SPIwrite(data){
	SPDR0 = data;					// Sends data
	while (!(SPSR0&(1<<SPIF)));		// Waits until data is sent
	SPSR0 |= (1<<SPIF);				//clears flag
}

void CSC(){
	PORTD |= (1<<PIND4);	//high 
	_delay_ms(20);			//delay for 20ms
	PORTD &= ~(1<<PIND4);	//low
}

void SendDigits(uint8_t digits[]) {													
	for (uint8_t i = 0; i < 4; i++) {
		SPIwrite(SEGMENT_MAP[digits[i]]);	//send seven-segment value
		SPIwrite(1<<(4+i));					//send value to seven-segment
		CSC();	//Clear the clock, 20ms delay, set clock
	}
}

void SetDigits(uint16_t temp, uint8_t array[]) {
	for (uint8_t i = 0; i < 4; i++) {
		array[i] =((uint8_t)(temp/pow(10, i)))%10;
	}
}

int main(void) {
	uint8_t digits[4];	//array to stores the the 4 digits for display
	SPIinitialize();	//initialize SPI
	USART_init();
	char celcius[15];	//to hole the value of the temperture
	
	while (1) {
		adc_value = Read_ADC();	//setting the value read by sensor to adc_value
		adc_cel = adc_value-27;	//temp in celcius 
		SetDigits(adc_cel, digits);	//to output the degree on the seven-segment display
		SendDigits(digits);
		
		USART_tx_string("Celcius degree: ");	//print string
		USART_tx_string(celcius); //print the temperture	
		snprintf(celcius,sizeof(celcius),"%u\r\n",adc_cel); //print formatted output
	}
}