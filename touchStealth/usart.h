#ifndef __USART_H_
#define __USART_H_

#define F_OSC	16000000

#define USART_BUFF_SIZE	528

#ifdef __cplusplus
extern "C"{
#endif

void usart_init(unsigned long br);
void usart_putc(unsigned char c);
void usart_puts(char *s);

unsigned char * usart_getBuff_ptr();
unsigned int usart_getBuff_size();
void usart_flush();
void usart_read_bytes(unsigned int count);

#ifdef __cplusplus
} // extern "C"
#endif

#endif


