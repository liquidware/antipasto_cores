/*
  wiring_serial.c - serial functions.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
#include "usart.h"
// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define RX_BUFFER_SIZE 64

unsigned char rx_buffer[RX_BUFFER_SIZE];

int rx_buffer_head = 0;
int rx_buffer_tail = 0;

SIGNAL(SIG_USART1_RECV)
{
	unsigned char c = UDR1;

	int i = (rx_buffer_head + 1) % RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rx_buffer_tail) {
		rx_buffer[rx_buffer_head] = c;
		rx_buffer_head = i;
	}
}

void serialBegin(unsigned long baud)
{
	beginSerial(baud);
}

void beginSerial(unsigned long baud)
{
	unsigned long UB;

	UB = F_OSC/(8*baud)-1;

	// set baud rate
	UBRR1H = (unsigned char)(UB>>8);
	UBRR1L = (unsigned char)UB;
	
	// Enable transmitter and receiver
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<< RXCIE1);

 	UCSR1A |= (1<<U2X1);  //double speed

	sei(); //enable interrupts
}

void serialWrite(unsigned char c)
{
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = c;
}

int serialAvailable()
{
	return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
}

int serialRead()
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (rx_buffer_head == rx_buffer_tail) {
		return -1;
	} else {
		unsigned char c = rx_buffer[rx_buffer_tail];
		rx_buffer_tail = (rx_buffer_tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}

void serialFlush()
{
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// were full, not empty.
	rx_buffer_head = rx_buffer_tail;
}

void printMode(int mode)
{
	// do nothing, we only support serial printing, not lcd.
}

void printByte(unsigned char c)
{
	serialWrite(c);
}

void printNewline()
{
	printByte('\n');
}

void printString(const char *s)
{
	while (*s)
		printByte(*s++);
}

void printIntegerInBase(unsigned long n, unsigned long base)
{ 
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) {
		printByte('0');
		return;
	} 

	while (n > 0) {
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
		printByte(buf[i - 1] < 10 ?
			'0' + buf[i - 1] :
			'A' + buf[i - 1] - 10);
}

void printInteger(long n)
{
	if (n < 0) {
		printByte('-');
		n = -n;
	}

	printIntegerInBase(n, 10);
}

void printHex(unsigned long n)
{
	printIntegerInBase(n, 16);
}

void printOctal(unsigned long n)
{
	printIntegerInBase(n, 8);
}

void printBinary(unsigned long n)
{
	printIntegerInBase(n, 2);
}
