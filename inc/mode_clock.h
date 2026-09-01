/**
 * @file mode_clock.h
 * @brief デジタル時計 時計モード処理ヘッダ
 */

#ifndef MODE_CLOCK_H_
#define MODE_CLOCK_H_

#include <stdbool.h>
#include "datetime.h"

/** 時計モードの状態を初期化する。 */
void init_mode_clock(void);

/**
 * 時計モードの入力を処理する。
 *
 * @param[out] display_datetime LCDに表示する日付時刻
 * @return true：表示とカーソルの更新が必要、false：更新不要
 */
bool mode_clock(datetime_t *display_datetime);

/**
 * 日付時刻設定中かを返す。
 *
 * @return true：設定中、false：通常表示中
 */
bool is_clock_setting(void);

/** 現在の時計状態に合わせてLCDカーソルを更新する。 */
void update_clock_cursor(void);

#endif /* MODE_CLOCK_H_ */
