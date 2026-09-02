/**
 * @file Melody.c
 * @brief 圧電ブザー用アラームメロディ制御
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Melody.h"
#include "Timer.h"

/* 無音を表すタイマー周期 */
#define TONE_REST   (0U)

/* 各音程のTimer0割込み周期（マイクロ秒） */
#define TONE_E6     (379U)
#define TONE_G6     (319U)
#define TONE_GS6    (301U)
#define TONE_A6     (284U)
#define TONE_AS6    (268U)
#define TONE_B6     (253U)
#define TONE_C7     (239U)
#define TONE_D7     (213U)
#define TONE_DS7    (201U)
#define TONE_E7     (190U)
#define TONE_F7     (179U)
#define TONE_FS7    (169U)
#define TONE_G7     (159U)
#define TONE_A7     (142U)

/* 音符の発音時間を90パーセントとし、同音の連続も区切って聞こえるようにする */
#define NOTE_GATE_PERCENT  (90U)

typedef struct {
    uint16_t half_period_us;
    uint16_t duration_ms;
} melody_note_t;

/*
 * 高音部の代表的な旋律を単音ブザー向けに簡略化した20秒のフレーズ。
 * 1小節を1000msとし、20小節終了後は先頭から繰り返す。
 * アラーム側の60秒制御により、3回目の終了時に再生を停止する。
 */
static const melody_note_t melody_notes[] = {
    /* 1小節目 */
    { TONE_E7, 125U }, { TONE_E7, 125U }, { TONE_REST, 125U },
    { TONE_E7, 125U }, { TONE_REST, 125U }, { TONE_C7, 125U },
    { TONE_E7, 250U },

    /* 2小節目 */
    { TONE_G7, 500U }, { TONE_REST, 250U }, { TONE_G6, 250U },

    /* 3小節目 */
    { TONE_C7, 375U }, { TONE_G6, 125U },
    { TONE_REST, 250U }, { TONE_E6, 250U },

    /* 4小節目 */
    { TONE_A6, 250U }, { TONE_B6, 250U },
    { TONE_AS6, 125U }, { TONE_A6, 125U }, { TONE_G6, 250U },

    /* 5小節目 */
    { TONE_E7, 125U }, { TONE_G7, 125U }, { TONE_A7, 250U },
    { TONE_F7, 125U }, { TONE_G7, 125U }, { TONE_REST, 250U },

    /* 6小節目 */
    { TONE_E7, 250U }, { TONE_C7, 125U }, { TONE_D7, 125U },
    { TONE_B6, 250U }, { TONE_REST, 250U },

    /* 7小節目 */
    { TONE_C7, 375U }, { TONE_G6, 125U },
    { TONE_REST, 250U }, { TONE_E6, 250U },

    /* 8小節目 */
    { TONE_A6, 250U }, { TONE_B6, 250U },
    { TONE_AS6, 125U }, { TONE_A6, 125U }, { TONE_G6, 250U },

    /* 9小節目 */
    { TONE_E7, 125U }, { TONE_G7, 125U }, { TONE_A7, 250U },
    { TONE_F7, 125U }, { TONE_G7, 125U }, { TONE_REST, 250U },

    /* 10小節目 */
    { TONE_E7, 250U }, { TONE_C7, 125U },
    { TONE_D7, 125U }, { TONE_B6, 500U },

    /* 11小節目 */
    { TONE_REST, 125U }, { TONE_G7, 125U },
    { TONE_FS7, 125U }, { TONE_F7, 125U },
    { TONE_DS7, 250U }, { TONE_E7, 250U },

    /* 12小節目 */
    { TONE_REST, 125U }, { TONE_GS6, 125U },
    { TONE_A6, 125U }, { TONE_C7, 125U },
    { TONE_REST, 125U }, { TONE_A6, 125U },
    { TONE_C7, 125U }, { TONE_D7, 125U },

    /* 13小節目 */
    { TONE_REST, 125U }, { TONE_DS7, 125U },
    { TONE_D7, 125U }, { TONE_C7, 125U },
    { TONE_REST, 250U }, { TONE_G6, 250U },

    /* 14小節目 */
    { TONE_C7, 125U }, { TONE_C7, 125U }, { TONE_C7, 250U },
    { TONE_REST, 125U }, { TONE_C7, 125U }, { TONE_D7, 250U },

    /* 15小節目 */
    { TONE_E7, 250U }, { TONE_C7, 250U },
    { TONE_A6, 250U }, { TONE_G6, 250U },

    /* 16小節目 */
    { TONE_E7, 125U }, { TONE_E7, 125U }, { TONE_REST, 125U },
    { TONE_E7, 125U }, { TONE_REST, 125U }, { TONE_C7, 125U },
    { TONE_E7, 250U },

    /* 17小節目 */
    { TONE_G7, 500U }, { TONE_REST, 250U }, { TONE_G6, 250U },

    /* 18小節目 */
    { TONE_C7, 250U }, { TONE_G6, 250U },
    { TONE_E6, 250U }, { TONE_A6, 250U },

    /* 19小節目 */
    { TONE_B6, 250U }, { TONE_AS6, 125U }, { TONE_A6, 125U },
    { TONE_G6, 250U }, { TONE_E7, 250U },

    /* 20小節目 */
    { TONE_C7, 125U }, { TONE_D7, 125U },
    { TONE_B6, 250U }, { TONE_C7, 500U }
};

