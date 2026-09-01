/**
 * @file mode_stopwatch.h
 * @brief ストップウォッチモード制御インターフェース
 */

#ifndef MODE_STOPWATCH_H_
#define MODE_STOPWATCH_H_

#include <stdbool.h>

/* ストップウォッチの動作状態 */
typedef enum {
    STOPWATCH_STOP,
    STOPWATCH_RUN
} stopwatch_state_t;

/* ストップウォッチ表示情報 */
typedef struct {
    unsigned int minute;
    unsigned int second;
    unsigned int centisecond;
    stopwatch_state_t state;
} stopwatch_t;

/** ストップウォッチを停止状態かつ計測時間0で初期化する。 */
void init_mode_stopwatch(void);

/**
 * ストップウォッチモードのスイッチ入力を処理する。
 *
 * @return LCD表示の更新が必要な場合はtrue、それ以外はfalse
 */
bool mode_stopwatch(void);

/**
 * 経過した10ms割込み回数に従って計測時間を更新する。
 *
 * 表示モードに関係なく、メインループから毎回呼び出すこと。
 *
 * @param[in] elapsed_10ms 前回確認時から経過した10ms割込み回数
 * @return 計測時間を更新した場合はtrue、それ以外はfalse
 */
bool update_stopwatch(unsigned int elapsed_10ms);

/**
 * 現在のストップウォッチ表示情報を取得する。
 *
 * @param[out] stopwatch ストップウォッチ表示情報の格納先
 */
void get_stopwatch(stopwatch_t *stopwatch);

#endif /* MODE_STOPWATCH_H_ */
