//*******************************************************************************
//*		
//*	
//*	
//*******************************************************************************
//*	Detailed Edit History
//*		PLEASE put comments here every time you work on this code
//*******************************************************************************
//*	Jan  2,	2009	<MLS> Started working on converging touchStealth with touchSlide cores
//*	Jan  4,	2009	<MLS> Changed touchShield touchscreen_process_x args to int to be compatible with slide
//*******************************************************************************

#include	<avr/io.h>
#include	<inttypes.h>
#include	<stdlib.h>
#include	<inttypes.h>
#include	<avr/interrupt.h>
#include	<avr/pgmspace.h>
#include	<avr/eeprom.h>

#include	"touchscreen.h"
#include	"graphics.h"
#include	"bitops.h"
#include	"calibrate.h"
#include	"usart.h"
#include	"font.h"

#define SAMPLE_CNT_MAX	1

#define NO	0
#define YES 1

#define PRESSURE_TOUCHED	20

volatile unsigned char touching	=	NO;
volatile unsigned char touched	=	NO;
volatile unsigned int x_loc=0;
volatile unsigned int y_loc=0;
volatile unsigned int z1_loc=0;
volatile unsigned int z2_loc=0;
volatile unsigned int x_loc_touched=0;
volatile unsigned int y_loc_touched=0;

unsigned int x_samples=0;
unsigned int y_samples=0;
unsigned int z1_samples=0;
unsigned int z2_samples=0;
unsigned char sample_count=0;
float pressure=0;

MATRIX	matrix;

#define ee_isCalibrated 28
#define ee_matrix 0
//EEMEM unsigned char ee_isCalibrated;
//EEMEM MATRIX ee_matrix;

#define PT1_X 32
#define PT1_Y 24
#define PT2_X 160
#define PT2_Y 216
#define PT3_X 288
#define PT3_Y 120

