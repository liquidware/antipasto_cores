/*==============================================================================
 * PREPROCESSOR DIRECTIVES
 *============================================================================*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "graphics.h"
#include "oled.h"
#include "bitops.h"
#include "font.h"

#define incx() x++, dxt += d2xt, t += dxt
#define incy() y--, dyt += d2yt, t += dyt

#define DUFF_DEVICE_8(aCount, aAction) \
do { \
    int32_t count_ = (aCount); \
    int32_t times_ = (count_ + 15) >> 4; \
    switch (count_ & 7){ \
        case 0: do { aAction; \
        case 15: aAction; \
        case 14: aAction; \
        case 13: aAction; \
        case 12: aAction; \
        case 11: aAction; \
        case 10: aAction; \
        case 9: aAction; \
        case 8: aAction; \
        case 7: aAction; \
        case 6: aAction; \
        case 5: aAction; \
        case 4: aAction; \
        case 3: aAction; \
        case 2: aAction; \
        case 1: aAction; \
    } while (--times_ > 0); \
    } \
} while (0)

/*==============================================================================
 * TYPES
 *============================================================================*/

 /*==============================================================================
 * GLOBAL CONSTANTS AND VARIABLES
 *============================================================================*/

/* CONSTANTS */
/* None */

/* INPUT VARIABLES */
COLOR   GraphicsColor;

/* OUTPUT VARIABLES */
/* None */

/*==============================================================================
 * LOCAL CONSTANTS, VARIABLES AND FUNCTION PROTOTYPES
 *============================================================================*/

/* CONSTANTS */
/* None */

/* VARIABLES */
/* None */

/* LOCAL FUNCTION PROTOTYPES */
/* None */

/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/


/* ===========================================================================
*  FUNCTION: 
*
*  DESIGN DESCRIPTION:
*
*
*  PARAMETER LIST:
*     
*
*  RETURNED:
*     
*
*  DESIGN OUTLINE:
*
*  DESIGN NOTES/CONSTRAINTS:
*     None.
*
*=========================================================================== */

/*==============================================================================
 * GLOBAL FUNCTIONS
 *============================================================================*/

void dispCommand(unsigned char command)
{

volatile unsigned char c = command;

CLRBIT(OLED_CTRL_PORT,OLED_DC);
CLRBIT(OLED_CTRL_PORT,OLED_CS);
SETBIT(OLED_CTRL_PORT,OLED_RD);
CLRBIT(OLED_CTRL_PORT,OLED_WR);

OLED_DATA_HIGH = 0;
OLED_DATA_MED = (c >> 5) << 2;
OLED_DATA_LOW = c << 3;

SETBIT(OLED_CTRL_PORT,OLED_WR);
SETBIT(OLED_CTRL_PORT,OLED_CS);
SETBIT(OLED_CTRL_PORT,OLED_DC);

}

void dispData(unsigned int data)
{

    volatile unsigned char lB = (unsigned char)data;
    volatile unsigned char hB = (unsigned char)(data>>8);
    
    SETBIT(OLED_CTRL_PORT,OLED_DC);
    CLRBIT(OLED_CTRL_PORT,OLED_CS);
    // SETBIT(OLED_CTRL_PORT,OLED_RD);
    CLRBIT(OLED_CTRL_PORT,OLED_WR);
    
    OLED_DATA_LOW = (lB << 3);
    OLED_DATA_MED = (lB >> 3);
    OLED_DATA_MED = OLED_DATA_MED | (hB << 6);
    OLED_DATA_HIGH = hB;
    
    
    SETBIT(OLED_CTRL_PORT,OLED_WR);
    SETBIT(OLED_CTRL_PORT,OLED_CS);
    // SETBIT(OLED_CTRL_PORT,OLED_DC);
}

void dispColor(COLOR c)
{
    GraphicsColor.red = c.red;
    GraphicsColor.green = c.green;
    GraphicsColor.blue = c.blue;
}


void dispPix()
{
	CLRBIT(OLED_CTRL_PORT,OLED_CS);
	SETBIT(OLED_CTRL_PORT,OLED_DC);
	SETBIT(OLED_CTRL_PORT,OLED_RD);
	CLRBIT(OLED_CTRL_PORT,OLED_WR);

	OLED_DATA_LOW = GraphicsColor.blue;
	OLED_DATA_MED = GraphicsColor.green;
	OLED_DATA_HIGH =GraphicsColor.red;

	SETBIT(OLED_CTRL_PORT,OLED_WR);
	SETBIT(OLED_CTRL_PORT,OLED_CS);
}