#define MELODY_NOTE_COUNT \
    ((unsigned int)(sizeof(melody_notes) / sizeof(melody_notes[0])))

static bool melody_playing;
static bool note_gate_closed;
static unsigned int note_index;
static unsigned int note_elapsed_ms;

static void start_current_note(void);

/**
 * メロディ再生状態を初期化する。
 */
void Melody_init(void)
{
    melody_playing = false;
    note_gate_closed = true;
    note_index = 0U;
    note_elapsed_ms = 0U;
    Timer_stop();
}

/**
 * メロディを先頭から再生する。
 */
void Melody_start(void)
{
    note_index = 0U;
    note_elapsed_ms = 0U;
    melody_playing = true;
    start_current_note();
}

/**
 * メロディを停止する。
 */
void Melody_stop(void)
{
    melody_playing = false;
    note_gate_closed = true;
    note_index = 0U;
    note_elapsed_ms = 0U;
    Timer_stop();
}

/**
 * 10ms単位の経過時間に従って音符を更新する。
 */
void Melody_update(unsigned int elapsed_10ms)
{
    unsigned int note_duration_ms;
    unsigned int gate_close_time_ms;

    if ((melody_playing == false) || (elapsed_10ms == 0U)) {
        return;
    }

    note_elapsed_ms += elapsed_10ms * 10U;
    note_duration_ms = melody_notes[note_index].duration_ms;

    /* 長時間メイン処理が停止した場合も経過時間分だけ音符を進める */
    while (note_elapsed_ms >= note_duration_ms) {
        note_elapsed_ms -= note_duration_ms;
        note_index++;

        if (note_index >= MELODY_NOTE_COUNT) {
            note_index = 0U;
        }

        start_current_note();
        note_duration_ms = melody_notes[note_index].duration_ms;
    }

    /* 音符の末尾に短い無音区間を設ける */
    gate_close_time_ms =
        (note_duration_ms * NOTE_GATE_PERCENT) / 100U;

    if ((melody_notes[note_index].half_period_us != TONE_REST) &&
        (note_gate_closed == false) &&
        (note_elapsed_ms >= gate_close_time_ms)) {
        Timer_stop();
        note_gate_closed = true;
    }
}

/**
 * 現在選択中の音符を発音する。
 */
static void start_current_note(void)
{
    if (melody_notes[note_index].half_period_us == TONE_REST) {
        Timer_stop();
        note_gate_closed = true;
    } else {
        Timer_start(melody_notes[note_index].half_period_us);
        note_gate_closed = false;
    }
}
