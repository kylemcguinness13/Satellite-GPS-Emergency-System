/*
 * FinalTesting.c
 *
 * Created: 5/2/2023 1:34:07 PM
 * Author : itzenhuiser, mcguinness
 */ 

#include <avr/io.h>
#include "lcd.h"
#include "defines.h"

//import the needed libraries 
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdbool.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1




int uart_putchar(char c, FILE *s){

	if (c == '\n')   
		uart_putchar('\r', s);

    while (!(UCSR0A & (1 << UDRE0)));   /* Wait for empty transmit buffer */
	
	UDR0 = c;     /* Put data into buffer, sends the data */
	
	return 0;

}


int uart_getchar(FILE *s) {

	while(!(UCSR0A & (1<< RXC0))); /* Wait for data to be received */

	uint8_t c = UDR0;  /* Get and return received data from buffer */

	return c;

}

static FILE lcd_io = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

const int ledPin = 0;

void USART_init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* Set frame format: 8data, 1stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

unsigned char USART_receive(void)
{
    /* Wait for data to be received */
    while (!(UCSR0A & (1 << RXC0)));

    /* Get and return received data from buffer */
    return UDR0;
}

void readString(char *buf){ //reads in a string from the serial monitor
		for (int i = 0; i < 128; i++){ //check to see if it exists in the
			char temp = USART_receive(); //recieve the next Value
			buf[i] = temp;
		}
}
int main(void)
{
	DDRC |= (1 << PC0); // configure led as output

	USART_init(MYUBRR);//initialize the USART connection

	
	lcd_init();//initialzie the LCD library
	UCSR0A = 1 << U2X0; 
	UBRR0L = (F_CPU / (8UL * UART_BAUD))-1;//set baud
	
	UCSR0B = 1 << TXEN0 | 1 << RXEN0;
	stdout = &lcd_io;// initializes stdout to allow use of printf
	stdin = &uart_io; 
	
	printf("  Waiting For \n    Request   \n");//set LCD to display a waiting screen
    while (1) //inifnate loop
    {	
		char coord[128];
		readString(coord);//read in the transmit data
		char* token = strtok(coord,",");//parse input data by commas 
		bool found = false;
		char n = 'N';
		char w = 'W';
		while ((token != NULL) && (!found)) {//if there is GPS data
			if(!(strncmp(token,"$GPRMC",6))){//check to see if the needed data is found
				token = strtok(NULL, ",");//skip 3 lines
				token = strtok(NULL, ",");
				token = strtok(NULL, ",");
				printf("%s\n", strncat(token,&n,1));//send the latitude to the LCD screen and get a new line
				token = strtok(NULL, ",");
				token = strtok(NULL, ",");//skip two lines
				printf("%s\n", strncat(token,&w,1));//send the longitude to the LCD screen
				found = true;//stop parsing data
				PORTC |= (1 <<  PC0);//turn led and buzzer on
				_delay_ms(10000);//wait ten seconds
				PORTC &= ~(1 << PC0);//stop buzzer and turn off led
			}
			else{
				token = strtok(NULL, ",");//get the next token
			}
		}
    }
}