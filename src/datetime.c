/**
 * @file datetime.c
 * @brief 日付時刻操作部
 */

#include "datetime.h"

/* 現在日時：このファイル内だけで使用する */
static datetime_t current_datetime;


/**
 * 日付時刻を初期化する。
 */
void init_datetime(void)
{
	current_datetime.year = 2026;
	current_datetime.month = 1;
	current_datetime.day = 1;
	current_datetime.weekday = WEEK_THR;

	current_datetime.hour = 0;
	current_datetime.minute = 0;
	current_datetime.second = 0;
}


/**
 * 日付時刻を1秒更新する。
 *
 * 現段階では時・分・秒まで更新する。
 */
void update_datetime(void)
{
	current_datetime.second++;

	if (current_datetime.second >= 60) {
		current_datetime.second = 0;
		current_datetime.minute++;

		if (current_datetime.minute >= 60) {
			current_datetime.minute = 0;
			current_datetime.hour++;

			if (current_datetime.hour >= 24) {
				current_datetime.hour = 0;

				/*
				 * 次の段階で日付更新処理を追加する。
				 */
			}
		}
	}
}


/**
 * 現在の日付時刻を呼出元へ返す。
 */
void get_datetime(datetime_t *datetime)
{
	if (datetime != 0) {
		*datetime = current_datetime;
	}
}
