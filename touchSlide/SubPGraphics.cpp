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
#include <avr/io.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include "SubPGraphics.h"
#include "wiring.h"
#include "bitops.h"
#include "oled.h"
#include "pindefs.h"
#include "touchscreen.h"
#include "HardwareSerial.h"
#include "usart.h"

#define SCREENX 320
#define SCREENY 240

#define incx() x++, dxt += d2xt, t += dxt
#define incy() y--, dyt += d2yt, t += dyt

#define MIN3(a,b,c) ((((a)<(b))&&((a)<(c))) ? (a) : (((b)<(c)) ? (b) : (c)))
#define MAX3(a,b,c) ((((a)>(b))&&((a)>(c))) ? (a) : (((b)>(c)) ? (b) : (c)))

/*==============================================================================
 * TYPES
 *============================================================================*/
/* None */

/*==============================================================================
 * GLOBAL CONSTANTS AND VARIABLES
 *============================================================================*/

/* CONSTANTS */
/* None */

/* INPUT VARIABLES */
COLOR fcolor = { 255, 255, 255 };
COLOR bcolor = { 0, 0, 0 };
COLOR green = { 0, 255, 0 };
COLOR blue = {0,0,255};
COLOR yellow = {255,255,0};
COLOR grey = {0x77,0x77,0x77};
COLOR red = {255,0,0};
COLOR black = {0,0,0};
COLOR white = {255,255,255};
COLOR mycolor = {255, 255, 255};

int brightness=5;
POINT my_point;
int trix1, triy1, trix2, triy2, trix3, triy3;

int mouseX = SCREENX / 2;
int mouseY = SCREENY / 2;
int width = SCREENX;
int height = SCREENY;
uint8_t strokeEnb = true;
uint8_t fillEnb = true;
uint8_t cptr;
uint8_t buf[32];
uint8_t serverEnb = false;

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
void dispFillEllipse(int xc, int yc, int a, int b);
void dispOutlineEllipse(unsigned int xc,unsigned int yc,unsigned int a,unsigned int b);
void dispWuLine( int X0, int Y0, int X1, int Y1); 
void dispTriangle( int x1, int y1, int x2, int y2, int x3, int y3);
int point_triangle_intersection(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3);
int orientation (int x1, int y1, int x2, int y2, int px, int py);
void getstring(uint8_t *buf, int bufsize);

/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/

void dispOutlineEllipse( int xc, int yc, int a, int b)
{
    int x = 0, y = b;
    long a2 = (long)a*a, b2 = (long)b*b;
    long crit1 = -(a2/4 + a%2 + b2);
    long crit2 = -(b2/4 + b%2 + a2);
    long crit3 = -(b2/4 + b%2);
    long t = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long dxt = 2*b2*x, dyt = -2*a2*y;
    long d2xt = 2*b2, d2yt = 2*a2;

    while (y>=0 && x<=a)
    {
        dispPixel(xc+x, yc+y);
        if (x!=0 || y!=0)
            dispPixel(xc-x, yc-y);
        if (x!=0 && y!=0) {
            dispPixel(xc+x, yc-y);
            dispPixel(xc-x, yc+y);
        }
        if (t + b2*x <= crit1 ||   /* e(x+1,y-1/2) <= 0 */
            t + a2*y <= crit3)     /* e(x+1/2,y) <= 0 */
            incx();
        else if (t - a2*y > crit2) /* e(x+1/2,y-1) > 0 */
            incy();
        else {
            incx();
            incy();
        }
    }
}

