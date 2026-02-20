/**
 * @file tactSW.h
 * @brief 組込みソフトウェア基礎, タクタイルスイッチ制御関数のヘッダファイル
 *
 * @author Copyright(C) 2023, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */
#ifndef TACTSW_H
#define TACTSW_H

#include <stdbool.h>
#include <stdint.h>
#include "general.h"

enum {
	   BTN_LEFT, //!< LEFT
	   BTN_RIGHT, //!< RIGHT
	   BTN_DOWN, //!< DOWN
	   BTN_UP, //!< UP
	   BTN_CANCEL, //!< CANCEL
	   BTN_OK, //!< OK
};

enum { OFF, ON };

#define BIT_TACTSW_LEFT  ((uint32_t)1<<BP18)
#define BIT_TACTSW_RIGHT  ((uint32_t)1<<BP19)
#define BIT_TACTSW_DOWN  ((uint32_t)1<<BP20)
#define BIT_TACTSW_UP  ((uint32_t)1<<BP21)
#define BIT_TACTSW_CANCEL  ((uint32_t)1<<BP22)
#define BIT_TACTSW_OK  ((uint32_t)1<<BP23)

extern void tactSW_setup(void);
extern bool tactSW_isfix(void);
extern uint32_t tactSW_read(void);
extern void tactSW_update(uint32_t data);
extern unsigned int tactSW_getStatus(int no);
extern uint32_t tactSW_getTsw(void);

#endif
