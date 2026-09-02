/**
 * @file mode_clock.h
 * @brief 時計モード制御インターフェース
 */

#ifndef MODE_CLOCK_H_
#define MODE_CLOCK_H_

#include <stdbool.h>
#include "datetime.h"

/** 時計モードの状態を初期化する。 */
void init_mode_clock(void);

/*
 * 時計モードのスイッチ入力を処理する。

 * @param[out] display_datetime LCDへ表示する日付時刻
 * @return LCDの日付時刻を再表示する場合はtrue、それ以外はfalse
 */
bool mode_clock(datetime_t *display_datetime);

/*
 * 日付時刻設定中かを取得する。

 * @return 設定中の場合はtrue、それ以外はfalse
 */
bool is_clock_setting(void);

/*
 時計モードから移行する際に日付時刻設定を終了する。
 変更後の値を保存し、LCDカーソルを消去する。
 */
void exit_clock_setting(void);

/*
 * 時計モードの状態に合わせてLCDカーソルを更新する。

 * メインループで毎回呼び出す。設定中は下線カーソルのみを
 * 500ms表示、500ms非表示で点滅させる。
 */
void update_clock_cursor(void);

#endif /* MODE_CLOCK_H_ */
