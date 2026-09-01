
 /* @file mode_clock.c
 * @brief デジタル時計 時計モード処理
 *
 * 第1段階：
 * 変更切替スイッチ（CANCEL／SW_CENTER）による日時設定状態への
 * 進入・終了と、500ms周期の下線カーソル点滅を実装する。
 */

#include "mode_clock.h"
#include "input.h"
#include "LCD.h"
#include "SysTick.h"

/* 10ms割込み50回 = 500ms */
#define CURSOR_BLINK_TIME_10MS  (50U)

/* 秒の1の位（2行目12桁目） */
#define CURSOR_SECOND_X         (12)
#define CURSOR_SECOND_Y         (2)

/* 時計モード内部状態 */
typedef enum {
    CLOCK_DISPLAY_STATE,
    CLOCK_SETTING_STATE
} clock_state_t;

/* 現在の時計モード内部状態 */
static clock_state_t clock_state;

/* 設定画面に表示する日付時刻 */
static datetime_t setting_datetime;

/* カーソル表示状態 */
static bool cursor_visible;

/* 最後にカーソル表示を切り替えた10ms時刻 */
static unsigned int cursor_last_time_10ms;

/* LCDへカーソル状態を反映する必要があるか */
static bool cursor_update_required;

/**
 * 時計モードを通常表示状態で初期化する。
 */
void init_mode_clock(void)
{
    clock_state = CLOCK_DISPLAY_STATE;
    cursor_visible = false;
    cursor_last_time_10ms = get_time_10ms();
    cursor_update_required = true;
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

            /* 設定開始時はカーソルを直ちに表示する。 */
            cursor_visible = true;
            cursor_last_time_10ms = get_time_10ms();
            cursor_update_required = true;
        } else {
            /* 第1段階では値を変更しないため、そのまま通常表示へ戻る。 */
            get_datetime(display_datetime);
            clock_state = CLOCK_DISPLAY_STATE;

            /* 通常表示へ戻るときはカーソルを消す。 */
            cursor_visible = false;
            cursor_update_required = true;
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
    unsigned int now_time_10ms;

    if (clock_state == CLOCK_SETTING_STATE) {
        now_time_10ms = get_time_10ms();

        /* 500ms経過するたびに下線カーソルの表示状態を反転する。 */
        if ((unsigned int)(now_time_10ms - cursor_last_time_10ms) >=
            CURSOR_BLINK_TIME_10MS) {
            cursor_last_time_10ms = now_time_10ms;
            cursor_visible = !cursor_visible;
            cursor_update_required = true;
        }
    }

    if (cursor_update_required == true) {
        if ((clock_state == CLOCK_SETTING_STATE) &&
            (cursor_visible == true)) {
            /* 最初の変更対象は秒の1の位。 */
            LCD_locate(CURSOR_SECOND_X, CURSOR_SECOND_Y);
            LCD_cursor_on();
        } else {
            LCD_cursor_off();
        }

        cursor_update_required = false;
    }
}

/******************************************************************************
 * End Of File
 ******************************************************************************/
