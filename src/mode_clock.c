/**
 * @file mode_clock.c
 * @brief デジタル時計 時計モード処理
 *
 * 第1段階：
 * 変更切替スイッチ（OK／SW1）による日時設定状態への
 * 進入・終了と、LCDカーソルの表示を実装する。
 */

#include "mode_clock.h"
#include "input.h"
#include "LCD.h"

/* 時計モード内部状態 */
typedef enum {
    CLOCK_DISPLAY_STATE,
    CLOCK_SETTING_STATE
} clock_state_t;

/* 現在の時計モード内部状態 */
static clock_state_t clock_state;

/* 設定画面に表示する日付時刻 */
static datetime_t setting_datetime;

/**
 * 時計モードを通常表示状態で初期化する。
 */
void init_mode_clock(void)
{
    clock_state = CLOCK_DISPLAY_STATE;
}

/**
 * 時計モードの入力を処理する。
 */
bool mode_clock(datetime_t *display_datetime)
{
    bool update_required = false;

    if (display_datetime == 0) {
        return false;
    }

    if (clicked_FunctionSW() == true) {
        if (clock_state == CLOCK_DISPLAY_STATE) {
            /* 現在日時を設定用データへコピーして設定を開始する。 */
            get_datetime(&setting_datetime);
            *display_datetime = setting_datetime;
            clock_state = CLOCK_SETTING_STATE;
        } else {
            /* 第1段階では値を変更しないため、そのまま通常表示へ戻る。 */
            get_datetime(display_datetime);
            clock_state = CLOCK_DISPLAY_STATE;
        }

        update_required = true;
    }

    return update_required;
}

/**
 * 日付時刻設定中かを返す。
 */
bool is_clock_setting(void)
{
    return (clock_state == CLOCK_SETTING_STATE);
}

/**
 * 時計モード内部状態に合わせてLCDカーソルを更新する。
 */
void update_clock_cursor(void)
{
    if (clock_state == CLOCK_SETTING_STATE) {
        /* 最初の変更対象は秒の1の位（2行目12桁目）。 */
        LCD_cursor_on();
        LCD_locate(12, 2);
    } else {
        LCD_cursor_off();
    }
}

/******************************************************************************
 * End Of File
 ******************************************************************************/
