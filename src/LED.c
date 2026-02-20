/**
 * @file LED.c
 * @brief 組込みソフトウェア基礎, LED操作関数
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */

#include "general.h"

#include "FIOREG.h"
#include "PINCTRL.h"

#include "GPIO.h"

#include "LED.h"

/**
 *  LEDに対応するGPIO ポート番号とビットポジション情報
 */
static uint8_t LED_info[][2]={
		{0,0}, // dummy {port No. , bit position}
		{0,10}, // LED1
		{0,11}, // LED2
		{2,13}, // LED3
		{2,12}, // LED4
		{2,11}, // LED5
		{2,8}, // LED6
		{2,7}, // LED7
		{2,6}, // LED8
		{0,9}, // LED9
		{0,8}, // LED10
		{0,7}, // LED11
		{0,6}, // LED12
		{0,5}, // LED13
		{0,4}, // LED14
		{2,5}, // LED15
		{2,3}, // LED16
};

/**
 * LEDを操作するための端子とポートの初期設定を行う
 * 接続ポートの出力ラッチを0にする
 * プルアップ抵抗、プルダウン抵抗を付加しない
 * オープンドレインにしない
 *
 * @param 無し
 * @return 無し
 * @attention after power-on reset, All pin has GPIO-function..
 *
 */
void LED_setup(void)
{
	// ポートの出力ラッチの値を0にする(LED OFF)
	//	              P0.4	     P0.5       P0.6	   P0.7	      P0.8	     P0.9       P0.10       P0.11
		FIO0PIN &= ~((1<<BP4) | (1<<BP5) | (1<<BP6) | (1<<BP7) | (1<<BP8) | (1<<BP9) | (1<<BP10) | (1<<BP11));
	// ポートにプルアップ抵抗およびプルダウン抵抗を付加しない
		PINMODE0 &= ~((3<<BP8) | (3<<BP10) | (3<<BP12) | (3<<BP14) | (3<<BP16) | (3<<BP18) | (3<<BP20) | (3<<BP22));
		PINMODE0 |=  ((2<<BP8) | (2<<BP10) | (2<<BP12) | (2<<BP14) | (2<<BP16) | (2<<BP18) | (2<<BP20) | (2<<BP22));
	// ポートをオープンドレインにしない
		PINMODE_OD0 &= ~((1<<BP4) | (1<<BP5) | (1<<BP6) | (1<<BP7) | (1<<BP8) | (1<<BP9) | (1<<BP10) | (1<<BP11));
	// 端子をGPIOにする
	//	              P0.4	     P0.5	     P0.6	     P0.7	     P0.8	     P0.9	     P0.10	     P0.11
		PINSEL0 &= ~((3<<BP8) | (3<<BP10) | (3<<BP12) | (3<<BP14) | (3<<BP16) | (3<<BP18) | (3<<BP20) | (3<<BP22));

	//LED   set GPIO's direction to output.
		GPIO_setDir(PORT0,BP4,OUTDIRECTION);//LED14
		GPIO_setDir(PORT0,BP5,OUTDIRECTION);//LED13
		GPIO_setDir(PORT0,BP6,OUTDIRECTION);//LED12
		GPIO_setDir(PORT0,BP7,OUTDIRECTION);//LED11
		GPIO_setDir(PORT0,BP8,OUTDIRECTION);//LED10
		GPIO_setDir(PORT0,BP9,OUTDIRECTION);//LED9
		GPIO_setDir(PORT0,BP10,OUTDIRECTION);//LED1
		GPIO_setDir(PORT0,BP11,OUTDIRECTION);//LED2

	// ポートの出力ラッチの値を0にする(LED OFF)
	//	              P2.3	     P2.5       P2.6	   P2.7	      P2.8	     P2.11	     P2.12	     P2.13
		FIO2PIN &= ~((1<<BP3) | (1<<BP5) | (1<<BP6) | (1<<BP7) | (1<<BP8) | (1<<BP11) | (1<<BP12) | (1<<BP13));
	// ポートにプルアップ抵抗およびプルダウン抵抗を付加しない
		PINMODE4 &= ~((3<<BP6) | (3<<BP10) | (3<<BP12) | (3<<BP14) | (3<<BP16) | (3<<BP22) | (3<<BP24) | (3<<BP26));
		PINMODE4 |=  ((2<<BP6) | (2<<BP10) | (2<<BP12) | (2<<BP14) | (2<<BP16) | (2<<BP22) | (2<<BP24) | (2<<BP26));
	// ポートをオープンドレインにしない
		PINMODE_OD2  &= (~((1<<BP3) | (1<<BP5) | (1<<BP6) | (1<<BP7) | (1<<BP8) | (1<<BP11) | (1<<BP12) | (1<<BP13)));
	// 端子をGPIOにする
	//	              P2.3	     P2.5	     P2.6	     P2.7	     P2.8	     P2.11	     P2.12	     P2.13
		PINSEL4 &= ~((3<<BP6) | (3<<BP10) | (3<<BP12) | (3<<BP14) | (3<<BP16) | (3<<BP22) | (3<<BP24) | (3<<BP26));

		GPIO_setDir(PORT2,BP3,OUTDIRECTION);//LED16
		GPIO_setDir(PORT2,BP5,OUTDIRECTION);//LED15
		GPIO_setDir(PORT2,BP6,OUTDIRECTION);//LED8
		GPIO_setDir(PORT2,BP7,OUTDIRECTION);//LED7
		GPIO_setDir(PORT2,BP8,OUTDIRECTION);//LED6
		GPIO_setDir(PORT2,BP11,OUTDIRECTION);//LED5
		GPIO_setDir(PORT2,BP12,OUTDIRECTION);//LED4
		GPIO_setDir(PORT2,BP13,OUTDIRECTION);//LED3
}

/**
 *
 * LEDを点灯する.
 *
 * @param[in] no LED番号
 * @return 無し
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
void LED_on(int no)
{
	if ((LED1 <= no) && (no <= LED16)){
		GPIO_setValueBit(LED_info[no][0], LED_info[no][1], HIGH);
	}else{
		;//nothing to do
	}
}

/**
 *
 * LEDを消灯する
 *
 * @param[in] no LED番号
 * @return 無し
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
void LED_off(int no)
{
	if ((LED1 <= no) && (no <= LED16)){
		GPIO_setValueBit(LED_info[no][0], LED_info[no][1], LOW);
	}else{
		;//nothing to do
	}
}

/**
 * すべてのLEDを消灯する
 *
 * @param 無し
 * @return 無し
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
void LED_alloff(void)
{
	int i;
	for(i=LED1; i <= LED16; i++){
		LED_off(i);
	}
}
/******************************************************************************
                           End Of File
******************************************************************************/