void dispFillEllipse(int xc, int yc, int a, int b)
{           /* e(x,y) = b^2*x^2 + a^2*y^2 - a^2*b^2 */
    int x = 0, y = b;
    unsigned int width = 1;
    long a2 = (long)a*a, b2 = (long)b*b;
    long crit1 = -(a2/4 + a%2 + b2);
    long crit2 = -(b2/4 + b%2 + a2);
    long crit3 = -(b2/4 + b%2);
    long t = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long dxt = 2*b2*x, dyt = -2*a2*y;
    long d2xt = 2*b2, d2yt = 2*a2;
    while (y>=0 && x<=a) {
        if (t + b2*x <= crit1 ||     /* e(x+1,y-1/2) <= 0 */
            t + a2*y <= crit3) {     /* e(x+1/2,y) <= 0 */
            incx();
            width += 2;
        }
        else if (t - a2*y > crit2) { /* e(x+1/2,y-1) > 0 */
            //dispRectangle(xc-x, yc-y, width,1);
            dispRectangle(xc-x, yc-y, width, 1);
            if (y!=0)
                //dispRectangle(xc-x, yc+y, width,1);
                dispRectangle(xc-x, yc+y, width, 1);
            incy();
        }
        else {
            //dispRectangle(xc-x, yc-y, width,1);
            dispRectangle(xc-x, yc-y, width, 1);
            if (y!=0)
                //dispRectangle(xc-x, yc+y, width,1);
                dispRectangle(xc-x, yc+y, width, 1);
            incx();
            incy();
            width += 2;
        }
    }
    if (b == 0)
        //dispRectangle(xc-a, yc, 2*a+1,1);
        dispRectangle(xc-a, yc, 2*a+1, 1);
} 
 
