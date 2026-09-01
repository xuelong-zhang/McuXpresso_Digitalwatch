/**
 * @file LCD.c
 * @brief LCD制御関数
 *
 * @author Copyright(C) 2025, PEX, All rights reserved.
 * @attention  CPUクロックの設定はスタートアップルーチンで実施済
 * 			   画面クリアやカーソル位置変更などのコマンドはエスケープシーケンス文字列を処理する
 * 			   this program have been tested on DTC-R05.
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             merchantability or fitness for a particular purpose.
 */

#include <ctype.h>
#include <stdbool.h>

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "general.h"
#include "PINCTRL.h"
#include "GPIO.h"
#include "LCD.h"
#include "Delay.h"
#include "clib.h"
#include "SysTick.h"

#define BUF_ESC_SIZE	(10)		// エスケープシーケンスバッファサイズ
#define TMOUT_LCD		(10)		// ビジータイムアウト時間 100ms : 10ms割込みの検出回数が10回

#define BP_LCD_RS		(26)		// LCD_RS
#define BP_LCD_RW		(25)		// LCD_RW
#define BP_LCD_E		(24)		// LCD_E
#define BP_LCD_DB7		(9)			// LCD_DB7
#define BP_LCD_DB6		(20)		// LCD_DB6
#define BP_LCD_DB5		(19)		// LCD_DB5
#define BP_LCD_DB4		(29)		// LCD_DB4

#define LCD_CLR_DSP		(1<<0)		// 表示クリア

#define LCD_HOME_CUR	(1<<1)		// カーソルをホーム位置へ

#define LCD_EMODE		(1<<2)		// エントリーモードコマンド
#define LCD_EMODE_INC	(1<<1)		// アドレスインクリメントモード
#define LCD_EMODE_DEC	(0<<1)		// アドレスデクリメントモード
#define LCD_EMODE_SHIFT	(1<<0)		// 表示シフトモードにする
#define LCD_EMODE_NOSFT	(0<<0)		// 表示シフトモードにしない

#define LCD_DSP			(1<<3)		// 表示制御コマンド
#define LCD_DSP_ON		(1<<2)		// カーソル位置の文字を表示する
#define LCD_DSP_OFF		(0<<2)		// カーソル位置の文字を表示しない
#define LCD_DSP_CUR_ON	(1<<1)		// カーソルを表示
#define LCD_DSP_CUR_OFF	(0<<1)		// カーソルを非表示
#define LCD_DSP_BLINK	(1<<0)		// カーソル位置の文字を点滅させる
#define LCD_DSP_NOBLINK	(0<<0)		// カーソル位置の文字を点滅させない

#define LCD_SHIFT		(1<<4)		// カーソル表示シフトコマンド
#define LCD_SHIFT_L_CUR	(0<<2)		// カーソル左シフト
#define LCD_SHIFT_R_CUR	(1<<2)		// カーソル右シフト
#define LCD_SHIFT_L_DSP	(2<<2)		// 全体を左シフト
#define LCD_SHIFT_R_DSP	(3<<2)		// 全体を右シフト

#define LCD_SET			(1<<5)		// 機能設定コマンド
#define LCD_SET_8		(1<<4)		// 8ビットインタフェース
#define LCD_SET_4		(0<<4)		// 8ビットインタフェース
#define LCD_SET_2LINES	(1<<3)		// 2行表示
#define LCD_SET_1LINE	(0<<3)		// 1行表示
#define LCD_SET_BIG		(1<<2)		// 大きいフォント
#define LCD_SET_SMALL	(0<<2)		// 小さいフォント

#define LCD_CGADDR		(1<<6)		// CGRAMアドレス設定

#define LCD_DDADDR		(1<<7)		// DDRAMアドレス設定
#define LINE_2_BASE		((uint8_t)0x40)	// 2行目のRAMの先頭アドレス

#define LCD_BUSY		(1<<7)		// ビジーフラッグビット

#define LCDRS_INST		LOW			// LCD RS 信号　インストラクション
#define LCDRS_DATA		HIGH		// LCD RS 信号　データ

#define BUF_SIZE		(256)

static char buf_lcd[BUF_SIZE];		// LCDリングバッファ
static char *write_ptr;				// リングバッファへの書込みポインタ変数
static char *read_ptr;				// リングバッファへの書込みポインタ変数
static int cnt;						// リングバッファ中の有効データ格納数
static int chk_tmout;				// ビジー時間検査用10ms割込み検出回数
static int err_lcd;					// LCD制御エラーコード

