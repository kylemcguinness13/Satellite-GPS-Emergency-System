/*
 * EmbeddedSystemsProject.c
 *
 * Created: 5/3/2023 10:34:13 PM
 * Author : kmcguinness, itzenhuiser
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>

#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1



void USART_Init (unsigned int ubrr){
	// set baud rate
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1<<RXEN0) | (1<< TXEN0);
	// set frame format : 8data, 2stop bit
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
}
void USART_Transmit(unsigned char data){
	//wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)))
	;
	// put data into buffer, sends the data
	UDR0 = data;
}
char USART_Receive(void)
{
	//Wait for data to be received
	while (!(UCSR0A & (1<<RXC0)))
	;
	//get and return received data from buffer
	return UDR0;
}
// make a function that you can pass in a string and it transmits them individually
void transmitString(char str[]){
	for(int i = 0; i < strlen(str); i++){
		USART_Transmit(str[i]);
	}
}
void readString(char *buf){ //reads in a string from the serial monitor
	//create an array with all values we expect to see
	for (int i = 0; i < 128; i++){ //check to see if it exists in the
		char temp = USART_Receive();
		buf[i] = temp;
	}
}
int main(void) {
	/* Setup */
	DDRC &= ~(1 << DDC0); //set PC0 to input
	USART_Init(MYUBRR);//initialize the USART connection
	while (1)//infinate loop
	{
		char x[128];//initialize character array to store the GPS output
		readString(x);//collect the GPS output
		if (!(PINC & (1 << PINC0)))//check to see if push button is pressed
		{
			transmitString(x);//send the GPS output to the HC-06 to transmit to the reciever 
		}
		
	}
}
//"$GPRMC,203549.000,A,4139.5959,N,09132.2179,W,0.43,232.32,030523,,,A*7C" this is a sample of the GPS output