void dispSetWindow(int x, int y, int width, int height)
{
    uint16_t temp;

    /* Error checking */
    if ( (width<=0) || (height<=0) ) {
        return;
    }

    /* Bounds clipping */
    if (x < 0 ){ width += x-0; x = 0;  }
    if (y < 0 ){ height += y-0; y = 0; }
    if (x >= 320 ){ x = 319; }
    if (y >= 240 ){ y = 239; }
    if (x+width  >= 320){ width = 319 - x; }
    if (y+height >= 240){ height = 239 - y; }


	/* Specify the Vertical start positions */              
	dispCommand(0x35); 
	dispData(x); 
	
    /* Specify the Vertical end positions */
    temp = (x+width);		
	dispCommand(0x36);                           
	dispData(temp); 
	
    /* Specify the horizontal start/end positions */
	dispCommand(0x37);                       
	dispData( ((y)<<8)|(y+height) ); 	
    	
    /* Specify the x address in RAM */			
    dispCommand(0x20);                       
	dispData(y); 
	
    /* Specify the y address in RAM */		    
	dispCommand(0x21);                       
	dispData(x);

    /* RAM write */
	dispCommand(0x22);                       				

}

void dispPixel(int x, int y)
{
	
/* Bounds clipping */	
	if ( (x>=320) || (y>=240)) { return; }
	if ( (x<0) || (y<0)) { return; }

/* Set XY location   */
    dispCommand(0x20); //Specify the x address in RAM
	dispData(0x00FF&y);
    		    
	dispCommand(0x21); //Specify the y address in RAM
    dispData(0x01FF & x);

	dispCommand(0x22); //RAM write	

/* Draw pixel */
	CLRBIT(OLED_CTRL_PORT,OLED_CS);
	CLRBIT(OLED_CTRL_PORT,OLED_WR);

	OLED_DATA_LOW = GraphicsColor.blue;
	OLED_DATA_MED = GraphicsColor.green;
	OLED_DATA_HIGH= GraphicsColor.red;

	SETBIT(OLED_CTRL_PORT,OLED_WR);
	SETBIT(OLED_CTRL_PORT,OLED_CS);

}

void dispRead(COLOR *buffer, uint16_t  x, uint16_t  y)
{

    /* Set XY location   */
    dispCommand(0x20); //Specify the x address in RAM
	dispData(0x00FF&y);
    		    
	dispCommand(0x21); //Specify the y address in RAM
    dispData(0x01FF&x);

    /* RAM write */
	dispCommand(0x22); 

    /* Input Direction */
    OLED_DATA_LOW_DDR = 0x00;
    OLED_DATA_MED_DDR = 0x00;
    OLED_DATA_HIGH_DDR = 0x00;

    /* Read pixel */
    SETBIT(OLED_CTRL_PORT,OLED_WR);
    CLRBIT(OLED_CTRL_PORT,OLED_CS);
    CLRBIT(OLED_CTRL_PORT,OLED_RD);       
    
    CLRBIT(OLED_CTRL_PORT,OLED_RD);
    CLRBIT(OLED_CTRL_PORT,OLED_RD);

    buffer[0].blue = OLED_DATA_LOW_PIN;
    buffer[0].green = OLED_DATA_MED_PIN;
	buffer[0].red = OLED_DATA_HIGH_PIN;
    
    SETBIT(OLED_CTRL_PORT,OLED_RD);
	SETBIT(OLED_CTRL_PORT,OLED_CS);

    /* Output direction */
    OLED_DATA_LOW_DDR = 0xFF;
    OLED_DATA_MED_DDR = 0xFF;
    OLED_DATA_HIGH_DDR = 0xFF;
}

void dispRectangle( int  x,  int  y,  int width,  int  height) 
{
    int32_t len = (width*height); 


    if (x>=320) return;
    if (y>=240) return;
    if ( (x+width) >= 320) { width = 319-x; }
    if ( (y+height) >= 240) { height = 239-y; }
	
   /* Set XY location   */
    dispSetWindow(x,y,width,height);

   /* Clear the screen */
   CLRBIT(OLED_CTRL_PORT,OLED_CS);

   /* Set the color once */
   OLED_DATA_LOW = GraphicsColor.blue;
   OLED_DATA_MED = GraphicsColor.green;
   OLED_DATA_HIGH =GraphicsColor.red;

       DUFF_DEVICE_8(len,
                  asm("ldi r24,0x20       ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
                      :
                      :
                      : "r24" ););

   /* SET the screen CS */
   SETBIT(OLED_CTRL_PORT,OLED_CS);
}

void dispClearScreen()
{
   unsigned int i=2400;
  
   /* Set XY location   */
   dispCommand(0x20); //Specify the x address in RAM
    dispData(0);
               
    dispCommand(0x21); //Specify the y address in RAM
   dispData(0);

    dispCommand(0x22); //RAM write


   /* Clear the screen */
   CLRBIT(OLED_CTRL_PORT,OLED_CS);

   /* Set the color once */
    OLED_DATA_LOW = GraphicsColor.blue;
    OLED_DATA_MED = GraphicsColor.green;
    OLED_DATA_HIGH =GraphicsColor.red;
  
   /* Start the clocking of the WR pin */

   while(i--)
   {
   /* Set & Clear */
    asm("ldi r24,0x20       ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"

        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"


        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"

        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        "sts 0x0109,r24    ""\n\t" "sts 0x0109,r24    ""\n\t"
        :
        :
        : "r24"
    );

   }
}
