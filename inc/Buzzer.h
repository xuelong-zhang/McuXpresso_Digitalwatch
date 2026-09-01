/**
 * @file Buzzer.h
 * @brief ブザーGPIO制御インターフェース
 */

#ifndef BUZZER_H_
#define BUZZER_H_

/** ブザー端子をGPIO出力として初期化する。 */
void Buzzer_setup(void);

/** ブザー出力をHIGHにする。 */
void Buzzer_on(void);

/** ブザー出力をLOWにする。 */
void Buzzer_off(void);

/** ブザー出力のHIGH／LOWを反転する。 */
void Buzzer_toggle(void);

#endif /* BUZZER_H_ */
