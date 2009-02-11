/*
  wiring_serial.c - serial functions.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis //hacked by Chris Ladden

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
//*******************************************************************************
//*	Jan 28,	2009	<MLS> working on merging Shield and Slide version
//*	Jan 29,	2009	<MLS> wiring_serial.c merge completed, now test it
//*******************************************************************************
#include	"usart.h"
#include	"wiring_private.h"
#include	"HardwareDef.h"

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define RX_BUFFER_SIZE 64

unsigned char rx_buffer[RX_BUFFER_SIZE];

int rx_buffer_head	=	0;
int rx_buffer_tail	=	0;

#ifdef _TOUCH_SLIDE_
//*******************************************************************************
SIGNAL(SIG_USART1_RECV)
{
	unsigned char c	=	UDR1;

	int i	=	(rx_buffer_head + 1) % RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rx_buffer_tail)
	{
		rx_buffer[rx_buffer_head]	=	c;
		rx_buffer_head	=	i;
	}
}
#endif

//*******************************************************************************
void serialBegin(unsigned long baud)
{
#ifdef _TOUCH_SLIDE_
	beginSerial(baud);
#endif
#ifdef _TOUCH_STEALTH_
	//disable hardware UART
	CLRBIT(UCSR0B,RXEN0);   
	CLRBIT(UCSR0B,TXEN0);


	SETBIT(RXTX_PORT, TX_PIN);  //idle state
	SETBIT(RXTX_DDR, TX_PIN);   //output
	CLRBIT(RXTX_DDR, RX_PIN);   //input
	sei();
#endif
}

//*******************************************************************************
void beginSerial(unsigned long baud)
{
#ifdef _TOUCH_SLIDE_
	unsigned long UB;

	UB	=	F_OSC/(8*baud)-1;

	// set baud rate
	UBRR1H	=	(unsigned char)(UB>>8);
	UBRR1L	=	(unsigned char)UB;
	
	// Enable transmitter and receiver
	UCSR1B	=	(1<<RXEN1) | (1<<TXEN1) | (1<< RXCIE1);

 	UCSR1A |= (1<<U2X1);  //double speed

	sei(); //enable interrupts
#endif
}

//*******************************************************************************
void serialWrite(unsigned char c)
{
#ifdef _TOUCH_SLIDE_
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1	=	c;
#endif
#ifdef _TOUCH_STEALTH_
	volatile unsigned int x;
	volatile unsigned char bits=0;

	cli();

	CLRBIT(RXTX_PORT, TX_PIN); 

	for (x=0; x<90; x++)
	{
		;
	}

	for (bits = 0; bits<8; bits++)
	{


		if ( CHECKBIT(c,bits) )
		{
			SETBIT(RXTX_PORT,TX_PIN); //bit is a '1'
		}
		else
		{
			CLRBIT(RXTX_PORT,TX_PIN); //bit is a '0'
		}


		for (x=0; x<90; x++)
		{
			;
		}

	}

	SETBIT(RXTX_PORT, TX_PIN);
	for (x=0; x<90; x++)
	{
		;
	}

	sei(); //enable interrupts
#endif
}

//*******************************************************************************
int serialAvailable()
{
#ifdef _TOUCH_SLIDE_
	return(RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
#endif
#ifdef _TOUCH_STEALTH_
	return 1; //for now
	return(RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
	// return 0; //for now
#endif
}

//*******************************************************************************
int serialRead()
{
#ifdef _TOUCH_SLIDE_
	// if the head isn't ahead of the tail, we don't have any characters
	if (rx_buffer_head == rx_buffer_tail)
	{
		return -1;
	}
	else
	{
		unsigned char c	=	rx_buffer[rx_buffer_tail];
		rx_buffer_tail	=	(rx_buffer_tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
#endif
#ifdef _TOUCH_STEALTH_
	/*
	// if the head isn't ahead of the tail, we don't have any characters
	if (rx_buffer_head == rx_buffer_tail)
	{
		return -1;
	}
	else
	{
		unsigned char c	=	rx_buffer[rx_buffer_tail];
		rx_buffer_tail	=	(rx_buffer_tail + 1) % RX_BUFFER_SIZE;
		return c;
	} */

	volatile unsigned int x;
	volatile unsigned char bits=0;
	volatile unsigned char rx_msg=0;

	cli();

	while (CHECKBIT(RXTX_PIN, RX_PIN) >>  RX_PIN)
	{
		;
	}

	for (x=0; x<45; x++)
	{
		;
	}

//if ( (CHECKBIT(RXTX_PIN, RX_PIN) >>  RX_PIN) != 0) //noise on the line?
//	return 0;

	for (bits = 0; bits<8; bits++)
	{

		for (x=0; x<90; x++)
		{
			;
		}

		rx_msg >>= 1; //shift over one bit
		if (CHECKBIT(RXTX_PIN, RX_PIN))
			rx_msg |= B10000000;	//bit is a '1'
	}

	for (x=0; x<45; x++)
	{
		;
	}

	sei(); //enable interrupts

	return rx_msg;
#endif
}

//*******************************************************************************
void serialFlush()
{
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// were full, not empty.
	rx_buffer_head	=	rx_buffer_tail;
}

//*******************************************************************************
void printMode(int mode)
{
	// do nothing, we only support serial printing, not lcd.
}

//*******************************************************************************
void printByte(unsigned char c)
{
	serialWrite(c);
}

//*******************************************************************************
void printNewline()
{
	printByte('\n');
}

//*******************************************************************************
void printString(const char *s)
{
	while (*s)
		printByte(*s++);
}

//*******************************************************************************
void printIntegerInBase(unsigned long n, unsigned long base)
{ 
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i	=	0;

	if (n == 0)
	{
		printByte('0');
		return;
	}

	while (n > 0)
	{
		buf[i++]	=	n % base;
		n /= base;
	}

	for (; i > 0; i--)
		printByte(buf[i - 1] < 10 ?
				'0' + buf[i - 1] :
				'A' + buf[i - 1] - 10);
}

//*******************************************************************************
void printInteger(long n)
{
	if (n < 0)
	{
		printByte('-');
		n	=	-n;
	}

	printIntegerInBase(n, 10);
}

//*******************************************************************************
void printHex(unsigned long n)
{
	printIntegerInBase(n, 16);
}

//*******************************************************************************
void printOctal(unsigned long n)
{
	printIntegerInBase(n, 8);
}

//*******************************************************************************
void printBinary(unsigned long n)
{
	printIntegerInBase(n, 2);
}
