/**
 * @file Timer.h
 * @brief Timer0制御インターフェース
 */

#ifndef TIMER_H_
#define TIMER_H_

/* Timer0を初期化し、停止状態にする。 */
void Timer_init(void);

/*
 指定した周期でTimer0割込みを開始する。

 interval_us 割込み周期（マイクロ秒）
 */
void Timer_start(unsigned int interval_us);

/* Timer0割込みを停止し、ブザー出力をLOWにする。 */
void Timer_stop(void);

/* Timer0割込みサービスルーチン */
void TIMER0_IRQHandler(void);

#endif /* TIMER_H_ */
