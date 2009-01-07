#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
unsigned int left;
unsigned int top;
unsigned int right;
unsigned int bottom;
} RECT_T;

typedef struct
{
unsigned char red;
unsigned char green;
unsigned char blue;
} COLOR;

extern COLOR GraphicsColor;


void dispClearScreen();
void dispColor(COLOR c);
void dispCommand(unsigned char command);
void dispData(unsigned int data);
void dispLine( int x1,  int y1, int x2,  int y2);
void dispPixel( int x,  int y);
void dispRead(COLOR *buffer, uint16_t  x, uint16_t  y);
void dispRectangle( int  x,  int  y,  int width,  int  height);
void dispSetWindow(int x, int y, int width, int height);

void dispFillRect();
void dispPix();



#ifdef __cplusplus
} // extern "C"
#endif

#endif

