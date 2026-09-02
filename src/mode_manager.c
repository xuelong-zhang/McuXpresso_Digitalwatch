/**
 * @file mode_manager.c
 * @brief システムモード管理
 */

#include "mode_manager.h"
#include "input.h"

/* 現在のシステムモード */
static system_mode_t current_mode;

/*
 システムモードを初期化する。
 */
void init_mode_manager(void)
{
    current_mode = MODE_CLOCK;
}

/*
 OK／SW1が押されたときにモードを切り替える
 */
bool update_mode_manager(void)
{
    if (clicked_ModeSW() == false) {
        return false;
    }

    switch (current_mode) {
    case MODE_CLOCK:
        current_mode = MODE_STOPWATCH;
        break;

    case MODE_STOPWATCH:
        current_mode = MODE_ALARM;
        break;

    case MODE_ALARM:
    default:
        current_mode = MODE_CLOCK;
        break;
    }

    return true;
}

/*
 現在のシステムモードを取得する
 */
system_mode_t get_current_mode(void)
{
    return current_mode;
}

