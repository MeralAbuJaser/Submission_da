/*
 * DA5_TASK3.c
 *
 * Created: 5/6/2020 1:08:53 AM
 * Author : Meral
 */ 
#define F_CPU 16000000UL
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>							
#define PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)	

void write(uint8_t value){
	//set low and high input/output
	PORTC &= ~ (1<<PC1); 
	DDRC |= (1<<PC1); 

	if(value)	//if 1 is passed go high
		DDRC &= ~(1<<PC1);
	else //if not 1 stay low
		DDRC |= (1<<PC1);
		
	//wait 60uS and release data
	_delay_us(50);
	DDRC &= ~(1<<PC1); //input
}

uint8_t ds18b20_readbit(void){
	uint8_t bit=0;
	
	//1us delay on low
	PORTC &= ~ (1<<PC1); //low
	DDRC |= (1<<PC1); //output
	_delay_us(1);
	
	//send data and wait for 14us
	DDRC &= ~(1<<PC1); //input
	_delay_us(14);

	//read the value
	if(PINC & (1<<PC1))
		bit=1;
		
	//wait for 45us and return bit value 
	_delay_us(45);
	return bit;
}

void write_byte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		write(byte&1);
		byte >>= 1;
	}
}

uint8_t ds18b20_readbyte(void){
	uint8_t i=8, n=0;
	while(i--){
		n >>= 1;
		n |= (ds18b20_readbit()<<7);
	}
	return n;
}

void USART_Init(unsigned long BAUDRATE)	{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);	//enable USART transmitter and receiver
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);//write USCRC and 1 stop bit 
	UBRR0L =  PRESCALE;							//load UBRRL
	UBRR0H = (PRESCALE >> 8);					//load UBRRH 
}

void USART_Tx(char data){
	UDR0 = data;					//Write data 
	while (!(UCSR0A & (1<<UDRE0)));	//data transmit? buffer set to empty
}

void USART_SendString(char *str){
	int i=0;
	while (str[i]!=0){
		USART_Tx(str[i]);						
		i++;
	}
}

double Read_temp(){
	PORTC &= ~ (1<<PC1); //low
	DDRC |= (1<<PC1); //output
	_delay_us(480);//low for 480us
	
	//release line and wait for 60uS
	DDRC &= ~(1<<PC1); //input
	_delay_us(500);
	
	write(0xCC); //skip next
	write(0x44); //convert the temp
	if(!ds18b20_readbit()); //while still converting stay here
	
	//reset 
	PORTC &= ~ (1<<PC1); 
	DDRC |= (1<<PC1); 
	_delay_us(480);
	
	//input the data 
	DDRC &= ~(1<<PC1); //input
	_delay_us(500);
		
	write_byte(0xCC); //skip next
	write_byte(0xBE); //read
	
	//read 2 byte
	uint8_t temp1 = ds18b20_readbyte();
	uint8_t temp2 = ds18b20_readbyte();

	//return the converted temp
	return (( temp2 << 8 ) + temp1) * 0.0625;
}

void display(){
	char celcius[50];
	dtostrf(Read_temp(), 3, 0, celcius);
	USART_SendString("Celcius degree: "); 
	USART_SendString(celcius); 
	USART_SendString("\n\n");
	_delay_ms(500);	
}
int main(void) {
	USART_Init(9600);
		while (1){
			display();
		}
	return 0;
}