/**
 * @file mode_alarm.c
 * @brief アラームモード制御
 */

#include "mode_alarm.h"
#include "input.h"
#include "LCD.h"
#include "LED.h"
#include "SysTick.h"
//#include "Timer.h"
#include "Melody.h"

/* 10msタイマ50回で500msとする */
#define CURSOR_BLINK_TIME_10MS       (50U)

/* 125マイクロ秒ごとに出力反転し、約4kHzの方形波とする */
//#define ALARM_HALF_PERIOD_US         (125U)

/* アラーム鳴動時間 */
#define ALARM_DURATION_SECONDS       (60U)

typedef enum {
    ALARM_DISPLAY_STATE,
    ALARM_SETTING_STATE
} alarm_state_t;

/* 仕様書の移動順序に合わせ、分、時の順に並べる */
typedef enum {
    ALARM_ITEM_MINUTE,
    ALARM_ITEM_HOUR,
    ALARM_ITEM_COUNT
} alarm_setting_item_t;

/* 分の1の位、時の1の位のカーソル位置 */
static const int cursor_x[ALARM_ITEM_COUNT] = { 11, 8 };

/* 保存済みアラーム時刻 */
static unsigned int alarm_hour;
static unsigned int alarm_minute;

/* 設定画面で編集中のアラーム時刻 */
static unsigned int setting_hour;
static unsigned int setting_minute;

/* アラームモード内部状態 */
static alarm_state_t alarm_state;
static alarm_setting_item_t setting_item;

/* アラーム有効状態および鳴動状態 */
static bool alarm_enabled;
static bool alarm_sounding;
static unsigned int alarm_elapsed_seconds;

/* カーソル点滅管理 */
static bool cursor_visible;
static unsigned int cursor_last_time_10ms;
static bool cursor_update_required;

/* 内部関数 */
static void restart_cursor_blink(void);
static void move_setting_item(void);
static void change_setting_value(int direction);
static unsigned int change_cyclic_value(unsigned int value,
                                        unsigned int minimum,
                                        unsigned int maximum,
                                        int direction);
static void start_alarm(void);
static void stop_alarm(void);

/**
 * アラームを初期化する。
 */
void init_mode_alarm(void)
{
    alarm_hour = 0U;
    alarm_minute = 0U;
    setting_hour = 0U;
    setting_minute = 0U;

    alarm_state = ALARM_DISPLAY_STATE;
    setting_item = ALARM_ITEM_MINUTE;

    alarm_enabled = false;
    alarm_sounding = false;
    alarm_elapsed_seconds = 0U;

    cursor_visible = false;
    cursor_last_time_10ms = get_time_10ms();
    cursor_update_required = false;

    //Timer_stop();
    Melody_init();
    LED_off(LED8);
    LED_off(LED9);
}

/**
 * アラームモードの入力を処理する。
 */
bool mode_alarm(void)
{
    bool update_required = false;

    /* 変更切替スイッチで設定開始と設定終了を切り替える */
    if (clicked_FunctionSW() == true) {
        if (alarm_state == ALARM_DISPLAY_STATE) {
            setting_hour = alarm_hour;
            setting_minute = alarm_minute;
            alarm_state = ALARM_SETTING_STATE;
            setting_item = ALARM_ITEM_MINUTE;
            restart_cursor_blink();
        } else {
            exit_alarm_setting();
        }

        return true;
    }

    if (alarm_state == ALARM_SETTING_STATE) {
        /* 設定項目は分と時の2項目なので、左右のどちらでも交互に移動する */
        if ((clicked_LeftSW() == true) ||
            (clicked_RightSW() == true)) {
            move_setting_item();
        }

        if (clicked_UpSW() == true) {
            change_setting_value(1);
            update_required = true;
        }

        if (clicked_DownSW() == true) {
            change_setting_value(-1);
            update_required = true;
        }
    }

    return update_required;
}

/**
 * 目覚ましスイッチでアラーム有効状態を切り替える。
 */
bool update_alarm_enable(void)
{
    if (clicked_AlarmSW() == false) {
        return false;
    }

    alarm_enabled = !alarm_enabled;

    if (alarm_enabled == true) {
        LED_on(LED9);
    } else {
        LED_off(LED9);

        /* 無効へ変更した場合は鳴動中のアラームも停止する */
        stop_alarm();
    }

    return true;
}

/**
 * 現在時刻に従ってアラーム鳴動状態を更新する。
 */
void update_alarm(const datetime_t *current_datetime)
{
    if (current_datetime == 0) {
        return;
    }

    if (alarm_sounding == true) {
        alarm_elapsed_seconds++;

        if (alarm_elapsed_seconds >= ALARM_DURATION_SECONDS) {
            stop_alarm();
        }

        return;
    }

    /* アラーム時刻の0秒で鳴動を開始する */
    if ((alarm_enabled == true) &&
        ((unsigned int)current_datetime->hour == alarm_hour) &&
        ((unsigned int)current_datetime->minute == alarm_minute) &&
        (current_datetime->second == 0)) {
        start_alarm();
    }
}

