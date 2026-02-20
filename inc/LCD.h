/**
 * @file LCD.h
 * @brief 組込みソフトウェア基礎, LCD制御関数のヘッダファイル
 *
 * @author Copyright(C) 2023, PEX, All rights reserved.
 * @attention  this project is for DTC-R05.
 *
 */
#ifndef LCD_H
#define LCD_H

#define	ESC ((int)'\x1b')


#define LCD_X_SIZE		(16)		/* SC1602BSxB */
#define LCD_Y_SIZE		(2)

/* for err_lcd */
#define ERR_TOUT		(1)
#define ERR_BFULL		(2)
#define ERR_ESC			(3)

/* Macro define */
#define LCD_cls()			(void)LCD_puts("\x1b""[2J")
#define LCD_cursor_on()		(void)LCD_puts("\x1b""[>5l")
#define LCD_cursor_off()	(void)LCD_puts("\x1b""[>5h")

/* Function prototype declaration */
extern void LCD_setup(void);
extern void LCD_init(void);
extern int LCD_putchar(char c);
extern int LCD_puts(const char *s);
extern int LCD_display(void);
extern void LCD_reset_lcdbuf(void);
extern void LCD_locate(int x, int y);
#endif	/* _LCD_H */

/*------------------------- end of lcd.h --------------------------*/
