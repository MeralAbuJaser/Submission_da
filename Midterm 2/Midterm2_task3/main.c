/*
 * Midterm2_task3.c
 *
 * Created: 5/9/2020 4:43:06 PM
 * Author : Meral
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>

volatile uint32_t Counter = 0; 
volatile float potentialMeter = 0;//to store Potentiometer value
volatile double temp_rpm = 0;//to store rpm value
volatile uint32_t encoder = 0;//determine which encoder is used either 1x or 2x

volatile double RPM1X = 0;
volatile double RPM2X = 0; 

double period1x = 0;//to store period of rise/fall
double period2x = 0;//to store period of rise/fall

void USART_init(){
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);//8-bit data
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);//Enable RX and TX
}

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

//Send data to the serial port  
void USART_tx_string( char *data ){   
	while ((*data != '\0')){ 
		//while the register is empty enter date   
		while (!(UCSR0A & (1 <<UDRE0)));     
			UDR0 = *data;     
			data++; //increment data location forward  
		}    
}

void read_adc(){
	unsigned char i = 10;
	while(i--){
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		potentialMeter += ADC;
	}
	potentialMeter = potentialMeter/10;//average the adc values
}

void rpm1x(){
	unsigned char i = 10;
	while(i--){
		/*
		divide by 16000000, then we convert to seconds 
		*/
		temp_rpm = (1/((period1x/16000000)*960))*60;
		RPM1X += temp_rpm;//sum up the values
	}
	RPM1X = RPM1X/10;//average the value of rpm by didving by the count of loops
}

void rpm2x(){
	unsigned char i= 10;
	while(i--){
		/*
		we divide by two to get the value of the rising and faling edge
		then the value is converted by diving it by 1 and multiplying by 60
		*/ 
		temp_rpm = (1/(((period2x/2.0)/16000000)*960))*60;
		RPM2X += temp_rpm;//sum up the values 
	}
	RPM2X = RPM2X/10;//average the value of rpm by didving by the count of loops
}

ISR(TIMER1_CAPT_vect){	//Capture ISR
	volatile uint32_t duration = 0;	
	if(encoder == 0){	//calculate the period for 2x
		encoder = 1;	//set encoder to be 1
		duration = ICR1;	//save duration 
		period2x = (duration + (Counter * 65535) + period1x);//calculate total period
		TCNT1 = 0;	//reset timer1 
		Counter = 0;	//reset overflow counter 
		TCCR1B |= (1 << ICES1);	
	}
	if(encoder == 1){	//calculate the period for 1x
		encoder = 0;	//reset the encoder 
		duration = ICR1;	//save duration 
		period1x = (duration + (Counter * 65535));//calculate total period using overflow timer and ticks
		TCNT1 = 0;	//reset timer1 
		Counter = 0;	//reset overflow counter 
		TCCR1B |= (0<<ICES1);
	}
}

ISR(TIMER1_OVF_vect){
		Counter++;//overflow counter
}

void timer(){
	TCCR0A |= (1<<COM0A1)|(0<<COM0A0);//fast PWM, non-inverted
	TCCR0A |= (1<<WGM02)|(1<<WGM01)|(1<<WGM00); 
	TCCR0B |= (1<<CS00);//no prescaling
	TCNT1 = 0;//initialize timer timer1 counter to 0
	TCCR1A = 0;//set the capture to start 
	TCCR1B = (0<<ICNC1)|(1<<ICES1)|(1<<CS10);//start timer
	TCCR1C = 0;
	TIFR1 = (1<<ICF1)|(1<<TOV1);//interrupt enabled
	TIMSK1 = (1<<ICIE1)|(1<<TOIE1);
}

void readPotentialMeter(){
		if ((potentialMeter >= 62260) && (potentialMeter < 65535)){//PWM is 95% which is the limit
			OCR0A = 62260;//max PWM value is 95%
			_delay_ms(1000);
		}
		else if ((potentialMeter < 62257) && (potentialMeter >= 3250)){//PWM less then continue to read ADC value
			OCR0A = potentialMeter;//save potentiometer value in registor 
			_delay_ms(1000);
		}
		else{//if PWM is below 5% reset value
			OCR0A = 0;//reset compare registor
			_delay_ms(100);
		}
}

void display(){
	char value1x[20];//buffer to store 1x rpm
	char value2x[20];//buffer to store 2x rpm
	
	USART_tx_string("\n1x: ");
	sprintf(value1x, "%f", RPM1X);	
	USART_tx_string(value1x);//display 1x value 
	
	USART_tx_string("\n2x: ");
	sprintf(value2x, "%f", RPM2X);
	USART_tx_string(value2x);//display 2x value
		
	USART_tx_string("\n\n");
	_delay_ms(1000);	
}

int main(void){
	OCR0A = 0;//set register to 0 
	//setting the input and output pins according to the wiring 
	DDRD |= (1<<6);
	DDRD |= (1<<5);
	DDRD |= (1<<4);
	DDRC &= ~(1<<0);
	PORTC |= (1<<0);
	DDRB &= ~(1 << DDB0);
	
	adc_init();//intialize adc
	USART_init();//initialize UART
	timer();//initialize PWM
	sei();//enable global interrupts
	
	while (1){
		PORTD |=(1<<DDD5);
		PORTD &=~(1<<DDD4);
		
		read_adc();
		readPotentialMeter();
	
		rpm1x();
		rpm2x(); 
		
		display();
	}
}

