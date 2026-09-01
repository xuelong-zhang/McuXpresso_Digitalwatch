/*
 * input.h
 *
 *  Created on: 2026/09/01
 *      Author: 4104720
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <stdbool.h>

/** スイッチ入力管理を初期化する。 */
void init_switch(void);

/** 全スイッチのクリックイベントを更新する。 */
void update_switch(void);

/** 各スイッチのクリックイベントを取得する。 */
bool clicked_ModeSW(void);
bool clicked_FunctionSW(void);
bool clicked_LeftSW(void);
bool clicked_RightSW(void);
bool clicked_UpSW(void);
bool clicked_DownSW(void);
bool clicked_AlarmSW(void);

#endif /* INPUT_H_ */
