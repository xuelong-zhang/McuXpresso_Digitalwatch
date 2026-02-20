/**
 * @file tactSW.c
 * @brief タクタイルスイッチ操作関数
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention チャタリング対応のスイッチの状態読み取りはSysTickタイマ割込みサービスルーチン内で実施
 *            ただし、ISP_ENTRYスイッチは除外している.
 * 			   this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

#include "PINCTRL.h"
#include "GPIO.h"
#include "tactSW.h"

/**
 * タクタイルスイッチに対応するGPIO ポート番号とビットポジション情報
 */
#define NO_OF_TSW  6
static uint8_t TSW_info[][2]={ //  {port No. , bit position}
		{PORT1,18}, // LEFT
		{PORT1,19}, // RIGHT
		{PORT1,20}, // DOWN
		{PORT1,21}, // UP
		{PORT1,22}, // CANCEL
		{PORT1,23}, // OK
};

static uint32_t tsw;   // 確定したスイッチの状態(FPIOINレジスタの読み出し値のまま)
static int isfix;		// =1: スイッチの状態が確定していることを表す

/**
 * タクタイルスイッチを操作するための端子とポートの初期設定を行う
 * 端子をGPIOにする
 * 接続ポートを入力方法
 * プルアップ抵抗あり
 * オープンドレインにしない
 * スイッチ制御関連の変数を初期化
 *
 * @param 無し
 * @return 無し
 * @attention none
 */
void tactSW_setup(void)
{
	int i;
	// プルアップ抵抗あり
	//	           P1.18	  P1.19	     P1.20      P1.21       P1.22       P1.23
	PINMODE3 &= ~((3<<BP4) | (3<<BP6) | (3<<BP8) | (3<<BP10) | (3<<BP12) | (3<<BP14));
	// オープンドレインにしない
	PINMODE_OD1 &= ~((1<<BP18) | (1<<BP19) | (1<<BP20) | (1<<BP21) | (1<<BP22) | (1<<BP23));
	//	GPIOにする
	//            P1.18	     P1.19      P1.20      P1.21       P1.22       P1.23
	PINSEL3 &= ~((3<<BP4) | (3<<BP6) | (3<<BP8) | (3<<BP10) | (3<<BP12) | (3<<BP14));
	for(i=BTN_LEFT; i <= BTN_OK; i++){
		GPIO_setDir(TSW_info[i][0], TSW_info[i][1], INDIRECTION);
	}
	tsw=~0;	 // どのスイッチも押されていない(回路は負論理)
	isfix=0; // スイッチ状態未確定
}

/**
 * スイッチの状態が確定しているかを返す
 *
 * @param 無し
 * @return 確定しているかどうか
 * @attention none
 */
bool tactSW_isfix(void)
{
	return (bool)isfix;
}

/**
 * 全スイッチの接続入力ポートの状態を返す
 *
 * @param 無し
 * @return all tact switch status
 * @attention 全スイッチの接続入力ポートの状態.
 */
uint32_t tactSW_read(void)
{
	return (GPIO_getValue( PORT1 ) & 0x00FC0000);
}

/**
 * 確定済の全スイッチのポートの値を返す
 *
 * @param 無し
 * @return 確定済の全スイッチのポートの値
 * @attention this program returns a value of "tsw"variable.
 */
uint32_t tactSW_getTsw(void)
{
	return tsw;
}

/**
 * 現在のスイッチの状態を更新する
 *
 * @param[in] data 更新する値
 * @return 無し
 * @attention 無し
 */
void tactSW_update(uint32_t data)
{
	tsw = data;
	isfix=1;		// 状態確定
}

/**
 * 指定されたスイッチの状態を返す
 *
 * @param[in] sw_no スイッチ番号
 * @return status 0:OFF, 1:ON
 * @attention 入力回路は負論理だが、戻り値は正論理値を返す!
 */
unsigned int tactSW_getStatus(int no)
{
	unsigned int ret=0;

	if ((0 <= no) && (no < NO_OF_TSW)){
		uint32_t bit = ((uint32_t)1 << TSW_info[no][1]);
		if((tsw & bit) == 0){
			ret = 1;
		}else{
			// do nothing
		}
	}else{
		// do nothing
	}
	return ret;
}
