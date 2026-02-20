/**
 * Library function (C language)
 *
 * Copyright(C) 2016, Panasonic HRDC, All rights reserved.
 *
 * @file
 * @attention  none
 */
#include <string.h>
#include <stdlib.h>

#include "clib.h"

/*
 * C99 has abs() function.
 #define abs(x) (((x) < 0) ? -(x) : (x))
 */

/* Function prototype declaration */
static char *reverse(char *s);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *	Name :	itoa	Change number to character
 *
 *	Form :	char *itoa(int n, char *s, int w, char pad)
 *				int n;		Change number
 *				char *s;	Top address of result
 *				int w;		Minimum value to change
 *				char pad;	Padding value at lower than minimum
 *
 *	Function :	1.Number  ---> Change to characters, 
 *					if it is lower than minimum number, space is filled up the padding value.
 *
 *	Result : Return value : Top address of stored strings
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *myitoa(int n, char *s, int w, char pad)		/* refer K&R p.77,78 */
{
	int i, sign;

	sign = n;					/* Memory the sign */

	i = 0;
	do {
		s[i++] = abs(n % 10) + '0';
	} while ((n /= 10) != 0);

	if (sign < 0) {
		s[i++] = '-';
	}

	while (i < w) {					/* The case of not fully to minimum width */
		s[i++] = pad;
	}
	s[i] = '\0';

	return reverse(s);
}
/*------------------ end of itoa() -----------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *	Function :	reverse		Reverse the string row
 *
 *	Form :	char *reverse(char *s)
 *				char *s;	Top memory address of result
 *
 *	Function :	1. Reverse the string row
 *
 *	Result  :	Return value : Top address of stored strings
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static char *reverse(char *s)				/* Refer K&R p.76 */
{
	int i, j;
	char c;

	j = strlen((const char *)s) - 1U;
	for (i = 0 ; i < j; i++) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
		j--;
	}

	return s;
}
/*------------------ end of reverse() --------------------- EJECT ----*/


/**
 * swap uint16_t data
 *
 * @param[in,out] x pointer to data
 * @param[in,out] y pointer to data
 * @return none
 * @attention none
 *
 */
static void swapUint16( uint16_t *x, uint16_t *y )
{
	uint16_t temp = *x;
	*x		 = *y;
	*y		 = temp;
}

/**
 * sort uint16_t array[] in ascending order
 * *
 * @param[in,out] data[] pointer to data array
 * @param[in] n number of elements in array
 * @return none
 * @attention none
 *
 */
void sortUint16( uint16_t data[], size_t n )
{
	int k = n - 1;

	while( k >= 0 ){
		int	i, j;
		j = -1;
		for( i = 1; i <= k; i++ ) {
			if( data[i-1] > data[i] ) {
				j = i - 1;
				swapUint16( &data[i], &data[j] );
			}
		}
		k = j;
	}
}
/*----------------- end of clib.c -------------------------------------*/
