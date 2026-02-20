#ifndef CLIB_H
#define	CLIB_H
/**
 * Header file for c language utility functions
 *
 * Copyright(C) 2017, Panasonic HRDC, All rights reserved.
 *
 * @file
 * @attention  none
 */

#include <stdint.h>

/* Function prototype declaration */
extern char *myitoa(int n, char *s, int w, char pad);
extern void sortUint16( uint16_t data[], size_t n );

/*------------------------- end of clib.h ----------------------*/

#endif