/* Function prototype declaration */
static int read_buf(void);
static bool LCD_is_busy(void);
static bool esc_op(int c);

static uint8_t LCD_read_4bitDB(void);
static void LCD_write_4bitDB(uint8_t data);
static uint8_t LCD_read_nibble(uint8_t rs);
static uint8_t LCD_read_byte(uint8_t rs);
static void LCD_write_nibble(uint8_t rs, uint8_t data);
static void LCD_write_byte(uint8_t rs, uint8_t data);


/**
 * LCDモジュールとの接続端子とポートの初期設定を行う
 * LCDの動作モードを設定する
 *
 * @param 無し
 * @return 無し
 * @attention マイコンリセット後、GPIOは入力方向、プルアップ抵抗あり、オープンドレインにしない
 *            に設定されている.
 *            この関数はデバイス依存の関数である!
 *
 */
void LCD_setup(void)
{
	// 端子をGPIOとして使用する
	PINSEL9 &= ~(3<<BP26);	//P4[29]LCD DB4
	PINSEL1 &= ~(3<<BP6);	//P0[19]LCD DB5
	PINSEL1 &= ~(3<<BP8);	//P0[20]LCD DB6
	PINSEL4 &= ~(3<<BP18);	//P2[9]LCD DB7
	PINSEL3 &= ~(3<<BP18);	//P1[25]LCD RW
	PINSEL3 &= ~(3<<BP20);	//P1[26]LCD RS
	PINSEL3 &= ~(3<<BP16);	//P1[24]LCD E

	// 動作モード設定のために、最初はコマンドを送信するので、ポートを出力方向にする
	GPIO_setDir( PORT4, BP29, OUTDIRECTION );	//DB4
	GPIO_setDir( PORT0, BP19, OUTDIRECTION );	//DB5
	GPIO_setDir( PORT0, BP20, OUTDIRECTION );	//DB6
	GPIO_setDir( PORT2, BP9, OUTDIRECTION );	//DB7
	GPIO_setDir( PORT1, BP25, OUTDIRECTION );	//RW
	GPIO_setDir( PORT1, BP26, OUTDIRECTION );	//RS
	GPIO_setDir( PORT1, BP24, OUTDIRECTION );	//E

	LCD_init();			// 動作モードを設定
	LCD_reset_lcdbuf();	// リングバッファの初期化
}

/**
 *  LCDモジュールの動作モードを設定Initialize LCD module.
 *	Function :	1.4ビット長のインタフェース, 2行表示, duty:1/16,
 *			    2.画面クリア,表示シフトしない
 *			    3.カーソルオフ、カーソル位置自動インクリメント
 *
 * @param  none
 * @return none
 * @attention DTC-R05において、LCDモジュール(LCM-S01602DTR/M)は4ビットデータバス接続されている.
 */
void LCD_init(void)
{
	Delay_ms(41);						// 40ms以上の遅延
	LCD_write_nibble(LCDRS_INST, 0x3);	// 8ビット長のインタフェースに設定
	Delay_ms(5);						// 4.1ms以上の遅延
	LCD_write_nibble(LCDRS_INST, 0x3);	// 8ビット長のインタフェースに設定
	Delay_us(101);						// 100us以上の遅延
	LCD_write_nibble(LCDRS_INST, 0x3);	// 8ビット長のインタフェースに設定
	Delay_us(101);						// 100us以上の遅延
	LCD_write_nibble(LCDRS_INST, 0x2);	// 4ビット長のインタフェースに設定
	Delay_us(101);						// 100us以上の遅延
	LCD_write_byte(LCDRS_INST, LCD_SET | LCD_SET_4 | LCD_SET_2LINES | LCD_SET_BIG);
	while (LCD_is_busy() == true){ // LCDモジュールがビジーである間
		;
	}
	LCD_write_byte(LCDRS_INST, LCD_DSP | LCD_DSP_OFF);
	while (LCD_is_busy() == true){
		;
	}
	LCD_write_byte(LCDRS_INST, LCD_CLR_DSP);
	while (LCD_is_busy() == true){
		;
	}
	LCD_write_byte(LCDRS_INST, LCD_DSP | LCD_DSP_ON | LCD_DSP_CUR_OFF | LCD_DSP_NOBLINK);
	while (LCD_is_busy() == true){
		;
	}
	LCD_write_byte(LCDRS_INST, LCD_EMODE | LCD_EMODE_INC | LCD_EMODE_NOSFT);
	while (LCD_is_busy() == true){
		;
	}

}

