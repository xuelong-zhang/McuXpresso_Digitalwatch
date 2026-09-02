/**
 * @file LCDcounter.c
 * @brief デジタル時計 メイン処理およびLCD表示処理
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "LED.h"
#include "LCD.h"
#include "tactSW.h"
#include "SysTick.h"
#include "clib.h"
#include "datetime.h"
#include "input.h"
#include "mode_clock.h"
#include "mode_manager.h"
#include "mode_stopwatch.h"
#include "mode_alarm.h"
#include "Buzzer.h"
#include "Timer.h"

/* 曜日表示文字列 */
static const char *const weekday_text[] = {
    "SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT"
};

/* 内部関数 */
static void SYS_init(void);
static void disp_datetime(const datetime_t *datetime);
static void disp_stopwatch(const stopwatch_t *stopwatch);
static void disp_alarm(const alarm_info_t *alarm_info,
                       const datetime_t *current_datetime);
static void disp_mode_screen(system_mode_t mode, datetime_t *display_datetime);

/**
 * メイン処理
 */
int main(void)
{
    datetime_t display_datetime;
    datetime_t current_datetime;
    stopwatch_t display_stopwatch;
    alarm_info_t display_alarm;
    system_mode_t previous_mode;

    int error_no = 0;
    int err_lcd = 0;
    int datetime_updated;
    int stopwatch_updated;
    int alarm_enable_updated;
    unsigned int chk_10ms;
    unsigned int elapsed_10ms;
    unsigned int past1sec = 0;

    SYS_init();
    init_datetime();
    init_mode_clock();
    init_mode_manager();
    init_mode_stopwatch();
    init_mode_alarm();

    chk_10ms = get_time_10ms();

    LCD_cls();
    get_datetime(&display_datetime);
    disp_datetime(&display_datetime);

    while (1) {
        /* LCDリングバッファのデータを1つ処理する */
        err_lcd = LCD_display();

        /* スイッチ状態はメインループごとに1回だけ更新する */
        update_switch();

        /* モード切替前のモードを保存する */
        previous_mode = get_current_mode();

        /* OK／SW1で表示モードを切り替える */
        if (update_mode_manager() == true) {
            /* 時計モードから移行する場合は日付時刻設定を終了する */
        	if (previous_mode == MODE_CLOCK) {
        	    exit_clock_setting();
        	} else if (previous_mode == MODE_ALARM) {
        	    /* アラームモードから移行する場合は時刻設定を終了する */
        	    exit_alarm_setting();
        	}

            disp_mode_screen(
                get_current_mode(),
                &display_datetime
            );
        }

        /* 目覚ましスイッチは表示モードに関係なく処理する */
        alarm_enable_updated = update_alarm_enable();

        if ((alarm_enable_updated != 0) &&
            (get_current_mode() == MODE_ALARM)) {
            get_alarm(&display_alarm);
            get_datetime(&current_datetime);
            disp_alarm(&display_alarm, &current_datetime);
            request_alarm_cursor_update();
        }

        /* 現在の表示モードに対応する処理だけを実行する */
        switch (get_current_mode()) {
        case MODE_CLOCK:
            if (mode_clock(&display_datetime) == true) {
                disp_datetime(&display_datetime);
            }
            update_clock_cursor();
            break;

        case MODE_STOPWATCH:
            if (mode_stopwatch() == true) {
                get_stopwatch(&display_stopwatch);
                disp_stopwatch(&display_stopwatch);
            }
            break;

        case MODE_ALARM:
            if (mode_alarm() == true) {
                get_alarm(&display_alarm);
                get_datetime(&current_datetime);
                disp_alarm(&display_alarm, &current_datetime);
                request_alarm_cursor_update();
            }

            update_alarm_cursor();
            break;

        default:
            break;
        }

        /* LCD制御エラーを保存し、リングバッファを初期化する */
        if (err_lcd != 0) {
            if (error_no == 0) {
                error_no = err_lcd;
            }
            LCD_reset_lcdbuf();
        }

        switch (error_no) {
        case ERR_TOUT:
        case ERR_BFULL:
        case ERR_ESC:
            LED_on(error_no);
            break;

        default:
            LED_off(LED1);
            LED_off(LED2);
            LED_off(LED3);
            LED_off(LED4);
            LED_off(LED5);
            LED_off(LED6);
            LED_off(LED7);
            LED_off(LED10);
            LED_off(LED11);
            LED_off(LED12);
            LED_off(LED13);
            LED_off(LED14);

            break;
        }


        /* 前回確認時から経過した10ms割込み回数を取得する */
        elapsed_10ms = get_time_10ms() - chk_10ms;

        if (elapsed_10ms != 0U) {
            chk_10ms += elapsed_10ms;
            past1sec += elapsed_10ms;
            datetime_updated = 0;

            /* ストップウォッチは表示モードに関係なく更新する */
            stopwatch_updated = update_stopwatch(elapsed_10ms);

            /* アラームメロディは表示モードに関係なく更新する */
            update_alarm_10ms(elapsed_10ms);

            /* 10ms割込み100回ごとに日付時刻を1秒進める */
            while (past1sec >= 100U) {
                past1sec -= 100U;
                update_datetime();

                /* アラーム判定と鳴動時間は1秒ごとに更新する */
                get_datetime(&current_datetime);
                update_alarm(&current_datetime);

                datetime_updated = 1;
            }

            /* 日付時刻は、表示モードに関係なく常に更新する */
            if ((datetime_updated != 0) &&
                (get_current_mode() == MODE_CLOCK) &&
                (is_clock_setting() == false)) {
                /* 通常の時計表示中だけLCD表示を更新する */
                get_datetime(&display_datetime);
                disp_datetime(&display_datetime);
            }

            /* ストップウォッチ表示中は計測時間を10msごとに更新する */
            if ((stopwatch_updated != 0) &&
                (get_current_mode() == MODE_STOPWATCH)) {
                get_stopwatch(&display_stopwatch);
                disp_stopwatch(&display_stopwatch);
            }

            /* アラームモードでは現在時刻の時・分を更新する */
            if ((datetime_updated != 0) &&
                (get_current_mode() == MODE_ALARM)) {
                get_alarm(&display_alarm);
                get_datetime(&current_datetime);
                disp_alarm(&display_alarm, &current_datetime);
                request_alarm_cursor_update();
            }
        }
    }

    return 0;
}

