/**
 * @file GPIO.c
 * @brief 組込みソフトウェア基礎, GPIO操作関数
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */
#include <limits.h>
#include "GPIO.h"
#include "FIOREG.h"

// enable MISRAC2012 coding
#define MISRAC2012

/*
 * FIOVAL（出力ポートラッチ）レジスタのアドレス
 */
static volatile uint32_t * FIOVAL_ptr[]={
		(volatile uint32_t *) 0x2009C014, // FIO0VAL
		(volatile uint32_t *) 0x2009C034, // FIO1VAL
		(volatile uint32_t *) 0x2009C054, // FIO2VAL
		(volatile uint32_t *) 0x2009C074, // FIO3VAL
		(volatile uint32_t *) 0x2009C094  // FIO4VAL
};

/**
 * 指定されたポート番号のビットポジションの方向を設定する
 *
 * @param[in] portNum ポート番号
 * @param[in] bitPosi ビットポジション
 * @param[in] dir 方向(1 out, 0 input)
 * @return なし
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
#ifdef MISRAC2012
void GPIO_setDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
	/*
	 * FIODIR レジスタのアドレス
	 */
	static volatile uint32_t * FIODIR_ptr[]={
			(volatile uint32_t *) 0x2009C000, // FIO0DIR
			(volatile uint32_t *) 0x2009C020, // FIO1DIR
			(volatile uint32_t *) 0x2009C040, // FIO2DIR
			(volatile uint32_t *) 0x2009C060, // FIO3DIR
			(volatile uint32_t *) 0x2009C080  // FIO4DIR
	};

	if(bitPosi <= 31){
		if( portNum <= PORTNO_MAX ){
			if(bitPosi < (sizeof(uint32_t)*(unsigned)CHAR_BIT)){
				uint32_t tmp = (uint32_t)1<<bitPosi;
				if ( dir == 1 ){
					*FIODIR_ptr[portNum] |= tmp;
				}else{
					*FIODIR_ptr[portNum] &= ~tmp;
				}
			}
		}else{
			// do nothing.
		}
	}
}

#else
/*
 cyclomatic complexity of following codes is high !
 but is easy to understand.
*/
void GPIO_setDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
  switch ( portNum )
  {
	case PORT0:
	  if ( !(FIO0DIR & (1<<bitPosi)) && (dir == 1) )
		FIO0DIR |= (1<<bitPosi);
	  else if ( (FIO0DIR & (1<<bitPosi)) && (dir == 0) )
		FIO0DIR &= ~(1<<bitPosi);
	break;
 	case PORT1:
	  if ( !(FIO1DIR & (1<<bitPosi)) && (dir == 1) )
		FIO1DIR |= (1<<bitPosi);
	  else if ( (FIO1DIR & (1<<bitPosi)) && (dir == 0) )
		FIO1DIR &= ~(1<<bitPosi);
	break;
	case PORT2:
	  if ( !(FIO2DIR & (1<<bitPosi)) && (dir == 1) )
		FIO2DIR |= (1<<bitPosi);
	  else if ( (FIO2DIR & (1<<bitPosi)) && (dir == 0) )
		FIO2DIR &= ~(1<<bitPosi);
	break;
	case PORT3:
	  if ( !(FIO3DIR & (1<<bitPosi)) && (dir == 1) )
		FIO3DIR |= (1<<bitPosi);
	  else if ( (FIO3DIR & (1<<bitPosi)) && (dir == 0) )
		FIO3DIR &= ~(1<<bitPosi);
	break;
	case PORT4:
	  if ( !(FIO4DIR & (1<<bitPosi)) && (dir == 1) )
		FIO4DIR |= (1<<bitPosi);
	  else if ( (FIO4DIR & (1<<bitPosi)) && (dir == 0) )
		FIO4DIR &= ~(1<<bitPosi);
	break;
	default:
	  break;
  }
  return;
}
#endif
/**
 * 指定された出力ポートの値を32ビット単位で変更する
 *
 * @param[in] portNum ポート番号
 * @param[in] data 値
 * @param[in] mask マスク値（変更したくないビット位置のみを1にした値）
 * @return なし
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
#ifdef MISRAC2012
void GPIO_setValue( uint32_t portNum, uint32_t data, uint32_t mask )
{
	/*
	 * FIOPIN レジスタのアドレス
	 */
	static volatile uint32_t * FIOPIN_ptr[]={
			(volatile uint32_t *) 0x2009C014, // FIO0PIN
			(volatile uint32_t *) 0x2009C034, // FIO1PIN
			(volatile uint32_t *) 0x2009C054, // FIO2PIN
			(volatile uint32_t *) 0x2009C074, // FIO3PIN
			(volatile uint32_t *) 0x2009C094  // FIO4PIN
	};
	/*
	 * FIOMASK レジスタのアドレス
	 */
	static volatile uint32_t * FIOMASK_ptr[]={
			(volatile uint32_t *) 0x2009C010, // FIO0MASK
			(volatile uint32_t *) 0x2009C030, // FIO1MASK
			(volatile uint32_t *) 0x2009C050, // FIO2MASK
			(volatile uint32_t *) 0x2009C070, // FIO3MASK
			(volatile uint32_t *) 0x2009C090  // FIO4MASK
	};
	if( portNum <= PORTNO_MAX ){
		*FIOMASK_ptr[portNum] = mask;
		*FIOPIN_ptr[portNum] = data;
		*FIOMASK_ptr[portNum] = 0;
	}else{
		// do nothing.
	}
}