//There's definitely an easier way to do this, but for now I'll use some help from:
//http://www.codeproject.com/KB/GDI/antialias.aspx#dwuln
//Edited by inthebitz
void dispWuLine( int X0, int Y0, int X1, int Y1)
{
   unsigned short IntensityShift, ErrorAdj, ErrorAcc;
   unsigned short ErrorAccTemp, Weighting, WeightingComplementMask;
   short DeltaX, DeltaY, Temp, XDir;
   short BaseColor = 0;
   short NumLevels = 2;
   unsigned short IntensityBits = 2;

   /* Make sure the line runs top to bottom */
   if (Y0 > Y1) {
      Temp = Y0; Y0 = Y1; Y1 = Temp;
      Temp = X0; X0 = X1; X1 = Temp;
   }
   /* Draw the initial pixel, which is always exactly intersected by
      the line and so needs no weighting */
   dispPixel(X0, Y0);

   if ((DeltaX = X1 - X0) >= 0) {
      XDir = 1;
   } else {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
   }
   /* Special-case horizontal, vertical, and diagonal lines, which
      require no weighting because they go right through the center of
      every pixel */
   if ((DeltaY = Y1 - Y0) == 0) {
      /* Horizontal line */
      while (DeltaX-- != 0) {
         X0 += XDir;
         dispPixel(X0, Y0);
      }
      return;
   }
   if (DeltaX == 0) {
      /* Vertical line */
      do {
         Y0++;
         dispPixel(X0, Y0);
      } while (--DeltaY != 0);
      return;
   }
   if (DeltaX == DeltaY) {
      /* Diagonal line */
      do {
         X0 += XDir;
         Y0++;
         dispPixel(X0, Y0);
      } while (--DeltaY != 0);
      return;
   }
   /* Line is not horizontal, diagonal, or vertical */
   ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
   /* # of bits by which to shift ErrorAcc to get intensity level */
   IntensityShift = 16 - IntensityBits;
   /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
   WeightingComplementMask = NumLevels - 1;
   /* Is this an X-major or Y-major line? */
   if (DeltaY > DeltaX) {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
      ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
      /* Draw all pixels other than the first and last */
      while (--DeltaY) {
         ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
         ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
         if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the X coord */
            X0 += XDir;
         }
         Y0++; /* Y-major, so always advance Y */
         /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         //////////////////////////////////////////////////////
         dispPixel(X0, Y0);
         //dispPixel(X0 + XDir, Y0, mycolora);
         //dispPixel(X0, Y0, BaseColor + Weighting);
         //dispPixel(X0 + XDir, Y0,
         //      BaseColor + (Weighting ^ WeightingComplementMask));
      }
      /* Draw the final pixel, which is
         always exactly intersected by the line
         and so needs no weighting */
      dispPixel(X1, Y1);
      return;
   }
   /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
      pixel that Y advances each time X advances 1 pixel, truncating the
      result to avoid overrunning the endpoint along the X axis */
   ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
   /* Draw all pixels other than the first and last */
   while (--DeltaX) {
      ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
      ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
      if (ErrorAcc <= ErrorAccTemp) {
         /* The error accumulator turned over, so advance the Y coord */
         Y0++;
      }
      X0 += XDir; /* X-major, so always advance X */
      /* The IntensityBits most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      ///////////////////////////////////////////////////
      dispPixel(X0, Y0);
      //dispPixel(X0, Y0 + 1, linecolor);
      //dispPixel(X0, Y0, BaseColor + Weighting);
      //dispPixel(X0, Y0 + 1,
      //      BaseColor + (Weighting ^ WeightingComplementMask));
   }
   /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
   dispPixel(X1, Y1);
}

//http://www.swissdelphicenter.ch/en/showcode.php?id=2400
//http://tog.acm.org/GraphicsGems/gemsiii/triangleCube.c
//I'm sure there's a more elegant and cleaner way, but with help from the links above I was able to cludge
//this together, so that it at least works! 
void dispTriangle( int x1, int y1, int x2, int y2, int x3, int y3) 
{

    if (strokeEnb) 
    {
        dispColor(fcolor);
        dispWuLine ( x1, y1, x2, y2);
        dispWuLine ( x2, y2, x3, y3);
        dispWuLine ( x3, y3, x1, y1);
    }

    if (fillEnb) 
    {
        dispColor(bcolor);
        for (int x=MIN3(x1, x2, x3); x<MAX3(x1, x2, x3); x++) 
        {
            for (int y=MIN3(y1,y2,y3); y<MAX3(y1,y2,y3); y++) 
            {
                if (point_triangle_intersection(x, y, x1, y1, x2, y2, x3, y3) ) 
                {
                    dispPixel(x , y);
                }
            }
        }
    }


}

int point_triangle_intersection(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3) {
  int or1, or2, or3;
 
  or1 = orientation(x1, y1, x2, y2, px, py);
  or2 = orientation(x2, y2, x3, y3, px, py);
  or3 = orientation(x3, y3, x1, y1, px, py);
 
  if ((or1 == or2) && (or2 == or3) ) {
    return 1;
  } else if (or1 == 0 ) {
    return ((or2 == 0) || (or3 == 0));
  } else if (or2 == 0) {
    return ((or1 == 0) || (or3 == 0));
  } else if ( or3 == 0) {
    return ((or2 == 0) || (or1 == 0));
  } else {
    return 0;
  }
}

int orientation (int x1, int y1, int x2, int y2, int px, int py) {
  int orin;
 
  orin = (x2 - x1) * (py - y1) - (px - x1) * (y2 - y1);
 
  if (orin > 0) {
    return 1;
  } else if (orin < 0) {
    return -1;
  } else {
    return 0;
  }
 
} 

/*==============================================================================
 * GLOBAL FUNCTIONS
 *============================================================================*/  

void background(uint8_t b)
{
    setbcolor( b, b, b); fillback();
}

void background( uint8_t r, uint8_t g, uint8_t b)
{
    setbcolor( r, g, b); fillback();
}

void clearscreen( void)
{
    setbcolor( 0, 0, 0); fillback();
}

float dist(float x1, float y1, float x2, float y2)
{
    int x = (x2-x1);
    int y = (y2-y1);
    return sqrt( (x*x)+(y*y));
}

void drawchar( int x, int y, char text)
{
    dispPutC(text, x, y, fcolor, bcolor);
}

void drawcircle( int x, int y, int r)
{
    ellipse(x, y, r, r);
}

void drawrect( int x1, int y1, int x2, int y2)
{
    rect(x1, y1, x2, y2);
}

void drawstring( int x, int y, char *text)
{
    dispPutS(text, x, y, fcolor, bcolor);
}

void ellipse( int x, int y, int radx, int rady)
{
 
    if (fillEnb) 
    {
        dispColor(bcolor);
        dispFillEllipse(x, y, radx-1, rady-1);
    }
       
    if(strokeEnb) 
    {
        dispColor(fcolor);
        dispOutlineEllipse(x,y,radx,rady);
    }


}


void fadein( int time)
{
    for (uint8_t i = 0; i<(brightness+1); i++) 
    {
        oled_brightness((uint8_t)i);
        delay(time);
    }
}

void fadeout( int time)
{

    for (uint8_t i = brightness; i>0; i--) 
    {
        oled_brightness((uint8_t)i);
        delay(time);
    }
}

void fill( int f)
{
    fillEnb = true;
    setbcolor(f,f,f);
}

void fill( uint8_t r, uint8_t g, uint8_t b)
{
    fillEnb = true;
    setbcolor(r,g,b);
}


void fillback( void)
{
    dispColor(bcolor);
    dispClearScreen();
}

void gettouch()
{
    if(touch_getCursor(&my_point))
    {
        mouseX = my_point.x;
        mouseY = my_point.y;
    }
}

void line( int x1, int y1, int x2, int y2)
{
    dispColor(fcolor);
    dispWuLine ( x1, y1, x2, y2);
}

void noFill(void)
{
    fillEnb = false;
}

void noStroke(void)
{
    strokeEnb = false;
}

void point( int x, int y)
{
    dispColor(fcolor);
    dispPixel( x, y);
}

void quad( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    /* Save the stroke state */
    uint8_t prevStroke = strokeEnb;

    if (fillEnb) 
    {
        /* Disable stroke */
        noStroke();
        triangle(x1, y1, x2, y2, x3, y3);
        triangle(x2, y2, x3, y3, x4, y4);
        triangle(x1, y1, x3, y3, x4, y4);

        /* Restore Stroke */
        if (prevStroke) { strokeEnb = true; }
    }

    if (strokeEnb) 
    {
        line(x1,y1,x2,y2);
        line(x2,y2,x3,y3);
        line(x3,y3,x4,y4);
        line(x4,y4,x1,y1);
    }
}

void rect(int x, int y, int width, int height)
{
    /* Correction for gram starting location (0,0) */ 
    width -=1;
    height -=1;

    if ( (width<=0) || (height<=0) ){ return; }
    
    //stroke
    if (strokeEnb) {
        dispColor(fcolor);
        line(x,y,x+width,y);
        line(x,y,x,y+height);
        line(x,y+height,x+width,y+height);
        line(x+width,y,x+width,y+height);
    }

    //fill
    if (fillEnb) {
        x+=1;
        y+=1;
        dispColor(bcolor);
        dispRectangle(x, y, width-2, height-2); 
    }
}

void setbcolor(uint8_t r, uint8_t g, uint8_t b)
{
    bcolor.red = r; bcolor.green = g; bcolor.blue = b;
}

void setbrightness( int bright)
{
    oled_brightness(bright);
    brightness = bright;
}

void setfcolor(uint8_t r, uint8_t g, uint8_t b)
{
    fcolor.red = r; fcolor.green = g; fcolor.blue = b;
}


void stroke( int s)
{
    setfcolor(s,s,s);
}

void stroke( uint8_t r, uint8_t g, uint8_t b)
{
    setfcolor(r,g,b);
}
 
void triangle( int x1, int y1, int x2, int y2, int x3, int y3) 
{ 
    dispTriangle(x1, y1, x2, y2, x3, y3);
} 

void beginCanvas(void)
{

    /* Start Serial, if not started already */
    if (!serverEnb) { beginSerial(9600); serverEnb = true; }

    /* Get our command string */
    getstring(buf,sizeof(buf));
    cptr=1;

    /* Is it a valid command? */
    if (buf[0] == '|') 
    {
        

        /* Determine the command */
        switch(buf[cptr]) 
        {
        case SETFCOLOR:
          setfcolor( buf[cptr+1], buf[cptr+2], buf[cptr+3]);
          break;
        case SETBCOLOR:
          setbcolor( buf[cptr+1], buf[cptr+2], buf[cptr+3]); 
          break;
        case FILLBACK:
          fillback();      
          break;
        case SET_BRIGHTNESS:
          setbrightness(buf[cptr+1]);
          break;
        case FADEOUT:
          fadeout((buf[cptr+2] << 8) + buf[cptr+1]);
          break;
        case FADEIN:
          fadein((buf[cptr+2] << 8) + buf[cptr+1]);
          break;
        case DRAWPOINT:
          point((buf[cptr+2] << 8) + buf[cptr+1],
                (buf[cptr+4] << 8) + buf[cptr+3]);
          break;
        case CIRCLE:
          int r;
          r = (buf[cptr+6] << 8) + buf[cptr+5];
          ellipse((buf[cptr+2] << 8) + buf[cptr+1], 
                  (buf[cptr+4] << 8) + buf[cptr+3],
                  r,
                  r);
          break;
        case RECT:
          rect((buf[cptr+2] << 8) + buf[cptr+1], 
                (buf[cptr+4] << 8) + buf[cptr+3], 
                (buf[cptr+6] << 8) + buf[cptr+5], 
                (buf[cptr+8] << 8) + buf[cptr+7]);
          break;
        case LINE:
          line((buf[cptr+2] << 8) + buf[cptr+1], 
               (buf[cptr+4] << 8) + buf[cptr+3], 
               (buf[cptr+6] << 8) + buf[cptr+5],
               (buf[cptr+8] << 8) + buf[cptr+7]);
          break;
        case CHAR:
          drawchar(buf[cptr+5], 
                   (buf[cptr+2] << 8) + buf[cptr+1],
                   (buf[cptr+4] << 8) + buf[cptr+3]);
          break;
        case STRING:
          drawstring((buf[cptr+2] << 8) + buf[cptr+1], 
                     (buf[cptr+4] << 8) + buf[cptr+3],
                     (char*)&buf[cptr+8]);
          break;
        case GET_TOUCH:
          gettouch();
          delay(2);
          serialWrite('|');
          delay(2);
          serialWrite((unsigned char)mouseX);
          delay(2);
		  serialWrite((unsigned char)(mouseX>>8));
          delay(2);
          serialWrite((unsigned char)mouseY);
          delay(2);
		  serialWrite((unsigned char)(mouseY>>8));
          delay(2);
          serialWrite((unsigned char)0);
          break;
        case ELLIPSE:
          ellipse((buf[cptr+2] << 8) + buf[cptr+1], 
                  (buf[cptr+4] << 8) + buf[cptr+3],
                  (buf[cptr+6] << 8) + buf[cptr+5],
                  (buf[cptr+8] << 8) + buf[cptr+7]);
          break;
        case TRIANGLE1:
          trix1 = (buf[cptr+2] << 8) + buf[cptr+1];
          triy1 = (buf[cptr+4] << 8) + buf[cptr+3];
          trix2 = (buf[cptr+6] << 8) + buf[cptr+5];
          triy2 = (buf[cptr+8] << 8) + buf[cptr+7];
          break;
        case TRIANGLE2:
          trix3 = (buf[cptr+2] << 8) + buf[cptr+1];
          triy3 = (buf[cptr+4] << 8) + buf[cptr+3];
          triangle(trix1, triy1, trix2, triy2, trix3, triy3);
          break;
       
        default:
          break; 
        }
    }
}

void getstring(uint8_t *buf, int bufsize)
{
    int i;
    
    /* Send the termination command, we're ready */
    serialWrite(0);

    /* Read data */
    for (i=0; i<bufsize - 1; ++i)
    {
        while(!serialAvailable()) { ; }
        buf[i] = serialRead();
        if (buf[i] == 0) {if (i>9) break;}; // is it the terminator byte?
    }
    
    buf[i] = 0; // 0 string terminator just in case
} 
