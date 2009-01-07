/*
  wiring.h - Partial implementation of the Wiring API for the ATmega8.
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

  $Id: wiring.h 254 2007-04-20 23:17:38Z mellis $
*/

#ifndef Wiring_h
#define Wiring_h

#ifdef __cplusplus
extern "C"{
#endif

#include <avr/io.h>
//Standard TouchShield headers
#include "bitops.h"
#include "usart.h"
#include "touchscreen.h"
#include "dataflash.h"
#include "image_interface.h"
#include "graphics.h"
#include "bmp.h"
#include "oled.h"
#include "binary.h"
#include "pindefs.h"
#include "font.h"

#define MILLISECOND_CNT_MAX	7 	//8*128uS = 1.024 mS

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

typedef uint8_t boolean;
typedef uint8_t byte;



void init(void);
void setup(void);
void loop(void);
void delay(unsigned long ms);
unsigned long millis();

//Arduino Functions, not implemented -just to silence errors
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

void beginSerial(unsigned long baud);
void serialWrite(unsigned char c);
int serialAvailable(void);
int serialRead(void);
void serialFlush(void);
void printMode(int);
void printByte(unsigned char c);
void printNewline(void);
void printString(const char *s);
void printInteger(long n);
void printHex(unsigned long n);
void printOctal(unsigned long n);
void printBinary(unsigned long n);
void printIntegerInBase(unsigned long n, unsigned long base);

void delayMicroseconds(unsigned int us);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
