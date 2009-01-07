#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <avr/interrupt.h>

#include "dataflash.h"
#include "graphics.h"
#include "bmp.h"
#include "bitops.h"
#include "oled.h"
#include "usart.h"

unsigned char bmp_lookup_table[BMP_LOOKUP_TABLE_SIZE];
unsigned char bmp_lookup_table_status = FALSE;



void bmp_init()
{

	dataflash_cont_read(&bmp_lookup_table[0],0,BMP_LOOKUP_TABLE_SIZE);
	bmp_lookup_table_status = TRUE;

}


unsigned char bmp_lookup_table_checkStatus()
{
	return bmp_lookup_table_status;
}




void bmp_draw(char * bmp_name, unsigned int x, unsigned int y)
{
unsigned char i=0;
unsigned char * lookup_table = &bmp_lookup_table[0];
unsigned long bmp_loc=0;
unsigned int index;

cli(); //disable interrupts

/*
for (i=0;i<100;i++)
{
usart_putNum1(bmp_lookup_table[i]);
usart_putc1('\n');
}
*/

//usart_init(57600);
//usart_puts(bmp_name);

//find bitmap
while( i < BMP_MAXBMP_COUNT)
	{
	if (!strncmp(bmp_name,(char*)lookup_table,7))
		{
      //usart_puts("Found ");
      //usart_puts(bmp_name);
		//get the offset
		bmp_loc = (unsigned long)lookup_table[7] << 24;
		bmp_loc |= (unsigned long)lookup_table[8] << 16;
		bmp_loc |= (unsigned long)lookup_table[9] << 8;
		bmp_loc |= (unsigned long)lookup_table[10];
		//memcpy(&bmp_loc,&lookup_table[8],4);
		break;
		}
   	
	lookup_table+=11;
	i++;
	}

//usart_puts("At: ");
//usart_putNum(bmp_loc);

unsigned char bmp_buff[DATAFLASH_PAGESIZE];

/* Get the width and height */
dataflash_read_block(bmp_buff,bmp_loc,4);

uint16_t width = (uint16_t)(bmp_buff[0]<<8) + bmp_buff[1];
uint16_t height = (uint16_t)(bmp_buff[2]<<8) + bmp_buff[3];
uint32_t length = (uint32_t)width * (uint32_t)height;
uint16_t byteCnt=0;

//usart_putNum(width);
//usart_putNum(height);
//usart_putNum(length);

dispSetWindow(x,y,width-1,height);

/* Incremement the bmp pointer passed the width/height */
bmp_loc+=4;
dataflash_read_block(bmp_buff, bmp_loc, DATAFLASH_PAGESIZE);
while(length--)
{
	GraphicsColor.blue 	= 	bmp_buff[byteCnt];
	GraphicsColor.green	= 	bmp_buff[byteCnt+1];
	GraphicsColor.red 	= 	bmp_buff[byteCnt+2];
	dispPix();

	byteCnt+=3;

	if (byteCnt>=528)
	{
		bmp_loc+=528;
		dataflash_read_block(bmp_buff, bmp_loc, DATAFLASH_PAGESIZE);
		byteCnt=0;
	}		
}

sei(); //enable interrupts
}
