/**
 * @file Melody.h
 * @brief アラームメロディ再生インターフェース
 */

#ifndef MELODY_H_
#define MELODY_H_

/** メロディ再生状態を初期化する。 */
void Melody_init(void);

/** メロディを先頭から再生する。 */
void Melody_start(void);

/** メロディを停止し、ブザー出力をLOWにする。 */
void Melody_stop(void);

/**
 * 経過時間に従って再生中の音符を更新する。
 *
 * @param[in] elapsed_10ms 経過した10ms割込み回数
 */
void Melody_update(unsigned int elapsed_10ms);

#endif /* MELODY_H_ */