/*
 * 日付時刻をLCD表示バッファへ格納する

 * 表示する日付時刻
 */
static void disp_datetime(const datetime_t *datetime)
{
    char number_string[6];

    /* LCDの1行目を表示する */
    LCD_locate(1, 1);

    /* 年を4桁のゼロ埋めで表示する */
    (void)LCD_puts(
        myitoa(datetime->year, number_string, 4, '0')
    );
    (void)LCD_putchar('/');

    /* 月を2桁分の領域へ右寄せで表示する */
    (void)LCD_puts(
        myitoa(datetime->month, number_string, 2, ' ')
    );
    (void)LCD_putchar('/');

    /* 日を2桁分の領域へ右寄せで表示する */
    (void)LCD_puts(
        myitoa(datetime->day, number_string, 2, ' ')
    );
    (void)LCD_putchar('(');

    /* 曜日を3文字で表示する */
    (void)LCD_puts(weekday_text[datetime->weekday]);
    (void)LCD_putchar(')');

    /* 1行を16文字にする */
    (void)LCD_putchar(' ');

    /* LCDの2行目を表示する */
    LCD_locate(1, 2);

    /* 時刻を中央付近へ配置する */
    (void)LCD_puts("    ");

    /* 時を2桁分の領域へ右寄せで表示する */
    (void)LCD_puts(
        myitoa(datetime->hour, number_string, 2, ' ')
    );
    (void)LCD_putchar(':');

    /* 分を2桁のゼロ埋めで表示する */
    (void)LCD_puts(
        myitoa(datetime->minute, number_string, 2, '0')
    );
    (void)LCD_putchar(':');

    /* 秒を2桁のゼロ埋めで表示する */
    (void)LCD_puts(
        myitoa(datetime->second, number_string, 2, '0')
    );

    /* 1行を16文字にする */
    (void)LCD_puts("    ");
}

