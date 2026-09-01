/**
 * @file mode_stopwatch.c
 * @brief ストップウォッチモード制御
 */

#include "mode_stopwatch.h"
#include "input.h"

/* 60分を10ms単位で表した計測時間上限 */
#define STOPWATCH_LIMIT_10MS  (60U * 60U * 100U)

/* 現在の計測時間を10ms単位で保持する */
static unsigned int stopwatch_time_10ms;

/* 現在の動作状態 */
static stopwatch_state_t stopwatch_state;

/**
 * ストップウォッチを初期化する。
 */
void init_mode_stopwatch(void)
{
    stopwatch_time_10ms = 0U;
    stopwatch_state = STOPWATCH_STOP;
}

/**
 * ストップウォッチモードのスイッチ入力を処理する。
 */
bool mode_stopwatch(void)
{
    /* 増加スイッチを押すたびに開始と停止を切り替える */
    if (clicked_UpSW() == true) {
        if (stopwatch_state == STOPWATCH_STOP) {
            stopwatch_state = STOPWATCH_RUN;
        } else {
            stopwatch_state = STOPWATCH_STOP;
        }

        return true;
    }

    /* 停止中に減少スイッチを押した場合だけ計測時間をクリアする */
    if (clicked_DownSW() == true) {
        if (stopwatch_state == STOPWATCH_STOP) {
            stopwatch_time_10ms = 0U;
            return true;
        }
    }

    return false;
}

/**
 * ストップウォッチの計測時間を更新する。
 */
bool update_stopwatch(unsigned int elapsed_10ms)
{
    unsigned int additional_time;

    if ((stopwatch_state != STOPWATCH_RUN) ||
        (elapsed_10ms == 0U)) {
        return false;
    }

    /* 1時間以上の経過分を先に除き、加算時の桁あふれを防止する */
    additional_time = elapsed_10ms % STOPWATCH_LIMIT_10MS;

    stopwatch_time_10ms =
        (stopwatch_time_10ms + additional_time) %
        STOPWATCH_LIMIT_10MS;

    return true;
}

/**
 * 現在のストップウォッチ表示情報を取得する。
 */
void get_stopwatch(stopwatch_t *stopwatch)
{
    unsigned int total_seconds;

    if (stopwatch == 0) {
        return;
    }

    total_seconds = stopwatch_time_10ms / 100U;

    stopwatch->minute = total_seconds / 60U;
    stopwatch->second = total_seconds % 60U;
    stopwatch->centisecond = stopwatch_time_10ms % 100U;
    stopwatch->state = stopwatch_state;
}

/******************************************************************************
 * ファイル終端
 ******************************************************************************/
