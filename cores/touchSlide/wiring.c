/* 

` wiring.c - Partial implementation of the Wiring API for the ATmega2560.
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
	
  **Adapted to the TouchShield and ported to the ATmega2560 - Chris Ladden

*/

#include "wiring_private.h"

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "bitops.h"
#include "touchscreen.h"
#include "adc_sampling.h"

volatile unsigned char millisecond_cnt;
volatile unsigned long millis_var=0;
volatile unsigned char millis_var_counter=0;

typedef enum
{
	RTI_TASK0,
	RTI_TASK1,
	RTI_TASK2,
	RTI_TASK3,
	RTI_TASK4,
	RTI_TASK5,
   RTI_TASK_MAX =40,
} RTI_TASK;

RTI_TASK curr_task = RTI_TASK0;

volatile unsigned char mSecCnt=0;
unsigned long mSec;

////////////////////////////////////////////////
//This is our 128uS time base from timer 0
//
////////////////////////////////////////////////
SIGNAL (SIG_OVERFLOW0) //every 128us
{

   /* Millisecond Counter */
    if (millis_var_counter == MILLISECOND_CNT_MAX)
    	{
    	millis_var++;
    	millis_var_counter=0;
    	}
    else
    	{
    	millis_var_counter++;
    	}
    
    if (millisecond_cnt < MILLISECOND_CNT_MAX)
    	millisecond_cnt++;

   /* RTI Tasks */
   if (curr_task == RTI_TASK0)
   {
	  touchscreen_setup_x();
	  SETBIT(ADCSRA,ADSC); //start ADC conversion!
   }
   else

   if (curr_task == RTI_TASK1)
   {	
	  touchscreen_process_x(ADC);
   }
   else

   if (curr_task == RTI_TASK2)
   {	
	  
	   touchscreen_setup_y();
	  SETBIT(ADCSRA,ADSC); //start ADC conversion!
   }
   else

   if (curr_task == RTI_TASK3)
   {	
	  touchscreen_process_y(ADC);
	  touchscreen_setup_z1();
	  SETBIT(ADCSRA,ADSC); //start ADC conversion!
   }
   else

   if (curr_task == RTI_TASK4)
   {	
	  touchscreen_process_z1(ADC);
	  touchscreen_setup_z2(); 
	  SETBIT(ADCSRA,ADSC); //start ADC conversion!
	   
   }
   else

   if (curr_task == RTI_TASK5)
   {	
	 touchscreen_process_z2(ADC);
	 touchscreen_service();	
   }

   /* Next task */
   curr_task++;
   if (curr_task == RTI_TASK_MAX)
   {
	  /* Reset the Task Number */
	   curr_task = RTI_TASK0;
   }

} //end interrupt

unsigned long millis()
{
	return millis_var;
}

void delay(unsigned long ms)
{

while(ms--)
	{
	millisecond_cnt = 0; //reset the counter
	while(millisecond_cnt != MILLISECOND_CNT_MAX) {;} //wait for 1 mS
	}

}

void arduinoRun()
{

    /* Disable USART transmitter and receiver */
    UCSR0B = (0<<RXEN0) | (0<<TXEN0) | (0<< RXCIE0);
    SETBIT(ARDUINO_RESET_PORT, ARDUINO_RESET_PNUM); //arduino not in reset
    CLRBIT(ARDUINO_RESET_DDR, ARDUINO_RESET_PNUM); //remove drive

}

void arduinoReset()
{

   SETBIT(ARDUINO_RESET_DDR, ARDUINO_RESET_PNUM);
   CLRBIT(ARDUINO_RESET_PORT, ARDUINO_RESET_PNUM); //arduino in reset
}

void init()
{

	//Init I/O  
	//SETBIT(ARDUINO_RESET_PORT, ARDUINO_RESET_PNUM);	 
	
	oled_init();

	dataflash_init();
	touch_init();
	bmp_init();
}

