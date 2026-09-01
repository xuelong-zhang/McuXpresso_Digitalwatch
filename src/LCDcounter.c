/**
 * @file LCDcounter.c
 * @brief デジタル時計 日時LCD表示
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


/* 曜日表示文字列 */
static const char *const weekday_text[] = {
	"SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT"
};

/* プロトタイプ宣言 */
static void SYS_init(void);
static void disp_datetime(const datetime_t *datetime);



/**
 * メイン処理
 */
int main(void)
{
	datetime_t display_datetime;

	int error_no = 0;
	int err_lcd = 0;
//	int test_led = 0;//TEST SW

	unsigned int chk_10ms;
	unsigned int past1sec = 0;

	SYS_init();
	init_datetime();
	init_mode_clock();

	chk_10ms = get_time_10ms();

	LCD_cls();

	get_datetime(&display_datetime);
	disp_datetime(&display_datetime);

	while (1) {
		/*
		 * LCDリングバッファのデータを1つ処理する
		 */
		err_lcd = LCD_display();

	    update_switch();

	    if (mode_clock(&display_datetime) == true) {
	        disp_datetime(&display_datetime);

	    }
	    update_clock_cursor();

	    /*Sw test*/
//	    if (clicked_LeftSW() == true) {
//	        test_led = 1;
//	    } else if (clicked_RightSW() == true) {
//	        test_led = 2;
//	    } else if (clicked_DownSW() == true) {
//	        test_led = 3;
//	    } else if (clicked_UpSW() == true) {
//	        test_led = 4;
//	    } else if (clicked_ModeSW() == true) {
//	        test_led = 5;
//	    } else if (clicked_FunctionSW() == true) {
//	        test_led = 6;
//	    } else if (clicked_AlarmSW() == true) {
//	        test_led = 7;
//	    }

//	    if (error_no == 0) {
//	        LED_alloff();
//
////	        if (test_led != 0) {
////	            LED_on(test_led);
////	        }
//	    }

		if (err_lcd != 0) {
			if (error_no == 0) {
				error_no = err_lcd;
			}

			LCD_reset_lcdbuf();
		}

		/* LCDエラー表示 */
		switch (error_no) {
		case ERR_TOUT:
		case ERR_BFULL:
		case ERR_ESC:
			LED_on(error_no);
			break;

		default:
			LED_alloff();
			break;
		}

		/*
		 * 10ms割込みの発生を確認する
		 */
		if (get_time_10ms() != chk_10ms) {
			chk_10ms++;
			past1sec++;

			/*
			 * 10ms割込み100回で1秒
			 */
			if (past1sec >= 100) {
			    past1sec = 0;

			    update_datetime();

			    if (is_clock_setting() == false) {

			        get_datetime(&display_datetime);
			        disp_datetime(&display_datetime);
			    }
			}
		}
	}

	return 0;
}




/**
 * 日付時刻をLCD表示バッファへ格納する。
 *
 * 1行目：2026/ 1/ 1(THR)
 * 2行目：     0:00:00
 */
static void disp_datetime(const datetime_t *datetime)
{
	char number_string[6];

	/* ---------- LCD 1行目 ---------- */

	LCD_locate(1, 1);

	/* 年：4桁 */
	(void)LCD_puts(
		myitoa(datetime->year, number_string, 4, '0')
	);

	(void)LCD_putchar('/');

	/* 月：2桁分の領域、前を空白にする */
	(void)LCD_puts(
		myitoa(datetime->month, number_string, 2, ' ')
	);

	(void)LCD_putchar('/');

	/* 日：2桁分の領域、前を空白にする */
	(void)LCD_puts(
		myitoa(datetime->day, number_string, 2, ' ')
	);

	(void)LCD_putchar('(');

	/* 曜日 */
	(void)LCD_puts(weekday_text[datetime->weekday]);

	(void)LCD_putchar(')');

	/* 1行を16文字にする */
	(void)LCD_putchar(' ');


	/* ---------- LCD 2行目 ---------- */

	LCD_locate(1, 2);

	/* 時刻を中央付近に配置 */
	(void)LCD_puts("    ");

	/* 時：2桁分の領域、前を空白にする */
	(void)LCD_puts(
		myitoa(datetime->hour, number_string, 2, ' ')
	);

	(void)LCD_putchar(':');

	/* 分：2桁ゼロ埋め */
	(void)LCD_puts(
		myitoa(datetime->minute, number_string, 2, '0')
	);

	(void)LCD_putchar(':');

	/* 秒：2桁ゼロ埋め */
	(void)LCD_puts(
		myitoa(datetime->second, number_string, 2, '0')
	);

	/* 1行を16文字にする */
	(void)LCD_puts("    ");
}


/**
 * システム初期化
 */
static void SYS_init(void)
{
    __disable_irq();

    LED_setup();
    tactSW_setup();
    SysTick_init();

    __enable_irq();

    while (tactSW_isfix() != true) {
        ;
    }

    init_switch();

    LCD_setup();
}

