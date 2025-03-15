// Code that was used in the atmel studio
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h> // used for transmission of temperature to the virtual terminal
// //the purpose of this delay function is to create a delay for sending data to UART
#define F_CPU 1000000
#include <util/delay.h>
unsigned long int temp=0, current_index=0,temperature_ten=0, temperature_integer = 0,temperature = 0,temperature_recevied=0,temperature_recevied2=0;
unsigned int max=0, min=0;
 unsigned char result_high=0, result_low=0;
 unsigned long int result[100]={0};
volatile unsigned char seven_seg[] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};

void UART_initi(){
	
	UBRRL=0xC; // baud rate is set to be 4800
	UCSRB=(1<<TXEN)|(1<<RXEN);
	UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1) ; // s e t data s i z e
}
void UART_transmit_string(char* str) {
	while (*str != 0) {
		while (!(UCSRA & (1<<UDRE)));
		UDR = *str;
		str++;
	}
}
void UART_receive() {
	unsigned long int data;
	if(UCSRA & (1<<RXC)) {
		data = UDR;
		
		temperature_recevied = data / 10;
		temperature_recevied2 = data % 10;
		// Display the received character on the 7-segment display
		PORTB = seven_seg[temperature_recevied];
		PORTA = (seven_seg[temperature_recevied2]<<1);
	}
}

int main(void) {
	UART_initi();
	// Configure ports
	DDRB = 0xFF; // Set PORTB as output for 7-segment display
	//DDRC = 0xFF; // Set PORTC as output for 7-segment display
	DDRA = 0xFE; // Set PORTA as input for LM35 sensor

	// Configure ADC
	ADMUX |= (1 << REFS0) | (1 << REFS1); // Use AREF as reference, right adjust, and select ADC0 as input
	ADCSRA = 0b10000111; // Enable ADC, ADC interrupt, and set prescaler to 128
	// Enable global interrupts
	sei();
	// Configure Timer1
	TCNT1 = 49911; // 65536 - (1s / (1/1000000)s) / 64
	TCCR1B = (1 << CS11); // Prescaler 64
	TIMSK = (1 << TOIE1); // Enable overflow interrupt

	while (1) {
		
		//PORTB = seven_seg[temperature_ten];
		//PORTA = (seven_seg[temperature_integer]<<1);
		char buffer[10];
		sprintf(buffer, "%lu\r\n", temperature);
		UART_transmit_string(buffer);
		_delay_ms(1000);// so that readings are easy to read and there is no load on PCB
		UART_receive();
	}
	return 0;
}

// ISR for Timer1 overflow interrupt
ISR(TIMER1_OVF_vect) {
	ADCSRA |= (1 << ADSC); // Start ADC conversion
	result_low = ADCL;
	result_high = ADCH & 0b00000011;
	//complete digital value
	result[current_index] = result_low + (256 * result_high);
	// for max temp
	if(result[current_index]>max){
		max=result[current_index];
	}
	// for minimum and max temp first give max and minimum the current value to get compared
	if(current_index==0){
		min=result[current_index];
		max=result[current_index];
	}
	if(min>result[current_index]){
		min=result[current_index];
	}
	// nominal value
	if(current_index<3){
		temp=result[current_index];
	}
	else{
		temp=(result[current_index]+result[current_index-1]+result[current_index-2]+result[current_index-3])/4;
	}
	// by replacing temp with min and max we can see the min and max on 7-segment respectively
	temperature = ((temp * 2560) / 1023) / 10;
	//temperature_ten = temperature / 10;
	//temperature_integer = temperature % 10;
	// Display temperature on 7-segment display
	current_index++;
	if(current_index==100){
		current_index=0;
	}
	TIFR |= (1 << TOV1); // Clear Timer1 overflow flag
}
