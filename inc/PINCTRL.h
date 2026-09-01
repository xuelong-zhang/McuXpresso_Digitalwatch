/**
 * LPC1763 Pin Control register definition
 *
 * Copyright(C) 2020, Panasonic HRDC, All rights reserved.
 *
 * @file
 * @attention  none
 */

#ifndef PINCTRL_H_
#define PINCTRL_H_

#define PINSEL0			(*(volatile uint32_t *)(0x4002C000))
#define PINSEL1			(*(volatile uint32_t *)(0x4002C004))
#define PINSEL2			(*(volatile uint32_t *)(0x4002C008))
#define PINSEL3			(*(volatile uint32_t *)(0x4002C00C))
#define PINSEL4			(*(volatile uint32_t *)(0x4002C010))
#define PINSEL7			(*(volatile uint32_t *)(0x4002C01C))
#define PINSEL8			(*(volatile uint32_t *)(0x4002C020))
#define PINSEL9			(*(volatile uint32_t *)(0x4002C024))
#define PINSEL10		(*(volatile uint32_t *)(0x4002C028))

#define PINMODE0		(*(volatile uint32_t *)(0x4002C040))
#define PINMODE1		(*(volatile uint32_t *)(0x4002C044))
#define PINMODE2		(*(volatile uint32_t *)(0x4002C048))
#define PINMODE3		(*(volatile uint32_t *)(0x4002C04C))
#define PINMODE4		(*(volatile uint32_t *)(0x4002C050))

#define PINMODE_OD0		(*(volatile uint32_t *)(0x4002C068))
#define PINMODE_OD1		(*(volatile uint32_t *)(0x4002C06C))
#define PINMODE_OD2		(*(volatile uint32_t *)(0x4002C070))


#endif /* PINCTRL_H_ */
