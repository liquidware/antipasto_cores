#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>


#include "image_interface.h"
#include "usart.h"
#include "dataflash.h"
#include "touchscreen.h"
//#include "wiring_private.h"
#include "bitops.h"
#include "oled.h"
#include "bmp.h"

//////////////////////////////////////
/*
	This is where we get the chunks of data 
	to program to the dataflash
*/
////////////////////////////////////////

void image_interface_begin()
{

	unsigned int page_count=0;
	unsigned char command=0;
	unsigned char * buff;
	unsigned int page_num=0;

	/*
		order of operations:
			1.) purge usart buffers
			2.) recv mode byte
			3.) if mode == STORE,
					recv page_count 
					recv all pages
				else mode == READ
					send page_count
					send pages
	*/


	SETBIT(ARDUINO_RESET_DDR, ARDUINO_RESET_PNUM);
   CLRBIT(ARDUINO_RESET_PORT, ARDUINO_RESET_PNUM); //arduino in reset

   usart_init(57600);
     
   cli();

	buff = usart_getBuff_ptr();

	//enough setup, lets begin
	while(1)
		{
		usart_read_bytes(1); //Wait for us to read a byte
		command = buff[0]; //store or read command


		if (command == IMAGE_INTERFACE_STORE)
			{

			unsigned int i,x,wait,wait2;
			unsigned char out_data[DATAFLASH_PAGESIZE],in_data[DATAFLASH_PAGESIZE];		
			buff = usart_getBuff_ptr();
			
			dataflash_erase();
			usart_putc(IMAGE_INTERFACE_PAGE_DONE); //done erasing 

			//give us the number of pages you're sending
			usart_read_bytes(2);
			page_count = (buff[0] << 8) + buff[1];  //number of pages			

			for (i=0; i< page_count; i++)
				{
				unsigned int buffer_count=0;
				usart_read_bytes(DATAFLASH_PAGESIZE);	//read the whole page

				buffer_count= usart_getBuff_size();



				for(x=0;x<DATAFLASH_PAGESIZE;x++)
					out_data[x] = buff[x];

				dataflash_program_page(&out_data[0], i); 	//program the page
            
            				
				for(x=0;x<DATAFLASH_PAGESIZE;x++)
					in_data[x] = 0x00;
	         
 
				dataflash_cont_read(&in_data[0], i, DATAFLASH_PAGESIZE);
   

				for (x=0;x<DATAFLASH_PAGESIZE;x++)
				{
				if (x==256)
					{
					for (wait=0;wait<254;wait++)
						{
						for (wait2=0;wait2<254;wait2++)
							{
							asm("nop");
							}
						}
					}
					usart_putc(in_data[x]);
				
				asm("nop");

				}
           
			}
				
			}
		else

		if (command == IMAGE_INTERFACE_READ)
			{
			unsigned int x;
			unsigned char out[DATAFLASH_PAGESIZE];


			page_num=0;

			//give us the number of pages you're sending
			usart_read_bytes(2);
			page_num = (buff[0] << 8) + buff[1];  //number of pages
			

			dataflash_cont_read(&out[0], page_num, DATAFLASH_PAGESIZE);


			for (x=0; x<DATAFLASH_PAGESIZE; x++)
				{
				usart_putc(out[x]);
				}

			asm("nop");
			}
		else



		if (command == IMAGE_INTERFACE_INFO)
			{
			usart_puts("Touch Shield | ver 002");
			}

		else



		if (command == IMAGE_INTERFACE_EXIT)
			{
			 sei();
			 bmp_init();
			 /* Disable transmitter and receiver */
			 UCSR0B = (0<<RXEN0) | (0<<TXEN0) | (0<< RXCIE0);
			 SETBIT(ARDUINO_RESET_PORT, ARDUINO_RESET_PNUM); //arduino not in reset
			 CLRBIT(ARDUINO_RESET_DDR, ARDUINO_RESET_PNUM); //remove drive
			break;
			}
	
	
		}//end while
	
} //end function
