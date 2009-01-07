/*==============================================================================
 *
 *                             Sub-Processing 
                          for TouchShield library
 *         by inthebitz @ antipastohw.blogspot.com & liquidware.com
 *
 ==============================================================================
 * History:
 *
 * Version 0.2 | Dec 4, 2008  | Chris @ antipastohw.blogspot.com & liquidware.com
 * Merged into the TouchShield core
 *
 * Version 0.1 | Nov 30, 2008 | inthebitz @ antipastohw.blogspot.com & liquidware.com
 * This is the beginnings of the "Sub-Processing" for TouchShield library
 * 
 *
 * DESCRIPTION:
 * Sub-Processing provides a rich graphics and interaction library 
 * for the TouchShield.
 * 
 *============================================================================*/ 

/*==============================================================================
 * PREPROCESSOR DIRECTIVES
 *============================================================================*/
#ifndef SUBPGRAPHICS_H /* Idempotence guard */
#define SUBPGRAPHICS_H

#ifdef __cplusplus

#include "lcd.h"

/*==============================================================================
* TYPES
*============================================================================*/
typedef enum
{
    SETFCOLOR       = 1,
    SETBCOLOR       = 2,
    FILLBACK        = 3,
    SET_BRIGHTNESS  = 4,
    FADEOUT         = 5,
    FADEIN          = 6,
    DRAWPOINT       = 7,
    CIRCLE          = 8,
    RECT            = 9,
    LINE            = 10,
    CHAR            = 11,
    STRING          = 12,
    GET_TOUCH       = 13,
    ELLIPSE         = 14,
    TRIANGLE1       = 15,
    TRIANGLE2       = 16,
} SubPCommand;

/*==============================================================================
 * GLOBAL CONSTANTS AND VARIABLES
 *============================================================================*/

/* CONSTANTS */
/* None */

/* INPUT VARIABLES */
/* None */

/* OUTPUT VARIABLES */
extern COLOR fCOLOR;
extern COLOR bcolor;
extern COLOR green;
extern COLOR blue;
extern COLOR yellow;
extern COLOR grey;
extern COLOR red;
extern COLOR black;
extern COLOR white;
extern COLOR mycolor;

extern int mouseX;
extern int mouseY;
extern int width;
extern int height;
//extern int max_distance;
 
/*==============================================================================
 * GLOBAL FUNCTION PROTOTYPES
 *============================================================================*/

void background(uint8_t b);
void background( uint8_t r, uint8_t g, uint8_t b);
void beginCanvas();
void clearscreen( void);
float dist(float x1, float y1, float x2, float y2);
void drawchar( int x, int y, char text);
void drawcircle( int x, int y, int r); 
void drawrect( int x1, int y1, int x2, int y2);
void drawstring( int x, int y, char *text);
void ellipse( int x, int y, int radx, int rady);
void fadein( int time);
void fadeout( int time);
void fill( int f);
void fill( uint8_t r, uint8_t g, uint8_t b);
void fillback( void);
void gettouch( void);
void line( int x1, int y1, int x2, int y2);
void noStroke(void);
void noFill(void);
void point( int x, int y);
void quad( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void rect(int x, int y, int width, int height);
void setbcolor(uint8_t r, uint8_t g, uint8_t b);
void setbrightness( int bright);
void setfcolor(uint8_t r, uint8_t g, uint8_t b);
void stroke( int s);
void stroke( uint8_t r, uint8_t g, uint8_t b);
void triangle( int x1, int y1, int x2, int y2, int x3, int y3);

#endif

#endif /* Idempotence guard */