#else
/*
 cyclomatic complexity of following codes is high !
 but is easy to understand.
*/
void GPIO_setValue( uint32_t portNum, uint32_t data, uint32_t mask )
{
	if( (PORTNO_MIN <= portNum) && (portNum <= PORTNO_MAX) ){
		switch ( portNum ){
		case PORT0:
			FIO0MASK = mask;
			FIO0PIN = data;
			FIO0MASK = 0;
			break;
		case PORT1:
			FIO1MASK = mask;
			FIO1PIN = data;
			FIO1MASK = 0;
			break;
		case PORT2:
			FIO2MASK = mask;
			FIO2PIN = data;
			FIO2MASK = 0;
			break;
		case PORT3:
			FIO3MASK = mask;
			FIO3PIN = data;
			FIO3MASK = 0;
			break;
		case PORT4:
			FIO4MASK = mask;
			FIO4PIN = data;
			FIO4MASK = 0;
			break;
		default:
			/* illegal portNo, nothing to do */
			break;
		}
	}
}
#endif

/**
 * 指定されたポート番号とビットポジションで示されるビットの値を変更する
 *
 * @param[in] portNum ポート番号
 * @param[in] bitPosi ビットポジション
 * @param[in] bitVal 変更したい値(1 or 0)
 * @return なし
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
#ifdef MISRAC2012
void GPIO_setValueBit( uint32_t portNum, uint32_t bitPosi, uint8_t bitVal )
{
	/*
	 * FIOSET register's address
	 */
	static volatile uint32_t * FIOSET_ptr[]={
			(volatile uint32_t *) 0x2009C018, // FIO0SET
			(volatile uint32_t *) 0x2009C038, // FIO1SET
			(volatile uint32_t *) 0x2009C058, // FIO2SET
			(volatile uint32_t *) 0x2009C078, // FIO3SET
			(volatile uint32_t *) 0x2009C098  // FIO4SET
	};

	/*
	 * FIOCLR register's address
	 */
	static volatile uint32_t * FIOCLR_ptr[]={
			(volatile uint32_t *) 0x2009C01C, // FIO0CLR
			(volatile uint32_t *) 0x2009C03C, // FIO1CLR
			(volatile uint32_t *) 0x2009C05C, // FIO2CLR
			(volatile uint32_t *) 0x2009C07C, // FIO3CLR
			(volatile uint32_t *) 0x2009C09C  // FIO4CLR
	};
	// Check bitVal is a binary value - 0 or 1
	if( portNum <= PORTNO_MAX ){
		if(bitPosi < (sizeof(uint32_t)*(unsigned)CHAR_BIT)){
			if (bitVal < 2 ){
				uint32_t tmp = (uint32_t)((uint32_t)1<<bitPosi);
				if(bitVal != 0){
					*FIOSET_ptr[portNum] = tmp;
				}else{
					*FIOCLR_ptr[portNum] = tmp;
				}
			}
		}
	}
}

