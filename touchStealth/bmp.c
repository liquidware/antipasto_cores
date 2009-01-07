#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <avr/interrupt.h>

#include "dataflash.h"
#include "bmp.h"
#include "bitops.h"
#include "lcd.h"

extern COLOR	global_color;

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




void bmp_draw(char * bmp_name, unsigned char x, unsigned char y)
{
unsigned char i=0;
unsigned char * lookup_table = &bmp_lookup_table[0];
unsigned long bmp_loc=0;
unsigned int index;

cli(); //disable interrupts

//find bitmap
while( i < BMP_MAXBMP_COUNT)
	{
	if (!strncmp(bmp_name,(char*)lookup_table,7))
		{
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


//bmp_loc has the bmp offset
unsigned char bmp_buff[DATAFLASH_PAGESIZE+3];
unsigned char * bmp = &bmp_buff[0];

dataflash_read_block(&bmp_buff[0],bmp_loc,DATAFLASH_PAGESIZE+3);


unsigned char width = bmp[0];
unsigned char height = bmp[1];
unsigned int length = width*height;
unsigned char pixel_count = 0;


lcd_setColumn(x, x+(width-1)); 	//set column start/ end
lcd_setRow(y,y+(height-1)); 	//set row start /end

bmp+=3;

	for (index=0; index<length; index++)
		{
		
		global_color.blue 	= 	bmp[0];
		global_color.green	 = 	bmp[1];
		global_color.red 	= 	bmp[2];
		lcd_pix();
		bmp+=3;
		
		pixel_count++;
		if ( pixel_count > 175 )
			{
			//get another page of data
			bmp_loc+=DATAFLASH_PAGESIZE;
			dataflash_read_block(&bmp_buff[0],bmp_loc,DATAFLASH_PAGESIZE+3);
			pixel_count = 0;
			bmp = &bmp_buff[0];
			}


		}

sei(); //enable interrupts
}