/**
 * 10ms経過時間に従ってアラームメロディを更新する。
 *
 * @param[in] elapsed_10ms 経過した10ms割込み回数
 */
void update_alarm_10ms(unsigned int elapsed_10ms)
{
    if (alarm_sounding == true) {
        Melody_update(elapsed_10ms);
    }
}

/**
 * 現在のアラーム表示情報を取得する。
 */
void get_alarm(alarm_info_t *alarm_info)
{
    if (alarm_info == 0) {
        return;
    }

    if (alarm_state == ALARM_SETTING_STATE) {
        alarm_info->hour = setting_hour;
        alarm_info->minute = setting_minute;
    } else {
        alarm_info->hour = alarm_hour;
        alarm_info->minute = alarm_minute;
    }

    alarm_info->enabled = alarm_enabled;
    alarm_info->sounding = alarm_sounding;
}

/**
 * アラーム時刻設定中かを取得する。
 */
bool is_alarm_setting(void)
{
    return (alarm_state == ALARM_SETTING_STATE);
}

/**
 * アラーム時刻設定を終了し、編集値を保存する。
 */
void exit_alarm_setting(void)
{
    if (alarm_state == ALARM_SETTING_STATE) {
        alarm_hour = setting_hour;
        alarm_minute = setting_minute;
        alarm_state = ALARM_DISPLAY_STATE;
    }

    cursor_visible = false;
    cursor_update_required = false;
    LCD_cursor_off();
}

/**
 * LCD再表示後のカーソル再設定を要求する。
 */
void request_alarm_cursor_update(void)
{
    if (alarm_state == ALARM_SETTING_STATE) {
        cursor_update_required = true;
    }
}

/**
 * 下線カーソルを500ms表示、500ms非表示で点滅させる。
 */
void update_alarm_cursor(void)
{
    unsigned int now_time_10ms;

    if (alarm_state == ALARM_SETTING_STATE) {
        now_time_10ms = get_time_10ms();

        if ((unsigned int)(now_time_10ms - cursor_last_time_10ms) >=
            CURSOR_BLINK_TIME_10MS) {
            cursor_last_time_10ms = now_time_10ms;
            cursor_visible = !cursor_visible;
            cursor_update_required = true;
        }
    }

    if (cursor_update_required == true) {
        if ((alarm_state == ALARM_SETTING_STATE) &&
            (cursor_visible == true)) {
            LCD_locate(cursor_x[setting_item], 1);
            LCD_cursor_on();
        } else {
            LCD_cursor_off();
        }

        cursor_update_required = false;
    }
}

/**
 * カーソルを直ちに表示し、500msの計時を再開する。
 */
static void restart_cursor_blink(void)
{
    cursor_visible = true;
    cursor_last_time_10ms = get_time_10ms();
    cursor_update_required = true;
}

/**
 * 選択中の設定項目を分と時の間で切り替える。
 */
static void move_setting_item(void)
{
    if (setting_item == ALARM_ITEM_MINUTE) {
        setting_item = ALARM_ITEM_HOUR;
    } else {
        setting_item = ALARM_ITEM_MINUTE;
    }

    restart_cursor_blink();
}

/**
 * 選択中のアラーム時刻を増減する。
 */
static void change_setting_value(int direction)
{
    if (setting_item == ALARM_ITEM_MINUTE) {
        setting_minute = change_cyclic_value(
            setting_minute, 0U, 59U, direction
        );
    } else {
        setting_hour = change_cyclic_value(
            setting_hour, 0U, 23U, direction
        );
    }

    restart_cursor_blink();
}

/**
 * 指定範囲内で値を循環させて変更する。
 */
static unsigned int change_cyclic_value(unsigned int value,
                                        unsigned int minimum,
                                        unsigned int maximum,
                                        int direction)
{
    if (direction > 0) {
        if (value >= maximum) {
            return minimum;
        }

        return value + 1U;
    }

    if (value <= minimum) {
        return maximum;
    }

    return value - 1U;
}

/**
 * LED8を点灯し、ブザー鳴動を開始する。
 */
static void start_alarm(void)
{
    alarm_sounding = true;
    alarm_elapsed_seconds = 0U;
    LED_on(LED8);
//    Timer_start(ALARM_HALF_PERIOD_US);
    Melody_start();
}

/**
 * LED8を消灯し、ブザー鳴動を停止する。
 */
static void stop_alarm(void)
{
    alarm_sounding = false;
    alarm_elapsed_seconds = 0U;
    LED_off(LED8);
//    Timer_stop();
    Melody_stop();
}

