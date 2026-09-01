/**
 * @file input.c
 * @brief デジタル時計 システム入力処理
 *
 * tactSWモジュールが確定したスイッチ状態から、
 * ONからOFFへ変化した瞬間を1回のクリックとして検出する。
 */

#include "input.h"
#include "tactSW.h"

/* 前回確認時のスイッチ状態 */
static unsigned int old_status[BTN_COUNT];

/* 今回のメインループで発生したクリックイベント */
static bool clicked_event[BTN_COUNT];

/* 内部関数 */
static bool get_clicked_event(int switch_no);

/**
 * スイッチ入力管理を初期化する。
 *
 * tactSW_isfix()がtrueになった後で呼び出すこと。
 */
void init_switch(void)
{
    int i;

    for (i = 0; i < BTN_COUNT; i++) {
        old_status[i] = tactSW_getStatus(i);
        clicked_event[i] = false;
    }
}

/**
 * 全スイッチのクリックイベントを更新する。
 *
 * メインループごとに1回だけ、clicked_xxxSW()より先に呼び出すこと。
 */
void update_switch(void)
{
    int i;
    unsigned int now_status;

    for (i = 0; i < BTN_COUNT; i++) {
        /* 前回ループの未使用イベントを残さない。 */
        clicked_event[i] = false;

        now_status = tactSW_getStatus(i);

        /* ONからOFFに変化したとき、1回のクリックとする。 */
        if ((old_status[i] == ON) &&
            (now_status == OFF)) {
            clicked_event[i] = true;
        }

        old_status[i] = now_status;
    }
}

/**
 * 指定スイッチのクリックイベントを取得する。
 *
 * 同じイベントを重複して処理しないよう、取得後にイベントを消去する。
 */
static bool get_clicked_event(int switch_no)
{
    bool result = false;

    if ((0 <= switch_no) &&
        (switch_no < BTN_COUNT)) {
        result = clicked_event[switch_no];
        clicked_event[switch_no] = false;
    }

    return result;
}

/** モード切替スイッチ（CANCEL／SW_CENTER） */
bool clicked_ModeSW(void)
{
    return get_clicked_event(BTN_OK);
}

/** 変更切替スイッチ（OK／SW1） */
bool clicked_FunctionSW(void)
{
    return get_clicked_event(BTN_CANCEL);
}

/** 左移動スイッチ */
bool clicked_LeftSW(void)
{
    return get_clicked_event(BTN_LEFT);
}

/** 右移動スイッチ */
bool clicked_RightSW(void)
{
    return get_clicked_event(BTN_RIGHT);
}

/** 増加スイッチ */
bool clicked_UpSW(void)
{
    return get_clicked_event(BTN_UP);
}

/** 減少スイッチ */
bool clicked_DownSW(void)
{
    return get_clicked_event(BTN_DOWN);
}

/** 目覚ましスイッチ（ENTRY／ISP_ENTRY） */
bool clicked_AlarmSW(void)
{
    return get_clicked_event(BTN_ENTRY);
}

/******************************************************************************
 * End Of File
 ******************************************************************************/
