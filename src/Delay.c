/**
 * @file delay.c
 * @brief embedded C, Exercise utility, software delay routine source code for LPC1763.
 *
 * Copyright(C) 2016, Panasonic HRDC, All rights reserved.
 *
 * @attention  this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

#include "Delay.h"

/**
 * Delay_40ns
 *
 * @brief delay in about 40ns.
 *
 * @param count delay count
 * @return none
 * @attention this routine counts in about 40us.
 */

void Delay_40ns(unsigned int count)
{
	__asm__(	// delay 40ns
			"    cmp	r0, #0\n"
	        "    beq	ns40d_end\n"
	        "ns40d1:\n"
	        "    subs	r0, #1\n"
	    	"    cmp	r0, #0\n"
	        "    bne	ns40d1\n"
			"ns40d_end:\n"
			);
}
/**
 * Delay_us
 *
 * @brief delay in about 1 us.
 *
 * @param us delay count
 * @return none
 * @attention this routine counts in about 1us.
 */

/*
    about 1us software delay routine.
    LPC1763 CPU CLOCK 100MHz 1cycle = 10ns
    1us = 1000ns,
    loop count = 1000 / (4*10) = 25
	__asm("    push {r1}\n"        1+1
    	  "usd1:\n"
          "    ldr	r1, =#25\n"     2
          "usd2:\n"
          "    subs	r1, #1\n"      1
          "    cmp	r1, #0\n"      1
          "    bne  usd2\n"         1 or 2
          "    subs r0, #1\n"      1
    	  "    cmp	r0, #0\n"      1
          "    bne  usd1\n"         1 or 2
    	  "    pop  {r1}\n");      1+1
 *
 */
void Delay_us(unsigned int us)
{
	__asm__(	// delay 1us
			"    cmp	r0, #0\n"
	        "    beq	usd_end\n"
			"    push {r1}\n"
	    	"usd1:\n"
	        "    ldr	r1, =#25\n"
	        "usd2:\n"
	        "    subs	r1, #1\n"
	        "    cmp	r1, #0\n"
	        "    bne  	usd2\n"
	        "    subs	r0, #1\n"
	    	"    cmp	r0, #0\n"
	        "    bne	usd1\n"
	    	"    pop	{r1}\n"
			"usd_end:\n"
			);
}

/**
 * Delay_ms
 *
 * @brief delay in about 1 ms.
 *
 * @param ms delay count
 * @return none
 * @attention counting by using Delay_us() makes a margin of error.
    LPC1763 CPU CLOCK 100MHz 1cycle = 10ns
    1ms = 1000000ns,
    loop count = 1000000 / (4*10) = 25000
	__asm("    push {r1}\n"        1+1
    	  "sd1:\n"
          "    ldr	r1, =#20000\n"  2
          "sd2:\n"
          "    subs	r1, #1\n"      1
          "    cmp	r1, #0\n"      1
          "    bne  sd2\n"         1 or 2
          "    subs r0, #1\n"      1
    	  "    cmp	r0, #0\n"      1
          "    bne  sd1\n"         1 or 2
    	  "    pop  {r1}\n");      1+1
 *
 */
void Delay_ms(unsigned int ms)
{
	__asm__(	// delay 1ms
			"    cmp	r0, #0\n"
	        "    beq  msd_end\n"
			"    push {r1}\n"
	    	"msd1:\n"
	        "    ldr	r1, =#25000\n"
	        "msd2:\n"
	        "    subs	r1, #1\n"
	        "    cmp	r1, #0\n"
	        "    bne  msd2\n"
	        "    subs r0, #1\n"
	    	"    cmp	r0, #0\n"
	        "    bne  msd1\n"
	    	"    pop  {r1}\n"
			"msd_end:\n"
			);
}

/**
 * Delay_sec
 *
 * @brief delay in about 1 second.
 *
 * @param sec delay count
 * @return none
 * @attention this routine counts in about 1sec
 *            by using Delay_ms().
 */
void Delay_sec(unsigned int sec)
{
	int i;
	for(i=0; i<sec; i++){
		Delay_ms(1000);
	}
}
