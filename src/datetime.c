/**
 * @file datetime.c
 * @brief 日付時刻管理
 */

#include "datetime.h"

/* 日付計算モジュールで扱うグレゴリオ暦の年範囲 */
#define DATETIME_MIN_YEAR  (1583)
#define DATETIME_MAX_YEAR  (3999)

/* 現在の日付時刻 */
static datetime_t current_datetime;

/* 内部関数 */
static int is_leap_year(int year);
static weekday_t calculate_weekday(int year, int month, int day);

/*
  現在の日付時刻を初期化する
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

/*
 現在の日付時刻を1秒進める
 */
void update_datetime(void)
{
    current_datetime.second++;

    if (current_datetime.second < 60) {
        return;
    }
    current_datetime.second = 0;

    current_datetime.minute++;

    if (current_datetime.minute < 60) {
        return;
    }
    current_datetime.minute = 0;

    current_datetime.hour++;

    if (current_datetime.hour < 24) {
        return;
    }
    current_datetime.hour = 0;

    current_datetime.day++;

    if (current_datetime.weekday >= WEEK_SAT) {
        current_datetime.weekday = WEEK_SUN;
    } else {
        current_datetime.weekday++;
    }

    if (current_datetime.day >
        get_last_day(current_datetime.year, current_datetime.month)) {
        current_datetime.day = 1;
        current_datetime.month++;

        if (current_datetime.month > 12) {
            current_datetime.month = 1;
            current_datetime.year++;
        }
    }
}

/*
 現在の日付時刻を取得する
 */
void get_datetime(datetime_t *datetime)
{
    if (datetime != 0) {
        *datetime = current_datetime;
    }
}

/*
  現在の日付時刻を設定する
 */
void set_datetime(const datetime_t *datetime)
{
    if (datetime != 0) {
        current_datetime = *datetime;
        normalize_datetime(&current_datetime);
    }
}

/*
  各項目を有効範囲に補正し、曜日を再計算する
 */
void normalize_datetime(datetime_t *datetime)
{
    int last_day;

    if (datetime == 0) {
        return;
    }

    if (datetime->year < DATETIME_MIN_YEAR) {
        datetime->year = DATETIME_MIN_YEAR;
    } else if (datetime->year > DATETIME_MAX_YEAR) {
        datetime->year = DATETIME_MAX_YEAR;
    }

    if (datetime->month < 1) {
        datetime->month = 1;
    } else if (datetime->month > 12) {
        datetime->month = 12;
    }

    last_day = get_last_day(datetime->year, datetime->month);

    if (datetime->day < 1) {
        datetime->day = 1;
    } else if (datetime->day > last_day) {
        datetime->day = last_day;
    }

    if (datetime->hour < 0) {
        datetime->hour = 0;
    } else if (datetime->hour > 23) {
        datetime->hour = 23;
    }

    if (datetime->minute < 0) {
        datetime->minute = 0;
    } else if (datetime->minute > 59) {
        datetime->minute = 59;
    }

    if (datetime->second < 0) {
        datetime->second = 0;
    } else if (datetime->second > 59) {
        datetime->second = 59;
    }

    datetime->weekday = calculate_weekday(
        datetime->year,
        datetime->month,
        datetime->day
    );
}

/*
  指定した年月の月末日を取得する
 */
int get_last_day(int year, int month)
{
    static const int last_day_table[12] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if ((month < 1) || (month > 12)) {
        return 0;
    }

    if ((month == 2) && (is_leap_year(year) != 0)) {
        return 29;
    }

    return last_day_table[month - 1];
}

/*
  指定した年が閏年の場合は0以外を返す
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

/*
  グレゴリオ暦に基づいて曜日を計算する
 */
static weekday_t calculate_weekday(int year, int month, int day)
{
    static const int month_offset[12] = {
        0, 3, 2, 5, 0, 3,
        5, 1, 4, 6, 2, 4
    };
    int adjusted_year = year;
    int weekday;

    if (month < 3) {
        adjusted_year--;
    }

    weekday = (
        adjusted_year +
        (adjusted_year / 4) -
        (adjusted_year / 100) +
        (adjusted_year / 400) +
        month_offset[month - 1] +
        day
    ) % 7;

    return (weekday_t)weekday;
}


