#ifndef _BITOPS_H_
#define _BITOPS_H_

#ifdef __cplusplus
extern "C"{
#endif

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) //!<Set a bit within a byte
#define CLRBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) //!< Clear a bit within a byte
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) //!< Flip a bit within a byte
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) //!< Check a bit within a byte
#define TRUE	1	//!< A Global true
#define FALSE	0	//!< A global false
#define ON		1	//!< A boolean
#define OFF		0	//!< A boolean
#define SET		1	//!< A boolean
#define CLEAR	0	//!< A boolean

#ifdef __cplusplus
} // extern "C"
#endif
#endif



