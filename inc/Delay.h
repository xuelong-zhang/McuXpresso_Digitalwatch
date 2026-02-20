#ifndef DELAY_H
#define DELAY_H
/**
 * @file delay.h
 * @brief embedded C, Exercise utility, software delay routine header file.
 *
 * Copyright(C) 2016, Panasonic HRDC, All rights reserved.
 *
 * @attention  this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

extern void Delay_sec(unsigned int sec);
extern void Delay_ms(unsigned int ms);
extern void Delay_us(unsigned int us);
extern void Delay_40ns(unsigned int count);

#endif
