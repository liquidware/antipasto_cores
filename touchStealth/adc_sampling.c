#ifdef __cplusplus
extern "C"{
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "bitops.h"
#include "touchscreen.h"
#include "adc_sampling.h"
#include "usart.h"

#if 0 //duplicated code, so removing 
 
volatile unsigned char adcCurrChan=0;
volatile unsigned char adc_delay=0;

////////////////////////////////////////////////
//This is our 128uS time base from timer 0
//
////////////////////////////////////////////////
SIGNAL (SIG_OVERFLOW0) //every 128us
{


if (adc_delay == ADC_DELAY_MAX)
	{
	
	adc_delay = 0; //reset the counter
	adcCurrChan++; //next channel
	if (adcCurrChan == ADC_TOTAL_CHANNELS) 
		adcCurrChan = 0; //loop back around to the first channel

	switch(adcCurrChan)
		{

		case ADC_CHAN0:
		//Touchscreen: XPLUS
		touchscreen_setup_x();
		SETBIT(ADCSRA,ADSC); //start conversion!
		break;



		case ADC_CHAN1:
		//Touchscreen: YMINUS
		touchscreen_setup_y();
		SETBIT(ADCSRA,ADSC); //start conversion!
		break;

		}//end switch





	}
else
	{
	adc_delay++; //increment the counter
	}

} //end interrupt






////////////////////////////////////////////////
//ADC sample complete!
/////////////////////////////////////////////////
SIGNAL (SIG_ADC) 
{

	switch(adcCurrChan)
		{

		case ADC_CHAN0:
		touchscreen_process_x(ADCH); 
		break;

		case ADC_CHAN1:
		touchscreen_process_y(ADCH);
		break;


		}

}

#endif

#ifdef __cplusplus
} // extern "C"
#endif




