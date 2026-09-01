/**
 * @file datetime.h
 * @brief Date and time control interface
 */

#ifndef DATETIME_H
#define DATETIME_H

/* Weekday */
typedef enum {
    WEEK_SUN,
    WEEK_MON,
    WEEK_TUE,
    WEEK_WED,
    WEEK_THR,
    WEEK_FRI,
    WEEK_SAT
} weekday_t;

/* Date and time information */
typedef struct {
    int year;
    int month;
    int day;
    weekday_t weekday;
    int hour;
    int minute;
    int second;
} datetime_t;

/* Public functions */
void init_datetime(void);
void update_datetime(void);
void get_datetime(datetime_t *datetime);
void set_datetime(const datetime_t *datetime);
void normalize_datetime(datetime_t *datetime);
int get_last_day(int year, int month);

#endif /* DATETIME_H */
