/*
 * Midterm2_task4.c
 *
 * Created: 5/9/2020 9:36:46 PM
 * Author : Meral
 */ 
#define BAUD 9600
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/setbaud.h>

volatile char size_of_outPut[20];
volatile uint8_t button = 0;		//ON/OFF switch button
volatile float rpm4x;				//stores the speed
volatile uint32_t period4x = 0;		//stores the period of the rise/fall edge
volatile uint16_t Counter1= 0;		//overflow counter
volatile uint16_t Counter2 = 0;		//
volatile uint16_t overflows = 0;
volatile uint16_t capture = 0;	

//Send data to the serial port  
void USART_tx_string( char *data ){   
	while ((*data != '\0')){ 
		//while the register is empty enter date   
		while (!(UCSR0A & (1 <<UDRE0)));     
			UDR0 = *data;     
			data++; //increment data location forward  
		}    
}

void USART_init(){
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);//8-bit data
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);//Enable RX and TX
}

void display(){
	float speed[20];	//buffer to store values
	_delay_ms(500);
	snprintf(speed, sizeof(speed), "4x: %f ", rpm4x);
	USART_tx_string(speed);	//display value
	USART_tx_string("\n");
}

void calculate_speed(){
	//calculate the speed of the motor in rpm
	period4x = (capture + overflows*0x10000L);
	rpm4x = (960000000/(12*period4x))/80; //calculating and converting to rpm
}

void enableCapture() {
	//sets ports directions
	DDRB &= ~(1<<PINB0);		
	TCCR1A = 0x00;			//normal mode
	TCCR1B = 0x41;			//enables capture on rising edge
	TCCR1C = 0x00;
	TIFR1 = (1<<ICF1)|(1<<TOV1);
	TIMSK1 = (1<<ICIE1)|(1<<TOIE1);//capture interrupt and overflow interrupt
}

void timer(){
	TCCR1A = 0x00;			//normal mode timer
	TCCR1B = 0x41;			//rising edge capture 
	TCCR1C = 0x00;
	TIFR1 = (1<<ICF1)|(1<<TOV1);
	TIMSK1 = (1<<ICIE1)|(1<<TOIE1);	//capture interrupt and overflow interrupt
	TCCR3A = 0x00;			
	TCCR3B = 0x45;			//capture on falling edge enable
	TCCR3C = 0x00;
	TIFR3 = (1<<ICF3)|(1<<TOV3);
	TIMSK3 = (1<<ICIE3)|(1<<TOIE3);
}

float read_adc(){
	ADCSRA |= (1<<6);				// Enables ADC conversion
	while (!(ADCSRA&(1<<4)));		// Waits until ADIF is set ADC is done
	ADCSRA |= (1<<4);				// Clears flag
	return ADC;						// Returns value
}
void initialize(){
	DDRC &= ~(1<<PINC0);  //potentiometer input
	ICR1 = 0XFFFF;
	ADMUX = 0x40;		//right justify
	ADCSRA = 0x87;		//128 pre-scaler
	DDRC &= ~(1 << PINC1);			
	PCICR = (1 << PCIE1);	//pin change interrupt enabled 
	PCMSK1 = (1 << PCINT9);	//			
}
int main(void){
	timer();
	USART_init();
	float duty = 0;	//store duty cycle
	initialize();
	sei();			//enable global interrupt
	
	_delay_ms(1000);
	USART_tx_string("Press S1-A1 button to start reading accurate values\n");
	USART_tx_string("use the potentiometer to change the values");
	_delay_ms(2500);
	
	while (1){
		read_adc();
		calculate_speed();
		duty = 100*(read_adc()/1023);		//calculated the potentiometer limit value
		OCR0A = (duty/100)*255;
		display();
	}
}

ISR(TIMER1_CAPT_vect) {	
	capture = ICR1;				//capture ICR3
	TCNT1 = 0;				
	overflows = Counter1;		
	Counter1 = 0;					//reset counter
	TIFR1 |= (1<<ICF1);				//clears flag
}

ISR(TIMER3_CAPT_vect) {	
	capture = ICR3;				//capture ICR3
	TCNT3 = 0;						
	overflows = Counter2;	
	Counter2 = 0;				//reset counter
	TIFR3 |= (1<<ICF3);				//clear flag
}

ISR(TIMER1_OVF_vect) {
	Counter1++;					
}

ISR(TIMER3_OVF_vect) {
	Counter2++;								
}

ISR(PCINT1_vect){
	if (PINC & (1<<PINC1)){
		button = ~button;			
			//clockwise direction 
			if (button){	
				DDRD |= (1<<PIND6);		
				PORTD|=(1<<DDD5);		
				PORTD&=~(1<<DDD4);		
				TCCR0A = 0x83;			
				TCCR0B = 0x02;			
			}
		//change direction of the motor to counterclockwise
		if(!button){
			DDRD |= (1<<PIND6);		
			PORTD&=~(1<<DDD5);		
			PORTD|=(1<<DDD4);		
			TCCR0A = 0x83;			
			TCCR0B = 0x02;			
		}
	}
	PCIFR = (1<<PCIF1);	 //clears flag
}
