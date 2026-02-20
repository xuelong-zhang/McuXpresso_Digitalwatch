/**
 * LPC1763 FIO(First GPIO) register definition
 *
 * Copyright(C) 2020, Panasonic HRDC, All rights reserved.
 *
 * @file
 * @attention  none
 */

#ifndef FIOREG_H_
#define FIOREG_H_

/**
 * LPC1763 FIO(First GPIO) register definition
 */
#define FIO0MASK	(*(volatile uint32_t *) 0x2009C010)
#define FIO1MASK	(*(volatile uint32_t *) 0x2009C030)
#define FIO2MASK	(*(volatile uint32_t *) 0x2009C050)
#define FIO3MASK	(*(volatile uint32_t *) 0x2009C070)
#define FIO4MASK	(*(volatile uint32_t *) 0x2009C090)

#define FIO0PIN		(*(volatile uint32_t *) 0x2009C014)
#define FIO1PIN		(*(volatile uint32_t *) 0x2009C034)
#define FIO2PIN		(*(volatile uint32_t *) 0x2009C054)
#define FIO3PIN		(*(volatile uint32_t *) 0x2009C074)
#define FIO4PIN		(*(volatile uint32_t *) 0x2009C094)

#define FIO0DIR		(*(volatile uint32_t *) 0x2009C000)
#define FIO1DIR		(*(volatile uint32_t *) 0x2009C020)
#define FIO2DIR		(*(volatile uint32_t *) 0x2009C040)
#define FIO3DIR		(*(volatile uint32_t *) 0x2009C060)
#define FIO4DIR		(*(volatile uint32_t *) 0x2009C080)

#define FIO0SET		(*(volatile uint32_t *) 0x2009C018)
#define FIO1SET		(*(volatile uint32_t *) 0x2009C038)
#define FIO2SET		(*(volatile uint32_t *) 0x2009C058)
#define FIO3SET		(*(volatile uint32_t *) 0x2009C078)
#define FIO4SET		(*(volatile uint32_t *) 0x2009C098)

#define FIO0CLR		(*(volatile uint32_t *) 0x2009C01C)
#define FIO1CLR		(*(volatile uint32_t *) 0x2009C03C)
#define FIO2CLR		(*(volatile uint32_t *) 0x2009C05C)
#define FIO3CLR		(*(volatile uint32_t *) 0x2009C07C)
#define FIO4CLR		(*(volatile uint32_t *) 0x2009C09C)

#define FIO0VAL		(*(volatile uint32_t *) 0x2009C014)
#define FIO1VAL		(*(volatile uint32_t *) 0x2009C034)
#define FIO2VAL		(*(volatile uint32_t *) 0x2009C054)
#define FIO3VAL		(*(volatile uint32_t *) 0x2009C074)
#define FIO4VAL		(*(volatile uint32_t *) 0x2009C094)


#endif /* FIOREG_H_ */
