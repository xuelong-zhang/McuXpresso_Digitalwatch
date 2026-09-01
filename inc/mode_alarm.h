/**
 * @file mode_alarm.h
 * @brief アラームモード制御インターフェース
 */

#ifndef MODE_ALARM_H_
#define MODE_ALARM_H_

#include <stdbool.h>

#include "datetime.h"

/* アラーム表示情報 */
typedef struct {
    unsigned int hour;
    unsigned int minute;
    bool enabled;
    bool sounding;
} alarm_info_t;

/** アラームを0時00分、無効状態で初期化する。 */
void init_mode_alarm(void);

/**
 * アラームモードのスイッチ入力を処理する。
 *
 * @return LCD表示の更新が必要な場合はtrue、それ以外はfalse
 */
bool mode_alarm(void);

/**
 * 目覚ましスイッチを処理し、アラーム有効状態を切り替える。
 *
 * 表示モードに関係なく、メインループから毎回呼び出すこと。
 *
 * @return 有効状態を変更した場合はtrue、それ以外はfalse
 */
bool update_alarm_enable(void);

/**
 * 現在時刻とアラーム時刻を比較し、鳴動時間を更新する。
 *
 * 日付時刻を1秒進めるたびに1回呼び出すこと。
 *
 * @param[in] current_datetime 現在の日付時刻
 */
void update_alarm(const datetime_t *current_datetime);

/**
 * 現在のアラーム表示情報を取得する。
 *
 * @param[out] alarm_info アラーム表示情報の格納先
 */
void get_alarm(alarm_info_t *alarm_info);

/**
 * アラーム時刻設定中かを取得する。
 *
 * @return 設定中の場合はtrue、それ以外はfalse
 */
bool is_alarm_setting(void);

/** アラームモードから移行する際に時刻設定を終了する。 */
void exit_alarm_setting(void);

/** LCD再表示後にカーソル位置を再設定するよう要求する。 */
void request_alarm_cursor_update(void);

/** アラーム設定カーソルを500ms表示、500ms非表示で点滅させる。 */
void update_alarm_cursor(void);

#endif /* MODE_ALARM_H_ */
