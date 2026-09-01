/**
 * @file mode_manager.h
 * @brief システムモード管理インターフェース
 */

#ifndef MODE_MANAGER_H_
#define MODE_MANAGER_H_

#include <stdbool.h>

/* システム表示モード */
typedef enum {
    MODE_CLOCK,
    MODE_STOPWATCH,
    MODE_ALARM,
    MODE_COUNT
} system_mode_t;

/** システムを時計モードで初期化する。 */
void init_mode_manager(void);

/**
 * モード切替スイッチ（OK／SW1）を処理する。
 *
 * @return モードを変更した場合はtrue、それ以外はfalse
 */
bool update_mode_manager(void);

/** 現在のシステムモードを取得する。 */
system_mode_t get_current_mode(void);

#endif /* MODE_MANAGER_H_ */
