/**
 * @file SysTick.c
 * @brief LPC1763のSysTickタイマ制御関数　
 *        割込みサービスルーチン内でタクタイルスイッチの読み取りを行い、スイッチ状態を更新する.
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention  クロックの設定はスタートアップルーチンで実施済
 * 			   this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

#include <stdint.h>
#include "SysTick.h"
#include "tactSW.h"

#define MATCHTIMES 3 /*  スイッチ状態の前回値と今回値の一致する回数 */

/* Systick Register address, refer datasheet for more info */
#define STCTRL      ( *( volatile uint32_t *) 0xE000E010 )
#define STRELOAD    ( *( volatile uint32_t *) 0xE000E014 )
#define STCURR      ( *( volatile uint32_t *) 0xE000E018 )

/*******STCTRLのビットポジション*******/
#define SBIT_ENABLE     0
#define SBIT_TICKINT    1
#define SBIT_CLKSOURCE  2


/* Core Clock 100MHz, 1ms(1000000ns) / 10ns = 100000
 * 100000 - 1= 99999 */
#define RELOAD_VALUE  99999

volatile static unsigned int tick_1ms=0;	// 1ms interrupt occurrence count
volatile static unsigned int tick_10ms=0;	// 10ms interrupt occurrence count

/**
 * 1ms 割込みの発生回数を返す
 *
 * @param 無し
 * @return 1ms 割込みの発生回数
 * @attention none
 */
unsigned int get_time_1ms(void)
{
	return tick_1ms;
}

/**
 * 10ms 割込みの発生回数を返す
 *
 * @param 無し
 * @return 10ms 割込みの発生回数
 * @attention none
 */
unsigned int get_time_10ms(void)
{
	return tick_10ms;
}

/**
 * SysTickタイマ割込みの発生させるための初期設定を行う
 * 割込み周期を1msとする
 *
 * @param 無し
 * @return 無し
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
void SysTick_init(void)
{
	STRELOAD = RELOAD_VALUE;    // リロード値の設定
    /* タイマカウンタを有効にする　タイマ割込みを有効にする　CPUクロックをクロックソースにする */
    STCTRL = (((uint32_t)1<<SBIT_ENABLE) | ((uint32_t)1<<SBIT_TICKINT) | ((uint32_t)1<<SBIT_CLKSOURCE));
}

/**
 * SysTickタイマ割込みサービスルーチン
 *
 * @brief 1ms割込み発生回数の変数と10ms割込み発生回数の変数の値を更新する
 *        タクタイルスイッチの状態が3回連続同じならスイッチの状態を更新する
 * @param 無し
 * @return 無し
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
void SysTick_Handler(void)
{
	static int mycnt=0;
	static unsigned int tactSW_now;	// 今、読んだ全スイッチの状態
	static unsigned int tactSW_old;	// 前回、読んだ全スイッチの状態
	static int matchCount=MATCHTIMES;	// スイッチ状態が連続で一致した目標回数

	tick_1ms++;
	mycnt++;
	if(mycnt >=10){
		tick_10ms++;
		mycnt=0;
	}
	tactSW_now = tactSW_read();			// 現在の全スイッチの状態を読み出す
	if (tactSW_now == tactSW_old) {		// 前回と同じ?
		matchCount--;
		if(matchCount == 0){				// 一致した回数が目標回数に到達したか？
			matchCount = MATCHTIMES;
			tactSW_update(tactSW_now);	// スイッチの状態を確定した値に更新する
		}
	}else{
		matchCount = MATCHTIMES;
	}
	tactSW_old = tactSW_now;			// 前回のスイッチの状態を保存
}
