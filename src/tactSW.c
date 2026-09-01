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
 * タクタイルスイッチに対応するGPIOポート番号とビット位置
 *
 * 既存6スイッチの対応関係は変更せず、
 * BTN_ENTRY（ISP_ENTRY、P2.10）だけを追加する。
 */
static uint8_t TSW_info[BTN_COUNT][2] = {
    {PORT1, BP18},    /* BTN_LEFT   */
    {PORT1, BP19},    /* BTN_RIGHT  */
    {PORT1, BP20},    /* BTN_DOWN   */
    {PORT1, BP21},    /* BTN_UP     */
    {PORT1, BP22},    /* BTN_CANCEL：SW_CENTER */
    {PORT1, BP23},    /* BTN_OK    ：SW1       */
    {PORT2, BP10}     /* BTN_ENTRY ：ISP_ENTRY */
};

/*
 * 確定したスイッチ状態。
 * 入力回路は負論理なので、ビット0が押下、ビット1が非押下を表す。
 */
static uint32_t tsw;

/* 1：スイッチ状態確定、0：未確定 */
static int isfix;

/**
 * タクタイルスイッチを使用するGPIO端子を初期化する。
 *
 * P1.18～P1.23：既存の6スイッチ
 * P2.10       ：ISP_ENTRYスイッチ
 */
void tactSW_setup(void)
{
    int i;

    /* P1.18～P1.23：内部プルアップを使用する。 */
    PINMODE3 &= ~(
          (3U << BP4)
        | (3U << BP6)
        | (3U << BP8)
        | (3U << BP10)
        | (3U << BP12)
        | (3U << BP14)
    );

    /* P2.10：内部プルアップを使用する。 */
    PINMODE4 &= ~(3U << BP20);

    /* P1.18～P1.23：オープンドレインにしない。 */
    PINMODE_OD1 &= ~(
          (1U << BP18)
        | (1U << BP19)
        | (1U << BP20)
        | (1U << BP21)
        | (1U << BP22)
        | (1U << BP23)
    );

    /* P2.10：オープンドレインにしない。 */
    PINMODE_OD2 &= ~(1U << BP10);

    /* P1.18～P1.23をGPIO機能にする。 */
    PINSEL3 &= ~(
          (3U << BP4)
        | (3U << BP6)
        | (3U << BP8)
        | (3U << BP10)
        | (3U << BP12)
        | (3U << BP14)
    );

    /* P2.10をGPIO機能にする。 */
    PINSEL4 &= ~(3U << BP20);

    /* 全7スイッチのGPIO方向を入力に設定する。 */
    for (i = 0; i < BTN_COUNT; i++) {
        GPIO_setDir(
            TSW_info[i][0],
            TSW_info[i][1],
            INDIRECTION
        );
    }

    /* 負論理：全ビット1を「どのスイッチも押されていない」とする。 */
    tsw = ~(uint32_t)0;

    /* スイッチ状態はまだ確定していない。 */
    isfix = 0;
}

/**
 * スイッチ状態が確定しているかを返す。
 *
 * @return true：確定済み、false：未確定
 */
bool tactSW_isfix(void)
{
    return (bool)isfix;
}

/**
 * 全スイッチのGPIO入力状態を読み取る。
 *
 * P1.18～P1.23の値とP2.10の値を、1つの32ビット値にまとめて返す。
 * 各ビット位置が重複しないため、論理和で格納できる。
 *
 * @return 全スイッチの未確定入力値
 */
uint32_t tactSW_read(void)
{
    uint32_t switch_data;

    /* P1.18～P1.23 */
    switch_data = GPIO_getValue(PORT1) & 0x00FC0000U;

    /* P2.10（ISP_ENTRY） */
    switch_data |= GPIO_getValue(PORT2) & (1U << BP10);

    return switch_data;
}

/**
 * 確定済みの全スイッチ状態を返す。
 *
 * @return 確定済みスイッチ状態
 */
uint32_t tactSW_getTsw(void)
{
    return tsw;
}

/**
 * 現在のスイッチ状態を確定値として更新する。
 *
 * @param[in] data tactSW_read()で読み取ったスイッチ状態
 */
void tactSW_update(uint32_t data)
{
    tsw = data;
    isfix = 1;
}

/**
 * 指定されたスイッチの状態を返す。
 *
 * 入力回路は負論理だが、戻り値は正論理とする。
 *
 * @param[in] no スイッチ番号（BTN_LEFT～BTN_ENTRY）
 * @return OFF：非押下、ON：押下
 */
unsigned int tactSW_getStatus(int no)
{
    unsigned int ret = OFF;

    if ((0 <= no) && (no < BTN_COUNT)) {
        uint32_t bit;

        bit = (uint32_t)1U << TSW_info[no][1];

        if ((tsw & bit) == 0U) {
            ret = ON;
        }
    }

    return ret;
}
