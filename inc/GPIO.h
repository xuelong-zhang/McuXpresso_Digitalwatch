/**
 * @file GPIO.h
 * @brief 組込みソフトウェア基礎, GPIO操作関数のヘッダファイル
 *
 * @author Copyright(C) 2023, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */
#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>

enum { LOW, HIGH }; //!< port digital signal value
enum { INDIRECTION, OUTDIRECTION }; //!< port digital signal value

typedef
enum gpioNo_{ PORT0, PORT1, PORT2, PORT3, PORT4 } gpioNo_t; //!< GPIO port No.

#define PORTNO_MIN ((unsigned)PORT0)
#define PORTNO_MAX ((unsigned)PORT4)

extern void GPIO_setDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );
extern void GPIO_setValue( uint32_t portNum, uint32_t data, uint32_t mask );
extern void GPIO_setValueBit( uint32_t portNum, uint32_t bitPosi, uint8_t bitVal );
extern uint8_t GPIO_getValueBit( uint32_t portNum, uint32_t bitPosi );
extern uint32_t GPIO_getValue( uint32_t portNum);

/* end GPIO_H */
#endif
