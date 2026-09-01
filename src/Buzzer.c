/**
 * @file Buzzer.c
 * @brief ブザーGPIO制御
 */

#include <stdbool.h>

#include "PINCTRL.h"
#include "general.h"
#include "GPIO.h"
#include "Buzzer.h"

/* ブザー接続端子：P4.28 */
#define BUZZER_PORT  (PORT4)
#define BUZZER_BIT   (BP28)

/* 現在のブザー出力状態 */
static volatile bool buzzer_output_high;

/**
 * ブザー端子をGPIO出力として初期化する。
 */
void Buzzer_setup(void)
{
    /* P4.28をGPIO機能に設定する */
    PINSEL9 &= ~(3U << BP24);

    /* 出力方向へ変更する前にLOWを設定する */
    buzzer_output_high = false;
    GPIO_setValueBit(BUZZER_PORT, BUZZER_BIT, LOW);
    GPIO_setDir(BUZZER_PORT, BUZZER_BIT, OUTDIRECTION);
}

/**
 * ブザー出力をHIGHにする。
 */
void Buzzer_on(void)
{
    buzzer_output_high = true;
    GPIO_setValueBit(BUZZER_PORT, BUZZER_BIT, HIGH);
}

/**
 * ブザー出力をLOWにする。
 */
void Buzzer_off(void)
{
    buzzer_output_high = false;
    GPIO_setValueBit(BUZZER_PORT, BUZZER_BIT, LOW);
}

/**
 * ブザー出力を反転する。
 */
void Buzzer_toggle(void)
{
    if (buzzer_output_high == true) {
        Buzzer_off();
    } else {
        Buzzer_on();
    }
}

