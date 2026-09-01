/**
 * @file datetime.c
 * @brief 日付時刻操作部
 */

#include "datetime.h"

/* 現在日時：datetime.c内部だけで使用する */
static datetime_t current_datetime;

/* 内部関数 */
static int get_last_day(int year, int month);
static int is_leap_year(int year);


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
 */
void update_datetime(void)
{
	/* 秒更新 */
	current_datetime.second++;

	if (current_datetime.second < 60) {
		return;
	}

	current_datetime.second = 0;

	/* 分更新 */
	current_datetime.minute++;

	if (current_datetime.minute < 60) {
		return;
	}

	current_datetime.minute = 0;

	/* 時更新 */
	current_datetime.hour++;

	if (current_datetime.hour < 24) {
		return;
	}

	current_datetime.hour = 0;

	/* 日付更新 */
	current_datetime.day++;

	/*
	 * 日付が1日進んだので曜日も1つ進める。
	 */
	if (current_datetime.weekday >= WEEK_SAT) {
		current_datetime.weekday = WEEK_SUN;
	} else {
		current_datetime.weekday++;
	}

	/*
	 * 現在の日が当月の月末日を超えたか確認する。
	 */
	if (current_datetime.day >
			get_last_day(current_datetime.year,
						 current_datetime.month)) {

		current_datetime.day = 1;
		current_datetime.month++;

		/* 12月を超えた場合は翌年1月にする */
		if (current_datetime.month > 12) {
			current_datetime.month = 1;
			current_datetime.year++;
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


/**
 * 指定された年月の月末日を返す。
 */
static int get_last_day(int year, int month)
{
	static const int last_day_table[12] = {
		31,	/* 1月 */
		28,	/* 2月 */
		31,	/* 3月 */
		30,	/* 4月 */
		31,	/* 5月 */
		30,	/* 6月 */
		31,	/* 7月 */
		31,	/* 8月 */
		30,	/* 9月 */
		31,	/* 10月 */
		30,	/* 11月 */
		31	/* 12月 */
	};

	/* 月の範囲異常 */
	if ((month < 1) || (month > 12)) {
		return 0;
	}

	/* 閏年の2月 */
	if ((month == 2) && (is_leap_year(year) != 0)) {
		return 29;
	}

	return last_day_table[month - 1];
}


/**
 * 指定された年が閏年か判定する。
 *
 * 戻り値：
 * 1：閏年
 * 0：平年
 */
static int is_leap_year(int year)
{
	if ((year % 400) == 0) {
		return 1;
	}

	if ((year % 100) == 0) {
		return 0;
	}

	if ((year % 4) == 0) {
		return 1;
	}

	return 0;
}