#ifdef __cplusplus
extern "C"{
#endif



/*
void touchscreen_check_cal()
{
	if (sample_count > 10000)//40,000 samples
	{
		eeprom_write_byte((unsigned char*)ee_isCalibrated, 23);
		//eeprom_write_byte(50, 23);

		//lcd_clear();
		COLOR c		=	{255,0,0};
		COLOR blk	=	{0,0,0};
		lcd_clearScreen(blk);
		char msg[40];
		strncpy_P(msg,PSTR("Touchscreen Reset."),40);
		lcd_puts(msg,0,50,c);

		strncpy_P(msg,PSTR("Please powercycle"),40);
		lcd_puts(msg,0,60,c);
		while (1)
		{
			;
		}					
	}
}
*/

//*******************************************************************************
void touchscreen_process_x(unsigned int adc_value)
{

	/* Store Value */
	x_samples	=	1023 - adc_value;

	/* Remove Excitation */
	DDRF	=	0x00; 

	/* Discharge Signal */
	PORTF	=	0x00;	//Ground
	DDRF	=	0xFF;	//Output
	DDRF	=	0x00;	//Floating
}


//*******************************************************************************
void touchscreen_process_y(unsigned int adc_value)
{
 
	/* Store Value */
	y_samples	=	1023 - adc_value;

	/* Remove Excitation */
	DDRF	=	0x00; 

	/* Discharge Signal */
	PORTF	=	0x00;	//Ground
	DDRF	=	0xFF;	//Output
	DDRF	=	0x00;	//Floating
}

//*******************************************************************************
void touchscreen_process_z1(unsigned int adc_value)
{
 
	/* Store Value */
	z1_samples	=	adc_value;

	/* Remove Excitation */
	DDRF	=	0x00;

	/* Discharge Signal */
	PORTF	=	0x00;	//Ground
	DDRF	=	0xFF;	//Output
	DDRF	=	0x00;	//Floating
}

//*******************************************************************************
void touchscreen_process_z2(unsigned int adc_value)
{
 
	/* Store Value */
	z2_samples	=	adc_value;

	/* Remove Excitation */
	DDRF	=	0x00;

	/* Discharge Signal */
	PORTF	=	0x00;	//Ground
	DDRF	=	0xFF;	//Output
	DDRF	=	0x00;	//Floating
}

//*******************************************************************************
void touchscreen_service()
{
	unsigned int pressure;

	z1_loc	=	z1_samples;
	z2_loc	=	z2_samples;
	
	if (z1_loc != 0)
	{
		pressure	=	z2_loc / z1_loc;
	}
	else
	{
		pressure	=	PRESSURE_TOUCHED;
	}

	/* Test pressure */
	if (pressure < PRESSURE_TOUCHED)
	{
		touching	=	YES;

		x_loc	=	y_samples;
		y_loc	=	x_samples; 
	
	}
	else
	{
		/* Check for previously touching */ 
		if (touching == YES)
		{
			x_loc_touched	=	x_loc; 
			y_loc_touched	=	y_loc;
			touched	=	YES;
		}

		/* Reset touch data */
		touching	=	NO;
		x_loc	=	0;
		y_loc	=	0;
	}
}

//*******************************************************************************
void touch_init()	// initialize the touch algorithm
{

	ADCSRA	=	(1<<ADEN) | (0<<ADIE) | 
			(0<<ADPS2) | (1<<ADPS1) |(1<<ADPS0); //clock freq fosc

	TCCR0B	=	(0<<CS02) |(1<<CS01) | (0<<CS00); //timer 0 setup to overflow every 128us
	TIMSK0	=	(1<<TOIE0);

	sei(); //enable global interupts

	if (eeprom_read_byte((unsigned char*)ee_isCalibrated) != TOUCHSCR_IS_CALIBRATED)// check to see if the interupt has been previously calibrated
	{
		touch_calibrate(); //calibrate the touch screen
	}
	else
	{
		eeprom_read_block (&matrix,(unsigned char*) ee_matrix, sizeof(MATRIX)); // read the previous saved calibration values in from memory
	}
}


//*******************************************************************************
void touchscreen_setup_x()
{

	/* Setup MUX */
	ADMUX	=	YPLUS;	

	/* Setup Excitation */
	CLRBIT(PORTF,YPLUS_PIN); CLRBIT(PORTF,YMINUS_PIN);	//Y 
	SETBIT(PORTF,XPLUS_PIN); CLRBIT(PORTF,XMINUS_PIN);	//X

	/* Drive Excitation */
	CLRBIT(DDRF,YPLUS_PIN); CLRBIT(DDRF,YMINUS_PIN);	//Y 
	SETBIT(DDRF,XPLUS_PIN); SETBIT(DDRF,XMINUS_PIN);	//X drive
	
} //end setup_y


//*******************************************************************************
void touchscreen_setup_y()
{

	/* Setup MUX */
	ADMUX	=	XPLUS;	

	/* Setup Excitation */
	SETBIT(PORTF,YPLUS_PIN); CLRBIT(PORTF,YMINUS_PIN);	//Y
	CLRBIT(PORTF,XPLUS_PIN); CLRBIT(PORTF,XMINUS_PIN);	//X

	/* Drive Excitation */
	SETBIT(DDRF,YPLUS_PIN); SETBIT(DDRF,YMINUS_PIN);	//Y drive
	CLRBIT(DDRF,XPLUS_PIN); CLRBIT(DDRF,XMINUS_PIN);	//X 
}//end setup_x

//*******************************************************************************
void touchscreen_setup_z1()
{

	/* Setup MUX */
	ADMUX	=	XPLUS;	

	/* Setup Excitation */
	SETBIT(PORTF,YPLUS_PIN); CLRBIT(PORTF,YMINUS_PIN);	//Y
	CLRBIT(PORTF,XPLUS_PIN); CLRBIT(PORTF,XMINUS_PIN);	//X

	/* Drive Excitation */
	SETBIT(DDRF,YPLUS_PIN); CLRBIT(DDRF,YMINUS_PIN);	//Y
	CLRBIT(DDRF,XPLUS_PIN); SETBIT(DDRF,XMINUS_PIN);	//X

}

//*******************************************************************************
void touchscreen_setup_z2()
{

	/* Setup MUX */
	ADMUX	=	YMINUS;	

	/* Setup Excitation */
	SETBIT(PORTF,YPLUS_PIN); CLRBIT(PORTF,YMINUS_PIN);	//Y
	CLRBIT(PORTF,XPLUS_PIN); CLRBIT(PORTF,XMINUS_PIN);	//X

	/* Drive Excitation */
	SETBIT(DDRF,YPLUS_PIN); CLRBIT(DDRF,YMINUS_PIN);	//Y
	CLRBIT(DDRF,XPLUS_PIN); SETBIT(DDRF,XMINUS_PIN);	//X

}

/****************************************************/
/*													*/
/* Touch Calibrate									*/
/*													*/
/****************************************************/

/* calibrating the touch screen compensates for 4 major issues 
		relating a touch location to a pixel location.
		1)	An offset in the x-aligenment
		2)	An offset in the y-aligenment
		3)	A rotational offset between the rows of pixels and the touch screen
		4)	Compensates the offset of each press location on the touch screen to a pixel location on the LCD screen
*/
//*******************************************************************************
char touch_calibrate(void)
{
COLOR blk	=	{0,0,0};
COLOR c1	=	{ 255, 0, 0}; //circle COLOR
POINT p;

	//array of input points
	POINT32	screenSample[3];

	//array of expected correct answers
	POINT32	displaySample[3] =	{
								{PT1_X,PT1_Y},
								{PT2_X,PT2_Y},
								{PT3_X,PT3_Y}
									};


	/* An array of perfect input screen points used to obtain a first pass	*/
	/*	calibration matrix good enough to collect calibration samples.		*/
	POINT32 perfectScreenSample[3] =	{
											{ 100, 100 },
											{ 900, 500 },
											{ 500, 900 }
										} ;



	/* An array of perfect display points used to obtain a first pass		*/
	/*	calibration matrix good enough to collect calibration samples.		*/
	POINT32 perfectDisplaySample[3]	=	{
											{ 100, 100 },
											{ 900, 500 },
											{ 500, 900 }
										} ;


	//initial pass
	setCalibrationMatrix( &perfectDisplaySample[0], 
								&perfectScreenSample[0], 
								&matrix ) ;


	dispPutS("Touch circle to calibrate",30,5,c1,blk);

	touched	=	NO;
	dispColor(c1);
	dispRectangle(PT1_X,PT1_Y,6,6); //draw


	while(!touch_getTouched(&p)) { ; }

	screenSample[0].x	=	p.x; 
	screenSample[0].y	=	p.y;

	dispColor(blk);
	dispRectangle(PT1_X,PT1_Y,6,6); //erase
	delay_ms(250);

	dispColor(c1);
	dispRectangle(PT2_X,PT2_Y,6,6); //draw

	while(!touch_getTouched(&p)) { ; }

	screenSample[1].x	=	p.x; 
	screenSample[1].y	=	p.y;

	dispColor(blk);
	dispRectangle(PT2_X,PT2_Y,6,6); //erase
	delay_ms(250);

	dispColor(c1);
	dispRectangle(PT3_X,PT3_Y,6,6); //draw

	while(!touch_getTouched(&p)) { ; }

	screenSample[2].x	=	p.x; 
	screenSample[2].y	=	p.y;

	dispColor(blk);
	dispRectangle(PT3_X,PT3_Y,6,6); //erase
	delay_ms(250);

	/* Clean up */
	dispColor(blk);
	dispClearScreen();
	dispPutS("Calibrating the",15,15,c1, blk);
	dispPutS("Touch Screen",20,25,c1,blk);

	//now, the real calibration pass
	setCalibrationMatrix( &displaySample[0], &screenSample[0], &matrix ) ;

	eeprom_write_byte((unsigned char*)ee_isCalibrated, TOUCHSCR_IS_CALIBRATED);
	eeprom_write_block(&matrix, (unsigned char*)ee_matrix, sizeof(MATRIX)); //MATRIX	=	28 bytes
						
						delay_ms(250);
						delay_ms(250);
						delay_ms(250);

	return 0;
}

/****************************************************/
/*													*/
/* touch_getTouched									*/
/*													*/
/****************************************************/
char touch_getTouched(POINT * p)
{

	if (touched)
	{
		POINT32 my_point, calibrated_point;
		my_point.x	=	(long)x_loc_touched;
		my_point.y	=	(long)y_loc_touched;

		/* Calibrate the coordinates */
		getDisplayPoint( &calibrated_point, &my_point, &matrix ) ;

		/* Store the values */
		(p->x)	=	(unsigned int)calibrated_point.x;
		(p->y)	=	(unsigned int)calibrated_point.y;
	
		touched	=	FALSE;	

		return 1;
	}

	return 0;

}

/****************************************************/
/*													*/
/* touch_getCursor									*/
/*													*/
/****************************************************/
char touch_getCursor(POINT * p)
{
	
	if (touching)
	{
		POINT32 my_point, calibrated_point;
		my_point.x	=	(long)x_loc;
		my_point.y	=	(long)y_loc;

		/* Calibrate the coordinates */
		getDisplayPoint( &calibrated_point, &my_point, &matrix ) ;

		/* Store the values */
		(p->x)	=	(unsigned int)calibrated_point.x;
		(p->y)	=	(unsigned int)calibrated_point.y;
			
		return 1;
	}

	return 0;

}//end touch_get_cursor


/****************************************************/
/*													*/
/* Delay milisecond									*/
/*													*/
/****************************************************/
//this is a really random place to put this function but we needed it for touch calibrate

//*******************************************************************************
void delay_ms(unsigned char x)
{
unsigned char w,y,g;

	for (w=0; w<x ; w++)
	{
		for (y=0; y<13; y++)
		{
			for (g=0; g<255; g++)
			{
			asm("nop");
			}
		}
	}

}

#ifdef __cplusplus
} // extern "C"
#endif