#else
/*
 cyclomatic complexity of following codes is high !
 but is easy to understand.
*/
void GPIO_setValueBit( uint32_t portNum, uint32_t bitPosi, uint8_t bitVal )
{
	// Check bitVal is a binary value - 0 or 1
	if( (PORTNO_MIN <= portNum) && (portNum <= PORTNO_MAX) ){
		if (bitVal < 2 )
		{
			switch ( portNum )
			{
			case PORT0:
				if(bitVal != 0){
					FIO0SET = (1<<bitPosi);
				}else{
					FIO0CLR = (1<<bitPosi);
				}
				break;
			case PORT1:
				if(bitVal != 0){
					FIO1SET = (1<<bitPosi);
				}else{
					FIO1CLR = (1<<bitPosi);
				}
				break;
			case PORT2:
				if(bitVal !=0){
					FIO2SET = (1<<bitPosi);
				}else{
					FIO2CLR = (1<<bitPosi);
				}
				break;
			case PORT3:
				if(bitVal !=0){
					FIO3SET = (1<<bitPosi);
				}else{
					FIO3CLR = (1<<bitPosi);
				}
				break;
			case PORT4:
				if(bitVal !=0){
					FIO4SET = (1<<bitPosi);
				}else{
					FIO4CLR = (1<<bitPosi);
				}
				break;
			default:
				/* illegal portNo, nothing to do */
				break;
			}
		}
	}
}
#endif

/**
 * 指定されたポート番号とビットポジションで示されるビットの値を得る
 *
 * @param[in] portNum ポート番号
 * @param[in] bitPosi ビットポジション
 * @return 読み取った値
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
#ifdef MISRAC2012
uint8_t GPIO_getValueBit( uint32_t portNum, uint32_t bitPosi )
{
	uint8_t ret = 0;
	if( portNum <= PORTNO_MAX ){
		if(bitPosi < (sizeof(uint32_t)*(unsigned)CHAR_BIT)){
			if((*FIOVAL_ptr[portNum] & ((uint32_t)1<<bitPosi)) != (uint32_t)0 ){
				ret = 1;
			}else{
				ret = 0;
			}
		}
	}else{
		// if illegal port number, do nothing.
	}
	return ret;
}

#else
/*
 cyclomatic complexity of following codes is high !
 but is easy to understand.
*/
unsigned GPIO_getValueBit( uint32_t portNum, uint32_t bitPosi )
{
	unsigned ret = 0;
	if( portNum <= PORTNO_MAX ){
		switch ( portNum )
		{
		case PORT0:
			if((FIO0VAL & (uint32_t)(1<<bitPosi)) != 0 ){
				ret = 1;
			}else{
				ret = 0;
			}
			break;
		case PORT1:
			if((FIO1VAL & (1<<bitPosi)) != 0 ){
				ret = 1;
			}else{
				ret = 0;
			}
			break;
		case PORT2:
			if((FIO2VAL & (1<<bitPosi)) != 0 ){
				ret = 1;
			}else{
				ret = 0;
			}
			break;
		case PORT3:
			if((FIO3VAL & (1<<bitPosi)) != 0 ){
				ret = 1;
			}else{
				ret = 0;
			}
			break;
		case PORT4:
			if((FIO4VAL & (1<<bitPosi)) != 0 ){
				ret = 1;
			}else{
				ret = 0;
			}
			break;
		default:
			/* illegal portNo, nothing to do */
			break;
		}
	}
	return ret;
}
#endif

/**
 * 指定されたポート番号のボートの値を得る.
 *
 * @param[in] portNum ポート番号
 * @return 読み取った値
 * @attention This code doesn't use LPCOpen library,
 *            because of software portability!
 */
#ifdef MISRAC2012
uint32_t GPIO_getValue( uint32_t portNum )
{
	uint32_t ret=(unsigned)0;
	if( portNum <= PORTNO_MAX ){
		ret = (uint32_t)*FIOVAL_ptr[portNum];
	}else{
		/* illegal portNo, nothing to do */
	}
	return ret;
}
#else
/*
 cyclomatic complexity of following codes is high !
 but is easy to understand.
*/
uint32_t GPIO_getValue( uint32_t portNum)
{
	if( portNum <= PORTNO_MAX ){
		switch ( portNum )
		{
		case PORT0:
			return (uint32_t)FIO0VAL;
			break;
		case PORT1:
			return (uint32_t)FIO1VAL;
			break;
		case PORT2:
			return (uint32_t)FIO2VAL;
			break;
		case PORT3:
			return (uint32_t)FIO3VAL;
			break;
		case PORT4:
			return (uint32_t)FIO4VAL;
			break;
		default:
			/* illegal portNo, nothing to do */
			break;
		}
	}
	return 0;
}
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
