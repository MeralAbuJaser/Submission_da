/*
 * DA5_TASK1.c
 *
 * Created: 5/1/2020 11:28:34 PM
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

#define LATCH 4 /* PD4-RCK */
#define CLOCK 7 /* PD7-SRCK */
#define DATA 0  /* PB0-SER IN */
#define LSBFIRST 0
#define MSBFIRST 1

volatile uint8_t counter = 0; //global variable
volatile int adc_cel;   //global variable FOR C

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
							   0x92, 0x82, 0xF8, 0X80, 0X90};
/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};
void shift_out_init(void) {
	DDRB |= (1 << DATA);
	DDRD |= (1 << CLOCK) | (1 << LATCH);
}
void uart_init(void) {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
	UCSR0A |= _BV(U2X0);
	#else
	UCSR0A &= ~(_BV(U2X0));
	#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

int uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

int uart_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void shift_out(uint8_t indata) {
	for (uint8_t i = 0; i < 8; i++) {
		/* Write bit to data port. */
		if (0 == (indata & _BV(7 - i))) {
			// digital_write(SHIFT_OUT_DATA, LOW);
			PORTB &= (0 << DATA);
			} else {
			// digital_write(SHIFT_OUT_DATA, HIGH);
			PORTB |= (1 << DATA);
		}

		/* Pulse clock to write next bit. */
		PORTD |= (1 << CLOCK);
		PORTD &= (0 << CLOCK);
	}
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder,
uint8_t val) {
	uint8_t i;

	for (i = 0; i < 8; i++) {
		if (bitOrder == LSBFIRST)
		dataPin |= !!(val & (1 << i));
		else
		dataPin |= !!(val & (1 << (7 - i)));

		PORTD |= (1 << CLOCK);
		PORTD &= (0 << CLOCK);
	}
}

void shift_out_latch(void) {
	PORTD &= (0 << LATCH);
	PORTD |= (1 << LATCH);
}

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
	counter++;		//increment counter
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

void read_adc(void){
	unsigned char i = 10;
	adc_cel = 0;
	while(i--){
		ADCSRA |=(1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_cel += ADC;
	}
	adc_cel = (adc_cel/18);//convert fahrenheit to celcius
}

void display(){
	_delay_ms(300);
	read_adc(); 
			
	int digit1 = adc_cel/10; //to display the seven segment position 1-4
	int digit2 = adc_cel%10; //to display the number 0-9
	
		switch(digit1){
			//dipaly 1-4 for 300ms
			case 1:
					//diplpay 1
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[1]); //0xF9
					shift_out(SEGMENT_SELECT[0]);	//0xF1
					PORTD |= (1 << LATCH);
					break;
			case 2: 
					//display 2
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[2]);	// 0xA4
					shift_out(SEGMENT_SELECT[0]);	//0xF1
					PORTD |= (1 << LATCH);
					break;
			case 3: 
					//display 3
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[3]); //0xB0
					shift_out(SEGMENT_SELECT[0]);	//0xF1
					PORTD |= (1 << LATCH);
					break;
			case 4: 
					//display 4
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[4]);	//0x99
					shift_out(SEGMENT_SELECT[0]);	//0xF1
					PORTD |= (1 << LATCH);
					break;			
		}
		_delay_ms(200);
		
			switch(digit2){
				//display 0-9 for 200 ms 
				case 0: 
					//display 0
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[0]);		//0xc0
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);	
					break;
				case 1: 
					//display 1
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[1]);		//0xF9	
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 2: 
					//display 2
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[2]);		//0xF9
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 3: 
					//display 3
					PORTD &= (0 << LATCH);	
					shift_out(SEGMENT_MAP[3]);		//0xB0	
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 4: 
					//display 4
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[4]);		//0x99
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 5: 
					//display 5
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[5]);		//0x92
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 6: 
					//display 6
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[6]);		//0x82
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);	
					break;
				case 7: 
					//display 7
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[7]);		//0xF8
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 8: 
					//display 8
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[8]);		//0x80
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);
					break;
				case 9: 
					//display 9
					PORTD &= (0 << LATCH);
					shift_out(SEGMENT_MAP[9]);		//0x90
					shift_out(SEGMENT_SELECT[1]);	//0xF2
					PORTD |= (1 << LATCH);	
					break;
			}	
}

int main(){
	USART_init();	//call function to initialize 
	TCCR0A = 0x00;	//normal mode timer
	TCCR0B = 0x05;	//set pre-scaler = 1024
	TCNT0 = 0;		//counter = 0
	TIMSK0 = (1<<TOIE0);	//enable interrupt
	sei();	//enable global
	adc_init();//initialize adc
	shift_out_init();
	char celcius[20];
	
	while (1){	
		//call the display function to output on temperture of the senser 
		display();
			
		USART_tx_string("Celcius degree: ");	//print string
		USART_tx_string(celcius); //print the temperture	
		snprintf(celcius,sizeof(celcius),"%u\r\n",adc_cel); //print formatted output
	}
	return 0;
}