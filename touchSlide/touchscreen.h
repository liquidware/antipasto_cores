#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "pindefs.h"
#include "binary.h"



typedef struct
{
	unsigned int x; /*!< X coordinate */
	unsigned int y; /*!< Y coordinate  */
}POINT; 

extern volatile unsigned int x_loc;
extern volatile unsigned int y_loc;
extern volatile unsigned int z1_loc;
extern volatile unsigned int z2_loc;
extern unsigned char sample_count;
extern unsigned int x_samples;
extern unsigned int y_samples;



/* Functions */
char touch_getCursor(POINT * p);
char touch_getTouched(POINT * p);

void touch_init();
char touch_calibrate(void);

void touchscreen_setup_x();
void touchscreen_setup_y();
void touchscreen_setup_z1();
void touchscreen_setup_z2();
void touchscreen_process_y(unsigned int adc_value);
void touchscreen_process_x(unsigned int adc_value);
void touchscreen_process_z1(unsigned int adc_value);
void touchscreen_process_z2(unsigned int adc_value);
void touchscreen_service();
void delay_ms(unsigned char x);

#ifdef __cplusplus
} // extern "C"
#endif

#define TOUCHSCR_IS_CALIBRATED 20 //!< Variable, that if changed will force a calibrate 


#define xDir	0
#define yDir	1
#define yes	1
#define no	0

//pin defines
#define XPLUS_PIN	   PF7
#define YPLUS_PIN	   PF6
#define YMINUS_PIN	PF5
#define XMINUS_PIN	PF4

#define XPLUS	B01000111  //10-bit precision
#define YPLUS	B01000110 
#define YMINUS	B01000101
#define XMINUS	B01000100



#endif



