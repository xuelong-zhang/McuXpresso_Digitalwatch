/**
 * @file mode_clock.c
 * @brief 時計モード制御
 */

#include "mode_clock.h"
#include "input.h"
#include "LCD.h"
#include "SysTick.h"

/* 10msタイマ50回で500msとする */
#define CURSOR_BLINK_TIME_10MS  (50U)

/* 時計設定画面で変更可能な年範囲 */
#define CLOCK_SETTING_YEAR_MIN  (2018)
#define CLOCK_SETTING_YEAR_MAX  (2100)

typedef enum {
    CLOCK_DISPLAY_STATE,
    CLOCK_SETTING_STATE
} clock_state_t;

/* 設定項目はLCD上の表示順に並べる */
typedef enum {
    CLOCK_ITEM_YEAR,
    CLOCK_ITEM_MONTH,
    CLOCK_ITEM_DAY,
    CLOCK_ITEM_HOUR,
    CLOCK_ITEM_MINUTE,
    CLOCK_ITEM_SECOND,
    CLOCK_ITEM_COUNT
} clock_setting_item_t;

/* 年、月、日、時、分、秒のカーソル位置 */
static const int cursor_x[CLOCK_ITEM_COUNT] = { 4, 7, 10, 6, 9, 12 };
static const int cursor_y[CLOCK_ITEM_COUNT] = { 1, 1, 1, 2, 2, 2 };

static clock_state_t clock_state;
static clock_setting_item_t setting_item;
static datetime_t setting_datetime;
static bool setting_modified;
static bool cursor_visible;
static unsigned int cursor_last_time_10ms;
static bool cursor_update_required;

/* 内部関数 */
static void restart_cursor_blink(void);
static void move_setting_item(int direction);
static void change_setting_value(int direction);
static int change_cyclic_value(int value, int minimum, int maximum,
                               int direction);

/*
 時計モードを初期化する
 */
void init_mode_clock(void)
{
    clock_state = CLOCK_DISPLAY_STATE;
    setting_item = CLOCK_ITEM_SECOND;
    setting_modified = false;
    cursor_visible = false;
    cursor_last_time_10ms = get_time_10ms();
    cursor_update_required = true;
}

/*
  時計モードの入力を処理する
 */
bool mode_clock(datetime_t *display_datetime)
{
    bool update_required = false;

    if (display_datetime == 0) {
        return false;
    }

    if (clicked_FunctionSW() == true) {
        if (clock_state == CLOCK_DISPLAY_STATE) {
            get_datetime(&setting_datetime);
            *display_datetime = setting_datetime;
            clock_state = CLOCK_SETTING_STATE;
            setting_item = CLOCK_ITEM_SECOND;
            setting_modified = false;
            restart_cursor_blink();
        } else {
            exit_clock_setting();
            get_datetime(display_datetime);
        }

        return true;
    }

    if (clock_state == CLOCK_SETTING_STATE) {
        if (clicked_LeftSW() == true) {
            move_setting_item(-1);
        }

        if (clicked_RightSW() == true) {
            move_setting_item(1);
        }

        if (clicked_UpSW() == true) {
            change_setting_value(1);
            setting_modified = true;
            *display_datetime = setting_datetime;
            update_required = true;
        }

        if (clicked_DownSW() == true) {
            change_setting_value(-1);
            setting_modified = true;
            *display_datetime = setting_datetime;
            update_required = true;
        }
    }

    return update_required;
}

/*
  日付時刻設定中かを取得
 */
bool is_clock_setting(void)
{
    return (clock_state == CLOCK_SETTING_STATE);
}

/*
  時計モードから移行する前に設定を終了する
 */
void exit_clock_setting(void)
{
    if (clock_state == CLOCK_SETTING_STATE) {
        /* 値を変更していない場合は、通常時計の進行を維持する */
        if (setting_modified == true) {
            set_datetime(&setting_datetime);
        }

        clock_state = CLOCK_DISPLAY_STATE;
        setting_modified = false;
    }

    cursor_visible = false;
    cursor_update_required = false;
    LCD_cursor_off();
}

/*
  下線カーソルを更新し、500ms周期でソフトウェア点滅させる
 */
void update_clock_cursor(void)
{
    unsigned int now_time_10ms;

    if (clock_state == CLOCK_SETTING_STATE) {
        now_time_10ms = get_time_10ms();

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
            LCD_locate(cursor_x[setting_item], cursor_y[setting_item]);
            LCD_cursor_on();
        } else {
            LCD_cursor_off();
        }

        cursor_update_required = false;
    }
}

/*
  カーソルを直ちに表示し、500msの計時を再開する
 */
static void restart_cursor_blink(void)
{
    cursor_visible = true;
    cursor_last_time_10ms = get_time_10ms();
    cursor_update_required = true;
}

/*
  選択中の設定項目を移動する。
  directionが0より大きい場合は右、0より小さい場合は左へ移動
 */
static void move_setting_item(int direction)
{
    if (direction > 0) {
        if (setting_item >= (CLOCK_ITEM_COUNT - 1)) {
            setting_item = CLOCK_ITEM_YEAR;
        } else {
            setting_item++;
        }
    } else {
        if (setting_item <= CLOCK_ITEM_YEAR) {
            setting_item = CLOCK_ITEM_SECOND;
        } else {
            setting_item--;
        }
    }

    restart_cursor_blink();
}

/*
  選択中の日付時刻の値を増減する。
  directionが0より大きい場合は加算、0より小さい場合は減算する
 */
static void change_setting_value(int direction)
{
    int maximum_day;

    switch (setting_item) {
    case CLOCK_ITEM_YEAR:
        setting_datetime.year = change_cyclic_value(
            setting_datetime.year,
            CLOCK_SETTING_YEAR_MIN,
            CLOCK_SETTING_YEAR_MAX,
            direction
        );
        break;

    case CLOCK_ITEM_MONTH:
        setting_datetime.month = change_cyclic_value(
            setting_datetime.month, 1, 12, direction
        );
        break;

    case CLOCK_ITEM_DAY:
        maximum_day = get_last_day(
            setting_datetime.year,
            setting_datetime.month
        );
        setting_datetime.day = change_cyclic_value(
            setting_datetime.day, 1, maximum_day, direction
        );
        break;

    case CLOCK_ITEM_HOUR:
        setting_datetime.hour = change_cyclic_value(
            setting_datetime.hour, 0, 23, direction
        );
        break;

    case CLOCK_ITEM_MINUTE:
        setting_datetime.minute = change_cyclic_value(
            setting_datetime.minute, 0, 59, direction
        );
        break;

    case CLOCK_ITEM_SECOND:
        setting_datetime.second = change_cyclic_value(
            setting_datetime.second, 0, 59, direction
        );
        break;

    default:
        break;
    }

    /* 年月変更後の日を有効範囲へ補正し、曜日を更新する */
    normalize_datetime(&setting_datetime);
    restart_cursor_blink();
}

/*
  指定範囲内で値を循環させて変更する
 */
static int change_cyclic_value(int value, int minimum, int maximum,
                               int direction)
{
    if (direction > 0) {
        if (value >= maximum) {
            return minimum;
        }
        return value + 1;
    }

    if (value <= minimum) {
        return maximum;
    }
    return value - 1;
}