/*
 * ストップウォッチの状態と計測時間をLCD表示バッファへ格納する。

 * stopwatch 表示するストップウォッチ情報
 */
static void disp_stopwatch(const stopwatch_t *stopwatch)
{
    char number_string[4];

    /* LCDの1行目へ動作状態を表示する */
    LCD_locate(1, 1);

    if (stopwatch->state == STOPWATCH_RUN) {
        (void)LCD_puts("STOPWATCH RUN   ");
    } else {
        (void)LCD_puts("STOPWATCH STOP  ");
    }

    /* LCDの2行目へ分、秒、百分秒を表示する */
    LCD_locate(1, 2);
    (void)LCD_puts("    ");

    (void)LCD_puts(
        myitoa((int)stopwatch->minute, number_string, 2, '0')
    );
    (void)LCD_putchar(':');

    (void)LCD_puts(
        myitoa((int)stopwatch->second, number_string, 2, '0')
    );
    (void)LCD_putchar(':');

    (void)LCD_puts(
        myitoa((int)stopwatch->centisecond, number_string, 2, '0')
    );

    /* 1行を16文字にする */
    (void)LCD_puts("    ");
}

/*
 アラーム時刻と現在時刻をLCD表示バッファへ格納する。
 current_datetime 現在の日付時刻
 */
static void disp_alarm(const alarm_info_t *alarm_info,
                       const datetime_t *current_datetime)
{
    char number_string[4];

    if ((alarm_info == 0) || (current_datetime == 0)) {
        return;
    }

    /* LCDの1行目へアラーム時刻を表示する */
    LCD_locate(1, 1);
    (void)LCD_puts("ALARM ");

    /* アラーム時刻の時を表示する */
    (void)LCD_puts(
        myitoa((int)alarm_info->hour, number_string, 2, ' ')
    );
    (void)LCD_putchar(':');

    /* アラーム時刻の分を表示する */
    (void)LCD_puts(
        myitoa((int)alarm_info->minute, number_string, 2, '0')
    );

    /* 1行を16文字にする */
    (void)LCD_puts("     ");

    /* LCDの2行目へ現在時刻を表示する */
    LCD_locate(1, 2);
    (void)LCD_puts("  ");

    /* 現在時刻の時を表示する */
    (void)LCD_puts(
        myitoa(current_datetime->hour, number_string, 2, ' ')
    );
    (void)LCD_putchar(':');

    /* 現在時刻の分を表示する */
    (void)LCD_puts(
        myitoa(current_datetime->minute, number_string, 2, '0')
    );

    /* 1行を16文字にする */
    (void)LCD_puts("         ");
}

/*
 システムを初期化する。
 */
static void SYS_init(void)
{
    __disable_irq();

    LED_setup();
    tactSW_setup();
    Buzzer_setup();
    Timer_init();
    SysTick_init();

    __enable_irq();

    /* スイッチ状態が確定するまで待機する */
    while (tactSW_isfix() != true) {
        ;
    }

    init_switch();
    LCD_setup();
}

/*
 * 指定されたモードの初期画面をLCDへ表示する。

 *  mode 表示するシステムモード
 *  display_datetime 時計表示用の日付時刻
 */
static void disp_mode_screen(system_mode_t mode,
                             datetime_t *display_datetime)
{
    stopwatch_t display_stopwatch;
    alarm_info_t display_alarm;
    datetime_t current_datetime;

    LCD_cls();

    switch (mode) {
    case MODE_CLOCK:
        get_datetime(display_datetime);
        disp_datetime(display_datetime);
        break;

    case MODE_STOPWATCH:
        get_stopwatch(&display_stopwatch);
        disp_stopwatch(&display_stopwatch);
        break;

    case MODE_ALARM:
        get_alarm(&display_alarm);
        get_datetime(&current_datetime);
        disp_alarm(&display_alarm, &current_datetime);
        break;

    default:
        break;
    }
}