/**
 *  リングバッファに格納されている文字またはコマンドを1つ、LCDモジュールに送る.
 *
 *	1.バッファが空なら、何もしない
 *	2.LCDモジュールのビジー状態が100ms以上続いたら、エラー発生とし、処理を終了する.
 *
 * @param  無し
 * @return エラーコード
 * @attention 画面クリアやカーソル位置変更などのコマンドはエスケープシーケンス文字列を処理する
 *
 */

int LCD_display(void)
{
	static unsigned int chk_10ms;
	static bool is_esc = false;

	if (cnt != 0) {						// バッファに有効データあり？
		if (LCD_is_busy() == false) {
			int c;
			c = read_buf();
			if ((c != ESC) && (is_esc == false)) {	// エスケープシーケンス文字列でないか？
				LCD_write_byte(LCDRS_DATA, (uint8_t)c);
			} else {
				is_esc = esc_op(c);					// エスケープシーケンス文字列を解析
			}
			chk_tmout = TMOUT_LCD;					// ビジータイムアウト検査の準備
			chk_10ms = get_time_10ms();
		} else {
			if (get_time_10ms() != chk_10ms) {
				++chk_10ms;
				--chk_tmout;
				if (chk_tmout <= 0) {		// 10msの割込みが規定回数発生した？
					err_lcd = ERR_TOUT;
				}
			}
		}
	}
	return err_lcd;
}

/**
 * エスケープシーケンス文字列を解析し、LCDモジュールにコマンドを送信
 *
 * @param[in] c 文字コード
 * @return 解析結果. ok:true, ng:false
 * @attention staic変数esc_stateにより、どのエスケープシーケンスを解析中か判別する
 */

