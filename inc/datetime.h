#ifndef DATETIME_H
#define DATETIME_H

/* 曜日 */
typedef enum {
	WEEK_SUN,
	WEEK_MON,
	WEEK_TUE,
	WEEK_WED,
	WEEK_THR,
	WEEK_FRI,
	WEEK_SAT
} weekday_t;

/* 日付時刻情報 */
typedef struct {
	int year;
	int month;
	int day;
	weekday_t weekday;
	int hour;
	int minute;
	int second;
} datetime_t;

/* 公開関数 */
extern void init_datetime(void);
extern void update_datetime(void);
extern void get_datetime(datetime_t *datetime);

#endif

