/**
 * @file Timer.c
 * @brief LPC1763 Timer0割込み制御
 */

#include <stdint.h>

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "Buzzer.h"
#include "Timer.h"

/* システム制御レジスタ */
#define PCONP       (*(volatile uint32_t *)(0x400FC0C4U))
#define PCLKSEL0    (*(volatile uint32_t *)(0x400FC1A8U))

/* Timer0制御レジスタ */
#define T0IR        (*(volatile uint32_t *)(0x40004000U))
#define T0TCR       (*(volatile uint32_t *)(0x40004004U))
#define T0TC        (*(volatile uint32_t *)(0x40004008U))
#define T0PR        (*(volatile uint32_t *)(0x4000400CU))
#define T0PC        (*(volatile uint32_t *)(0x40004010U))
#define T0MCR       (*(volatile uint32_t *)(0x40004014U))
#define T0MR0       (*(volatile uint32_t *)(0x40004018U))

/* レジスタビット */
#define TIMER0_POWER_BIT       (1U)
#define TIMER0_PCLK_BIT        (2U)
#define TIMER_MATCH0_FLAG      (1U << 0)
#define TIMER_MATCH0_INTERRUPT (1U << 0)
#define TIMER_MATCH0_RESET     (1U << 1)
#define TIMER_COUNTER_ENABLE   (1U << 0)
#define TIMER_COUNTER_RESET    (1U << 1)

/* 内部関数 */
static unsigned int get_timer0_pclk(void);
static unsigned int get_prescaler_for_1us(void);

/**
 * Timer0を1マイクロ秒単位で計数できるよう初期化する。
 */
void Timer_init(void)
{
    /* Timer0へクロックを供給する。 */
    PCONP |= (1U << TIMER0_POWER_BIT);

    /* 設定中はカウンタをリセット状態にする。 */
    T0TCR = TIMER_COUNTER_RESET;
    T0TC = 0U;
    T0PC = 0U;

    /* TCが1マイクロ秒ごとに増加するプリスケーラを設定する。 */
    T0PR = get_prescaler_for_1us();

    /* MR0一致時に割込みを発生させ、TCを0へ戻す。 */
    T0MCR = TIMER_MATCH0_INTERRUPT | TIMER_MATCH0_RESET;
    T0MR0 = 1U;

    /* 保留中のTimer0割込み要因を消去する。 */
    T0IR = 0xFFU;
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* 初期状態ではTimer0を停止する。 */
    T0TCR = 0U;
    Buzzer_off();
}

/**
 * 指定した周期でTimer0割込みを開始する。
 */
void Timer_start(unsigned int interval_us)
{
    if (interval_us == 0U) {
        return;
    }

    /* カウンタを停止して0へ戻してから周期を設定する。 */
    T0TCR = TIMER_COUNTER_RESET;
    T0TC = 0U;
    T0PC = 0U;
    T0MR0 = interval_us;
    T0IR = 0xFFU;

    Buzzer_off();

    /* Timer0の計数を開始する。 */
    T0TCR = TIMER_COUNTER_ENABLE;
}

/**
 * Timer0を停止する。
 */
void Timer_stop(void)
{
    T0TCR = 0U;
    T0TC = 0U;
    T0PC = 0U;
    T0IR = 0xFFU;
    Buzzer_off();
}

/**
 * Timer0割込みごとにブザー出力を反転する。
 */
void TIMER0_IRQHandler(void)
{
    if ((T0IR & TIMER_MATCH0_FLAG) != 0U) {
        /* MR0一致割込み要因を消去する。 */
        T0IR = TIMER_MATCH0_FLAG;
        Buzzer_toggle();
    }
}

/**
 * Timer0へ供給される周辺クロック周波数を取得する。
 */
static unsigned int get_timer0_pclk(void)
{
    unsigned int clock_selection;

    clock_selection =
        (PCLKSEL0 >> TIMER0_PCLK_BIT) & 0x03U;

    switch (clock_selection) {
    case 0x01U:
        return SystemCoreClock;

    case 0x02U:
        return SystemCoreClock / 2U;

    case 0x03U:
        return SystemCoreClock / 8U;

    case 0x00U:
    default:
        return SystemCoreClock / 4U;
    }
}

/**
 * TCを1マイクロ秒周期で増加させるプリスケーラ値を求める。
 */
static unsigned int get_prescaler_for_1us(void)
{
    unsigned int timer_pclk;
    unsigned int clocks_per_us;

    timer_pclk = get_timer0_pclk();
    clocks_per_us = timer_pclk / 1000000U;

    if (clocks_per_us == 0U) {
        return 0U;
    }

    return clocks_per_us - 1U;
}