static bool esc_op(int c)
{
	static enum {
		Esc_start, Esc_bracket,
		Esc_cur_dsp, Esc_cur_dsp_on_off,
		Esc_digit, Esc_loc_x,
		Esc_err
	} esc_state;

	static uint8_t x, y;			//　座標

	bool is_esc = true;

	if (c == ESC) {
		esc_state = Esc_start;
	} else {
		switch (esc_state) {
		case Esc_start:
			if (c == (int)'[') {
				esc_state = Esc_bracket;
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		case Esc_bracket:
			if (c == (int)'>') {			/* Is there need of cursor ON/OFF control */
				esc_state = Esc_cur_dsp;
			} else if (isdigit(c) != 0) {	/* there is numeric character after '[' */
				y = (uint8_t)(c - (int)'0');
				esc_state = Esc_digit;
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		case Esc_cur_dsp:
			if (c == (int)'5') {			/* Cursor ON/OFF control */
				esc_state = Esc_cur_dsp_on_off;
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		case Esc_cur_dsp_on_off:
			if (c == (int)'l') {				/* Cursor ON */
				LCD_write_byte(LCDRS_INST, LCD_DSP | LCD_DSP_ON | LCD_DSP_CUR_ON);
				is_esc = false;			/* end of ESC sequence */
			} else if (c == (int)'h') {		/* Cursor OFF */
				LCD_write_byte(LCDRS_INST, LCD_DSP | LCD_DSP_ON | LCD_DSP_CUR_OFF);
				is_esc = false;			/* end of ESC sequence */
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		case Esc_digit:
			if (isdigit(c) != 0) {	/* Next of '[' is number */
				y = (y * 10) + (uint8_t)((uint8_t)c - (uint8_t)'0');
			} else if (c == (int)'J') {	/* Clear display */
				if (y == 2) {
					LCD_write_byte(LCDRS_INST, LCD_CLR_DSP);
					is_esc = false;	/* end of ESC sequence */
				} else {
					err_lcd = ERR_ESC;
				}
			} else if (c == (int)';') {	/* Designate position to display */
				esc_state = Esc_loc_x;
				x = 0;				/* It turns to x axis */
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		case Esc_loc_x:
			if (isdigit(c) != 0) {	/* Next of '[' is number */
				x = (x * 10) + (uint8_t)((uint8_t)c - (uint8_t)'0');
			} else if (c == (int)'H') {	/* Complete designate position to display */
				LCD_write_byte(LCDRS_INST, (uint8_t)(LCD_DDADDR | (x - 1 + ((y - 1) * LINE_2_BASE))));
				is_esc = false;		/* end of ESC sequence */
			} else {
				err_lcd = ERR_ESC;
			}
			break;
		default:
			err_lcd = ERR_ESC;
			break;
		}
	}
	if(err_lcd == ERR_ESC){
		return false;
	}else{
		return is_esc;
	}
}

/**
 * カーソル位置変更を表すANSIエスケープシーケンス文字列をリングバッファに書き込む
 *
 * reference
 *  #define locate(x,y) fprintf(stderr,"\x1b[%d;%dH", y, x)
 *
 * @param[in] x x座標
 * @param[in] y y座標
 * @return 無し
 * @attention この関数を実行してもLCDの表示状態は変化しない!
 *            This LCD is 16-character, 2-lines.
 *            write data to 'static char buf_lcd[BUF_SIZE]'
 */

void LCD_locate(int x, int y)
{
	char buf[BUF_ESC_SIZE];

	(void)LCD_puts("\x1b""[");
	(void)LCD_puts(myitoa(y, buf, 1, ' '));
	(void)LCD_putchar(';');
	(void)LCD_puts(myitoa(x, buf, 1, ' '));
	(void)LCD_putchar('H');
}

/**
 * 文字列をリングバッファに書き込む
 *
 * @param[in] s 文字列へのポインタ
 * @return 処理結果, 0 : 成功, -1 : 失敗
 * @attention この関数を実行してもLCDの表示状態は変化しない!
 *            write data to 'static char buf_lcd[BUF_SIZE]'
 */

int LCD_puts(const char *s)
{
	while (*s != '\0') {
		(void)LCD_putchar(*s);
		++s;
	}
	return (0 != err_lcd) ? -1 : 0;
}

/**
 * 文字をリングバッファに書き込む
 *
 * @param[in] c 文字コード
 * @return 処理結果, 0 : 成功, -1 : 失敗
 * @attention この関数を実行してもLCDの表示状態は変化しない!
 *            write data to 'static char buf_lcd[BUF_SIZE]'
 */

int LCD_putchar(char c)
{
	__disable_irq();
	if (cnt < BUF_SIZE) {
		err_lcd = 0;
		*write_ptr = c;
		write_ptr++;
		if (write_ptr >= &buf_lcd[BUF_SIZE] ) {
			write_ptr = &buf_lcd[0];
		}
		cnt++;
	} else {
		err_lcd = ERR_BFULL;
	}
	__enable_irq();

	return (0 != err_lcd) ? -1 : 0;
}

/**
 * 現在の読み取りポインタが指す、リングバッファのデータを返す
 *
 * @param 無し
 * @return データ, もし、有効データが無い場合は、-1
 * @attention 無し.
 */

static int read_buf(void)
{
	int c;
	__disable_irq();

	if (cnt > 0) {
		c = (int)*read_ptr;
		read_ptr++;
		if (read_ptr >= &buf_lcd[BUF_SIZE]) {
			read_ptr = &buf_lcd[0];
		}
		cnt--;
	} else {
		c = -1;
	}
	__enable_irq();

	return c;
}

/**
 * リングバッファをリセットする
 *
 *	書込みポインタ、読み取りポインタをバッファの先頭にする
 *	有効データ格納数を0にする
 *	エラーが発生していない状態にする
 *
 * @param 無し
 * @return 無し
 * @attention LCDモジュールにまだ、送信されていないデータがあっても無視する
 */

void LCD_reset_lcdbuf(void)
{
	__disable_irq();
	write_ptr = read_ptr = buf_lcd;
	cnt = 0;
	chk_tmout = TMOUT_LCD;
	err_lcd = 0;
	__enable_irq();
}

/**
 * バス経由でLCDモジュールから4ビット長のデータを読み取る
 *
 * @param 無し
 * @return 読み取ったデータ
 * @attention この関数を呼び出す前にGPIOの方向を入力にしておくこと
 */

static uint8_t LCD_read_4bitDB(void)
{
	uint8_t data;
	data = 0;
	data |= (GPIO_getValueBit( PORT4, BP_LCD_DB4 ) << 0);
	data |= (GPIO_getValueBit( PORT0, BP_LCD_DB5 ) << 1);
	data |= (GPIO_getValueBit( PORT0, BP_LCD_DB6 ) << 2);
	data |= (GPIO_getValueBit( PORT2, BP_LCD_DB7 ) << 3);
	return data;
}

/**
 * バス経由でLCDモジュールへ4ビット長のデータを書き込む
 *
 * @param 書込みデータ
 * @return 無し
 * @attention この関数を呼び出す前にGPIOの方向を出力にしておくこと
 */

static void LCD_write_4bitDB(uint8_t data)
{
	GPIO_setValueBit( PORT4, BP_LCD_DB4, (data&1)>>0 );
	GPIO_setValueBit( PORT0, BP_LCD_DB5, (data&2)>>1 );
	GPIO_setValueBit( PORT0, BP_LCD_DB6, (data&4)>>2 );
	GPIO_setValueBit( PORT2, BP_LCD_DB7, (data&8)>>3 );
}

/**
 * LCDモジュールから4ビットのデータを読み込む
 * with 4-bit data transfer timing sequence.
 *
 * @param[in] rs RS信号
 * @return read 4 bits data
 * @attention 無し
 */

static uint8_t LCD_read_nibble(uint8_t rs)
{
	uint8_t data;

	// LPC1763's All GPIO's default to input with pull-up after reset.
	GPIO_setDir( PORT4, BP_LCD_DB4, LOW ); // set direction to input
	GPIO_setDir( PORT0, BP_LCD_DB5, LOW );
	GPIO_setDir( PORT0, BP_LCD_DB6, LOW );
	GPIO_setDir( PORT2, BP_LCD_DB7, LOW );

	GPIO_setValueBit(PORT1, BP_LCD_RS, rs);
	GPIO_setValueBit(PORT1, BP_LCD_RW, HIGH);
	GPIO_setValueBit(PORT1, BP_LCD_E, LOW);
	Delay_40ns(1);						// tAS 40ns
	GPIO_setValueBit(PORT1, BP_LCD_E, HIGH);
	Delay_40ns(6);						// PWEH Min.230ns
	data = LCD_read_4bitDB();
	GPIO_setValueBit(PORT1, BP_LCD_E, LOW);
	return data;
}

/**
 * LCDモジュールから1バイトデータを読み取る
 *
 * @param[in] rs RS信号
 * @return 1 byte data
 * @attention バスが4ビットなので、4ビットずつデータを読み取り、1バイトデータにする
 */

static uint8_t LCD_read_byte(uint8_t rs)
{
	uint8_t data;
	data = LCD_read_nibble(rs);		// read upper 4bits data
	data <<= 4;
	data |= LCD_read_nibble(rs);	// read lower 4bits data
	return data;
}

/**
 * LCDモジュールに4ビット長のデータを書き込む
 * with 4-bit data transfer timing sequence.
 *
 * @param[in] rs RS信号
 * @param[in] data 1バイトの書込みデータ
 * @return 無し
 * @attention 上位4ビットは0でなければならない
 */

static void LCD_write_nibble(uint8_t rs, uint8_t data)
{
	// Set output bidirectional bus (Never open)
	// you should better remove pull-up!
	GPIO_setDir( PORT4, BP_LCD_DB4, HIGH ); // set direction to output
	GPIO_setDir( PORT0, BP_LCD_DB5, HIGH );
	GPIO_setDir( PORT0, BP_LCD_DB6, HIGH );
	GPIO_setDir( PORT2, BP_LCD_DB7, HIGH );

	GPIO_setValueBit(PORT1, BP_LCD_RS, rs);
	GPIO_setValueBit(PORT1, BP_LCD_RW, LOW);
	GPIO_setValueBit(PORT1, BP_LCD_E, LOW);
	Delay_40ns(1);						// tAS Min.40ns
	GPIO_setValueBit(PORT1, BP_LCD_E, HIGH);
	LCD_write_4bitDB(data);
	Delay_40ns(6);						// PWEH Min.230ns
	GPIO_setValueBit(PORT1, BP_LCD_E, LOW);
	Delay_40ns(7);						// tcycE-PWEH=270ns, tcycE Min.500ns
}

/**
 * LCDモジュールに1バイト長のデータを書き込む
 * with 4-bit data transfer timing sequence.
 *
 * @param[in] rs RS信号
 * @param[in] data 1バイトの書込みデータ
 * @return 無し
 * @attention 無し
 */

static void LCD_write_byte(uint8_t rs, uint8_t data)
{
	LCD_write_nibble(rs, data >> 4);	// write upper 4bits data
	LCD_write_nibble(rs, data & 0xF);	// write lower 4bits data
}

/**
 * LCDモジュールがビジー状態か検査する
 *
 * @param  無し
 * @return 検査結果 YES(true) : busy, NO(false) : not busy
 * @attention 無し
 */

static bool LCD_is_busy(void)
{
	uint8_t ret;

	ret = LCD_read_byte(LCDRS_INST);
	if((ret & LCD_BUSY) != 0){
		return true;
	}else{
		return false;
	} //	return (LCD_read_byte(LCDRS_INST) & LCD_BUSY) ? true : false;
}

/*----------------- end of lcd.c -------------------------------------*/
