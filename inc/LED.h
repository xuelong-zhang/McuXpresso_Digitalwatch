/**
 * @file LED.h
 * @brief 組込みソフトウェア基礎, LED制御関数のヘッダファイル
 *
 * @author Copyright(C) 2023, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */
#ifndef LED_H
#define LED_H

typedef
enum ledNo_{ DUMMY, LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9, LED10,
	   LED11, LED12, LED13, LED14, LED15, LED16 } ledNo_t; //!< LED No.

extern void LED_setup(void);
extern void LED_on(int no);
extern void LED_off(int no);
extern void LED_alloff(void);

#endif
