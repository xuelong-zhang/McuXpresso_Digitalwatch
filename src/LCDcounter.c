/**
 * @file LCDcounter.c
 * @brief 組込みソフトウェア基礎, 演習 リングバッファ
 *
 * main : OKスイッチの押下検出で、1秒毎にカウントを開始する。
 *        カウントする度にカウンタの値をLCDに4桁ゼロパディング表示する.
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention CPUクロックの設定はスタートアップルーチンで実施済
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "GPIO.h"
#include "LED.h"
#include "LCD.h"
#include "Delay.h"
#include "tactSW.h"
#include "SysTick.h"
#include "clib.h"

                 // 停止状態   カウント状態
typedef enum stat { STOP,       COUNT       }stat_t;

static void SYS_init(void);

int main(void)
{
	uint32_t counter=0;
	int status=STOP;
	unsigned int tactSW_ok_now;		// 今回読んだデータ
	unsigned int tactSW_ok_old;		// 前回読んだデータ
	int error_no = 0;				// エラー番号
	int err_lcd = 0;				// LCD制御時に発生したエラー番号
	unsigned int chk_10ms;			// 10ms割込み発生回数
	unsigned int past1sec=0;		// 10ms割込み発生確認回数
	char tempbuff[20];				// 一時的な文字列格納領域	

	SYS_init();
    tactSW_ok_old = tactSW_getStatus(BTN_OK);	// OKスイッチの状態を読み取る
	(void)myitoa(counter, tempbuff, 4, '0');	// カウンタの初期値の4桁ゼロパディング文字列作成
	LCD_locate(1,1);							// カーソル位置変更指示をバッファに格納
	(void)LCD_puts(tempbuff);					// 表示文字列をバッファに格納

    while(1){
		err_lcd = LCD_display();				// リングバッファに格納されているLCDモジュールに対する処理を１つ実行
		if (err_lcd != 0) {
			if (error_no == 0) {
				error_no = err_lcd;
			}
			LCD_reset_lcdbuf();
		}

		switch(error_no){
		case ERR_TOUT:
		case ERR_BFULL:
		case ERR_ESC:
			LED_on(error_no);
			break;
		default:
			LED_alloff();
			break;
		}

    	tactSW_ok_now = tactSW_getStatus(BTN_OK);
		if (tactSW_ok_now == OFF) {		// スイッチがオフだった？
			if((tactSW_ok_now ^ tactSW_ok_old) != 0){	// 前回と今回のスイッチ状態は異なるか？
				if(status == COUNT){
					status = STOP;
				}else{
					status = COUNT;
					chk_10ms = get_time_10ms();
				}
			}
		}
		tactSW_ok_old = tactSW_ok_now;

		if(status == COUNT){
			if (get_time_10ms() != chk_10ms){
				chk_10ms++;
				past1sec++;
				if(past1sec >=100){ // 10msの割込みが100回発生したか ?
					past1sec=0;
	 				counter++;
					(void)myitoa(counter, tempbuff, 4, '0');	// カウンタ値の4桁ゼロパディング文字列作成
					LCD_locate(1,1);					// カーソル位置を(1,1)に変更する指示をバッファに格納
					(void)LCD_puts(tempbuff);			// カウント値の文字列をバッファに格納
				}
			}
		}
    }

    return 0 ;
}

/**
 * システム起動時の初期化処理を行う.
 *
 * @param 無し
 * @return 無し
 * @attention __disable_irq()と__enable_irq()はCMSISライブラリ関数.
 */
static void SYS_init(void)
{
	__disable_irq();	// CPUの制御レジスタにより、全てのマスカブル割込みをマスクする

	LED_setup();
	tactSW_setup();
	SysTick_init();

	__enable_irq();    // CPUの制御レジスタにより、全てのマスカブル割込みのマスクを解除する

	while(tactSW_isfix() != true){ // スイッチの状態が確定するのを待つ
		;
	}
	LCD_setup();
}
