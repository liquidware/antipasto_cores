#ifndef __BMP_H_
#define __BMP_H_

#define BMP_LOOKUP_TABLE_SIZE DATAFLASH_PAGESIZE*2 //!< Two dataflash pages
#define BMP_MAXBMP_COUNT	44						//!< Max BMP count


#ifdef __cplusplus
extern "C"{
#endif

void bmp_init();
unsigned char bmp_lookup_table_checkStatus();
void bmp_draw(char * bmp_name, unsigned char x, unsigned char y);

#ifdef __cplusplus
} // extern "C"
#endif

#endif


