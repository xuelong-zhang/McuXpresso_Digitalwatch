/**
 * @file SysTick.h
 * @brief embedded C, Exercise utility,  SysTick Timer control header file.
 *
 * Copyright(C) 2017, Panasonic HRDC, All rights reserved.
 *
 * @attention  this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

#ifndef SYSTICK_H
#define SYSTICK_H

extern void SysTick_init(void);
extern unsigned int get_time_1ms(void);
extern unsigned int get_time_10ms(void);
extern void SysTick_Handler(void);

#endif
